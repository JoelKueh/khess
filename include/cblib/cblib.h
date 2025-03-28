
#ifndef CBLIB_H
#define CBLIB_H

#include "board.h"
#include "move.h"
#include "gen.h"
#include "cbconst.h"
#include "cbdbg.h"

/* Functions for allocating and freeing space for a board. */
cb_errno_t cb_board_init(cb_error_t *err, cb_board_t *board);
void cb_board_free(cb_board_t *board);

/* Functions for generating a board from string representations. */
cb_errno_t cb_board_from_fen(cb_error_t *err, cb_board_t *board, char *fen);
cb_errno_t cb_board_from_uci(cb_error_t *err, cb_board_t *board, char *uci);
cb_errno_t cb_board_from_pgn(cb_error_t *err, cb_board_t *board, char *pgn);

/* Functions for generating moves from string representations. */
cb_errno_t cb_mv_from_short_algbr(cb_error_t *err, cb_move_t *mv, cb_board_t *board,
                                  const char *algbr);
cb_errno_t cb_mv_from_uci_algbr(cb_error_t *err, cb_move_t *mv, cb_board_t *board,
                                const char *algbr);
void cb_mv_to_uci_algbr(char buf[6], cb_move_t mv);

/* Functions for making and unmaking moves on a board. */
cb_errno_t cb_reserve_for_make(cb_error_t *err, cb_board_t *board, uint32_t added_depth);
void cb_make(cb_board_t *board, const cb_move_t mv);
void cb_unmake(cb_board_t *board);

#endif /* CBLIB_H */
