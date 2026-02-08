#ifndef CAT_H
#define CAT_H

#include "types.h"

#define CAT_STATE_INACTIVE 0
#define CAT_STATE_FALLING  1
#define CAT_STATE_SIT      2
#define CAT_STATE_EXIT     3

typedef struct {
    s32 x;
    s32 y;
    s32 v_accel;    /* 개별 낙하 가속도 */
    u8  state;      /* INACTIVE / FALLING / SIT */
} Cat;

void cats_init(Cat cats[]);
/* 반환: 이번 프레임에서 플레이어에게 준 피해 수 */
u8 cats_update(Cat cats[], u16 score, s32 player_x,
               bool bomb_active, s16* score_add);

#endif /* CAT_H */
