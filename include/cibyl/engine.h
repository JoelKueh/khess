
#ifndef CYBIL_ENGINE_H
#define CYBIL_ENGINE_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#ifdef _WIN32
#incllude <namedpipeapi.h>
#endif

#include "cb_move.h"
#include "cibyl.h"

/**
 * @breif Transposition table that contains precalculated positions.
 */
typedef struct {

} ttable_t;

/**
 * @breif A bag of parameters for an engine search.
 */
typedef struct {
    cb_mvlst_t searchmoves; /**< A list of moves to search from the root position. */
    int64_t wtime;          /**< The amount of time that white has left. */
    int64_t btime;          /**< The amount of time that black has left. */
    int64_t winc;           /**< The time increment for white. */
    int64_t binc;           /**< The time increment for black. */
    int64_t movestogo;      /**< The number of moves to search. */
    int64_t depth;          /**< The depth of the search. */
    int64_t nodes;          /**< The number of nodes to search. */
    int64_t mate;           /**< Signifies this should be a mate search in mate moves. */
    int64_t movetime;       /**< The amount of time that the bot should think for. */
    bool ponder;            /**< Flag that states if this is a ponder search. */
    bool infinite;          /**< Flag that states if this search should be infinite. */
} go_param_t;

/**
 * @breif Defines struct for a thread that thinks. */
typedef struct {
    cb_board_t *root;       /**< The root position to think on. */
    ttable_t *ttable;       /**< The transposition table to add thoughts to. */
    pthread_t thread;          /**< The thread itself. */
} thinker_t;

/**
 * @breif Defines a pool of threads that funcitons as an engine.
 */
typedef struct {
    cb_board_t board;           /**< The current position. */
    ttable_t ttable;            /**< The transposition tables. */
    thinker_t *thinkers;        /**< The pool of thinkers. */
    pthread_t mgr;              /**< The manager thread. */
    go_param_t go_params;       /**< The parameters for any active search. */
    bool go_ready;              /**< True if thinkers should begin searching. */

    pthread_cond_t sync_cnd;    /**< A condition variable that handles thinker sync. */
    pthread_mutex_t sync_mtx;   /**< A mutex that handles thinker sync. */
    int rdy_thrds;              /**< Holds the number of threads that are ready to run. */
    atomic_bool exit_flag;      /**< Checked by the thinkers to see if they should shut down. */

#ifdef _WIN32
    PHANDLE h_msg_read;         /**< The read handle for the pipe on windows. */
    PHANDLE h_msg_write;        /**< THe write handle for the pipe on windows. */
#else
    int msg_pipe[2];            /**< A message pipe for result output. */
#endif
} engine_t;

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

/**
 * @brief Begins initializing the engine.
 * @param engine The engine to initialize.
 * @return An error code for any failed threading calls.
 */
kh_errno_t eng_begin_init(engine_t *eng);

/**
 * @breif Waits for engine initialization to be completed.
 * @param engine The engine that was to be initialized.
 */
kh_errno_t eng_await_isready(engine_t *eng);

/**
 * @breif Nicely frees all allocated memory and terminates threads.
 * @param engine The engine to cleanup.
 * @return An error code for any failed threading calls.
 */
kh_errno_t eng_cleanup(engine_t *eng);

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
kh_errno_t eng_set_ucifen(engine_t *eng, char *fen);

/**
 * @breif Notifies the engine that it should begin a search.
 * @param eng The engine to notify.
 * @param opts The options for the search.
 */
void eng_notify_go(engine_t *eng, const go_param_t *opts);

/**
 * @breif Notifies the engine that it should stop a search as soon as possible.
 *
 * Upon completing the search, the engine will send the bestmove and pondermove
 * out from its message pipe.
 *
 * @param eng The engine to notify.
 */
void eng_notify_stop(engine_t *eng);

/**
 * @breif Notifies the engine that the pondered move was played.
 *
 * The engine should switch from a ponder search to a normal search.
 *
 * @param eng THe engine to notify.
 */
void eng_notify_ponderhit(engine_t *eng);

#endif /* CIBYL_ENGINE_H */
