
#ifndef CB_CONST
#define CB_CONST

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ENEMY_COLOR(c) (!c)

#define CB_MAX_NUM_MOVES 218
#define CB_ERROR_STRLEN 128

/* Error codes for different operations that can take place. */
/* FIXME: Error reporting sucks atm. Errors are handled, but most things just throw CB_EINVAL. */
typedef enum {
    /* Non-recoverable errors. Usually due to a failed system call. */
    CB_ENOMEM = -2,
    CB_EABORT = -1,

    /* This is okay, default return value for any function that can err. */
    CB_EOK = 0,

    /* Other errors that imply some function input was incorrect. */
    CB_EINVAL, /* Invalid format specified. */
    CB_EILLEGAL, /* Illegal move specified. */
} cb_errno_t;

typedef struct {
    cb_errno_t num;
    char desc[CB_ERROR_STRLEN];
} cb_error_t;

typedef enum {
    CB_WHITE = 1,
    CB_BLACK = 0
} cb_color_t;

typedef enum  {
    CB_PID_EMPTY = 0b0000,

    CB_PID_WHITE_PAWN   = 0b0001,
    CB_PID_WHITE_KNIGHT = 0b0010,
    CB_PID_WHITE_BISHOP = 0b0011,
    CB_PID_WHITE_ROOK   = 0b0100,
    CB_PID_WHITE_QUEEN  = 0b0101,
    CB_PID_WHITE_KING   = 0b0110,

    CB_PID_BLACK_PAWN   = 0b1001,
    CB_PID_BLACK_KNIGHT = 0b1010,
    CB_PID_BLACK_BISHOP = 0b1011,
    CB_PID_BLACK_ROOK   = 0b1100,
    CB_PID_BLACK_QUEEN  = 0b1101,
    CB_PID_BLACK_KING   = 0b1110
} cb_pid_t;

typedef enum {
    CB_PTYPE_PAWN   = 0,
    CB_PTYPE_KNIGHT = 1,
    CB_PTYPE_BISHOP = 2,
    CB_PTYPE_ROOK   = 3,
    CB_PTYPE_QUEEN  = 4,
    CB_PTYPE_KING   = 5,
    CB_PTYPE_EMPTY  = 6
} cb_ptype_t;

typedef uint16_t cb_move_t;
typedef struct {
    cb_move_t moves[CB_MAX_NUM_MOVES];
    uint8_t head;
} cb_mvlst_t;

typedef uint16_t cb_history_t;
typedef struct {
    cb_history_t hist;
    cb_move_t move;
} cb_hist_ele_t;
typedef struct {
    cb_hist_ele_t *data;
    int count;
    int size;
} cb_hist_stack_t;

typedef struct {
    uint64_t threats;
    uint64_t checks;
    uint64_t check_blocks;
    uint64_t pins[10];
} cb_state_tables_t;

typedef struct {
    uint64_t color[2];
    uint64_t piece[2][6];
    uint64_t occ;
} cb_bitboard_t;

typedef struct {
    uint8_t data[64]; /* This is an array of cb_ptype_t but it is stored as a uint8_t. */
} cb_mailbox_t;

typedef struct {
    cb_bitboard_t bb;
    cb_mailbox_t mb;
    cb_hist_stack_t hist;
    uint8_t turn;
    uint32_t fullmove_num;
} cb_board_t;

extern const uint16_t HIST_INIT_BOARD_STATE;
extern const uint16_t HIST_ENP_COL;
extern const uint16_t HIST_PID_COL;
extern const uint16_t HIST_ENP_AVAILABLE;
extern const uint16_t HIST_ENP_ALL;
extern const uint16_t HIST_HALFMOVE_CLOCK;
extern const uint16_t HIST_HALFMOVE_FIFTY;

