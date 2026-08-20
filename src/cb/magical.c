
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
    UINT64_C(0x0a8002c000108020),
    UINT64_C(0x4440200140003000),
    UINT64_C(0x8080200010011880),
    UINT64_C(0x0380180080141000),
    UINT64_C(0x1a00060008211044),
    UINT64_C(0x410001000a0c0008),
    UINT64_C(0x9500060004008100),
    UINT64_C(0x0100024284a20700),
    UINT64_C(0x0000802140008000),
    UINT64_C(0x0080c01002a00840),
    UINT64_C(0x0402004282011020),
    UINT64_C(0x9862000820420050),
    UINT64_C(0x0001001448011100),
    UINT64_C(0x6432800200800400),
    UINT64_C(0x040100010002000c),
    UINT64_C(0x0002800d0010c080),
    UINT64_C(0x90c0008000803042),
    UINT64_C(0x4010004000200041),
    UINT64_C(0x0003010010200040),
    UINT64_C(0x0a40828028001000),
    UINT64_C(0x0123010008000430),
    UINT64_C(0x0024008004020080),
    UINT64_C(0x0060040001104802),
    UINT64_C(0x00582200028400d1),
    UINT64_C(0x4000802080044000),
    UINT64_C(0x0408208200420308),
    UINT64_C(0x0610038080102000),
    UINT64_C(0x3601000900100020),
    UINT64_C(0x0000080080040180),
    UINT64_C(0x00c2020080040080),
    UINT64_C(0x0080084400100102),
    UINT64_C(0x4022408200014401),
    UINT64_C(0x0040052040800082),
    UINT64_C(0x0b08200280804000),
    UINT64_C(0x008a80a008801000),
    UINT64_C(0x4000480080801000),
    UINT64_C(0x0911808800801401),
    UINT64_C(0x822a003002001894),
    UINT64_C(0x401068091400108a),
    UINT64_C(0x000004a10a00004c),
    UINT64_C(0x2000800640008024),
    UINT64_C(0x1486408102020020),
    UINT64_C(0x000100a000d50041),
    UINT64_C(0x00810050020b0020),
    UINT64_C(0x0204000800808004),
    UINT64_C(0x00020048100a000c),
    UINT64_C(0x0112000831020004),
    UINT64_C(0x0009000040810002),
    UINT64_C(0x0440490200208200),
    UINT64_C(0x8910401000200040),
    UINT64_C(0x6404200050008480),
    UINT64_C(0x4b824a2010010100),
    UINT64_C(0x04080801810c0080),
    UINT64_C(0x00000400802a0080),
    UINT64_C(0x8224080110026400),
    UINT64_C(0x40002c4104088200),
    UINT64_C(0x01002100104a0282),
    UINT64_C(0x1208400811048021),
    UINT64_C(0x3201014a40d02001),
    UINT64_C(0x0005100019200501),
    UINT64_C(0x0101000208001005),
    UINT64_C(0x0002008450080702),
    UINT64_C(0x001002080301d00c),
    UINT64_C(0x410201ce5c030092)
};

