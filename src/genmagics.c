#include "cb/tables.h"

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help, "-h", "Show help") \
    BOOLEAN_ARG(bishop, "--bishop", "Generate bishop magics") \
    BOOLEAN_ARG(rook, "--rook", "Generate rook magics")
#include "utils/easy_args.h"

FILE *log_file;
pthread_mutex_t log_lock;

int main(int argc, char *argv[])
{
    cibyl_error_t err;
    bitboard_t magics[64];
    int result = 0;
    args_t args = make_default_args();

    log_file = stderr;
    pthread_mutex_init(&log_lock, NULL);

    if (!parse_args(argc, argv, &args) || args.help) {
        print_help(argv[0]);
        result = 1;
        goto out;
    }

    if (args.rook) {
        if (cb_gen_rook_magics(&err, magics)) {
            CIBYL_WRITE_ERR(&err);
            result = 1;
            goto out;
        }
        cb_print_magics(magics);
    }

    if (args.bishop) {
        if (cb_gen_bishop_magics(&err, magics)) {
            CIBYL_WRITE_ERR(&err);
            result = 1;
            goto out;
        }
        cb_print_magics(magics);
    }

out:
    return result;
}
