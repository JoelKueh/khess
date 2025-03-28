
#include <time.h>

#include "perft.h"

int perfting(cb_board_t *board, int depth)
{
    uint64_t cnt = 0;
    cb_mvlst_t mvlst;
    cb_state_tables_t state;
    int i;
    cb_move_t mv;

    /* Base case. */
    if (depth <= 0)
        return 1;

    /* Generate the moves. */
    cb_gen_board_tables(&state, board);
    cb_gen_moves(&mvlst, board, &state);

    /* Make moves and move down the tree. */
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        mv = cb_mvlst_at(&mvlst, i);
        /* This function can fail, but only when a reservation is needed.
         * As perft does a manual reservation, there is no need to reserve here and no error. */
        cb_make(board, mv);
        cnt += perfting(board, depth - 1);
        cb_unmake(board);
    }

    return cnt;
}

int perft_cheating(cb_board_t *board, int depth)
{
    return 0;
}

int perft(cb_board_t *board, int depth)
{
    cb_errno_t result;
    cb_error_t err;
    uint64_t cnt;

    /* Reserve the board history. This line guarantees that no function in perft will err. */
    if ((result = cb_reserve_for_make(&err, board, depth)) != 0) {
        fprintf(stderr, "cb_reserve_for_make: %s\n", err.desc);
        return result;
    }

    cnt = perfting(board, depth);
    printf("Nodes searched: %lu\n", cnt);

    return 0;
}

int perft_cheat(cb_board_t *board, int depth)
{
    return 0;
}