extern const uint8_t M_WHITE_KING_START;
extern const uint8_t M_WHITE_KING_SIDE_ROOK_START;
extern const uint8_t M_WHITE_QUEEN_SIDE_ROOK_START;
extern const uint8_t M_WHITE_KING_SIDE_CASTLE_TARGET;
extern const uint8_t M_WHITE_KING_SIDE_ROOK_TARGET;
extern const uint8_t M_WHITE_QUEEN_SIDE_CASTLE_TARGET;
extern const uint8_t M_WHITE_QUEEN_SIDE_ROOK_TARGET;

extern const uint8_t M_BLACK_KING_START;
extern const uint8_t M_BLACK_KING_SIDE_ROOK_START;
extern const uint8_t M_BLACK_QUEEN_SIDE_ROOK_START;
extern const uint8_t M_BLACK_KING_SIDE_CASTLE_TARGET;
extern const uint8_t M_BLACK_KING_SIDE_ROOK_TARGET;
extern const uint8_t M_BLACK_QUEEN_SIDE_CASTLE_TARGET;
extern const uint8_t M_BLACK_QUEEN_SIDE_ROOK_TARGET;

extern const uint8_t M_WHITE_MIN_ENPASSANT_TARGET;
extern const uint8_t M_BLACK_MIN_ENPASSANT_TARGET;

extern const uint64_t BB_RIGHT_COL;
extern const uint64_t BB_LEFT_COL;
extern const uint64_t BB_RIGHT_TWO_COLS;
extern const uint64_t BB_LEFT_TWO_COLS;
extern const uint64_t BB_TOP_ROW;
extern const uint64_t BB_BOTTOM_ROW;
extern const uint64_t BB_FULL;
extern const uint64_t BB_EMPTY;
extern const uint64_t BB_BLACK_PAWN_HOME;
extern const uint64_t BB_WHITE_PAWN_HOME;
extern const uint64_t BB_BLACK_PAWN_LINE;
extern const uint64_t BB_WHITE_PAWN_LINE;

extern const uint64_t BB_WHITE_KING_SIDE_CASTLE_TARGET;
extern const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_TARGET;

extern const uint64_t BB_WHITE_KING_SIDE_CASTLE_OCCUPANCY;
extern const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_OCCUPANCY;
extern const uint64_t BB_WHITE_KING_SIDE_CASTLE_CHECK;
extern const uint64_t BB_WHITE_QUEEN_SIDE_CASTLE_CHECK;
 
extern const uint64_t BB_BLACK_KING_SIDE_CASTLE_TARGET;
extern const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_TARGET;
 
extern const uint64_t BB_BLACK_KING_SIDE_CASTLE_OCCUPANCY;
extern const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_OCCUPANCY;
extern const uint64_t BB_BLACK_KING_SIDE_CASTLE_CHECK;
extern const uint64_t BB_BLACK_QUEEN_SIDE_CASTLE_CHECK;

extern const uint16_t CB_MV_TO_MASK;
extern const uint16_t CB_MV_FROM_MASK;
extern const uint16_t CB_MV_FLAG_MASK;

extern const cb_move_t CB_INVALID_MOVE;
extern const cb_hist_ele_t CB_INIT_STATE;

static inline uint8_t peek_rbit(uint64_t bb)
{
#if __has_builtin (__builtin_ctzl)
    return __builtin_ctzl(bb);
#else
#   error __builtin_ctzl not supported!
#endif
}

static inline uint8_t pop_rbit(uint64_t *bb)
{
    uint8_t idx = peek_rbit(*bb);
    *bb ^= UINT64_C(1) << idx;
    return idx;
}

static inline uint8_t popcnt(uint64_t bb)
{
#if __has_builtin (__builtin_popcountl)
    return __builtin_popcountl(bb);
#else
#   error __builtin_popcountl not supported!
#endif
}

cb_errno_t cb_mkerr(cb_error_t *err, cb_errno_t cb_errno, char *format, ...);

#endif /* CB_CONST */
