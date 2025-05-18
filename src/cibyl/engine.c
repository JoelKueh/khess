
#ifdef _WIN32
#define WIN_PIPE_SIZE 4096
#include <io.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "engine.h"

#define THINKER_POOL_COUNT 10

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit. This function has the added
 * requirement that it must release the mutex before exiting.
 *
 * @param eng A printf compliant format string.
 * @param format A printf compliant format string.
 * @param ... All remaining arguments passed into the format string.
 */
int thinker_unlock_handle_err(engine_t *eng, char *format, ...)
{
    va_list args;
    atomic_store(&eng->exit_flag, true);
    mtx_unlock(&eng->sync_mtx);
    va_start(args, format);
    cibyl_vwrite_log(format, args);
    va_end(args);
    thrd_exit(1);
}

/**
 * @breif Handles errors in a thinker thread.
 *
 * If a thinker encounters an error, it must set the atomic termination flag,
 * log the information to the console, and exit.
 *
 * @param eng The engine that the thinker belongs to.
 * @param format A printf compliant format string.
 * @param ... All remaining arguments passed into the format string.
 */
int thinker_handle_err(engine_t *eng, char *format, ...)
{
    va_list args;
    atomic_store(&eng->exit_flag, true);
    va_start(args, format);
    cibyl_vwrite_log(format, args);
    va_end(args);
    thrd_exit(1);
}

int thinker_entry(void *eng_addr)
{
    int result;
    engine_t *eng = (engine_t *)eng_addr;

    while (true) {
        if ((result = mtx_lock(&eng->sync_mtx)))
            thinker_handle_err(eng, "thinker: mtx_lock: %s\n");
    }

    return CIBYL_EOK;
}

/**
 * @breif The entrypoint for any engine manager.
 *
 * This is where the majority of initialization work for initilization is done.
 *
 * @param eng_addr A void pointer to the engine struct.
 */
int mgr_entry(void *eng_addr)
{
    engine_t *eng = (engine_t *)eng_addr;
    int i;

    /* Spawn all of the thinkers. */
    for (i = 0; i < THINKER_POOL_COUNT; i++) {
        if (thrd_create(&eng->mgr, thinker_entry, eng_addr))
            goto err_close_thrds;
    }

err_close_thrds:
    for (; i > 0; i--) {
    }

    return CIBYL_EOK;
}

cibyl_errno_t eng_begin_init(engine_t *eng)
{
    cibyl_errno_t result = CIBYL_EOK;

    /* Create the message pipe. */
#ifdef _WIN32
    if (CreatePipe(&eng->h_msg_read, &hWritePipe->h_msg_write, NULL, WIN_PIPE_SIZE) {
        cibyl_write_log("CreatePipe: %s\n", _strerror(NULL));
        result = CIBYL_EABORT;
        goto out;
    }
#else
    if (pipe(eng->msg_pipe) == -1) {
        cibyl_write_log("pipe: %s\n", strerror(errno));
        result = CIBYL_EABORT;
        goto out;
    }
#endif

    /* Spawn the manager thread and have it complete engine initialization. */
    if (thrd_create(&eng->mgr, mgr_entry, (void *)eng)) {
        cibyl_write_log("thrd_create: %s\n", strerror(errno));
        result = CIBYL_EABORT;
        goto err_free_pipe;
    }
    goto out;

err_free_pipe:
    
    /* Close the handles to the pipe on error. */
#ifdef _WIN32
    CloseHandle(eng->h_msg_read)):
    CloseHandle(eng->h_msg_write));
#else
    close(eng->msg_pipe[0]);
    close(eng->msg_pipe[1]);
#endif
        
out:
    return result;
}

