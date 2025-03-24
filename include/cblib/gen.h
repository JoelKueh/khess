
#ifndef CB_GEN_H
#define CB_GEN_H

#include <stdint.h>
#include <stdbool.h>

#include "board_const.h"
#include "board.h"
#include "move.h"

typedef struct {
    uint64_t threats;
    uint64_t checks;
    uint64_t check_blocks;
    uint64_t pins[9];
} cb_state_tables_t;

/**
 * Generates moves.
 */
void gen_moves(cb_mvlst_t *mvlst, cb_board_t *board, cb_state_tables_t *state);

/**
 * Generates a mask of legal moves a piece on a square.
 * This move mask is adjusted for pins but does not handle special moves like castling and enp.
 */
uint64_t gen_legal_mv_mask(cb_board_t *board, cb_state_tables_t *state, uint8_t sq);

/**
 * Generates the relevant board tables for a board state.
 */
void gen_board_tables(cb_state_tables_t *state, cb_board_t *board);

#endif /* CB_GEN_H */

