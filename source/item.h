#ifndef ITEM_H
#define ITEM_H

#include "types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 v_accel;    /* 낙하 가속도 */
    u8  type;       /* 1~4 */
    u8  active;     /* 활성 여부 */
    u8  landed;     /* 착지 여부 */
    u8  sit_cnt;    /* 착지 후 프레임 카운터 */
} Item;

/* 전방 선언 */
struct Player;

void item_init(Item* it);

/* 아이템 업데이트. 스폰/낙하/착지/만료 처리
 * item_cnt: 스폰 카운터 (외부에서 관리)
 * 반환: 아이템 효과 적용이 필요하면 type, 아니면 0 */
u8 item_update(Item* it, u16* item_cnt, s32 player_x);

/* 아이템 효과 적용 */
void item_apply_effect(u8 type, u8* life, u8* bomb_have,
                       u8* player_accel, s16* score_delta);

#endif /* ITEM_H */
