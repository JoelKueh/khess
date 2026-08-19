
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "cb/tables.h"
#include "cb/bitutil.h"
#include "log.h"

const int8_t dir_offset_mapping[8] = { 1, 9, 8, 7, -1, -9, -8, -7 };

const uint8_t NUM_BISHOP_BITS[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const uint8_t NUM_ROOK_BITS[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

const uint64_t ROOK_MAGICS[64] = {
    UINT64_C(0x2880002041000080),
    UINT64_C(0x280020001001080),
    UINT64_C(0x900410008040022),
    UINT64_C(0x280028004000800),
    UINT64_C(0x2480041000800801),
    UINT64_C(0x100200010090040),
    UINT64_C(0x6c00049b0002001),
    UINT64_C(0xa8002c000108020),
    UINT64_C(0x1002100004082),
    UINT64_C(0x2409000100040200),
    UINT64_C(0xb000401004208),
    UINT64_C(0x402800800040080),
    UINT64_C(0x411000d00100020),
    UINT64_C(0x2290802004801000),
    UINT64_C(0x4808020004000),
    UINT64_C(0xa000800080400034),
    UINT64_C(0x801020000441091),
    UINT64_C(0x201008080010200),
    UINT64_C(0x8000808004000200),
    UINT64_C(0xa08018014000880),
    UINT64_C(0x500808008001000),
    UINT64_C(0x801030040200012),
    UINT64_C(0x1090810021004010),
    UINT64_C(0x22878001e24000),
    UINT64_C(0x813241200148449),
    UINT64_C(0x9020010080800200),
    UINT64_C(0x100040080020080),
    UINT64_C(0x12040280080080),
    UINT64_C(0xd14880480100080),
    UINT64_C(0x120200402082),
    UINT64_C(0x1040200040100048),
    UINT64_C(0x800080204005),
    UINT64_C(0x204026458e001401),
    UINT64_C(0x4301083214000150),
    UINT64_C(0x810800601800400),
    UINT64_C(0x209009005000802),
    UINT64_C(0x400402202000812),
    UINT64_C(0x4820010021001040),
    UINT64_C(0x100401000402001),
    UINT64_C(0x491604001800080),
    UINT64_C(0x430000a044020001),
    UINT64_C(0x1000100200040208),
    UINT64_C(0x12000810020004),
    UINT64_C(0x804040008008080),
    UINT64_C(0x1003001000090020),
    UINT64_C(0x8410820820420010),
    UINT64_C(0x8001008040010020),
    UINT64_C(0x40204000808000),
    UINT64_C(0x2000009044210200),
    UINT64_C(0x8010100228810400),
    UINT64_C(0x2000810040200),
    UINT64_C(0x8000400801980),
    UINT64_C(0x2244100408008080),
    UINT64_C(0x200100401700),
    UINT64_C(0xe0100040002240),
    UINT64_C(0x280009023410300),
    UINT64_C(0x26002114058042),
    UINT64_C(0x4000011008020084),
    UINT64_C(0x1004400080a13),
    UINT64_C(0x489a000810200402),
    UINT64_C(0x502001008400422),
    UINT64_C(0x2005524060000901),
    UINT64_C(0x40002080411d01),
    UINT64_C(0x4080008040102101),
};

const uint64_t BISHOP_MAGICS[64] = {
    UINT64_C(0x28808801216001),
    UINT64_C(0xc00444222012000a),
    UINT64_C(0x100822020200011),
    UINT64_C(0x4042004000000),
    UINT64_C(0x62880a0220200808),
    UINT64_C(0x2068080051921000),
    UINT64_C(0x2004844802002010),
    UINT64_C(0x89a1121896040240),
    UINT64_C(0x8144042209100900),
    UINT64_C(0x1008005410080802),
    UINT64_C(0x30010c4108405004),
    UINT64_C(0x2000840504006000),
    UINT64_C(0x82080240060),
    UINT64_C(0x840800910a0010),
    UINT64_C(0x201c401040c0084),
    UINT64_C(0x400492088408100),
    UINT64_C(0x88010400410c9000),
    UINT64_C(0x800054042000),
    UINT64_C(0x40200200a42008),
    UINT64_C(0x841000820080811),
    UINT64_C(0x1004002802102001),
    UINT64_C(0xf18140408012008),
    UINT64_C(0x4800201208ca00),
    UINT64_C(0x208081020014400),
    UINT64_C(0x4800404002011c00),
    UINT64_C(0x188071040440a00),
    UINT64_C(0x658810000806011),
    UINT64_C(0x941408200c002000),
    UINT64_C(0x400c00c010142),
    UINT64_C(0x2002081833080021),
    UINT64_C(0x1004040051500081),
    UINT64_C(0x520040470104290),
    UINT64_C(0x809005202050100),
    UINT64_C(0x102008e00040242),
    UINT64_C(0x480810700020090),
    UINT64_C(0x8040010040820802),
    UINT64_C(0x80c0040400080120),
    UINT64_C(0x4022401120400),
    UINT64_C(0x511080202091021),
    UINT64_C(0x104442040404200),
    UINT64_C(0x208220a202004080),
    UINT64_C(0x4012020c04210308),
    UINT64_C(0x41010500040c020),
    UINT64_C(0x3308082008200100),
    UINT64_C(0x200014208040080),
    UINT64_C(0x19001802081400),
    UINT64_C(0x431008804142000),
    UINT64_C(0x8002024220104080),
    UINT64_C(0x822088220820214),
    UINT64_C(0x40100400809000),
    UINT64_C(0x2410204010040),
    UINT64_C(0x21082088000),
    UINT64_C(0x8200000041108022),
    UINT64_C(0x2101004202410000),
    UINT64_C(0x6803040141280a00),
    UINT64_C(0x111040120082000),
    UINT64_C(0x40102000a0a60140),
    UINT64_C(0x2520081090008908),
    UINT64_C(0x10008830412a00),
    UINT64_C(0x1000042304105),
    UINT64_C(0x90014004842410),
    UINT64_C(0x402814422015008),
    UINT64_C(0x910224040218c9),
    UINT64_C(0x40808090012004),
};

const uint8_t MAX_BITS_IN_TABLE = 12;
const uint32_t MAX_TABLE_SIZE = 1 << MAX_BITS_IN_TABLE;

uint64_t bishop_occ_mask[64];
uint64_t rook_occ_mask[64];

uint64_t *bishop_atks[64];
uint64_t *rook_atks[64];

static inline uint16_t get_bishop_key(const uint64_t magics[64], square_t sq, uint64_t occ)
{
    /* Hash the occupancy mask and compute the key. */
    occ &= bishop_occ_mask[sq.idx];
    occ *= magics[sq.idx];
    return occ >> (64 - NUM_BISHOP_BITS[sq.idx]);
}

static inline uint16_t get_rook_key(const uint64_t magics[64], square_t sq, uint64_t occ)
{
    /* Hash the occupancy mask and compute the key. */
    occ &= rook_occ_mask[sq.idx];
    occ *= magics[sq.idx];
    return occ >> (64 - NUM_ROOK_BITS[sq.idx]);
}

/**
 * Returns the bishop attack mask given an occupancy set and a square.
 */
uint64_t cb_read_bishop_atk_msk(square_t sq, uint64_t occ)
{
    uint16_t key = get_bishop_key(BISHOP_MAGICS, sq, occ);
    return bishop_atks[sq.idx][key];
}

/**
 * Returns the rook attack mask given an occupancy set and a square.
 */
uint64_t cb_read_rook_atk_msk(square_t sq, uint64_t occ)
{
    uint16_t key = get_rook_key(ROOK_MAGICS, sq, occ);
    return rook_atks[sq.idx][key];
}

/**
 * Returns the occupancy mask for a rook on a square. e.g:
 *
 *      . . . . . . . .
 *      . . x . . . . .
 *      . . x . . . . .
 *      . x . x x x x .
 *      . . x . . . . .
 *      . . x . . . . .
 *      . . x . . . . .
 *      . . . . . . . .
 */
uint64_t get_rook_occ_mask(square_t sq)
{
    uint64_t result = 0;
    square_t source = sq;

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_U];
    while (sq.rank <= 6) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_U];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_D];
    while (sq.rank >= 1) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_D];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_R];
    while (sq.file <= 6) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_R];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_L];
    while (sq.file >= 1) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_L];
    }

    return result;
}

