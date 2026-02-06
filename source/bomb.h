#ifndef BOMB_H
#define BOMB_H

#include "types.h"

typedef struct {
    u8  have;   /* 보유 여부 (0/1) */
    u8  use;    /* 사용중 여부 (0/1) */
    u8  timer;  /* 사용 타이머 */
} Bomb;

void bomb_init(Bomb* b);
void bomb_try_use(Bomb* b, u16 keys_pressed);
void bomb_update(Bomb* b);
bool bomb_is_active(const Bomb* b);

#endif /* BOMB_H */
