
#include <string.h>

#include "cb/debug.h"

#include "cb/cb.h"
#include "cb/boardrep.h"
#include "cb/tables.h"
#include "cb/const.h"
#include "cb/move.h"
#include "cb/bitutil.h"
#include "cb/history.h"

static inline bitboard_t pawn_smear(bitboard_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        (pawns << 7 & ~BB_RIGHT_COL) | (pawns << 9 & ~BB_LEFT_COL) :
        (pawns >> 9 & ~BB_RIGHT_COL) | (pawns >> 7 & ~BB_LEFT_COL);
}

static inline bitboard_t pawn_smear_left(bitboard_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        (pawns << 7 & ~BB_RIGHT_COL) :
        (pawns >> 7 & ~BB_LEFT_COL);
}

static inline bitboard_t pawn_smear_forward(bitboard_t pawns, cb_color_t color)
{
    return color == CB_WHITE ? pawns << 8 : pawns >> 8;
}

static inline bitboard_t pawn_smear_right(bitboard_t pawns, cb_color_t color)
{
    return color == CB_WHITE ?
        (pawns << 9 & ~BB_LEFT_COL) :
        (pawns >> 9 & ~BB_RIGHT_COL);
}

static inline void append_pushes(cb_mvlst_t *mvlst, cb_board_t *board, bitboard_t pushes)
{
    square_t target;
    square_t sq;

    while (pushes != 0) {
        target.idx = pop_rbit(&pushes);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -8 : 8);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_QUIET));
    }
}

static inline void append_doubles(cb_mvlst_t *mvlst, cb_board_t *board, bitboard_t doubles)
{
    square_t target;
    square_t sq;

    while (doubles != 0) {
        target.idx = pop_rbit(&doubles);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -16 : 16);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_DOUBLE_PAWN_PUSH));
    }
}

static inline void append_left_attacks(cb_mvlst_t *mvlst, cb_board_t *board,
                                       bitboard_t left_attacks)
{
    square_t target;
    square_t sq;

    while (left_attacks != 0) {
        target.idx = pop_rbit(&left_attacks);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -7 : 7);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_CAPTURE));
    }
}

static inline void append_right_attacks(cb_mvlst_t *mvlst, cb_board_t *board,
                                        bitboard_t right_attacks)
{
    square_t target;
    square_t sq;

    while (right_attacks != 0) {
        target.idx = pop_rbit(&right_attacks);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -9 : 9);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_CAPTURE));
    }
}

static inline void append_left_promos(cb_mvlst_t *mvlst, cb_board_t *board, bitboard_t left_promos)
{
    square_t target;
    square_t sq;

    while (left_promos != 0) {
        target.idx = pop_rbit(&left_promos);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -7 : 7);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_KNIGHT_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_BISHOP_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_ROOK_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_QUEEN_PROMO_CAPTURE));
    }
}

static inline void append_forward_promos(cb_mvlst_t *mvlst, cb_board_t *board,
                                         bitboard_t forward_promos)
{
    square_t target;
    square_t sq;

    while (forward_promos != 0) {
        target.idx = pop_rbit(&forward_promos);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -8 : 8);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_KNIGHT_PROMO));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_BISHOP_PROMO));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_ROOK_PROMO));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_QUEEN_PROMO));
    }
}

static inline void append_right_promos(cb_mvlst_t *mvlst, cb_board_t *board, bitboard_t right_promos)
{
    square_t target;
    square_t sq;

    while (right_promos != 0) {
        target.idx = pop_rbit(&right_promos);
        sq.idx = target.idx + (board->turn == CB_WHITE ? -9 : 9);
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_KNIGHT_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_BISHOP_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_ROOK_PROMO_CAPTURE));
        cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, CB_MV_QUEEN_PROMO_CAPTURE));
    }
}