/**
 * Returns the occupancy mask for a bishop on a square. e.g:
 *
 *      . . . . . . . .
 *      . . . . x . . .
 *      . x . x . . . .
 *      . . . . . . . .
 *      . x . x . . . .
 *      . . . . x . . .
 *      . . . . . x . .
 *      . . . . . . . .
 */
uint64_t get_bishop_occ_mask(square_t sq)
{
    uint64_t result = 0;
    square_t source = sq;

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_UR];
    while (sq.rank <= 6 && sq.file <= 6) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_UR];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_UL];
    while (sq.rank <= 6 && sq.file >= 1) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_UL];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_DR];
    while (sq.rank >= 1 && sq.file <= 6) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_DR];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_DL];
    while (sq.rank >= 1 && sq.file >= 1) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_DL];
    }

    return result;
}

/**
 * Returns the attack mask for a rook on a square with a given occupancy set.
 * This mask is much the same as the occupancy mask but you break out of the loop upon hitting
 * a piece in the occupancy mask.
 */
uint64_t get_rook_atk_mask(square_t sq, uint64_t occ)
{
    uint64_t result = 0;
    square_t source = sq;

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_U];
    while (sq.rank <= 7) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_U];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_D];
    while (sq.rank >= 0) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_D];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_R];
    while (sq.file <= 7) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_R];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_L];
    while (sq.file >= 0) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_L];
    }

    return result;
}

