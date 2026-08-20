
#include "cb/cb.h"
#include "cb/debug.h"
#include "cb/types.h"
#include "cb/move.h"
#include "cb/boardrep.h"
#include "cb/history.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINT_BUF_LEN 1024

/* Globals for managing a memstream for GDB debug printers. */
char *gdb_output_buffer;
size_t gdb_output_size;
FILE *gdb_output_stream;

/* GDB debug print helper functions. */
FILE *gdb_get_output_stream()
{
    if (!gdb_output_stream)
        gdb_output_stream = open_memstream(&gdb_output_buffer, &gdb_output_size);
    rewind(gdb_output_stream);
    return gdb_output_stream;
}

char *gdb_get_output_buffer()
{
    fflush(gdb_output_stream);
    return gdb_output_buffer;
}

void cb_print_mv_hist(FILE *f, cb_hist_stack_t *hist)
{
    int i = 0;
    cb_move_t mv;
    char buf[PRINT_BUF_LEN];

    /* First move isn't valid. */
    for (i = 1; i < (hist->count - 1); i++) {
        cb_mv_to_uci_algbr(buf, hist->data[i].move);
        fprintf(f, "%s -> ", buf);
    }
    cb_mv_to_uci_algbr(buf, hist->data[i].move);
    fprintf(f, "%s\n", buf);
}

void cb_print_board_ascii(FILE *f, cb_board_t *board)
{
    const char SEPERATOR[] = " +---+---+---+---+---+---+---+---+";
    const char PIECE_SEP[] = " | ";
    const char FILE_LINE[] = "   A   B   C   D   E   F   G   H";

    int row, col;
    char str_rep[8][8];

    cb_board_to_str(str_rep, board);
    fprintf(f, "%s\n", SEPERATOR);
    for (row = 7; row >= 0; row--) {
        fprintf(f, "%s", PIECE_SEP);
        for (col = 0; col < 8; col++) {
            fprintf(f, "%c%s", str_rep[row][col], PIECE_SEP);
        }
        fprintf(f, "%d\n", row + 1);
        fprintf(f, "%s\n", SEPERATOR);
    }
    fprintf(f, "%s\n", FILE_LINE);
}

void cb_print_board_utf8(FILE *f, cb_board_t *board)
{

}

void prep_bb_byte(char *buf, bitboard_t bb, bitboard_t rank) {
    sprintf(buf, "%s %s %s %s %s %s %s %s" ANSI_COLOR_RESET,
            (bb & (UINT64_C(1) << (rank * 8 + 0))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 1))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 2))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 3))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 4))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 5))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 6))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0",
            (bb & (UINT64_C(1) << (rank * 8 + 7))) ? ANSI_COLOR_GREEN "1" : ANSI_COLOR_RED "0");
}

void cb_print_bitboard(FILE *f, bitboard_t bitboard)
{
    char byte[PRINT_BUF_LEN];
    int i;

    for (i = 7; i >= 0; i--) {
        prep_bb_byte(byte, bitboard, i);
        fprintf(f, "%s\n", byte);
    }
    fprintf(f, "\n");
}