void append_pawn_moves(cb_mvlst_t *mvlst, cb_board_t *board)
{
    /* Get the mask of pawns that we want to evaluate. */
    bitboard_t pawns = board->bb.piece[board->turn][CB_PTYPE_PAWN];

    /* Remove all of the pinned pawns and add back those that lie on a left ray. */
    bitboard_t left_pin_mask = board->pins[CB_DIR_DR] | board->pins[CB_DIR_UL];
    bitboard_t left_pawns = (pawns & ~board->pins[8]) | (pawns & left_pin_mask);

    /* Remove all of the pinned pawns and add back those that lie on a forward ray. */
    bitboard_t forward_pin_mask = board->pins[CB_DIR_D] | board->pins[CB_DIR_U];
    bitboard_t forward_pawns = (pawns & ~board->pins[8]) | (pawns & forward_pin_mask);

    /* Remove all of the pinned pawns and add back those that lie on a right ray. */
    bitboard_t right_pin_mask = board->pins[CB_DIR_DL] | board->pins[CB_DIR_UR];
    bitboard_t right_pawns = (pawns & ~board->pins[8]) | (pawns & right_pin_mask);

    /* Generate masks for pawns moving left and right. */
    bitboard_t left_smear = pawn_smear_left(left_pawns, board->turn);
    bitboard_t left_attacks = left_smear & board->bb.color[!board->turn];
    bitboard_t right_smear = pawn_smear_right(right_pawns, board->turn);
    bitboard_t right_attacks = right_smear & board->bb.color[!board->turn];

    /* Generate masks for pushing pawns. */
    bitboard_t forward_smear = pawn_smear_forward(forward_pawns, board->turn);
    bitboard_t forward_moves = forward_smear & ~board->bb.occ;

    /* Smear the forward moves again to get the double pushes. */
    bitboard_t double_smear = pawn_smear_forward(forward_moves, board->turn);
    bitboard_t double_moves = double_smear & ~board->bb.occ;
    double_moves &= board->turn == CB_WHITE ? BB_WHITE_PAWN_LINE : BB_BLACK_PAWN_LINE;

    /* Adjust for checks. */
    left_attacks &= board->check_blocks;
    right_attacks &= board->check_blocks;
    forward_moves &= board->check_blocks;
    double_moves &= board->check_blocks;

    /* Select the moves that cuase a promotion. */
    bitboard_t left_promos = left_attacks & (BB_TOP_ROW | BB_BOTTOM_ROW);
    left_attacks ^= left_promos;
    bitboard_t right_promos = right_attacks & (BB_TOP_ROW | BB_BOTTOM_ROW);
    right_attacks ^= right_promos;
    bitboard_t forward_promos = forward_moves & (BB_TOP_ROW | BB_BOTTOM_ROW);
    forward_moves ^= forward_promos;

    /* Turn the masks into moves. */
    append_pushes(mvlst, board, forward_moves);
    append_doubles(mvlst, board, double_moves);
    append_left_attacks(mvlst, board, left_attacks);
    append_right_attacks(mvlst, board, right_attacks);
    append_forward_promos(mvlst, board, forward_promos);
    append_left_promos(mvlst, board, left_promos);
    append_right_promos(mvlst, board, right_promos);
}

bitboard_t gen_pseudo_mv_mask(cb_ptype_t ptype, cb_color_t pcolor, square_t sq, bitboard_t occ)
{
    switch (ptype) {
        case CB_PTYPE_PAWN:
            return cb_read_pawn_atk_msk(sq, pcolor);
        case CB_PTYPE_KNIGHT:
            return cb_read_knight_atk_msk(sq);
        case CB_PTYPE_BISHOP:
            return cb_read_bishop_atk_msk(sq, occ);
        case CB_PTYPE_ROOK:
            return cb_read_rook_atk_msk(sq, occ);
        case CB_PTYPE_QUEEN:
            return cb_read_bishop_atk_msk(sq, occ)
                | cb_read_rook_atk_msk(sq, occ);
        case CB_PTYPE_KING:
            return cb_read_king_atk_msk(sq);
        case CB_PTYPE_EMPTY:
            assert(false && "invalid piece type for pseudo legal move generation");
            return 0;
    }
}

static inline bitboard_t pin_adjust(cb_board_t *board, square_t sq, bitboard_t moves)
{
    bitboard_t mask;
    square_t king_sq;
    cb_dir_t dir = cb_get_ray_direction(king_sq, sq);
    king_sq.idx = peek_rbit(board->bb.piece[board->turn][CB_PTYPE_KING]);
    return (board->pins[dir] & (UINT64_C(1) << sq.idx)) == 0 ? moves : (moves & board->pins[dir]);
}

