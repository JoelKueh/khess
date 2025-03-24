
#ifndef CB_BOARD_H
#define CB_BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include "board_const.h"
#include "history.h"
#include "move.h"

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

/* Functions for converting a board to a 2d array of characters. */
int cb_board_to_str(char str_rep[8][8], const cb_board_t *board);
int cb_board_to_utf8(char str_rep[8][8], const cb_board_t *board);

/* Functions for making and unmaking moves. */
void make(cb_board_t *board, cb_move_t mv);
void unmake(cb_board_t *board);

/* Functions for reading the board representation. */
inline cb_ptype_t cb_ptype_at_sq(cb_board_t *board, uint8_t sq)
{
    return board->mb.data[sq];
}

inline cb_ptype_t cb_ptype_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    return cb_ptype_at_sq(board, row * 8 + col);
}

inline cb_pid_t cb_pid_at_sq(cb_board_t *board, uint8_t sq)
{
    assert(false && "not yet implemented");
    return board->mb.data[sq];
}

inline cb_pid_t cb_pid_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    assert(false && "not yet implemented");
    return cb_pid_at_sq(board, row * 8 + col);
}

inline cb_color_t cb_color_at_sq(cb_board_t *board, uint8_t sq)
{
    return board->bb.color[CB_WHITE] & (UINT64_C(1) << sq) ? CB_WHITE : CB_BLACK;
}

inline cb_color_t cb_color_at(cb_board_t *board, uint8_t row, uint8_t col)
{
    return cb_color_at_sq(board, row * 8 + col);
}

/* Functions for manipulating the board representation. */
inline void cb_replace_piece(cb_board_t *board, uint8_t sq, uint8_t ptype, uint8_t pcolor,
        uint8_t old_ptype, uint8_t old_pcolor)
{
    board->mb.data[sq] = ptype;
    board->bb.piece[pcolor][ptype] |= UINT64_C(1) << sq;
    board->bb.color[pcolor] |= UINT64_C(1) << sq;
    board->bb.piece[old_pcolor][old_ptype] &= ~(UINT64_C(1) << sq);
    board->bb.color[old_pcolor] &= ~(UINT64_C(1) << sq);
}

inline void cb_write_piece(cb_board_t *board, uint8_t sq, uint8_t ptype, uint8_t pcolor)
{
    board->mb.data[sq] = ptype;
    board->bb.piece[pcolor][ptype] |= UINT64_C(1) << sq;
    board->bb.color[pcolor] |= UINT64_C(1) << sq;
    board->bb.occ |= UINT64_C(1) << sq;
}

inline void cb_delete_piece(cb_board_t *board, uint8_t sq, uint8_t ptype, uint8_t pcolor)
{
    board->mb.data[sq] = BB_EMPTY;
    board->bb.piece[pcolor][ptype] &= ~(UINT64_C(1) << sq);
    board->bb.color[pcolor] &= ~(UINT64_C(1) << sq);
    board->bb.occ &= ~(UINT64_C(1) << sq);
}

#endif /* CB_BOARD_H */
