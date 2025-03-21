
#ifndef CB_BOARD_H
#define CB_BOARD_H

#include <stdint.h>
#include <stdbool.h>

#include "board/board_const.h"
#include "board/bitboard.h"
#include "board/mailbox.h"
#include "history.h"

typedef struct {
    cb_bitboard_t bb;
    cb_mailbox_t mb;
    cb_hist_stack_t hist;
    uint8_t turn;
    uint32_t fullmove_num;
} cb_board_t

#endif /* CB_BOARD_H */
