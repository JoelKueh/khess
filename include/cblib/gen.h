
#ifndef CB_GEN_H
#define CB_GEN_H

#include <stdint.h>
#include <stdbool.h>

#include "move.h"

typedef struct {
    uint64_t threats;
    uint64_t checks;
    uint64_t check_blocks;
    uint64_t pins[9];
} cb_state_tables_t;

void gen_moves(cb_mvlst_t mvlst, 

#endif /* CB_GEN_H */