/**
 * Returns the attack mask for a rook on a square with a given occupancy set.
 * This mask is much the same as the occupancy mask but you break out of the loop upon hitting
 * a piece in the occupancy mask.
 */
uint64_t get_bishop_atk_mask(square_t sq, uint64_t occ)
{
    uint64_t result = 0;
    square_t source = sq;

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_UR];
    while (sq.rank <= 7 && sq.file <= 7) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_UR];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_UL];
    while (sq.rank <= 7 && sq.file >= 0) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_UL];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_DR];
    while (sq.rank >= 0 && sq.file <= 7) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_DR];
    }

    sq.idx = source.idx + dir_offset_mapping[CB_DIR_DL];
    while (sq.rank >= 0 && sq.file >= 0) {
        result |= UINT64_C(1) << sq.idx;
        sq.idx += dir_offset_mapping[CB_DIR_DL];
    }

    return result;
}

/**
 * Allocates a magic table.
 */
cibyl_errno_t alloc_magic_table(cibyl_error_t *err, uint64_t *table[64], const uint8_t bits[64])
{
    cibyl_errno_t result = CIBYL_EOK;
    square_t sq;

    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        if ((table[sq.idx] = calloc(1 << NUM_BISHOP_BITS[sq.idx], sizeof(uint64_t))) == 0) {
            result = CIBYL_MKERR(err, CIBYL_ENOMEM, "calloc: %s", strerror(errno));
            goto out_free_tables;
        };
    }
    goto out_success;

out_free_tables:
    /* Unwind the calloc stack. */
    while (sq.idx > 0) {
        free(bishop_atks[--sq.idx]);
    }
out_success:
    return result;
}

/**
 * Free a magic table.
 */
void free_magic_table(uint64_t *table[64])
{
    square_t sq;
    for (sq.idx = 0; sq.idx < 64; sq.idx++)
        free(table[sq.idx]);
}

