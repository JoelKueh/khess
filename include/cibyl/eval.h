
#ifndef CYBIL_EVAL_H
#define CYBIL_EVAL_H

#include "cblib.h"

/**
 * Evaluates the position specified by board.
 */
float eval(const cb_board_t *board);

/**
 * Preforms an alphabeta move search on the position specified by board.
 */
cb_move_t alphabeta(cb_board_t *board);

#endif /* CIBYL_EVAL_H */