bitboard_t cb_gen_legal_mv_mask(cb_board_t *board, square_t sq)
{
    /* Generate the pseudo moves. */
    cb_ptype_t ptype = cb_ptype_at(board, sq);
    cb_color_t pcolor = cb_color_at(board, sq);
    bitboard_t moves = gen_pseudo_mv_mask(ptype, pcolor, sq, board->bb.occ);
    moves &= ~board->bb.color[board->turn];

    /* Adjust moves for pins and checks. */
    moves &= ptype == CB_PTYPE_KING ? ~board->threats : board->check_blocks;
    moves = pin_adjust(board, sq, moves);

    return moves;
}

void append_simple_moves(cb_mvlst_t *mvlst, cb_board_t *board)
{
    square_t sq, target;
    cb_mv_flag_t flags;
    bitboard_t mvmsk;
    bitboard_t pieces = board->bb.color[board->turn];

    /* Append all of the moves to the list. */
    pieces ^= board->bb.piece[board->turn][CB_PTYPE_PAWN];
    while (pieces) {
        sq.idx = pop_rbit(&pieces);
        mvmsk = cb_gen_legal_mv_mask(board, sq);
        while (mvmsk) {
            target.idx = pop_rbit(&mvmsk);
            flags = (UINT64_C(1) << target.idx) & board->bb.occ ? CB_MV_CAPTURE : CB_MV_QUIET;
            cb_mvlst_push(mvlst, cb_mv_from_data(sq, target, flags));
        }
    }
}

static inline bool ksc_legal(cb_board_t *board)
{
    cb_history_t hist = board->hist.data[board->hist.count - 1].hist;
    bitboard_t occ_mask = board->turn == CB_WHITE ? BB_WHITE_KING_SIDE_CASTLE_OCCUPANCY :
        BB_BLACK_KING_SIDE_CASTLE_OCCUPANCY;
    bitboard_t check_mask = board->turn == CB_WHITE ? BB_WHITE_KING_SIDE_CASTLE_CHECK :
        BB_BLACK_KING_SIDE_CASTLE_CHECK;

    /* If the occupancy intersects occ_mask or the threats intersect ckeck_mask. No castling. */
    return ((board->bb.occ & occ_mask) | (board->threats & check_mask)) == 0
        && cb_hist_has_ksc(hist, board->turn);
}

static inline bool qsc_legal(cb_board_t *board)
{
    cb_history_t hist = board->hist.data[board->hist.count - 1].hist;
    bitboard_t occ_mask = board->turn == CB_WHITE ? BB_WHITE_QUEEN_SIDE_CASTLE_OCCUPANCY :
        BB_BLACK_QUEEN_SIDE_CASTLE_OCCUPANCY;
    bitboard_t check_mask = board->turn == CB_WHITE ? BB_WHITE_QUEEN_SIDE_CASTLE_CHECK :
        BB_BLACK_QUEEN_SIDE_CASTLE_CHECK;

    /* If the occupancy intersects occ_mask or the threats intersect ckeck_mask. No castling. */
    return ((board->bb.occ & occ_mask) | (board->threats & check_mask)) == 0
        && cb_hist_has_qsc(hist, board->turn);
}

void append_castle_moves(cb_mvlst_t *mvlst, cb_board_t *board)
{
    square_t from = board->turn == CB_WHITE ? M_WHITE_KING_START : M_BLACK_KING_START;
    square_t to;

    if (ksc_legal(board)) {
        to = board->turn == CB_WHITE ? M_WHITE_KING_SIDE_CASTLE_TARGET :
            M_BLACK_KING_SIDE_CASTLE_TARGET;
        cb_mvlst_push(mvlst, cb_mv_from_data(from, to, CB_MV_KING_SIDE_CASTLE));
    }

    if (qsc_legal(board)) {
        to = board->turn == CB_WHITE ? M_WHITE_QUEEN_SIDE_CASTLE_TARGET :
            M_BLACK_QUEEN_SIDE_CASTLE_TARGET;
        cb_mvlst_push(mvlst, cb_mv_from_data(from, to, CB_MV_QUEEN_SIDE_CASTLE));
    }
}