void cb_print_piece_bitboards(FILE *f, cb_piece_bitboards_t *bb)
{
    const char *wheaders[] = { "WHITE", "PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING", "OCC" };
    const char *bheaders[] = { "BLACK", "PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING" };
    int i, j;
    char byte[PRINT_BUF_LEN];

    /* Print white pieces. */
    fprintf(f, "\n");
    for (i = 0; i < 8; i++)
        fprintf(f, "%-17s", wheaders[i]);
    fprintf(f, "\n");
    for (i = 0; i < 8; i++)
        fprintf(f, "===============  ");
    fprintf(f, "\n");
    for (i = 7; i >= 0; i--) {
        prep_bb_byte(byte, bb->color[1], i);
        fprintf(f, "%s  ", byte);
        for (j = 0; j < 6; j++) {
            prep_bb_byte(byte, bb->piece[1][j], i);
            fprintf(f, "%s  ", byte);
        }
        prep_bb_byte(byte, bb->occ, i);
        fprintf(f, "%s  ", byte);
        fprintf(f, "\n");
    }

    /* Print black pieces. */
    fprintf(f, "\n");
    for (i = 0; i < 7; i++)
        fprintf(f, "%-17s", bheaders[i]);
    fprintf(f, "\n");
    for (i = 0; i < 7; i++)
        fprintf(f, "===============  ");
    fprintf(f, "\n");
    for (i = 7; i >= 0; i--) {
        prep_bb_byte(byte, bb->color[0], i);
        fprintf(f, "%s  ", byte);
        for (j = 0; j < 6; j++) {
            prep_bb_byte(byte, bb->piece[0][j], i);
            fprintf(f, "%s  ", byte);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

void cb_print_state(FILE *f, cb_board_t *board)
{
    const char *headers[] = { "THREATS", "CHECKS", "CHECK_BLOCKS" };
    int i, j;
    char byte[PRINT_BUF_LEN];

    /* Print pins. */
    fprintf(f, "\nPINS\n");
    for (i = 0; i < 9; i++)
        fprintf(f, "===============  ");
    fprintf(f, "\n");
    for (i = 7; i >= 0; i--) {
        for (j = 0; j < 9; j++) {
            prep_bb_byte(byte, board->pins[j], i);
            fprintf(f, "%s  ", byte);
        }
        fprintf(f, "\n");
    }

    /* Print other tables. */
    fprintf(f, "\n");
    for (i = 0; i < 3; i++)
        fprintf(f, "%-17s", headers[i]);
    fprintf(f, "\n");
    for (i = 0; i < 3; i++)
        fprintf(f, "===============  ");
    fprintf(f, "\n");
    for (i = 7; i >= 0; i--) {
        prep_bb_byte(byte, board->threats, i);
        fprintf(f, "%s  ", byte);
        prep_bb_byte(byte, board->checks, i);
        fprintf(f, "%s  ", byte);
        prep_bb_byte(byte, board->check_blocks, i);
        fprintf(f, "%s  ", byte);
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
}

void cb_print_moves(FILE *f, cb_mvlst_t *mvlst)
{
    char buf[6];
    int i;

    for (i = 0; i < cb_mvlst_size(mvlst); i++) {
        cb_mv_to_uci_algbr(buf, cb_mvlst_at(mvlst, i));
        fprintf(f, "%s ", buf);
    }
    fprintf(f, "\n");
}

void cb_print_history(FILE *f, cb_history_t *hist)
{
    fprintf(f, "%d : ", cb_hist_get_halfmove_clk(*hist));

    if (cb_hist_enp_available(*hist)) {
        fprintf(f, "enp_col=%d : ", cb_hist_enp_col(*hist));
    } else {
        switch (cb_hist_get_captured_piece(hist)) {
            case CB_PTYPE_PAWN:
                fprintf(f, "cap_ptype=PAWN : ");
                break;
            case CB_PTYPE_KNIGHT:
                fprintf(f, "cap_ptype=KNIGHT : ");
                break;
            case CB_PTYPE_BISHOP:
                fprintf(f, "cap_ptype=BISHOP : ");
                break;
            case CB_PTYPE_ROOK:
                fprintf(f, "cap_ptype=ROOK : ");
                break;
            case CB_PTYPE_QUEEN:
                fprintf(f, "cap_ptype=QUEEN : ");
                break;
            case CB_PTYPE_KING:
                fprintf(f, "cap_ptype=KING : ");
                break;
            case CB_PTYPE_EMPTY:
                fprintf(f, "cap_ptype=NONE : ");
                break;
        }
    }

    if (cb_hist_has_ksc(*hist, CB_WHITE)) fprintf(f, "K");
    if (cb_hist_has_qsc(*hist, CB_WHITE)) fprintf(f, "Q");
    if (cb_hist_has_ksc(*hist, CB_BLACK)) fprintf(f, "k");
    if (cb_hist_has_qsc(*hist, CB_BLACK)) fprintf(f, "q");
    if (cb_hist_has_no_castling(*hist)) fprintf(f, "-");
    fprintf(f, "\n");
}
