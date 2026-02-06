#ifndef COLLISION_H
#define COLLISION_H

#include "types.h"

/* AABB 충돌 판정 */
bool collision_check_aabb(const HitBox* a, const HitBox* b);

/* 고양이-플레이어 충돌 (Y 116~136 범위에서만) */
bool collision_check_cat_player(s32 cat_x, s32 cat_y, s32 player_x);

/* 아이템-플레이어 충돌 */
bool collision_check_item_player(s32 item_x, s32 item_y, s32 player_x);

#endif /* COLLISION_H */
