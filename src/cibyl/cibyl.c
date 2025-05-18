
#include "cibyl.h"
#include "eval.h"
#include "uci.h"

#include <threads.h>

FILE *log_file;
mtx_t log_mtx;

void cibyl_vwrite_log(char *format, va_list args)
{
    /* Make sure the log file exists attempt to lock the mutex.
     * If there is an error locking the mutex, something is horribly wrong. */
    if (log_file == NULL)
        return;
    mtx_lock(&log_mtx);

    vfprintf(log_file, format, args);

    /* Unlock the mutex. */
    mtx_unlock(&log_mtx);
}

void cibyl_write_log(char *format, ...)
{
    va_list args;
    va_start(args, format);
    cibyl_vwrite_log(format, args);
    va_end(args);
}

int main()
{
    return 0;
}
