
#ifndef CBLIB_H
#define CBLIB_H

#include "board.h"
#include "move.h"
#include "gen.h"

/* Functions for generating a board from string representations. */
cb_error_t cb_board_from_fen(cb_board_t *board, const char *fen);
cb_error_t cb_board_from_uci(cb_board_t *board, const char *uci);
cb_error_t cb_board_from_pgn(cb_board_t *board, const char *pgn);

/* Functions for generating moves from string representations. */
cb_error_t cb_make(cb_board_t *board, const cb_move_t mv);
void cb_unmake(cb_board_t *board);

#endif /* CBLIB_H */
