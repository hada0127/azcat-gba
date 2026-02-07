#include "cat.h"
#include "difficulty.h"
#include "collision.h"
#include <stdlib.h>

void cats_init(Cat cats[]) {
    int i;
    for (i = 0; i < MAX_CATS; i++) {
        cats[i].x = 0;
        cats[i].y = FP(200); /* 화면 밖 */
        cats[i].v_accel = 0;
        cats[i].state = CAT_STATE_INACTIVE;
    }
}

static void cat_spawn(Cat* c) {
    c->x = FP(rand() % (CAT_SPAWN_X_MAX - CAT_SPAWN_X_MIN + 1) + CAT_SPAWN_X_MIN);
    c->y = FP(-(rand() % ((-CAT_SPAWN_Y_MIN) - (-CAT_SPAWN_Y_MAX) + 1) + (-CAT_SPAWN_Y_MAX)));
    c->v_accel = 0;
    c->state = CAT_STATE_FALLING;
}

u8 cats_update(Cat cats[], u16 score, s32 player_x,
               bool bomb_active, s16* score_add) {
    u8 damage = 0;
    u8 cat_qty = difficulty_get_cat_qty(score);
    u8 cat_accel = difficulty_get_cat_accel(score);
    int i;

    *score_add = 0;

    /* 폭탄 활성 → 전체 제거 */
    if (bomb_active) {
        for (i = 0; i < MAX_CATS; i++) {
            if (cats[i].state == CAT_STATE_FALLING) {
                cats[i].state = CAT_STATE_INACTIVE;
                cats[i].y = FP(200);
            }
        }
        return 0;
    }

    for (i = 0; i < MAX_CATS; i++) {
        /* 활성화: 비활성 고양이를 cat_qty까지 스폰 */
        if (cats[i].state == CAT_STATE_INACTIVE && (u8)i < cat_qty) {
            cat_spawn(&cats[i]);
        }

        if (cats[i].state == CAT_STATE_FALLING) {
            /* 낙하 */
            cats[i].y += CAT_BASE_FALL + cats[i].v_accel + FP(cat_accel) / FRAME_RATE_RATIO;
            cats[i].v_accel += CAT_GRAVITY;

            /* 충돌 체크 */
            if (collision_check_cat_player(cats[i].x, cats[i].y, player_x)) {
                damage++;
                cats[i].state = CAT_STATE_INACTIVE;
                cats[i].y = FP(200);
                continue;
            }

            /* 착지 */
            if (cats[i].y > CAT_LAND_Y) {
                cats[i].y = CAT_LAND_Y;
                cats[i].state = CAT_STATE_SIT;
                cats[i].v_accel = 0;  /* 체류 카운터로 재활용 */
                (*score_add)++;
            }
        } else if (cats[i].state == CAT_STATE_SIT) {
            /* 착지 후 잠시 체류 → 리스폰 */
            cats[i].v_accel++;
            if (cats[i].v_accel >= CAT_SIT_FRAMES) {
                if ((u8)i < cat_qty) {
                    cat_spawn(&cats[i]);
                } else {
                    cats[i].state = CAT_STATE_INACTIVE;
                    cats[i].y = FP(200);
                }
            }
        }
    }

    return damage;
}
