#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

#define DIR_LEFT  0
#define DIR_RIGHT 1

typedef struct {
    s32 x;              /* 고정소수점 X 좌표 */
    u8  player_accel;   /* 아이템으로 증가하는 추가 속도 (0~4) */
    u8  direction;      /* DIR_LEFT / DIR_RIGHT */
    u8  life;           /* HP (0~3) */
    u8  state;          /* 0=alive, 1=dead */
    u8  anim_counter;   /* 이동 애니메이션 카운터 */
} Player;

void player_init(Player* p);
void player_update(Player* p, u16 keys_held, bool bomb_active);

#endif /* PLAYER_H */
