
#ifndef CB_TABLES_H
#define CB_TABLES_H

#include <stdint.h>
#include <stdbool.h>

extern uint64_t pawn_atks[2][64];
extern uint64_t knight_atks[64];
extern uint64_t king_atks[64];
extern uint64_t to_from_table[64][64];

/* Functions to initialize tables that will be used by the move generator. */
int cb_init_magic_tables();
void cb_free_magic_tables();
void cb_init_normal_tables();

/* Functions to perform reads on the magical tables. */
uint64_t cb_read_bishop_atk_msk(uint8_t sq, uint64_t occ);
uint64_t cb_read_rook_atk_msk(uint8_t sq, uint64_t occ);

/* Functions to perform reads on the normal tables. */


#endif /* CB_TABLES_H */