const uint64_t BISHOP_MAGICS[64] = {
    UINT64_C(0x0440049104032280),
    UINT64_C(0x1021023c82008040),
    UINT64_C(0x0404040082000048),
    UINT64_C(0x48c4440084048090),
    UINT64_C(0x2801104026490000),
    UINT64_C(0x4100880442040800),
    UINT64_C(0x0181011002e06040),
    UINT64_C(0x9101004104200e00),
    UINT64_C(0x1240848848310401),
    UINT64_C(0x2000142828050024),
    UINT64_C(0x00001004024d5000),
    UINT64_C(0x0102044400800200),
    UINT64_C(0x8108108820112000),
    UINT64_C(0xa880818210c00046),
    UINT64_C(0x4008008801082000),
    UINT64_C(0x0060882404049400),
    UINT64_C(0x0104402004240810),
    UINT64_C(0x000a002084250200),
    UINT64_C(0x00100b0880801100),
    UINT64_C(0x0004080201220101),
    UINT64_C(0x0044008080a00000),
    UINT64_C(0x0000202200842000),
    UINT64_C(0x5006004882d00808),
    UINT64_C(0x0000200045080802),
    UINT64_C(0x0086100020200601),
    UINT64_C(0xa802080a20112c02),
    UINT64_C(0x0080411218080900),
    UINT64_C(0x000200a0880080a0),
    UINT64_C(0x9a01010000104000),
    UINT64_C(0x0028008003100080),
    UINT64_C(0x0211021004480417),
    UINT64_C(0x0401004188220806),
    UINT64_C(0x00825051400c2006),
    UINT64_C(0x00140c0210943000),
    UINT64_C(0x0000242800300080),
    UINT64_C(0x00c2208120080200),
    UINT64_C(0x2430008200002200),
    UINT64_C(0x1010100112008040),
    UINT64_C(0x8141050100020842),
    UINT64_C(0x0000822081014405),
    UINT64_C(0x800c049e40400804),
    UINT64_C(0x4a0404028a000820),
    UINT64_C(0x0022060201041200),
    UINT64_C(0x0360904200840801),
    UINT64_C(0x0881a08208800400),
    UINT64_C(0x0060202c00400420),
    UINT64_C(0x1204440086061400),
    UINT64_C(0x0008184042804040),
    UINT64_C(0x0064040315300400),
    UINT64_C(0x0c01008801090a00),
    UINT64_C(0x0808010401140c00),
    UINT64_C(0x04004830c2020040),
    UINT64_C(0x0080005002020054),
    UINT64_C(0x40000c14481a0490),
    UINT64_C(0x0010500101042048),
    UINT64_C(0x1010100200424000),
    UINT64_C(0x0000640901901040),
    UINT64_C(0x00000a0201014840),
    UINT64_C(0x00840082aa011002),
    UINT64_C(0x010010840084240a),
    UINT64_C(0x0420400810420608),
    UINT64_C(0x8d40230408102100),
    UINT64_C(0x4a00200612222409),
    UINT64_C(0x0a08520292120600)
};

const uint8_t MAX_BITS_IN_TABLE = 12;
const uint32_t MAX_TABLE_SIZE = 1 << MAX_BITS_IN_TABLE;

bitboard_t bishop_occ_mask[64];
bitboard_t rook_occ_mask[64];

bitboard_t *bishop_atks[64];
bitboard_t *rook_atks[64];

static inline uint16_t get_bishop_key(const uint64_t magics[64], square_t sq, bitboard_t occ)
{
    /* Hash the occupancy mask and compute the key. */
    occ &= bishop_occ_mask[sq.idx];
    occ *= magics[sq.idx];
    return occ >> (64 - NUM_BISHOP_BITS[sq.idx]);
}

static inline uint16_t get_rook_key(const uint64_t magics[64], square_t sq, bitboard_t occ)
{
    /* Hash the occupancy mask and compute the key. */
    occ &= rook_occ_mask[sq.idx];
    occ *= magics[sq.idx];
    return occ >> (64 - NUM_ROOK_BITS[sq.idx]);
}

/**
 * Returns the bishop attack mask given an occupancy set and a square.
 */
bitboard_t cb_read_bishop_atk_msk(square_t sq, bitboard_t occ)
{
    uint16_t key = get_bishop_key(BISHOP_MAGICS, sq, occ);
    return bishop_atks[sq.idx][key];
}

/**
 * Returns the rook attack mask given an occupancy set and a square.
 */
