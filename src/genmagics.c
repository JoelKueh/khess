#include "cb/tables.h"

FILE *log_file;
pthread_mutex_t log_lock;

int main()
{
    cibyl_error_t err;
    bitboard_t magics[64];
    int result = 0;

    log_file = stderr;
    pthread_mutex_init(&log_lock, NULL);

    if (cb_gen_rook_magics(&err, magics)) {
        CIBYL_WRITE_ERR(&err);
        result = 1;
        goto out;
    }
    cb_print_magics(magics);

    if (cb_gen_rook_magics(&err, magics)) {
        CIBYL_WRITE_ERR(&err);
        result = 1;
        goto out;
    }
    cb_print_magics(magics);

out:
    return result;
}
