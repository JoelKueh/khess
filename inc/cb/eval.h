/** Engine-facing API for position evaluation.
 *
 * Hand-Crafted Evaluation based on the following techniques.
 * 1. Incrementally Updated PeSTO Piece Square table evaluation.
 * 2. 
 */


#ifndef CYBIL_EVAL_H
#define CYBIL_EVAL_H

#include <stdbool.h>
#include "cb/types.h"

/**
 * @brief Incrementally updated evaluation data structure.
 */
typedef struct {
    uint32_t pesto_mg[2];    /**< The current PeSTO midgame eval (incrementally updated). */
    uint32_t pesto_eg[2];    /**< The current PeSTO endgame eval (incrementally updated). */
    uint32_t phase;          /**< The current PeSTO game phase (incrementally updated). */
} cb_incr_eval_t;

/**
 * @brief Initilalizes the tables that back an incremental evaluation structure.
 *
 * @param board The board to evaluate.
 * @return An evaluation struct that can be used to track position evaluation.
 */
cb_incr_eval_t eval_init(const cb_board_t *board);

/**
 * @brief Resets an incremental evaluation structure to the possition in board.
 *
 * @param eval The evaluation structure to update.
 * @param board The board to evaluate.
 */
void eval_reset(cb_incr_eval_t *eval, const cb_board_t *board);

/**
 * @brief Incrementally updates the evaluation in board by writing a piece to a square.
 * @param board The board to update.
 */
void eval_write_piece(cb_incr_eval_t *eval);

/**
 * @brief Incrementally updates the evaluation in board by deleting a piece on a square.
 * @param board The board to update.
 */
void eval_delete_piece(cb_incr_eval_t *eval);

/**
 * @brief Incrementally updates the evaluation in board by replacing a piece on a square.
 * @param board The board to update.
 */
void eval_replace_piece(cb_incr_eval_t *eval);

/**
 * @brief Returns the current evaluation of the board.
 * 
 * Preforms a mixture of evaluation techniques on board aided by the incremental evaluation
 * structure eval.
 *
 * @param eval The incremental evaluation structure to use.
 * @param board The board to evaluate.
 * @return The evaluation.
 */
int eval(const cb_incr_eval_t *eval, const cb_board_t *board);

/**
 * Applies ordering rules to the provided move list.
 * @param mvlst A pointer to the movelist to sort.
 * @param board The current board.
 * @param pvmv The principal variation move if this is a PV node, or else CB_INVALID_MV.
 */
void reorder_mvlst(cb_mvlst_t *mvlst, const cb_board_t *board, cb_move_t pvmv);

#endif /* CIBYL_EVAL_H */
