
#ifndef CB_DEBUG_H
#define CB_DEBUG_H

#include "board.h"
#include "cbconst.h"

/* Functions for printing information about the board. */
void cb_print_mv_hist(FILE *f, cb_board_t *mvlst);
void cb_print_board_ascii(FILE *f, cb_board_t *board);
void cb_print_board_utf8(FILE *f, cb_board_t *board);
void cb_print_bitboard(FILE *f, cb_board_t *board);
void cb_print_state(FILE *f, cb_state_tables_t *state);
void cb_print_moves(FILE *f, cb_mvlst_t *mvlst);

#endif /* CB_DEBUG_H */
