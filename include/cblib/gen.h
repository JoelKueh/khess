
#ifndef CB_GEN_H
#define CB_GEN_H

#include <stdint.h>
#include <stdbool.h>

#include "cbconst.h"

/**
 * Generates moves.
 */
void cb_gen_moves(cb_mvlst_t *mvlst, cb_board_t *board, cb_state_tables_t *state);

/**
 * Generates a mask of legal moves a piece on a square.
 * This move mask is adjusted for pins but does not handle special moves like castling and enp.
 */
uint64_t cb_gen_legal_mv_mask(cb_board_t *board, cb_state_tables_t *state, uint8_t sq);

/**
 * Generates the relevant board tables for a board state.
 */
void cb_gen_board_tables(cb_state_tables_t *state, cb_board_t *board);

#endif /* CB_GEN_H */

