
#ifndef CB_BITBOARD_H
#define CB_BITBOARD_H

#include <stdint.h>

#include "board_const.h"


inline uint64_t cb_pawn_smear(uint64_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        (pawns >> 9 & ~BB_RIGHT_COL) | (pawns >> 7 & ~BB_LEFT_COL) :
        (pawns << 7 & ~BB_RIGHT_COL) | (pawns << 9 & ~BB_LEFT_COL);
}

inline uint64_t pawn_smear_left(uint64_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        pawns >> 9 & ~BB_RIGHT_COL :
        pawns << 9 & ~BB_LEFT_COL;
}

inline uint64_t pawn_smear_forward(uint64_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        pawns >> 8 :
        pawns << 8;
}

inline uint64_t pawn_smear_right(uint64_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        pawns >> 7 & ~BB_RIGHT_COL :
        pawns << 7 & ~BB_LEFT_COL;
}
#endif /* CB_BITBOARD_H */