/**
 * Maps an index to the bits of an occupancy mask.
 * Effectively allows you to map an n-bit number to an n-bit mask.
 */
uint64_t map_index_to_occ_mask(uint16_t idx, uint8_t num_bits, uint64_t occ_mask)
{
    uint64_t result = 0;
    uint8_t pos;
    uint8_t i;

    /* Loop over all of the bits in the occupancy mask */
    for (i = 0; i < num_bits; i++) {
        /* Get the position of the bit in the occ mask. */
        pos = pop_rbit(&occ_mask);
        /* If it lines up with the current bit in the index, then place a bit in the result. */
        if (idx & (1 << i)) {
            result |= UINT64_C(1) << pos;
        }
    }

    return result;
}

/**
 * Generates a single bishop lookup table.
 */
cibyl_errno_t gen_bishop_map_for_sq(cibyl_error_t *err, uint64_t *table[64],
        const uint64_t magics[64], square_t sq)
{
    cibyl_errno_t result;
    uint64_t occupied_squares;
    uint64_t legal_moves;
    uint64_t occ;
    uint16_t key;
    int idx;

    for (idx = 0; idx < (1 << NUM_BISHOP_BITS[sq.idx]); idx++) {
        /* Generate the bishop occupancy mask. */
        occ = get_bishop_occ_mask(sq);
        bishop_occ_mask[sq.idx] = occ;

        /* Generate the relevant masks and key for the current index. */
        occupied_squares = map_index_to_occ_mask(idx, NUM_BISHOP_BITS[sq.idx], occ);
        legal_moves = get_bishop_atk_mask(sq, occupied_squares);
        key = get_bishop_key(magics, sq, occupied_squares);

        /* Catch any evil hash collisions. */
        if (table[sq.idx][key] != 0 && table[sq.idx][key] != legal_moves) {
            result = CIBYL_MKERR(err, CIBYL_EINVAL,
                    "hash collision in bishop table: sq=%d idx=%d", sq.idx, idx);
            goto out_err;
        }

        /* Write data into the table. */
        table[sq.idx][key] = legal_moves;
    }

out_err:
    return result;
}

/**
 * Generates the array of bishop lookup tables.
 */
cibyl_errno_t gen_bishop_table(cibyl_error_t *err)
{
    cibyl_errno_t result = CIBYL_EOK;
    square_t sq;
    int idx;

    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        if (gen_bishop_map_for_sq(err, bishop_atks, BISHOP_MAGICS, sq) != CIBYL_EOK) {
            result = CIBYL_ERR_ADD_CONTEXT(err);
            goto out_err;
        }
    }

out_err:
    return result;
}

/**
 * Generates a single rook lookup table.
 */
cibyl_errno_t gen_rook_map_for_sq(cibyl_error_t *err, uint64_t *table[64],
        const uint64_t magics[64], square_t sq)
{
    cibyl_errno_t result;
    uint64_t occupied_squares;
    uint64_t legal_moves;
    uint64_t occ;
    uint16_t key;
    int idx;

    for (idx = 0; idx < (1 << NUM_ROOK_BITS[sq.idx]); idx++) {
        /* Generate the rook occupancy mask. */
        occ = get_rook_occ_mask(sq);
        rook_occ_mask[sq.idx] = occ;

        /* Generate the relevant masks and key for the current index. */
        occupied_squares = map_index_to_occ_mask(idx, NUM_ROOK_BITS[sq.idx], occ);
        legal_moves = get_rook_atk_mask(sq, occupied_squares);
        key = get_rook_key(magics, sq, occupied_squares);

        /* Catch any evil hash collisions. */
        if (table[sq.idx][key] != 0 && table[sq.idx][key] != legal_moves) {
            result = CIBYL_MKERR(err, CIBYL_EINVAL,
                    "hash collision in rook table: sq=%d idx=%d", sq.idx, idx);
            goto out_err;
        }

        /* Write data into the table. */
        table[sq.idx][key] = legal_moves;
    }

out_err:
    return result;
}

