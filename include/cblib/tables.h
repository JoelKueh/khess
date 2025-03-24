
#ifndef CB_TABLES_H
#define CB_TABLES_H

#include <stdint.h>
#include <stdbool.h>

#include "board_const.h"

/* Enum that contains different direcitons for rays on the board. */
typedef enum {
    CB_DIR_R  = 0,
    CB_DIR_UR = 1,
    CB_DIR_U  = 2,
    CB_DIR_UL = 3,
    CB_DIR_L  = 4,
    CB_DIR_DL = 5,
    CB_DIR_D  = 6,
    CB_DIR_DR = 7,
    CB_DIR_INVALID = 8
} cb_dir_t;

/* Array that maps directions to offsets. */
const uint8_t dir_offset_mapping[8] = { 1, -7, -8, -9, -1, 7, 8, 9 };
uint8_t cb_get_ray_direction(uint8_t sq1, uint8_t sq2);

/* Functions to initialize tables that will be used by the move generator. */
int cb_init_magic_tables();
void cb_init_normal_tables();
void cb_free_magic_tables();

/* Functions to perform reads on the magical tables. */
uint64_t cb_read_bishop_atk_msk(uint8_t sq, uint64_t occ);
uint64_t cb_read_rook_atk_msk(uint8_t sq, uint64_t occ);

/* Functions to perform reads on the normal tables. */
uint64_t cb_read_pawn_atk_msk(uint8_t sq, cb_color_t color);
uint64_t cb_read_knight_atk_msk(uint8_t sq);
uint64_t cb_read_king_atk_msk(uint8_t sq);
uint64_t cb_read_tf_table(uint8_t sq1, uint8_t sq2);

#endif /* CB_TABLES_H */
