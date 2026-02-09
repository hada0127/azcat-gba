#include "bomb.h"

void bomb_init(Bomb* b) {
    b->have = 1;  /* 디버그: 폭탄 1개 보유 시작 */
    b->use = 0;
    b->timer = 0;
}

void bomb_try_use(Bomb* b, u16 keys_pressed) {
    if ((keys_pressed & KEY_R) && b->have == 1 && b->use == 0) {
        b->use = 1;
        b->have = 0;
        b->timer = 0;
    }
}

void bomb_update(Bomb* b) {
    if (b->use) {
        b->timer++;
        if (b->timer >= BOMB_DURATION) {
            b->use = 0;
            b->timer = 0;
        }
    }
}

bool bomb_is_active(const Bomb* b) {
    return b->use == 1;
}