/**
 * Generates the array of rook lookup tables.
 */
cibyl_errno_t gen_rook_table(cibyl_error_t *err)
{
    cibyl_errno_t result = CIBYL_EOK;
    square_t sq;
    int idx;

    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        if (gen_rook_map_for_sq(err, rook_atks, ROOK_MAGICS, sq) != CIBYL_EOK) {
            result = CIBYL_ERR_ADD_CONTEXT(err);
            goto out_err;
        }
    }

out_err:
    return result;
}

cibyl_errno_t cb_init_magic_tables(cibyl_error_t *err)
{
    int result;

    if ((result = alloc_magic_table(err, rook_atks, NUM_ROOK_BITS)) != CIBYL_EOK) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    if ((result = gen_rook_table(err)) != CIBYL_EOK) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out_free_rook;
    }

    if ((result = alloc_magic_table(err, bishop_atks, NUM_BISHOP_BITS)) != CIBYL_EOK) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out_free_rook;
    }

    if ((result = gen_bishop_table(err)) != CIBYL_EOK) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out_free_bishop;
    }

    goto out;

out_free_bishop:
    free_magic_table(bishop_atks);
out_free_rook:
    free_magic_table(rook_atks);
out:
    return result;
}

void cb_free_magic_tables()
{
    free_magic_table(bishop_atks);
    free_magic_table(rook_atks);
}

uint64_t rand_u64()
{
    uint64_t high = rand();
    uint64_t low = rand();
    return (high << 32) | low;
}

uint64_t rand_u64_few_bits()
{
    return rand_u64() | rand_u64() | rand_u64();
}

/**
 * Generate the array of rook magics. This is run once by a helper program and the resulting
 * magics are inserted into this file manually in the array above.
 */
cibyl_errno_t cb_gen_rook_magics(cibyl_error_t *err, uint64_t magics[64])
{
    cibyl_errno_t result = CIBYL_EINVAL;
    uint64_t *atks[64];
    square_t sq;
    int i = 0;

    /* Allocate space for the attack table. */
    if (alloc_magic_table(err, atks, NUM_ROOK_BITS)) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Generate random magics until one works. */
    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        do {
            magics[sq.idx] = rand_u64_few_bits();
            if ((result = gen_rook_map_for_sq(err, atks, magics, sq)) != CIBYL_EOK)
                goto out_err;
            i++;
        } while (i < 10000000);
    }
    
    /* Check the result. */
out_err:
    if (result != CIBYL_EOK) {
        result = CIBYL_MKERR(err, CIBYL_EABORT,
                "failed to generate rook magics: %s", err->desc);
        goto out_free;
    }

out_free:
    free_magic_table(atks);
out:
    return result;
}

/**
 * Generate the array of bishop magics. This is run once by a helper program and the resulting
 * magics are inserted into this file manually in the array above.
 */
cibyl_errno_t cb_gen_bishop_magics(cibyl_error_t *err, uint64_t magics[64])
{
    cibyl_errno_t result = CIBYL_EINVAL;
    uint64_t *atks[64];
    square_t sq;
    int i = 0;

    /* Allocate space for the attack table. */
    if (alloc_magic_table(err, atks, NUM_BISHOP_BITS)) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Generate random magics until one works. */
    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        while (gen_bishop_map_for_sq(err, atks, magics, sq) == CIBYL_EINVAL && i++ < 10000000);
    }
    
    /* Check the result. */
    if (result != CIBYL_EOK) {
        result = CIBYL_MKERR(err, CIBYL_EABORT,
                "failed to generate bishop magics: %s", err->desc);
        goto out_free;
    }

out_free:
    free_magic_table(atks);
out:
    return result;
}

/**
 * Print an array of magics as hex literals. Once again, this is run once by the helper program.
 */
void cb_print_magics(const uint64_t magics[64])
{
    for (int i = 0; i < 64; i++) {
        printf("0x%" PRIx64 "\n", magics[i]);
    }
}
