
#ifdef _WIN32
#define WIN_PIPE_SIZE 4096
#include <io.h>
#else
#include <unistd.h>
#endif

#include "engine.h"

cibyl_errno_t mgr_entrypoint(engine_t *eng)
{
    
}

cibyl_errno_t eng_begin_init(engine_t *eng)
{
    /* Create the message pipe. */
#ifdef _WIN32
    if (_pipe(engine->eng.msg_pipe, WIN_PIPE_SIZE, O_BINARY) == -1) {
        printf("info string ERROR: pipe failed. Terminating...");
        return CIBYL_EOK;
    }
#else
    if (pipe(eng->msg_pipe) == -1) {
        printf("info string ERROR: pipe failed. Terminating...");
        return CIBYL_EOK;
    }
#endif

    /* Spawn the manager thread and have it complete engine initialization. */
    
}
