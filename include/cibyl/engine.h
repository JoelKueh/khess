
#ifndef CYBIL_ENGINE_H
#define CYBIL_ENGINE_H

#include <threads.h>

#include "cibyl.h"

/* Transposition table that contains precalculated positions. */
typedef struct {

} ttable_t;

/* Defines struct for a thread that thinks. */
typedef struct {
    cb_board_t *root;
    ttable_t *ttable;
    thrd_t thread;
} thinker_t;

/* Defines an engine. */
typedef struct {
    cb_board_t pos;
    ttable_t ttable;
    thinker_t *thinkers;
    thrd_t mgr;
    int msg_pipe[2];
} engine_t;

/**
 * @breif A bag of parameters for an engine search.
 */
typedef struct {
    cb_mvlst_t searchmoves;
    bool ponder;
    int64_t wtime;
    int64_t btime;
    int64_t winc;
    int64_t binc;
    int64_t movestogo;
    int64_t depth;
    int64_t nodes;
    int64_t mate;
    int64_t movetime;
    bool infinite;
} go_param_t;

/**
 * @breif Clears a go_params struct.
 * @param params The struct to clear.
 */
static void clear_go_params(go_param_t *params) {
    cb_mvlst_clear(&params->searchmoves);
    params->ponder = false;
    params->wtime = -1;
    params->btime = -1;
    params->winc = -1;
    params->binc = -1;
    params->movestogo = -1;
    params->depth = -1;
    params->nodes = -1;
    params->mate = -1;
    params->movetime = -1;
    params->infinite = true;
}

/* Function to handle initialization and termination. */

/**
 * @brief Begins initializing the engine.
 * @param engine The engine to initialize.
 * @return An error code for any failed threading calls.
 */
cibyl_errno_t eng_begin_init(engine_t *eng);

/**
 * @breif Waits for engine initialization to be completed.
 * @param engine The engine that was to be initialized.
 */
cibyl_errno_t eng_await_isready(engine_t *eng);

/**
 * @breif Nicely frees all allocated memory and terminates threads.
 * @param engine The engine to cleanup.
 * @return An error code for any failed threading calls.
 */
cibyl_errno_t eng_cleanup(engine_t *eng);

/**
 * @breif Tells the engine that it is now playing a new game.
 * Dumps certain game-specific lookup tables.
 * @param engine The engine in question.
 */
void eng_newgame(engine_t *eng);

/**
 * @brief Initializes the position of the engine to a specific UCI fen string.
 * @param engine The engine in question.
 * @return An error code for any errors in initialization.
 */
cibyl_errno_t eng_set_ucifen(engine_t *eng, char *fen);

/* Functions to handle searching. */
void eng_notify_go(engine_t *eng, const go_param_t *opts);
void eng_notify_stop(engine_t *eng);

/* Returns best move information. */
cb_move_t eng_await_get_bestmove(engine_t *eng);
cb_move_t eng_await_get_pondermove(engine_t *eng);

/* Handles notifications that should be sent to the engine. */
void eng_notify_ponderhit(engine_t *eng);
void eng_notify_stop(engine_t *eng);

#endif /* CIBYL_ENGINE_H */
