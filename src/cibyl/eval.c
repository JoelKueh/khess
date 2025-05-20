
#include <float.h>

#include "eval.h"
#include "kh_logging.h"
#include "cb_move.h"
#include "cb_lib.h"

#define ALPHA_BETA_MAX_DEPTH 30

#define MIN(x, y) (x > y ? y : x)
#define MAX(x, y) (x > y ? x : y)

float piece_differential(const cb_board_t *board)
{
    if (board->bb.piece[board->turn][CB_PTYPE_KING] == 0)
        return board->turn ? FLT_MIN : FLT_MAX;

    return 0.0;
}

float eval(const cb_board_t *board)
{
    return piece_differential(board);
}

/* This is just a minimax search for now. I'll flesh it out into alphabeta later. */
float searching(cb_board_t *board, cb_state_tables_t *state, int depth)
{
    cb_mvlst_t mvlst;
    cb_move_t mv;
    float value = FLT_MAX;
    float result;
    int i;

    /* Perform static evaluation if we are ready to evaluate as is. */
    if (depth < 1)
        return eval(board);

    /* Make moves down the tree. */
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(board, mv);
        result = searching(board, state, depth - 1);
        value = board->turn ? MAX(result, value) : MIN(result, value);
        cb_unmake(board);
    }

    return value;
}

cb_move_t alphabeta(cb_board_t *board, int base_depth)
{
    kh_errno_t result;
    kh_error_t err;
    cb_mvlst_t mvlst;
    cb_move_t mv;
    uint64_t perft_results[CB_MAX_NUM_MOVES];
    cb_state_tables_t state;
    uint64_t cnt = 0;
    uint64_t total = 0;
    char buf[6];
    int i;

    /* Exit early if depth is less than 1. */
    if (base_depth < 1) {
        printf("Can't alphabeta with a depth below 1\n");
        return CB_INVALID_MOVE;
    }

    /* Reserve the board history. This line guarantees that make will never write
     * past its proper bounds. */
    if ((result = cb_reserve_for_make(&err, board, ALPHA_BETA_MAX_DEPTH)) != 0) {
        fprintf(stderr, "cb_reserve_for_make: %s\n", err.desc);
        return CB_INVALID_MOVE;
    }
}
