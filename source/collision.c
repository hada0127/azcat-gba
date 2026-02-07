#include "collision.h"

/* 스프라이트 크기 상수 */
#define PLAYER_W    FP(24)
#define PLAYER_H    FP(24)
#define CAT_W       FP(16)
#define CAT_H       FP(32)
#define ITEM_W      FP(16)
#define ITEM_H      FP(16)

bool collision_check_aabb(const HitBox* a, const HitBox* b) {
    if (a->x >= b->x + b->w) return false;
    if (a->x + a->w <= b->x) return false;
    if (a->y >= b->y + b->h) return false;
    if (a->y + a->h <= b->y) return false;
    return true;
}

bool collision_check_cat_player(s32 cat_x, s32 cat_y, s32 player_x) {
    /* Y 범위 체크: 116~136 구간에서만 충돌 */
    if (cat_y < CAT_HIT_Y_MIN || cat_y > CAT_HIT_Y_MAX)
        return false;

    /* X축 겹침 체크 */
    if (cat_x + CAT_W <= player_x) return false;
    if (cat_x >= player_x + PLAYER_W) return false;
    return true;
}

bool collision_check_item_player(s32 item_x, s32 item_y, s32 player_x) {
    /* 아이템은 플레이어 근처에서만 획득 가능 (Y 116~140) */
    if (item_y < CAT_HIT_Y_MIN || item_y > ITEM_LAND_Y)
        return false;

    if (item_x + ITEM_W <= player_x) return false;
    if (item_x >= player_x + PLAYER_W) return false;
    return true;
}