bitboard_t cb_read_rook_atk_msk(square_t sq, bitboard_t occ)
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
bitboard_t get_rook_occ_mask(square_t sq)
{
    bitboard_t result = 0;
    int8_t file;
    int8_t rank;

    rank = sq.rank + 1;
    file = sq.file;
    while (rank <= 6) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank += 1;
    }

    rank = sq.rank - 1;
    file = sq.file;
    while (rank >= 1) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank -= 1;
    }

    rank = sq.rank;
    file = sq.file + 1;
    while (file <= 6) {
        result |= UINT64_C(1) << (rank * 8 + file);
        file += 1;
    }

    rank = sq.rank;
    file = sq.file - 1;
    while (file >= 1) {
        result |= UINT64_C(1) << (rank * 8 + file);
        file -= 1;
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
bitboard_t get_bishop_occ_mask(square_t sq)
{
    bitboard_t result = 0;
    int8_t file;
    int8_t rank;

    rank = sq.rank + 1;
    file = sq.file + 1;
    while (rank <= 6 && file <= 6) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank += 1;
        file += 1;
    }

    rank = sq.rank + 1;
    file = sq.file - 1;
    while (rank <= 6 && file >= 1) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank += 1;
        file -= 1;
    }

    rank = sq.rank - 1;
    file = sq.file + 1;
    while (rank >= 1 && file <= 6) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank -= 1;
        file += 1;
    }

    rank = sq.rank - 1;
    file = sq.file - 1;
    while (rank >= 1 && file >= 1) {
        result |= UINT64_C(1) << (rank * 8 + file);
        rank -= 1;
        file -= 1;
    }

    return result;
}

/**
 * Returns the attack mask for a rook on a square with a given occupancy set.
 * This mask is much the same as the occupancy mask but you break out of the loop upon hitting
 * a piece in the occupancy mask.
 */
bitboard_t get_rook_atk_mask(square_t sq, bitboard_t occ)
{
    bitboard_t result = 0;
    int8_t file;
    int8_t rank;

    rank = sq.rank + 1;
    file = sq.file;
    while (rank <= 7) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank += 1;
    }

    rank = sq.rank - 1;
    file = sq.file;
    while (rank >= 0) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank -= 1;
    }

    rank = sq.rank;
    file = sq.file + 1;
    while (file <= 7) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        file += 1;
    }

    rank = sq.rank;
    file = sq.file - 1;
    while (file >= 0) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        file -= 1;
    }

    return result;
}

/**
 * Returns the attack mask for a rook on a square with a given occupancy set.
 * This mask is much the same as the occupancy mask but you break out of the loop upon hitting
 * a piece in the occupancy mask.
 */
bitboard_t get_bishop_atk_mask(square_t sq, bitboard_t occ)
{
    bitboard_t result = 0;
    int8_t file;
    int8_t rank;

    rank = sq.rank + 1;
    file = sq.file + 1;
    while (rank <= 7 && file <= 7) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank += 1;
        file += 1;
    }

    rank = sq.rank + 1;
    file = sq.file - 1;
    while (rank <= 7 && file >= 0) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank += 1;
        file -= 1;
    }

    rank = sq.rank - 1;
    file = sq.file + 1;
    while (rank >= 0 && file <= 7) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank -= 1;
        file += 1;
    }

    rank = sq.rank - 1;
    file = sq.file - 1;
    while (rank >= 0 && file >= 0) {
        result |= UINT64_C(1) << (rank * 8 + file);
        if (occ & (UINT64_C(1) << (rank * 8 + file)))
            break;
        rank -= 1;
        file -= 1;
    }

    return result;
}

/**
 * Allocates a magic table.
 */
cibyl_errno_t alloc_magic_table(cibyl_error_t *err, bitboard_t *table[64], const uint8_t bits[64])
{
    cibyl_errno_t result = CIBYL_EOK;
    square_t sq;

    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        if ((table[sq.idx] = calloc(1 << bits[sq.idx], sizeof(bitboard_t))) == 0) {
            result = CIBYL_MKERR(err, CIBYL_ENOMEM, "calloc: %s", strerror(errno));
            goto out_free_tables;
        };
    }
    goto out_success;

out_free_tables:
    /* Unwind the calloc stack. */
    while (sq.idx > 0) {
        free(table[--sq.idx]);
    }
out_success:
    return result;
}

/**
 * Free a magic table.
 */
void free_magic_table(bitboard_t *table[64])
{
    square_t sq;
    for (sq.idx = 0; sq.idx < 64; sq.idx++)
        free(table[sq.idx]);
}

/**
 * Maps an index to the bits of an occupancy mask.
 * Effectively allows you to map an n-bit number to an n-bit mask.
 */
