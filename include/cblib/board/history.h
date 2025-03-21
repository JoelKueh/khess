
#ifndef CB_HISTORY_H
#define CB_HISTORY_H

#include <stdint.h>
#include <stdbool.h>

#include "board_const.h"

typedef uint16_t cb_history_t;

/**
 * Returns true if the player has the right to king side castle, false otherwise.
 */
inline bool cb_hist_has_ksc(cb_history_t hist, enum cb_color color)
{
    return (hist & 0b1000 >> color * 2) != 0;
}

/**
 * Returns true if the player still has the right to queen side castle, false otherwise.
 */
inline bool cb_hist_has_qsc(cb_history_t hist, enum cb_color color)
{
    return (hist & 0b100 >> color * 2) != 0;
}


/**
 * Removes the right to king side castle.
 */
inline void cb_hist_remove_ksc(cb_history_t *hist, enum cb_color color)
{
    *hist &= ~0b1000 >> (color * 2);
}

/**
 * Removes the right to queen side castle.
 */
inline void cb_hist_remove_qsc(cb_history_t *hist, enum cb_color color)
{
    *hist &= ~0b100 >> (color * 2);
}

/**
 * Removes all castling rights for a specified color.
 */
inline void cb_hist_remove_castle(cb_history_t *hist, enum cb_color color)
{
    *hist &= ~0b1100 >> (color * 2);
}


/**
 * Adds king side castling right.
 */
inline void cb_hist_add_ksc(cb_history_t *hist, enum cb_color color)
{
    *hist |= 0b1000 >> (color * 2);
}

/**
 * Adds queen side castling right.
 */
inline void cb_hist_add_qsc(cb_history_t *hist, enum cb_color color)
{
    *hist |= 0b100 >> (color * 2);
}

/**
 * Removes all castling rights for specified color.
 */
inline void cb_hist_add_castle(cb_history_t *hist, enum cb_color color)
{
    *hist |= 0b1100 >> (color * 2);
}


/**
 * Returns true if there is an enpassant availiable.
 */
inline bool cb_hist_enp_availiable(cb_history_t hist)
{
    return (hist & HIST_ENP_COL) != 0;
}

/**
 * Sets up this move state to open an enpassant square.
 */
inline void cb_hist_set_enp(cb_history_t *hist, uint8_t enp_col)
{
    *hist = (*hist & ~HIST_ENP_COL) | (enp_col << 5);
    *hist |= HIST_ENP_AVAILABLE;
}

/**
 * Removes enpassant from the history state.
 */
inline void cb_hist_decay_enp(cb_history_t *hist)
{
    *hist &= ~HIST_ENP_ALL;
}

/**
 * Sets up this move state to hold a captured piece.
 */
inline void cb_hist_set_captured_piece(cb_history_t *hist, enum cb_bb_pid pid)
{
    *hist = (*hist & ~HIST_ENP_COL) | (HIST_ENP_COL << 5);
    *hist &= ~HIST_ENP_AVAILABLE;
}


/**
 * Returns true if the 50-move rule has been met, else returns false.
 */
inline bool cb_hist_halfmove_clk_done(cb_history_t hist)
{
    return (hist & HIST_HALFMOVE_CLOCK) == HIST_HALFMOVE_FIFTY;
}

/**
 * Resets the halfmove clock.
 */
inline void cb_hist_reset_halfmove_clk(cb_history_t *hist)
{
    *hist &= ~HIST_HALFMOVE_CLOCK;
}

/**
 * Increments the halfmove clock.
 */
inline void cb_hist_inc_halfmove_clk(cb_history_t *hist)
{
    *hist += UINT16_C(1) << 8;
}

/**
 * Decays castle rights after a move.
 */
inline void cb_hist_decay_castle_rights(cb_history_t *hist, uint8_t color,
        uint8_t to, uint8_t from)
{
    /* Remove castling rights for moving a king or rook. */
    *hist &= (from == M_WHITE_KING_START) || (from == M_BLACK_KING_START) ?
        ~(0b1100 >> (color * 2)) :
        0xFFFF;
    *hist &= (from == M_WHITE_KING_SIDE_ROOK_START)
        || (from == M_BLACK_KING_SIDE_ROOK_START) ?
        ~(0b1000 >> (color * 2)) :
        0xFFFF;
    *hist &= (from == M_WHITE_QUEEN_SIDE_ROOK_START)
        || (from == M_BLACK_QUEEN_SIDE_ROOK_START) ?
        ~(0b100 >> (color * 2)) :
        0xFFFF;

    /* Remove castling rights for taking a rook. */
    *hist &= (to == M_WHITE_KING_SIDE_ROOK_START)
        || (to == M_BLACK_KING_SIDE_ROOK_START) ?
        ~(0b1000 >> (ENEMY_COLOR(color) * 2)) :
        0xFFFF;
    *hist &= (to == M_WHITE_QUEEN_SIDE_ROOK_START)
        || (to == M_BLACK_QUEEN_SIDE_ROOK_START) ?
        ~(0b100 >> (ENEMY_COLOR(color) * 2)) :
        0xFFFF;
}

#endif /* CB_HISTORY_H */