void append_enp_moves(cb_mvlst_t *mvlst, cb_board_t *board)
{
    /* Exit early if there is not availiable enpassant. */
    if (!cb_hist_enp_availiable(board->hist.data[board->hist.count - 1].hist))
        return;

    /* Get the swares relavent to the piece that can enpassant. */
    square_t enp_row_start, enp_sq, enemy_sq;
    cb_history_t hist = board->hist.data[board->hist.count - 1].hist;
    enp_row_start = board->turn == CB_WHITE ?
        M_BLACK_ENP_LINE_START : M_WHITE_ENP_LINE_START;
    enp_sq.idx = enp_row_start.idx + cb_hist_enp_col(hist);
    enemy_sq.idx = enp_sq.idx + (board->turn == CB_WHITE ? -8 : 8);

    /* Get all of the pieces that can enpassnt. */
    bitboard_t enp_sources = cb_read_pawn_atk_msk(enp_sq, !board->turn)
        & board->bb.piece[board->turn][CB_PTYPE_PAWN];

    /* Loop through the pieces that can enpassant and generate the moves. */
    square_t sq, king_sq;
    cb_move_t mv;
    bitboard_t new_occ, bishop_threats, rook_threats;
    while (enp_sources) {
        sq.idx = pop_rbit(&enp_sources);
        mv = cb_mv_from_data(sq, enp_sq, CB_MV_ENPASSANT);

        /* Update the occupancy mask to what it will be after the move takes place. */
        new_occ = board->bb.occ;
        new_occ &= ~(UINT64_C(1) << sq.idx);
        new_occ &= ~(UINT64_C(1) << enemy_sq.idx);
        new_occ |= UINT64_C(1) << enp_sq.idx;

        /* Check if the king is in check after the move is made.
         * This could be the case if some piece was pinned before the enpassant was made. */
        king_sq.idx = peek_rbit(board->bb.piece[board->turn][CB_PTYPE_KING]);

        bishop_threats = cb_read_bishop_atk_msk(king_sq, new_occ)
            & (board->bb.piece[!board->turn][CB_PTYPE_BISHOP]
                | board->bb.piece[!board->turn][CB_PTYPE_QUEEN]);
        if (bishop_threats) continue;

        rook_threats = cb_read_rook_atk_msk(king_sq, new_occ)
            & (board->bb.piece[!board->turn][CB_PTYPE_ROOK]
                | board->bb.piece[!board->turn][CB_PTYPE_QUEEN]);
        if (rook_threats) continue;

        /* Push the move if it doesn't cause any problems. */
        cb_mvlst_push(mvlst, mv);
    }
}

static inline bitboard_t gen_threats(cb_board_t *board)
{
    bitboard_t threats;
    square_t sq;
    bitboard_t pawns = board->bb.piece[!board->turn][CB_PTYPE_PAWN];
    bitboard_t king = board->bb.piece[board->turn][CB_PTYPE_KING];

    /* Generate all of the threats. */
    bitboard_t pieces = board->bb.color[!board->turn] ^ pawns;
    bitboard_t occ = board->bb.occ ^ king; /* Remove the king to allow pieces to "see through" it. */
    cb_ptype_t ptype;
    cb_color_t pcolor;

    /* Generate all threats. */
    threats = pawn_smear(pawns, !board->turn);
    while (pieces) {
        sq.idx = pop_rbit(&pieces);
        ptype = cb_ptype_at(board, sq);
        pcolor = cb_color_at(board, sq);
        threats |= gen_pseudo_mv_mask(ptype, pcolor, sq, occ);
    }

    return threats;
}

