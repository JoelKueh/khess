
#ifndef CB_TABLES_H
#define CB_TABLES_H

#include <stdint.h>
#include <stdbool.h>
#include "cb/types.h"
#include "log.h"

/**
 * @breif Enum defining directions of rays on the board.
 *
 * Starts with 0 being right and goes counterclockwize.
 *
 *      3 2 1
 *      4 X 0
 *      5 6 7
 *
 * CB_DIF_UNION is the index of the union mask in the PIN set.
 * CB_DIR_INVALID is used for things that are not rays.
 */
typedef enum : uint8_t {
    CB_DIR_R  = 0,
    CB_DIR_UR = 1,
    CB_DIR_U  = 2,
    CB_DIR_UL = 3,
    CB_DIR_L  = 4,
    CB_DIR_DL = 5,
    CB_DIR_D  = 6,
    CB_DIR_DR = 7,
    CB_DIR_UNION = 8,
    CB_DIR_INVALID = 9
} cb_dir_t;

/**
 * @breif Maps ray directions to bit offsets.
 *
 * e.g. RIGHT is 1, UP RIGHT is -7, etc...
 */
extern const int8_t dir_offset_mapping[8];

/**
 * @breif Returns the direction of the ray that connects two squares.
 * @param sq1 The first square.
 * @param sq2 The second square.
 * @return The ray direction or CB_DIR_INVALID if the squares are not on a ray.
 */
cb_dir_t cb_get_ray_direction(square_t sq1, square_t sq2);

/**
 * @breif Initializes the magical tables
 * @return An int containing the error code for any errors that occured.
 */
cibyl_errno_t cb_init_magic_tables(cibyl_error_t *err);

/**
 * @breif Initializes the normal tables.
 */
void cb_init_normal_tables();

/**
 * @breif Cleans up the normal tables.
 */
void cb_free_magic_tables();

/* Functions to perform reads on the magical tables. */

/**
 * @breif Reads the bishop attack masks
 * @param sq The square.
 * @param occ The relevant occupancy mask.
 * @return A bitmask representing the attacked squares.
 */
uint64_t cb_read_bishop_atk_msk(square_t sq, uint64_t occ);

/**
 * @breif Reads the rook attack masks
 * @param sq The square.
 * @param occ The relevant occupancy mask.
 * @return A bitmask representing the attacked squares.
 */
uint64_t cb_read_rook_atk_msk(square_t sq, uint64_t occ);

/**
 * @breif Reads the pawn attack masks
 * @param sq The square.
 * @param color The color of the attacking pawn.
 * @return A bitmask representing the attacked squares.
 */
uint64_t cb_read_pawn_atk_msk(square_t sq, cb_color_t color);

/**
 * @breif Reads the knight attack masks
 * @param sq The square.
 * @return A bitmask representing the attacked squares.
 */
uint64_t cb_read_knight_atk_msk(square_t sq);

/**
 * @breif Reads the king attack masks
 * @param sq The square.
 * @return A bitmask representing the attacked squares.
 */
uint64_t cb_read_king_atk_msk(square_t sq);

/**
 * @breif Handles lookups for the ray that connects two squares
 *
 * The ray starts from sq1 and extends up to but not including sq1.
 * If sq1 and sq2 are not on a ray, returns 0.
 *
 * @param sq1 The starting square.
 * @param sq2 The ending square.
 * @return Either the ray connecting the squares or zero.
 */
uint64_t cb_read_tf_table(square_t sq1, square_t sq2);

/**
 * Generate the array of rook magics.
 * This is run once by a helper program and the resulting magics are inserted into magical.c.
 */
cibyl_errno_t cb_gen_rook_magics(cibyl_error_t *err, uint64_t magics[64]);

/**
 * @brief Generate the array of bishop magics.
 * This is run once by a helper program and the resulting magics are inserted into magical.c.
 */
cibyl_errno_t cb_gen_bishop_magics(cibyl_error_t *err, uint64_t magics[64]);

/**
 * @breif Print an array of magics as hex literals.
 * This is run once by the helper program and magics are inserted into magical.c.
 */
void cb_print_magics(const uint64_t magics[64]);

#endif /* CB_TABLES_H */
