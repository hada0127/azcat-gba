#include "player.h"

void player_init(Player* p) {
    p->x = PLAYER_INIT_X;
    p->player_accel = 0;
    p->direction = DIR_LEFT;
    p->life = PLAYER_LIFE_INIT;
    p->state = 0;
    p->anim_counter = 0;
}

void player_update(Player* p, u16 keys_held, bool bomb_active) {
    /* 폭탄 사용중이면 이동 불가 */
    if (bomb_active) return;
    /* 사망 시 이동 불가 */
    if (p->state == 1) return;

    s32 speed = PLAYER_BASE_SPEED + FP(p->player_accel) / FRAME_RATE_RATIO;

    if (keys_held & KEY_LEFT) {
        p->direction = DIR_LEFT;
        p->x -= speed;
        p->anim_counter++;
    } else if (keys_held & KEY_RIGHT) {
        p->direction = DIR_RIGHT;
        p->x += speed;
        p->anim_counter++;
    } else {
        p->anim_counter = 0;
    }

    /* X 클램핑 */
    if (p->x < PLAYER_X_MIN) p->x = PLAYER_X_MIN;
    if (p->x > PLAYER_X_MAX) p->x = PLAYER_X_MAX;
}