static inline bitboard_t gen_checks(cb_board_t *board, bitboard_t threats)
{
    square_t king_sq;
    bitboard_t *pieces = board->bb.piece[!board->turn];
    bitboard_t king = board->bb.piece[board->turn][CB_PTYPE_KING];
    bitboard_t occ = board->bb.occ;

    /* Exit early if the king isn't threatened. */
    if ((king & threats) == 0)
        return 0;

    /* Build the list of pieces that check the king. */
    king_sq.idx = peek_rbit(king);
    bitboard_t checks = cb_read_pawn_atk_msk(king_sq, board->turn) & pieces[CB_PTYPE_PAWN];
    checks |= cb_read_knight_atk_msk(king_sq) & pieces[CB_PTYPE_KNIGHT];
    checks |= cb_read_bishop_atk_msk(king_sq, occ)
        & (pieces[CB_PTYPE_BISHOP] | pieces[CB_PTYPE_QUEEN]);
    checks |= cb_read_rook_atk_msk(king_sq, occ)
        & (pieces[CB_PTYPE_ROOK] | pieces[CB_PTYPE_QUEEN]);
    /* Here's a helpful reminder that a king can never check another king. */

    return checks;
}

static inline bitboard_t gen_check_blocks(cb_board_t *board, bitboard_t checks)
{
    if (checks == 0)
        return BB_FULL;
    else if (popcnt(checks) != 1)
        return BB_EMPTY;

    square_t king_sq, check_sq;
    king_sq.idx = peek_rbit(board->bb.piece[board->turn][CB_PTYPE_KING]);
    check_sq.idx = peek_rbit(checks);
    return cb_read_tf_table(check_sq, king_sq) | (UINT64_C(1) << check_sq.idx);
}

static inline bitboard_t xray_bishop_attacks(bitboard_t occ, bitboard_t blockers, square_t sq)
{
    bitboard_t attacks = cb_read_bishop_atk_msk(sq, occ);
    blockers &= attacks;
    return attacks ^ cb_read_bishop_atk_msk(sq, occ ^ blockers);
}

static inline bitboard_t xray_rook_attacks(bitboard_t occ, bitboard_t blockers, square_t sq)
{
    bitboard_t attacks = cb_read_rook_atk_msk(sq, occ);
    blockers &= attacks;
    return attacks ^ cb_read_rook_atk_msk(sq, occ ^ blockers);
}

static inline void gen_pins(bitboard_t pins[10], cb_board_t *board)
{
    bitboard_t king = board->bb.piece[board->turn][CB_PTYPE_KING];
    bitboard_t occ = board->bb.occ;
    bitboard_t blockers = board->bb.color[board->turn];
    bitboard_t pinner;
    square_t king_sq, sq;
    cb_dir_t dir;

    /* Set all of the pins to full bitboards. */
    memset(pins, 0, 10 * sizeof(bitboard_t));

    /* Get all of the first pinners. */
    king_sq.idx = peek_rbit(king);
    pinner = xray_bishop_attacks(occ, blockers, king_sq)
        & (board->bb.piece[!board->turn][CB_PTYPE_BISHOP]
        | board->bb.piece[!board->turn][CB_PTYPE_QUEEN]);
    while (pinner) {
        sq.idx = pop_rbit(&pinner);
        dir = cb_get_ray_direction(king_sq, sq);
        pins[dir] = cb_read_tf_table(sq, king_sq);
        pins[8] ^= pins[dir];
    }

    /* Get all of the second pinners. */
    pinner = xray_rook_attacks(occ, blockers, king_sq)
        & (board->bb.piece[!board->turn][CB_PTYPE_ROOK]
        | board->bb.piece[!board->turn][CB_PTYPE_QUEEN]);
    while (pinner) {
        sq.idx = pop_rbit(&pinner);
        dir = cb_get_ray_direction(king_sq, sq);
        pins[dir] = cb_read_tf_table(sq, king_sq);
        pins[8] ^= pins[dir];
    }
}

void cb_gen_state_tables(cb_board_t *board)
{
    board->threats = gen_threats(board);
    board->checks = gen_checks(board, board->threats);
    board->check_blocks = gen_check_blocks(board, board->checks);
    gen_pins(board->pins, board);
}

void cb_gen_moves(cb_mvlst_t *mvlst, cb_board_t *board)
{
    cb_gen_state_tables(board);
    cb_mvlst_clear(mvlst);
    append_pawn_moves(mvlst, board);
    append_simple_moves(mvlst, board);
    append_castle_moves(mvlst, board);
    append_enp_moves(mvlst, board);
}
