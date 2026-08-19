
#ifndef CYBIL_EVAL_H
#define CYBIL_EVAL_H

#include <stdbool.h>
#include "cb/types.h"

/**
 * Evaluates the position specified by board.
 */
int eval(const cb_board_t *board);

/**
 * Applies ordering rules to the provided move list.
 * @param mvlst A pointer to the movelist to sort.
 * @param board The current board.
 * @param pvmv The principal variation move if this is a PV node, or else CB_INVALID_MV.
 */
void reorder_mvlst(cb_mvlst_t *mvlst, const cb_board_t *board, cb_move_t pvmv);

#endif /* CIBYL_EVAL_H */
