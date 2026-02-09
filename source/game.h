#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "player.h"
#include "cat.h"
#include "item.h"
#include "bomb.h"

typedef struct {
    u8      state;          /* STATE_TITLE / STATE_PLAY / STATE_GAMEOVER */
    s16     score;
    s16     hiscore;
    u8      bg_type;        /* BG_DAY / BG_NIGHT / BG_MATRIX */
    u16     item_cnt;       /* 아이템 스폰 카운터 */
    Player  player;
    Cat     cats[MAX_CATS];
    Item    item;
    Bomb    bomb;
    u8      item_collected; /* 이번 프레임 아이템 획득 여부 */
} GameState;

void game_init(GameState* gs);
void game_play_init(GameState* gs);
void game_play_update(GameState* gs, u16 keys_held, u16 keys_pressed);
bool game_check_gameover(GameState* gs);

#endif /* GAME_H */
