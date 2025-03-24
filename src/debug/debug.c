
#include <string.h>
#include <stdio.h>
#include "cblib.h"

#define MAX_COMMAND_LEN 512

const char DEFAULT_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int handle_position(cb_board_t *board)
{
    /* Slice the next word off the command. */
    char *token = strtok(NULL, " ");
    cb_error_t result;

    /* Error handling. */
    if (token == NULL) {
        printf("Invalid position command. Usage:\n"
               "position <fen/uci/pgn>\n");
        return 1;
    }

    /* Execute the command. */
    if (strcmp(token, "fen") == 0) {
        result = cb_board_from_fen(board, strtok(NULL, "\n"));
    } else if (strcmp(token, "uci") == 0) {
        result = cb_board_from_uci(board, strtok(NULL, "\n"));
    } else if (strcmp(token, "pgn") == 0) {
        result = cb_board_from_pgn(board, strtok(NULL, "\n"));
    } else {
        printf("Invalid position command. Usage:\n"
               "position <fen/uci/pgn>\n");
        return 1;
    }

    /* Display information about what happened in the command. */
    if (result == CB_EABORT) {
        return -1;
    } else if (result == CB_EINVAL) {
        printf("Invalid game string\n");
        return 1;
    } else if (result == CB_EILLEGAL) {
        printf("Illegal move in game string\n");
        return 1;
    }

    return 0;
}

int handle_move(cb_board_t *board)
{
    /* Slice off the algebraic part of the move. */
    char *token = strtok(NULL, " ");
    cb_error_t result;
    cb_move_t mv;

    /* Error handling. */
    if (token == NULL) {
        printf("Invalid move command. Usage:\n"
               "move <uci_algbr>\n");
        return 1;
    }

    /*


    return 0;
}

int handle_undo(cb_board_t *board)
{
    return 0;
}

int handle_debug(cb_board_t *board)
{
    return 0;
}

int handle_board(cb_board_t *board)
{
    return 0;
}

int handle_go(cb_board_t *board)
{
    return 0;
}

int parse_input(char *command, cb_board_t *board)
{
    /* Slice one token off of the command. */
    char *token = strtok(command, " ");

    /* Parse the first token in the command. */
    if (strcmp(token, "position") == 0)
        return handle_position(board);
    if (strcmp(token, "move") == 0)
        return handle_go(board);
    if (strcmp(token, "undo") == 0)
        return handle_undo(board);
    if (strcmp(token, "debug") == 0)
        return handle_debug(board);
    if (strcmp(token, "board") == 0)
        return handle_board(board);
    if (strcmp(token, "go") == 0)
        return handle_go(board);
    if (strcmp(token, "quit") == 0)
        return 1;
    
    printf("Invalid command\n");
    return 0;
}

int main()
{
    cb_mvlst_t moves;
    cb_board_t board;
    cb_state_tables_t state;
    char *command = NULL;
    size_t len = 0;
    ssize_t nread;
    int result;
    bool run_program = true;

    /* Print version information. */
    printf("kchess debug version 0.0.1 by Joel Kuehne");

    /* Set up the initial board state. */
    if (cb_board_from_fen(&board, DEFAULT_FEN)) {
        fprintf(stderr, "Failed to setup initial board state!\n");
        result = 1;
        goto out;
    }

    /* Accept commands from the user. */
    while (run_program) {
        if ((nread = getline(&command, &len, stdin)) != 0) {
            perror("getline");
            result = 1;
            goto out_free_command;
        }
        run_program = parse_input(command, &board) < 0;
    }

out_free_command:
    free(command);
out:
    return result;
}
