#include "item.h"
#include "collision.h"
#include <stdlib.h>

void item_init(Item* it) {
    it->x = 0;
    it->y = FP(200); /* 화면 밖 */
    it->v_accel = 0;
    it->type = 0;
    it->active = 0;
    it->landed = 0;
    it->sit_cnt = 0;
}

static void item_spawn(Item* it) {
    it->x = FP(rand() % (ITEM_SPAWN_X_MAX - ITEM_SPAWN_X_MIN + 1) + ITEM_SPAWN_X_MIN);
    it->y = ITEM_SPAWN_Y;
    it->v_accel = 0;
    it->type = (u8)(rand() % ITEM_TYPE_COUNT + 1);
    it->active = 1;
    it->landed = 0;
    it->sit_cnt = 0;
}

u8 item_update(Item* it, u16* item_cnt, s32 player_x) {
    u8 collected_type = 0;

    if (!it->active) {
        (*item_cnt)++;
        if (*item_cnt > ITEM_SPAWN_INTERVAL) {
            item_spawn(it);
            *item_cnt = 0;
        }
        return 0;
    }

    /* 착지 상태 */
    if (it->landed) {
        it->sit_cnt++;
        if (it->sit_cnt >= ITEM_SIT_TIMEOUT) {
            it->active = 0;
            return 0;
        }
        /* 착지 중에도 획득 가능 */
        if (collision_check_item_player(it->x, it->y, player_x)) {
            collected_type = it->type;
            it->active = 0;
            *item_cnt = 0;
            return collected_type;
        }
        return 0;
    }

    /* 낙하 */
    it->y += ITEM_BASE_FALL + it->v_accel;
    it->v_accel += ITEM_GRAVITY;

    /* 착지 체크 */
    if (it->y >= ITEM_LAND_Y) {
        it->y = ITEM_LAND_Y;
        it->landed = 1;
        it->sit_cnt = 0;
    }

    /* 충돌 체크 */
    if (collision_check_item_player(it->x, it->y, player_x)) {
        collected_type = it->type;
        it->active = 0;
        *item_cnt = 0;
        return collected_type;
    }

    return 0;
}

void item_apply_effect(u8 type, u8* life, u8* bomb_have,
                       u8* player_accel, s16* score_delta) {
    *score_delta = 0;

    switch (type) {
    case ITEM_TYPE_HP:
        if (*life < PLAYER_LIFE_MAX) {
            (*life)++;
        } else {
            *score_delta = 5;
        }
        break;
    case ITEM_TYPE_BOMB:
        if (*bomb_have == 0) {
            *bomb_have = 1;
        } else {
            *score_delta = 5;
        }
        break;
    case ITEM_TYPE_POISON:
        if (*life > 1) {
            (*life)--;
        } else {
            *score_delta = -5;
        }
        break;
    case ITEM_TYPE_SPEED:
        if (*player_accel < PLAYER_ACCEL_MAX) {
            (*player_accel)++;
        } else {
            *score_delta = 5;
        }
        break;
    }
}
