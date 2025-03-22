
#ifndef CB_BOARD_CONST
#define CB_BOARD_CONST

#include <stdint.h>

#define ENEMY_COLOR(c) (!c)

typedef enum {
    CB_WHITE = 1,
    CB_BLACK = 0
} cb_color_t;

typedef enum  {
    M_PID_EMPTY = 0b0000,

    M_PID_WHITE_PAWN   = 0b0001,
    M_PID_WHITE_KNIGHT = 0b0010,
    M_PID_WHITE_BISHOP = 0b0011,
    M_PID_WHITE_ROOK   = 0b0100,
    M_PID_WHITE_QUEEN  = 0b0101,
    M_PID_WHITE_KING   = 0b0110,

    M_PID_BLACK_PAWN   = 0b1001,
    M_PID_BLACK_KNIGHT = 0b1010,
    M_PID_BLACK_BISHOP = 0b1011,
    M_PID_BLACK_ROOK   = 0b1100,
    M_PID_BLACK_QUEEN  = 0b1101,
    M_PID_BLACK_KING   = 0b1110
} cb_pid_t;

typedef enum {
    BB_PID_PAWN   = 0,
    BB_PID_KNIGHT = 1,
    BB_PID_BISHOP = 2,
    BB_PID_ROOK   = 3,
    BB_PID_QUEEN  = 4,
    BB_PID_KING   = 5,
    BB_PID_EMPTY  = 6
} cb_ptype_t;

const uint16_t HIST_INIT_BOARD_STATE = 0b0;
const uint16_t HIST_ENP_COL          = 0b11100000;
const uint16_t HIST_PID_COL          = 0b11100000;
const uint16_t HIST_ENP_AVAILABLE    = 0b10000;
const uint16_t HIST_ENP_ALL          = 0b11110000;
const uint16_t HIST_HALFMOVE_CLOCK   = 0b11111100000000;
const uint16_t HIST_HALFMOVE_FIFTY   = 50 << 8;

const uint8_t M_WHITE_KING_START               = 60;
const uint8_t M_WHITE_KING_SIDE_ROOK_START     = 63;
const uint8_t M_WHITE_QUEEN_SIDE_ROOK_START    = 56;
const uint8_t M_WHITE_KING_SIDE_CASTLE_TARGET  = 62;
const uint8_t M_WHITE_KING_SIDE_ROOK_TARGET    = 61;
const uint8_t M_WHITE_QUEEN_SIDE_CASTLE_TARGET = 58;
const uint8_t M_WHITE_QUEEN_SIDE_ROOK_TARGET   = 59;

const uint8_t M_BLACK_KING_START               = 4;
const uint8_t M_BLACK_KING_SIDE_ROOK_START     = 7;
const uint8_t M_BLACK_QUEEN_SIDE_ROOK_START    = 0;
const uint8_t M_BLACK_KING_SIDE_CASTLE_TARGET  = 6;
const uint8_t M_BLACK_KING_SIDE_ROOK_TARGET    = 5;
const uint8_t M_BLACK_QUEEN_SIDE_CASTLE_TARGET = 2;
const uint8_t M_BLACK_QUEEN_SIDE_ROOK_TARGET   = 3;

const uint8_t M_WHITE_MIN_ENPASSANT_TARGET = 40;
const uint8_t M_BLACK_MIN_ENPASSANT_TARGET = 16;

const uint64_t BB_RIGHT_COL        = 0x8080808080808080;
const uint64_t BB_LEFT_COL         = 0x0101010101010101;
const uint64_t BB_RIGHT_TWO_COLS   = 0xC0C0C0C0C0C0C0C0;
const uint64_t BB_LEFT_TWO_COLS    = 0x0303030303030303;
const uint64_t BB_TOP_ROW          = 0x00000000000000FF;
const uint64_t BB_BOTTOM_ROW       = 0xFF00000000000000; 
const uint64_t BB_FULL             = 0xFFFFFFFFFFFFFFFF;
const uint64_t BB_EMPTY            = 0x0000000000000000;
const uint64_t BB_BLACK_PAWN_HOME  = 0x000000000000FF00; /* Second-to-top row */
const uint64_t BB_WHITE_PAWN_HOME  = 0x00FF000000000000; /* Second-to-bottom row */
const uint64_t BB_BLACK_PAWN_LINE  = 0x00000000FF000000; /* Fourth row from top */
const uint64_t BB_WHITE_PAWN_LINE  = 0x000000FF00000000; /* Fourth row from bottom */

const uint64_t BB_WHITE_KING_SIDE_CASTLE_TARGET  = UINT64_C(1) << M_WHITE_KING_SIDE_CASTLE_TARGET;
const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_TARGET = UINT64_C(1) << M_WHITE_QUEEN_SIDE_CASTLE_TARGET;

const uint64_t BB_WHITE_KING_SIDE_CASTLE_OCCUPANCY  = 0x6000000000000000;
const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_OCCUPANCY = 0x0E00000000000000;
const uint64_t BB_WHITE_KING_SIDE_CASTLE_CHECK      = 0x7000000000000000;
const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_CHECK     = 0x1C00000000000000;

const uint64_t BB_BLACK_KING_SIDE_CASTLE_TARGET  = UINT64_C(1) << M_BLACK_KING_SIDE_CASTLE_TARGET;
const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_TARGET = UINT64_C(1) << M_BLACK_QUEEN_SIDE_CASTLE_TARGET;

const uint64_t BB_BLACK_KING_SIDE_CASTLE_OCCUPANCY  = 0x0000000000000060;
const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_OCCUPANCY = 0x000000000000000E;
const uint64_t BB_BLACK_KING_SIDE_CASTLE_CHECK      = 0x0000000000000070;
const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_CHECK     = 0x000000000000001C;

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

#endif /* CB_BOARD_CONST */
