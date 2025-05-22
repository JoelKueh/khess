
#ifdef _WIN32
#define WIN_PIPE_SIZE 4096
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <event2/event.h>

#include "engine.h"
#include "eval.h"

#define THINKER_POOL_COUNT 10

/**
 * @breif Writes a message to the engines output pipe.
 * 
 * Called by a thinker thread to send a message to some manager thread.
 *
 * @param eng The engine.
 * @param msg The message to send.
 */
void eng_msg_out(engine_t *eng, char *msg)
{
#ifdef _WIN32
    /* TODO: Implement me on win32. */
#else
    
#endif
}

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit. This function has the added
 * requirement that it must release the mutex before exiting.
 *
 * @param eng A printf compliant format string.
 * @param fmt A printf compliant format string.
 * @param ... All remaining arguments passed into the format string.
 */
void thinker_unlock_write_err(engine_t *eng, char *fmt, ...)
{
    va_list args;
    atomic_store(&eng->exit_flag, true);
    pthread_mutex_unlock(&eng->sync_mtx);
    va_start(args, fmt);
    kh_vwrite_log(fmt, args);
    va_end(args);
    pthread_exit((void *)1);
}

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit.
 *
 * @param eng The engine that the thinker belongs to.
 * @param fmt A printf compliant format string.
 * @param ... All remaining arguments passed into the format string.
 */
void thinker_write_err(engine_t *eng, char *fmt, ...)
{
    va_list args;
    atomic_store(&eng->exit_flag, true);
    va_start(args, fmt);
    kh_vwrite_log(fmt, args);
    va_end(args);
    pthread_exit((void *)1);
}

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit.
 *
 * @param eng The engine that the thinker belongs to.
 * @param prefix The prefix string (followed by ": ")
 * @param err_code The error code.
 */
void thinker_unlock_perror(engine_t *eng, char *prefix, int err_code)
{
    atomic_store(&eng->exit_flag, true);
    pthread_mutex_unlock(&eng->sync_mtx);
    kh_perror(prefix, err_code);
    pthread_exit((void *)1);
}

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit.
 *
 * @param eng The engine that the thinker belongs to.
 * @param prefix The prefix string (followed by ": ")
 * @param err_code The error code.
 */
void thinker_perror(engine_t *eng, char *prefix, int err_code)
{
    atomic_store(&eng->exit_flag, true);
    kh_perror(prefix, err_code);
    pthread_exit((void *)1);
}

void *thinker_entry(void *eng_addr)
{
    int result;
    engine_t *eng = (engine_t *)eng_addr;
    ttable_t *ttable = &eng->ttable;
    cb_board_t board;

    /* Each thinker can attempt to initialize the tables. */
    if (cb_tables_init())


    while (true) {
        if ((result = pthread_mutex_lock(&eng->sync_mtx)) != 0)
            thinker_perror(eng, "thinker: pthread_mutex_lock", result);

        /* Synchonize on the condition and wait for a signal from the manager thread. */
        eng->waiting_threads++;
        while (!eng->go_ready || atomic_load(&eng->exit_flag)) {
            if ((result = pthread_cond_wait(&eng->sync_cnd, &eng->sync_mtx)) != 0)
                thinker_perror(eng, "thinker: pthread_cond_wait", result);
        }
        eng->waiting_threads--;

        /* Exit if we have encountered the exit flag. */
        if (atomic_load(&eng->exit_flag))
            break;
        
        /* Release the lock. */
        if ((result = pthread_mutex_unlock(&eng->sync_mtx)) != 0)
            thinker_perror(eng, "thinker: pthread_mutex_lock", result);

        /* Copy the board into the thinker. */
        board = eng->board;

        /* Start searching. */
        alphabeta(&board, eng->go_params.depth);
    }

    return (void *)KH_EOK;
}

/**
 * @breif The entrypoint for any engine manager.
 *
 * This is where the majority of initialization work for initilization is done.
 *
 * @param eng_addr A void pointer to the engine struct.
 */
void *mgr_entry(void *eng_addr)
{
    int64_t result;
    int64_t retval = 0;
    engine_t *eng = (engine_t *)eng_addr;
    int i;

    /* Spawn all of the thinkers. */
    for (i = 0; i < THINKER_POOL_COUNT; i++) {
        if ((result = pthread_create(&eng->mgr, NULL, thinker_entry, eng_addr)) != 0) {
            retval = result;
            kh_perror("manager: pthread_join", result);
            goto err_close_thrds;
        }
    }
    goto out;

err_close_thrds:
    /* Close the thinker threads. */
    for (; i > 0; i--) {
        result = pthread_join(eng->thinkers[i], NULL);

        /* Only log the first error we encounter. */
        if (result & !retval) {
            retval = result;
            kh_perror("manager: pthread_join", result);
        }
    }
out:
    return (void *)retval;
}

#ifdef _WIN32

kh_errno_t eng_begin_init(engine_t *eng)
{
    kh_errno_t result = KH_EOK;

    /* Create the message pipe. */
    if (CreatePipe(&eng->h_msg_read, &hWritePipe->h_msg_write, NULL, WIN_PIPE_SIZE) {
        kh_write_log("CreatePipe: %s\n", _strerror(NULL));
        result = KH_EABORT;
        goto out;
    }

    /* Spawn the manager thread and have it complete engine initialization. */
    if (thrd_create(&eng->mgr, mgr_entry, (void *)eng)) {
        kh_perror("thrd_create: %s\n", errno);
        result = KH_EABORT;
        goto err_free_pipe;
    }
    goto out;

err_free_pipe:
    /* Close the handles to the pipe on error. */
    CloseHandle(eng->h_msg_read)):
    CloseHandle(eng->h_msg_write));
out:
    return result;
}

#else

cb_errno_t eng_begin_init(engine_t *eng)
{
    kh_errno_t result = KH_EOK;

    /* Create the message pipe. */
    if (pipe(eng->msg_pipe) == -1) {
        kh_perror("pipe: %s\n", errno);
        result = KH_EABORT;
        goto out;
    }

    /* Spawn the manager thread and have it complete engine initialization. */
    if (pthrea(&eng->mgr, mgr_entry, (void *)eng)) {
        kh_perror("thrd_create: %s\n", errno);
        result = KH_EABORT;
        goto err_free_pipe;
    }
    goto out;

err_free_pipe:
    /* Close the handles to the pipe on error. */
    close(eng->msg_pipe[0]);
    close(eng->msg_pipe[1]);
out:
    return result;
}


#endif
