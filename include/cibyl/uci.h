
#ifndef CIBYL_UCI_H
#define CIBYL_UCI_H

typedef enum {
    UCI = 0,        /* Tells the engine to use the UCI interface. */
    DEBUG,          /* Tells the engine to use DEBUG mode (more INFO responses). */
    ISREADY,        /* Used to ping the engine to see if it is still alive. */
    SETOPTION,      /* Allows you to change internal engine parameters. */
    REGISTER,       /* Username and password stuff */
    UCINEWGAME,     /* Tells the engine that the next search will be from a different game. */
    POSITION,       /* Set up the position defined by the following string. */
    GO,             /* Start calculating on the current position. */
    STOP,           /* Stop calculating as soon as possible. */
    PONDERHIT,      /* The user has played the expected move. */
    QUIT            /* Quit the program as soon as possible. */
} uci_cmd_t;

typedef enum {
    ID = 0,         /* Mandatory: Sent after UCI to identify the engine. */
    UCIOK,          /* Mandatory: Sent after UCI. */
    READYOK,        /* Mandatory: Always sent after isready. */
    BESTMOVE,       /* Mandatory: Sent after a go command. Also may have ponder. */
    COPYPROTECTION, /* Optional */
    REGISTRATION,   /* Optional */
    INFO,           /* Suggested: Read details. */
    OPTION          /* Optional: Tells the GUI what internal params can be changed. */
} uci_rsp_t;

void uci_init();

#endif /* CIBYL_UCI_H */
