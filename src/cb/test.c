#include <utils/utest.h>

#include "cb/cb.h"
#include "cb/move.h"
#include "cb/types.h"
#include <inttypes.h>

uint64_t walking(cb_board_t *board, int depth)
{
    uint64_t cnt = 0;
    int i;
    cb_move_t mv;
    cb_mvlst_t mvlst;

    /* Base case. */
    if (depth <= 0)
        return 1;

    /* Make moves down the tree. */
    cb_gen_moves(&mvlst, board);
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(board, mv);
        cnt += walking(board, depth - 1);
        cb_unmake(board);
    }

    return cnt;
}

cibyl_errno_t walk(cibyl_error_t *err, cb_board_t *board, int depth,
        void (*check_fn)(cb_board_t *board, bool is_leaf))
{
    cibyl_errno_t result = CIBYL_EOK;
    cb_mvlst_t mvlst;
    cb_move_t mv;
    uint64_t perft_results[CB_MAX_NUM_MOVES];
    uint64_t cnt = 0;
    uint64_t total = 0;
    char buf[6];
    int i;

    /* Exit early if depth is less than 1. */
    if (depth < 1) {
        result = CIBYL_MKERR(err, CIBYL_EINVAL, "illegal depth %d specified for perft", depth);
        goto out;
    }

    /* Reserve the board history up to maximum perft depth. */
    if ((result = cb_reserve_for_make(err, board, depth)) != 0) {
        CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Loop through all of the first levels and calculate the number of moves. */
    cb_gen_moves(&mvlst, board);
    for (i = 0; i < cb_mvlst_size(&mvlst); i++) {
        mv = cb_mvlst_at(&mvlst, i);
        cb_make(board, mv);
        cnt = walking(board, depth - 1);
        total += cnt;
        cb_mv_to_uci_algbr(buf, mv);
        printf("%s: %" PRIu64 "\n", buf, cnt);
        cb_unmake(board);
    }

out:
    return result;
}
