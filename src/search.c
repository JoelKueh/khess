
#include <float.h>
#include <time.h>

#include "engine.h"
#include "cb/cb.h"
#include "cb/move.h"
#include "cb/eval.h"
#include <stdatomic.h>

#define ALPHA_BETA_MAX_DEPTH 30

#define MIN(x, y) (x > y ? y : x)
#define MAX(x, y) (x > y ? x : y)

bool check_stop_conditions(cibyl_error_t *err, thinker_t *tk)
{
    struct timespec now;
    bool result = false;
    int64_t movetime;
    int64_t node_count;
    int i;

    /* Check the search flag to see if we should stop. */
    if (!atomic_load_explicit(&tk->eng->search_flag, memory_order_relaxed)) {
        result = true;
        goto out;
    }

    /* Only the thread with tid 0 checks stop conditions. */
    if (tk->tid != 0)
        goto out;

    /* Only check stop conditions every 4096 nodes. */
    if (!(tk->nodes & 0xFFF))
        goto out;

    /* Check the move timer. */
    if (tk->eng->params.movetime >= 0) {
        timespec_get(&now, TIME_UTC);
        movetime = 1e3 * (now.tv_sec - tk->eng->start_time.tv_sec)
            + 1e-6 * (now.tv_nsec - tk->eng->start_time.tv_nsec);
        if (movetime >= tk->eng->params.movetime) {
            atomic_store_explicit(&tk->eng->search_flag, false, memory_order_relaxed);
            result = false;
            goto out;
        }
    }

    /* Check the node counts. */
    if (tk->eng->params.nodes >= 0) {
        node_count = 0;
        for (i = 0; i < tk->eng->nthinkers; i++) {
            node_count += tk->eng->thinkers[i].nodes;
        }
        if (node_count >= tk->eng->params.nodes) {
            atomic_store_explicit(&tk->eng->search_flag, false, memory_order_relaxed);
            result = false;
            goto out;
        }
    }

out:
    return result;
}

/* This is just a minimax search for now. I'll flesh it out into alphabeta later. */
cibyl_errno_t searching(cibyl_error_t *err, float *evaluation,
                        thinker_t *tk, cb_board_t *board, int depth)
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_mvlst_t mvlst;
    cb_move_t mv;
    float value = FLT_MAX;
    float current_eval;
    int i;

    /* Perform static evaluation if we are ready to evaluate as is. */
    if (depth < 1) {
        value = eval(board);
        goto out;
    }

    /* Make moves down the tree. */
    cb_gen_moves(&mvlst, board);
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        /* Check search termination conditions. */
        if (check_stop_conditions(err, tk)) {
            value = 0.0;
            goto out;
        }

        /* Make another move and explore the search tree recursively. */
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(board, mv);
        tk->nodes += 1;
        current_eval = searching(err, &current_eval, tk, board, depth - 1);
        value = board->turn ? MAX(current_eval, value) : MIN(current_eval, value);
        cb_unmake(board);
    }

out:
    *evaluation = value;
    return result;
}

cibyl_errno_t alphabeta(cibyl_error_t *err, cb_move_t *bestmove,
                        thinker_t *tk, cb_board_t *board, int base_depth)
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_mvlst_t mvlst;
    cb_move_t current_bestmove;
    cb_move_t mv;

    float best_eval = FLT_MIN;
    float eval;
    int i;

    /* Exit early if depth is less than 1. */
    if (base_depth < 1) {
        result = CIBYL_MKERR(err, CIBYL_EINVAL, "invalid alphabeta search depth\n");
        goto out;
    }

    /* Reserve the board history up to ALPHA_BETA_MAX_DEPTH. */
    if (cb_reserve_for_make(NULL, board, ALPHA_BETA_MAX_DEPTH) != 0) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Start the alpha-beta search. */
    cb_gen_moves(&mvlst, board);
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        /* Check search termination conditions. */
        if (check_stop_conditions(err, tk)) {
            goto out;
        }

        /* Make the move and search recursively. */
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(board, mv);
        tk->nodes += 1;
        if (searching(err, &eval, tk, board, base_depth - 1) != CIBYL_EOK) {
            result = CIBYL_ERR_ADD_CONTEXT(err);
            goto out;
        }

        /* Collect the results and unmake the move. */
        if (eval > best_eval) {
            best_eval = eval;
            current_bestmove = mv;
        }
        cb_unmake(board);
    }

out:
    *bestmove = current_bestmove;
    return result;
}

cibyl_errno_t iterative_deepening(cibyl_error_t *err, cb_move_t *bestmove,
                                  thinker_t *tk, cb_board_t *board)
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_move_t current_bestmove = CB_INVALID_MOVE;
    cb_move_t mv;
    int i;

    /* Set initial values for stop condition checks. */
    tk->nodes = 0;
    if (tk->tid == 0) {
        timespec_get(&tk->eng->start_time, TIME_UTC);
    }
    
    /* Perform the search with iterative deepening. */
    for (i = 1; i < ALPHA_BETA_MAX_DEPTH; i++) {
        /* TODO: Reuse the previous move ordering for this search. */
        if (alphabeta(err, &mv, tk, board, i) != CIBYL_EOK) {
            result = CIBYL_ERR_ADD_CONTEXT(err);
            goto out;
        }

        /* Do not update bestmove if this was a partial search. */
        if (!atomic_load_explicit(&tk->eng->search_flag, memory_order_relaxed))
            goto out;
        current_bestmove = mv;

        /* Check if we have exceeded the search depth. */
        if (tk->eng->params.depth > 0 && i >= tk->eng->params.depth) {
            atomic_store_explicit(&tk->eng->search_flag, false, memory_order_relaxed);
            goto out;
        }
    }

out:
    *bestmove = current_bestmove;
    return result;
}