bitboard_t map_index_to_occ_mask(uint16_t idx, uint8_t num_bits, bitboard_t occ_mask)
{
    bitboard_t result = 0;
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
cibyl_errno_t gen_bishop_map_for_sq(cibyl_error_t *err, bitboard_t *table[64],
        const uint64_t magics[64], square_t sq)
{
    cibyl_errno_t result = CIBYL_EOK;
    bitboard_t occupied_squares;
    bitboard_t legal_moves;
    bitboard_t occ;
    uint16_t key;
    int idx;

    /* Generate the bishop occupancy mask. */
    occ = get_bishop_occ_mask(sq);
    bishop_occ_mask[sq.idx] = occ;

    for (idx = 0; idx < (1 << NUM_BISHOP_BITS[sq.idx]); idx++) {
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
cibyl_errno_t gen_rook_map_for_sq(cibyl_error_t *err, bitboard_t *table[64],
        const uint64_t magics[64], square_t sq)
{
    cibyl_errno_t result = CIBYL_EOK;
    bitboard_t occupied_squares;
    bitboard_t legal_moves;
    bitboard_t occ;
    uint16_t key;
    int idx;

    /* Generate the rook occupancy mask. */
    occ = get_rook_occ_mask(sq);
    rook_occ_mask[sq.idx] = occ;

    for (idx = 0; idx < (1 << NUM_ROOK_BITS[sq.idx]); idx++) {

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
    uint64_t b1 = rand() & 0xFFFF;
    uint64_t b2 = rand() & 0xFFFF;
    uint64_t b3 = rand() & 0xFFFF;
    uint64_t b4 = rand() & 0xFFFF;
    return b1 | (b2 << 16) | (b3 << 32) | (b4 << 48);
}

uint64_t rand_u64_few_bits()
{
    return rand_u64() & rand_u64() & rand_u64();
}

/**
 * Generate the array of rook magics. This is run once by a helper program and the resulting
 * magics are inserted into this file manually in the array above.
 */
cibyl_errno_t cb_gen_rook_magics(cibyl_error_t *err, uint64_t magics[64])
{
    cibyl_errno_t result = CIBYL_EINVAL;
    bitboard_t *atks[64];
    square_t sq;
    int i = 0;

    /* Allocate space for the attack table. */
    if (alloc_magic_table(err, atks, NUM_ROOK_BITS)) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Generate random magics until one works. */
    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        for (i = 0; i < 10000000; i++) {
            memset(atks[sq.idx], 0, (1 << NUM_ROOK_BITS[sq.idx]) * sizeof(bitboard_t));
            magics[sq.idx] = rand_u64_few_bits();
            if ((result = gen_rook_map_for_sq(err, atks, magics, sq)) == CIBYL_EOK)
                break;
        }

        if (result != CIBYL_EOK)
            goto out_err;
    }
    
    /* Check the result. */
out_err:
    if (result != CIBYL_EOK) {
        result = CIBYL_MKERR(err, CIBYL_EABORT,
                "failed to generate rook magics for sq %d", sq.idx);
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
    bitboard_t *atks[64];
    square_t sq;
    int i = 0;

    /* Allocate space for the attack table. */
    if (alloc_magic_table(err, atks, NUM_BISHOP_BITS)) {
        result = CIBYL_ERR_ADD_CONTEXT(err);
        goto out;
    }

    /* Generate random magics until one works. */
    for (sq.idx = 0; sq.idx < 64; sq.idx++) {
        do {
            memset(atks[sq.idx], 0, (1 << NUM_BISHOP_BITS[sq.idx]) * sizeof(bitboard_t));
            magics[sq.idx] = rand_u64_few_bits();
            if ((result = gen_bishop_map_for_sq(err, atks, magics, sq)) == CIBYL_EOK)
                break;
            i++;
        } while (i < 10000000);

        if (result != CIBYL_EOK)
            goto out_err;
    }
    
    /* Check the result. */
out_err:
    if (result != CIBYL_EOK) {
        result = CIBYL_MKERR(err, CIBYL_EABORT,
                "failed to generate bishop magics for sq %d", sq.idx);
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
        printf("0x%016" PRIx64 "\n", magics[i]);
    }
}
