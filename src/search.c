#include <stdatomic.h>
#include <time.h>
#include <limits.h>

#include "engine.h"
#include "cb/cb.h"
#include "cb/move.h"
#include "cb/eval.h"

#define EVAL_LOSE (INT_MIN + ALPHA_BETA_MAX_DEPTH)
#define EVAL_DRAW 0

bool check_stop_conditions(thinker_t *tk)
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
    if (tk->nodes & 0xFFF)
        goto out;

    /* Check the move timer. */
    if (tk->eng->params.movetime >= 0) {
        timespec_get(&now, TIME_UTC);
        movetime = 1e3 * (now.tv_sec - tk->eng->start_time.tv_sec)
            + 1e-6 * (now.tv_nsec - tk->eng->start_time.tv_nsec);
        if (movetime >= tk->eng->params.movetime) {
            atomic_store_explicit(&tk->eng->search_flag, false, memory_order_relaxed);
            result = true;
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
            result = true;
            goto out;
        }
    }

out:
    return result;
}

int negamax(thinker_t *tk, line_t *pv, int alpha, int beta, int depth)
{
    cb_mvlst_t mvlst;
    cb_move_t mv;
    line_t line;

    int value;
    int i;

    /* Update the pv and node count. */
    pv->move_count = 0;
    tk->nodes += 1;

    /* If this is a leaf node, then return the static evaluation of the position. */
    if (depth == 0) {
        value = eval(&tk->board);
        goto out;
    }

    /* Recursively call negamax for all child nodes. */
    cb_gen_moves(&mvlst, &tk->board);
    if (cb_mvlst_size(&mvlst) == 0) {
        if (tk->board.threats != 0) {
            /* Faster checkmates are higher value. */
            value = EVAL_LOSE - depth;
        } else {
            value = EVAL_DRAW;
        }
    }

    /* Call negamax recursively. */
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        /* Check search termination conditions. */
        if (check_stop_conditions(tk)) {
            goto out;
        }

        /* Make the move. */
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(&tk->board, mv);

        /* Call negamax with an inverted evaluation function. */
        value = -negamax(tk, &line, -beta, -alpha, depth -1);
        
        /* Collect the results and unmake the move. */
        cb_unmake(&tk->board);

        /* The opponent would never allow us to make a move better than the beta cutoff. */
        if (value >= beta) {
            goto out;
        }

        /* The alpha cutoff is the best move that we can force. */
        if (value > alpha) {
            alpha = value;
            pv->moves[0] = mv;
            memcpy(pv->moves + 1, line.moves, line.move_count * sizeof(cb_move_t));
            pv->move_count = line.move_count + 1;
        }
    }

out:
    return value;
}

cibyl_errno_t alphabeta(cibyl_error_t *err, line_t *pv,
                        thinker_t *tk, cb_board_t *board, int base_depth)
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_mvlst_t mvlst;
    cb_move_t mv;
    line_t line;

    int bestvalue;
    int value;
    int i;

    /* Exit early if depth is less than 1. */
    if (base_depth < 1 || base_depth >= ALPHA_BETA_MAX_DEPTH) {
        result = CIBYL_MKERR(err, CIBYL_EINVAL, "invalid alphabeta search depth\n");
        goto out;
    }

    /* Reserve the board history up to ALPHA_BETA_MAX_DEPTH. */
    if (cb_reserve_for_make(NULL, board, ALPHA_BETA_MAX_DEPTH) != 0) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Start the alpha-beta search. */
    (void)negamax(tk, pv, INT_MIN, INT_MAX, base_depth);

out:
    return result;
}

cibyl_errno_t iterative_deepening(cibyl_error_t *err, line_t *pv, thinker_t *tk, cb_board_t *board)
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_move_t current_bestmove = CB_INVALID_MOVE;
    cb_move_t mv;
    line_t line;
    int i;

    /* Set initial values for stop condition checks. */
    tk->nodes = 0;
    if (tk->tid == 0) {
        timespec_get(&tk->eng->start_time, TIME_UTC);
    }
    
    /* Perform the search with iterative deepening. */
    for (i = 1; i < ALPHA_BETA_MAX_DEPTH; i++) {
        /* TODO: Reuse the previous move ordering for this search. */
        /* TODO: Use good move ordering. :P */
        if (alphabeta(err, &line, tk, board, i) != CIBYL_EOK) {
            result = CIBYL_ERR_ADD_CONTEXT(err);
            goto out;
        }

        /* Do not update bestmove if this was a partial search. */
        if (!atomic_load_explicit(&tk->eng->search_flag, memory_order_relaxed))
            goto out;

        /* TODO: Only the main thread should update the principle variation. */
        /* Update the principle variation if we have completed the search. */
        pv->move_count = line.move_count;
        memcpy(pv->moves, line.moves, line.move_count * sizeof(cb_move_t));

        /* Check if we have exceeded the search depth. */
        if (tk->eng->params.depth > 0 && i >= tk->eng->params.depth) {
            atomic_store_explicit(&tk->eng->search_flag, false, memory_order_relaxed);
            goto out;
        }
    }

out:
    return result;
}
