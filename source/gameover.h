#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "types.h"

typedef struct {
    u8  grade_index;
    const char* grade_code;
    bool new_hiscore;
} GameOverResult;

/* 게임오버 초기화: 등급 판정, 하이스코어 갱신 여부 */
void gameover_init(GameOverResult* result, s16 score, s16 hiscore);

/* 게임오버 입력: 0=대기, STATE_TITLE=타이틀, STATE_PLAY=재도전 */
u8 gameover_update(u16 keys_pressed);

#endif /* GAMEOVER_H */
