/**
 * Implements a number of common operations on a chessboard.
 * See cb/types.h for details of the board representation.
 */

#ifndef CB_BOARDREP_H
#define CB_BOARDREP_H

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "cb/types.h"

/**
 * @brief Returns the piece type at the given square.
 * @param board The board.
 * @param sq The square.
 */
static inline cb_ptype_t cb_ptype_at(const cb_board_t *board, square_t sq)
{
    return board->mb.data[sq.idx];
}

/**
 * @brief Returns the piece id at the given square specified.
 * @param board The board.
 * @param sq The square.
 */
static inline cb_pid_t cb_pid_at(const cb_board_t *board, square_t sq)
{
    assert(false && "not yet implemented");
    return board->mb.data[sq.idx];
}

/**
 * @brief Returns the color of the piece at the given square.
 * @param board The board.
 * @param sq The square.
 */
static inline cb_color_t cb_color_at(const cb_board_t *board, square_t sq)
{
    return board->bb.color[CB_WHITE] & (UINT64_C(1) << sq.idx) ? CB_WHITE : CB_BLACK;
}

/**
 * @brief Replace a piece on the chessboard.
 * @param board The board.
 * @param sq The square.
 * @param ptype The type of the new piece.
 * @param pcolor The color of the new piece.
 * @param ptype The type of the old piece.
 * @param pcolor The color of the old piece.
 */
static inline void cb_replace_piece(cb_board_t *board, square_t sq, uint8_t ptype, uint8_t pcolor,
        uint8_t old_ptype, uint8_t old_pcolor)
{
    board->mb.data[sq.idx] = ptype;
    board->bb.piece[pcolor][ptype] |= UINT64_C(1) << sq.idx;
    board->bb.color[pcolor] |= UINT64_C(1) << sq.idx;
    board->bb.piece[old_pcolor][old_ptype] &= ~(UINT64_C(1) << sq.idx);
    board->bb.color[old_pcolor] &= ~(UINT64_C(1) << sq.idx);
}

/**
 * @brief Write a piece on the chessboard.
 * @param board The board.
 * @param sq The square.
 * @param ptype The type of the piece to write.
 * @param pcolor The color of the piece to write.
 */
static inline void cb_write_piece(cb_board_t *board, square_t sq, uint8_t ptype, uint8_t pcolor)
{
    board->mb.data[sq.idx] = ptype;
    board->bb.piece[pcolor][ptype] |= UINT64_C(1) << sq.idx;
    board->bb.color[pcolor] |= UINT64_C(1) << sq.idx;
    board->bb.occ |= UINT64_C(1) << sq.idx;
}

/**
 * @brief Deltes a piece on the chessboard.
 * @param board The board.
 * @param sq The square.
 * @param ptype The type of the piece to delete.
 * @param pcolor The color of the piece to delete.
 */
static inline void cb_delete_piece(cb_board_t *board, square_t sq, uint8_t ptype, uint8_t pcolor)
{
    board->mb.data[sq.idx] = CB_PTYPE_EMPTY;
    board->bb.piece[pcolor][ptype] &= ~(UINT64_C(1) << sq.idx);
    board->bb.color[pcolor] &= ~(UINT64_C(1) << sq.idx);
    board->bb.occ &= ~(UINT64_C(1) << sq.idx);
}

/**
 * @brief Wipes the chessboard.
 * @param board The board to wipe.
 */
static inline void cb_wipe_board(cb_board_t *board)
{
    int i;
    memset(&board->mb.data, CB_PTYPE_EMPTY, 64 * sizeof(uint8_t));
    memset(&board->bb.piece, 0, 12 * sizeof(uint64_t));
    memset(&board->bb.color, 0, 2 * sizeof(uint64_t));
    board->bb.occ = 0;
    board->hist.count = 0;
}

/**
 * @brief Returns the ascii character for a particular piece and color.
 * @param ptype The type of the piece.
 * @param color The color of the piece.
 */
static inline char ptype_to_ascii(cb_ptype_t piece, cb_color_t color)
{
    switch (piece) {
        case CB_PTYPE_PAWN:
            return color == CB_WHITE ? 'P' : 'p';
        case CB_PTYPE_KNIGHT:
            return color == CB_WHITE ? 'N' : 'n';
        case CB_PTYPE_BISHOP:
            return color == CB_WHITE ? 'B' : 'b';
        case CB_PTYPE_ROOK:
            return color == CB_WHITE ? 'R' : 'r';
        case CB_PTYPE_QUEEN:
            return color == CB_WHITE ? 'Q' : 'q';
        case CB_PTYPE_KING:
            return color == CB_WHITE ? 'K' : 'k';
        case CB_PTYPE_EMPTY:
            return ' ';
    }
}

/**
 * @brief Populates a 2D array with ascii characters representing board pieces.
 * @param str_rep The 2D array to populate.
 * @param board The board to parse.
 */
static inline void cb_board_to_str(char str_rep[8][8], const cb_board_t *board)
{
    square_t sq;
    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        str_rep[sq.rank][sq.file] = ptype_to_ascii(cb_ptype_at(board, sq),
                cb_color_at(board, sq));
    }
}

#endif /* CB_BOARDREP_H */
