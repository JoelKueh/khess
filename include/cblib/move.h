
#ifndef CB_MOVE_H
#define CB_MOVE_H

#include <stdint.h>
#include <stdbool.h>

#define CB_MAX_NUM_MOVES 218
#define CB_INVALID_MOVE 0b0110111111111111

/**
 * Enum holding the different flags that a move can contain
 */
typedef enum {
    CB_MV_QUIET                =  0 << 12,
    CB_MV_DOUBLE_PAWN_PUSH     =  1 << 12,
    CB_MV_KING_SIDE_CASTLE     =  2 << 12,
    CB_MV_QUEEN_SIDE_CASTLE    =  3 << 12,
    CB_MV_CAPTURE              =  4 << 12,
    CB_MV_ENPASSANT            =  5 << 12,
    CB_MV_KNIGHT_PROMO         =  8 << 12,
    CB_MV_BISHOP_PROMO         =  9 << 12,
    CB_MV_ROOK_PROMO           = 10 << 12,
    CB_MV_QUEEN_PROMO          = 11 << 12,
    CB_MV_KNIGHT_PROMO_CAPTURE = 12 << 12,
    CB_MV_BISHOP_PROMO_CAPTURE = 13 << 12,
    CB_MV_ROOK_PROMO_CAPTURE   = 14 << 12,
    CB_MV_QUEEN_PROMO_CAPTURE  = 15 << 12
} cb_mv_flag_t;

/**
 * Masks for different sections of the move.
 */
const uint16_t CB_MV_TO_MASK   = 0x3F;
const uint16_t CB_MV_FROM_MASK = 0x3F << 6;
const uint16_t CB_MV_FLAG_MASK = 0xF << 12;

/**
 * Types to hold move information
 */
typedef uint16_t cb_move_t;
typedef struct {
    cb_move_t moves[CB_MAX_NUM_MOVES];
    uint8_t head;
} cb_mvlst_t;

/**
 * Returns the "to" square for the move as a 6-bit integer.
 */
inline uint8_t cb_mv_get_to(cb_move_t mv)
{
    return mv & CB_MV_TO_MASK;
}

/**
 * Returns the from square for the move as a 6-bit integer.
 */
inline uint8_t cb_mv_get_from(cb_move_t mv)
{
    return mv & CB_MV_FROM_MASK;
}

/**
 * Returns the flags for the move as a cb_move_flags.
 */
inline uint16_t cb_mv_get_flags(cb_move_t mv)
{
    return mv & CB_MV_FLAG_MASK;
}

/**
 * Masks together a move from the raw data.
 */
inline uint8_t cb_mv_from_data(uint16_t from, uint16_t to, uint16_t flags)
{
    return flags | (from << 6) | to;
}

/**
 * Returns the size of a move list.
 */
inline uint8_t cb_mvlst_size(cb_mvlst_t *mvlst)
{
    return mvlst->head;
}

/**
 * Clears the move list.
 */
inline void cb_mvlst_clear(cb_mvlst_t *mvlst)
{
    mvlst->head = 0;
}

/**
 * Pushes an element to the move list.
 */
inline void cb_mvlst_push(cb_mvlst_t *mvlst, cb_move_t move)
{
    mvlst->moves[mvlst->head] = move;
}

/**
 * Pops one elemnt off the move list.
 * This function does not perform bounds checking.
 * User must guarantee that move list has elements in it.
 */
inline cb_move_t cb_mvlst_pop(cb_mvlst_t *mvlst)
{
    return mvlst->moves[--mvlst->head];
}

/**
 * Returns the move at a specified index.
 */
inline cb_move_t cb_mvlst_at(cb_mvlst_t *mvlst, uint8_t idx)
{
    return mvlst->moves[idx];
}

#endif /* CB_MOVE_H */
