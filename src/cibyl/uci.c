
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cibyl.h"
#include "uci.h"

/* Command strings for matching. */
const char STR_UCI[] = "uci";
const char STR_DEBUG[] = "debug";
const char STR_ISREADY[] = "isready";
const char STR_SETOPTION[] = "setoption";
const char STR_REGISTER[] = "register";
const char STR_UCINEWGAME[] = "ucinewgame";
const char STR_POSITION[] = "position";
const char STR_GO[] = "go";
const char STR_STOP[] = "stop";
const char STR_PONDERHIT[] = "ponderhit";
const char STR_QUIT[] = "quit";

/* Non-uci command strings. */
const char STR_DISPLAY[] = "d";
const char STR_EVAL[] = "eval";

const char ENGINE_NAME[] = "Cibyl";
const char ENGINE_AUTHOR[] = "Joel Kuehne";

uci_engine_t engine;

cibyl_errno_t handle_position(char *opts)
{
    return eng_set_ucifen(&engine.eng, opts);
}

cibyl_errno_t handle_searchmoves(char *moves)
{
    /* TODO: Implement me. */
    return CIBYL_EABORT;
}

cibyl_errno_t parse_i64(char *token, int64_t *out)
{
    char *endp;

    if (token == NULL && *token == '\0') {
        cibyl_write_log("parse_i64: valid string expected\n");
        return CIBYL_EABORT;
    }
    *out = strtoll(token, &endp, 10);
    if (*endp != '\0') {
        cibyl_write_log("parse_i64: token is not an integer\n");
        return CIBYL_EABORT;
    }
    
    return CIBYL_EOK;
}

cibyl_errno_t handle_go(char *opts)
{
    typedef enum {
        GO_SEARCHMOVES,
        GO_PONDER,
        GO_WTIME,
        GO_BTIME,
        GO_WINC,
        GO_BINC,
        GO_MOVESTOGO,
        GO_DEPTH,
        GO_NODES,
        GO_MATE,
        GO_MOVETIME,
        GO_INFINITE
    } go_opt_t;

    const char STR_GO_SEARCHMOVES[] = "searchmoves";
    const char STR_GO_PONDER[] = "ponder";
    const char STR_GO_WTIME[] = "wtime";
    const char STR_GO_BTIME[] = "btime";
    const char STR_GO_WINC[] = "winc";
    const char STR_GO_BINC[] = "binc";
    const char STR_GO_MOVESTOGO[] = "movestogo";
    const char STR_GO_DEPTH[] = "depth";
    const char STR_GO_NODES[] = "nodes";
    const char STR_GO_MATE[] = "mate";
    const char STR_GO_MOVETIME[] = "movetime";
    const char STR_GO_INFINITE[] = "infinite";

    go_param_t go_params;
    char *token;
    char *endp;
    
    /* Clear go params. */
    clear_go_params(&go_params);

    /* Handle null input. */
    if (opts == NULL) {
        return CIBYL_EABORT;
    }

    /* Loop through all of the arguments to the go command. */
    token = strtok(opts, " ");
    while (token != NULL) {
        /* Handle searchmoves, this will consume all remaining arguments. */
        if (strcmp(token, STR_GO_SEARCHMOVES) == 0) {
            handle_searchmoves(strtok(opts, " "));
            break;
        }

        /* Handle various flags. */
        else if (strcmp(token, STR_GO_PONDER) == 0) {
            go_params.ponder = true;
        } else if (strcmp(token, STR_GO_INFINITE) == 0) {
            go_params.infinite = true;
        }

        /* Handle time information. */
        else if (strcmp(token, STR_GO_WTIME) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.wtime) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_BTIME) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.btime) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_WINC) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.winc) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_BINC) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.binc) < 0)
                return CIBYL_EABORT;
        }

        /* Handle move stopping. */
        else if (strcmp(token, STR_GO_MOVESTOGO) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.movestogo) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_DEPTH) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.depth) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_NODES) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.nodes) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_MATE) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.mate) < 0)
                return CIBYL_EABORT;
        } else if (strcmp(token, STR_GO_MOVETIME) == 0) {
            if (parse_i64(strtok(opts, " "), &go_params.movetime) < 0)
                return CIBYL_EABORT;
        }

        /* Grab the next token. */
        token = strtok(NULL, " ");
    }

    /* Start thinking. */
    eng_notify_go(&engine.eng, &go_params);

    return CIBYL_EOK;
}

void hanlde_display()
{

}

cibyl_errno_t handle_cmd(char *cmd)
{
    cibyl_errno_t result = CIBYL_EOK;

    char *token = strtok(cmd, " \n");
    char *opts;
    bool token_accepted = false;

    while (token_accepted && token != NULL) {
        token_accepted = true;

        /* Initialization commands. */
        if (strcmp(cmd, STR_UCI) == 0) {
            /* Respond to the UCI command. */
            printf("id name %s\n", ENGINE_NAME);
            printf("id author %s\n", ENGINE_AUTHOR);
            printf("uciok\n");

            /* Begin asynchronous initialization. */
            eng_begin_init(&engine.eng);
        } else if (strcmp(cmd, STR_DEBUG) == 0) {
            engine.debug = true;
        } else if (strcmp(cmd, STR_ISREADY) == 0) {
            /* Wait for initialization to be complete. */
            eng_await_isready(&engine.eng);
            printf("readyok\n");
        } else if (strcmp(cmd, STR_SETOPTION) == 0) {
            /* TODO: Implement me. */
        } else if (strcmp(cmd, STR_REGISTER) == 0) {
            /* Do nothing as this is not password protected. */
        }

        /* Board setup commands. */
        else if (strcmp(cmd, STR_UCINEWGAME) == 0) {
            /* TODO: Implement me. */
            eng_set_ucifen(&engine.eng, "startpos");
        } else if (strcmp(cmd, STR_POSITION) == 0) {
            eng_set_ucifen(&engine.eng, strtok(cmd, "\n"));
        }

        /* Functions for controlling thinking. */
        else if (strcmp(cmd, STR_GO) == 0) {
            result = handle_go(strtok(NULL, ""));   /* Slice off the rest of the cmd. */
        } else if (strcmp(cmd, STR_STOP) == 0) {
            eng_notify_stop(&engine.eng);
        } else if (strcmp(cmd, STR_PONDERHIT) == 0) {
            eng_notify_ponderhit(&engine.eng);
        }

        /* Miscelaneous functions. */
        else if (strcmp(cmd, STR_QUIT) == 0) {
            eng_cleanup(&engine.eng);
        } else if (strcmp(cmd, STR_DISPLAY) == 0) {
            /* TODO: Implement me. */
        }

        /* If the first word is invalid, then parse starting with the next as per the spec. */
        else {
            token = strtok(NULL, " ");
            token_accepted = false;
        }
    }

    return result;
}

cibyl_errno_t uci_init(uci_engine_t *engine)
{
    /* TODO: Implement me. */
}

cibyl_errno_t uci_process(uci_engine_t *engine)
{
    /* TODO: Implement me. */
}

