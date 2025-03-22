
#ifndef CB_BOARD_H
#define CB_BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include "board_const.h"
#include "history.h"

typedef struct {
    uint64_t color[2];
    uint64_t piece[2][6];
    uint64_t occ;
} cb_bitboard_t;

typedef struct {
    uint8_t data[64]; /* This is an array of cb_ptype_t but it is stored as a uint8_t. */
} cb_mailbox_t;

typedef struct {
    cb_bitboard_t bb;
    cb_mailbox_t mb;
    cb_hist_stack_t hist;
    uint8_t turn;
    uint32_t fullmove_num;
} cb_board_t;

/* Functions for generating a board from string representations. */
int cb_board_from_fen(cb_board_t *board, const char *fen);
int cb_board_from_uci(cb_board_t *board, const char *uci);
int cb_board_from_pgn(cb_board_t *board, const char *pgn);

/* Functions for converting a board to a 2d array of characters. */
int cb_board_to_str(char str_rep[8][8], const cb_board_t *board);
int cb_board_to_utf8(char str_rep[8][8], const cb_board_t *board);

/* Functions for reading the board representation. */
inline cb_pid_t cb_ptype_at_sq(cb_board_t *board, uint8_t sq)
{
    return board->mb.data[sq];
}

inline cb_pid_t cb_ptype_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    return cb_ptype_at_sq(board, row * 8 + col);
}

inline cb_pid_t cb_pid_at_sq(cb_board_t *board, uint8_t sq)
{
    assert(false && "not yet implemented");
    return board->mb.data[sq] * ;
}

inline cb_pid_t cb_pid_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    assert(false && "not yet implemented");
    return cb_pid_at_sq(board, row * 8 + col);
}

inline cb_pid_t cb_color_at_sq(cb_board_t *board, uint8_t sq)
{
    return board->bb.color[C_WHITE]
}

inline cb_pid_t cb_color_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    return cb_color_at_sq(board, row * 8 + col);
}

#endif /* CB_BOARD_H */
