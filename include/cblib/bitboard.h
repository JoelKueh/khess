
#ifndef CB_BITBOARD_H
#define CB_BITBOARD_H

#include <stdint.h>

#include "board/board_const.h"

typedef struct {
    uint64_t color[2];
    uint64_t piece[2][6];
    uint64_t occ;
} cb_bitboard_t;

inline uint64_t cb_pawn_smear(uint64_t pawns, enum cb_color color)
{
    return color == C_WHITE ?
        (pawns >> 9 & ~BB_RIGHT_COL) | (pawns >> 7 & ~BB_LEFT_COL) :
        (pawns << 7 & ~BB_RIGHT_COL) | (pawns << 9 & ~BB_LEFT_COL);
}

inline uint64_t pawn_smear_left(uint64_t pawns, enum cb_color color)
{
    return color == C_WHITE ?
        pawns >> 9 & ~BB_RIGHT_COL :
        pawns << 9 & ~BB_LEFT_COL;
}

inline uint64_t pawn_smear_forward(uint64_t pawns, enum cb_color color)
{
    return color == C_WHITE ?
        pawns >> 8 :
        pawns << 8;
}

inline uint64_t pawn_smear_right(uint64_t pawns, enum cb_color color)
{
    return color == C_WHITE ?
        pawns >> 7 & ~BB_RIGHT_COL :
        pawns << 7 & ~BB_LEFT_COL;
}

inline uint8_t peek_rbit(uint64_t bb)
{
#if __has_builtin (__builtin_ctzl)
    return __builtin_ctzl(bb);
#else
#   error __builtin_ctzl not supported!
#endif
}

inline uint8_t pop_rbit(uint64_t *bb)
{
    uint8_t idx = peek_rbit(*bb);
    *bb ^= UINT64_C(1) << idx;
    return idx;
}

inline uint8_t popcnt(uint64_t bb)
{
#if __has_builtin (__builtin_popcountl)
    return __builtin_popcountl(bb);
#else
#   error __builtin_popcountl not supported!
#endif
}

#endif /* CB_BITBOARD_H */
