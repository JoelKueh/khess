
#include <string.h>

#include "cb/tables.h"
#include "cb/const.h"

bitboard_t pawn_atks[2][64];
bitboard_t knight_atks[64];
bitboard_t king_atks[64];
bitboard_t to_from_table[64][64];

/**
 * Generates a lookup table from a set of offsets on each square.
 */
void gen_table_from_offsets(bitboard_t table[64], const int8_t offsets[], uint8_t offset_len)
{
    int i, j, sq;

    /* Zero the table. */
    memset(table, 0, 64 * sizeof(bitboard_t));

    /* Loop through all squares. Build the mask from the set of offsets. */
    for (i = 0; i < 64; i++) {
        for (j = 0; j < offset_len; j++) {
            sq = i + offsets[j];
            if (sq < 0 || sq >= 64)
                continue;
            table[i] |= UINT64_C(1) << sq;
        }

        /* Remove all invalid moves.
         * If we are in the right two columns, we cannot jump to the left. */
        if ((UINT64_C(1) << i) & BB_RIGHT_TWO_COLS)
            table[i] &= ~BB_LEFT_TWO_COLS;
        else if ((UINT64_C(1) << i) & BB_LEFT_TWO_COLS)
            table[i] &= ~BB_RIGHT_TWO_COLS;
    }
}

void gen_pawn_atk_table()
{
    const int8_t OFFSETS[2][8] = {{-7, -9}, {7, 9}};
    gen_table_from_offsets(pawn_atks[0], OFFSETS[0], 2);
    gen_table_from_offsets(pawn_atks[1], OFFSETS[1], 2);
}

void gen_knight_atk_table()
{
    const int8_t OFFSETS[8] = {-17, -15, -10, -6, 6, 10, 15, 17};
    gen_table_from_offsets(knight_atks, OFFSETS, 8);
}

void gen_king_atk_table()
{
    const int8_t OFFSETS[8] = {9, 8, 7, 1, -1, -7, -8, -9};
    gen_table_from_offsets(king_atks, OFFSETS, 8);
}

/**
 * Get the direction of the ray that extends from sq1 to sq2.
 */
cb_dir_t cb_get_ray_direction(square_t sq1, square_t sq2)
{
    uint8_t direction;

    /* Compute the ray based on the square. */
    if (sq1.rank == sq2.rank) {
        /* Slide left and right if the ranks are equal. */
        return sq1.idx < sq2.idx ? CB_DIR_R : CB_DIR_L;
    } else if (sq1.file == sq2.file) {
        /* Slide up and down if the files are equal. */
        return sq1.idx < sq2.idx ? CB_DIR_U : CB_DIR_D;
    } else if (sq1.file + sq1.rank == sq2.file + sq2.rank) {
        /* UL and DR diagonals have equal Manhattan distance from square A1. */
        return sq1.idx < sq2.idx ? CB_DIR_UL : CB_DIR_DR;
    } else if (sq1.file - sq1.rank == sq2.file - sq2.rank) {
        /* Rank - File is equal for all UR and DL diagonals. */
        return sq1.idx < sq2.idx ? CB_DIR_UR : CB_DIR_DL;
    } else {
        return CB_DIR_INVALID;
    }
}

/**
 * Generate the ray that connects sq1 and sq2.
 */
bitboard_t get_connecting_ray(square_t sq1, square_t sq2)
{
    bitboard_t mask = 0;
    uint8_t direction;

    /* Get the ray direction. */
    if ((direction = cb_get_ray_direction(sq1, sq2)) == CB_DIR_INVALID) {
        return mask;
    }

    /* Slide along the ray until we reach the destination. */
    mask = 0;
    while (sq1.idx != sq2.idx) {
        mask |= UINT64_C(1) << sq1.idx;
        sq1.idx += dir_offset_mapping[direction];
    }

    return mask;
}

/**
 * Generate the table of rays that extend from one square to another.
 */
void gen_to_from_table()
{
    square_t sq1, sq2;

    /* Loop over all of the squares on the board and generate the respective rays. */
    for (sq1.idx = 0; sq1.idx < 64; sq1.idx++) {
        for (sq2.idx = 0; sq2.idx < 64; sq2.idx++) {
            to_from_table[sq1.idx][sq2.idx] = get_connecting_ray(sq1, sq2);
        }
    }
}

void cb_init_normal_tables()
{
    gen_pawn_atk_table();
    gen_knight_atk_table();
    gen_king_atk_table();
    gen_to_from_table();
}

bitboard_t cb_read_pawn_atk_msk(square_t sq, cb_color_t color)
{
    return pawn_atks[color][sq.idx];
}

bitboard_t cb_read_knight_atk_msk(square_t sq)
{
    return knight_atks[sq.idx];
}

bitboard_t cb_read_king_atk_msk(square_t sq)
{
    return king_atks[sq.idx];
}

bitboard_t cb_read_tf_table(square_t sq1, square_t sq2)
{
    return to_from_table[sq1.idx][sq2.idx];
}
