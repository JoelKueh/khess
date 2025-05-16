
#ifndef CIBYL_H
#define CIBYL_H

#include "cblib.h"

typedef enum {
    /* Non-recoverable errors. Almost all errors are treated this way. */
    CIBYL_ENOMEM = -2,
    CIBYL_EABORT = -1,

    /* This is okay, default return value for any function that can err. */
    CIBYL_EOK = 0,
} cibyl_errno_t;

/**
 * Evaluates the position specified by board.
 */
float eval(const cb_board_t *board);

/**
 * Preforms an alphabeta move search on the position specified by board.
 * Works on a copy of the board and not on the board itself.
 */
cb_move_t alphabeta(cb_board_t board);

#endif /* CIBYL_H */
