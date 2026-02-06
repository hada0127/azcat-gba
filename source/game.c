#include "game.h"
#include "difficulty.h"

void game_init(GameState* gs) {
    gs->state = STATE_TITLE;
    gs->score = 0;
    gs->hiscore = 0;
    gs->bg_type = BG_DAY;
    gs->item_cnt = 0;
    player_init(&gs->player);
    cats_init(gs->cats);
    item_init(&gs->item);
    bomb_init(&gs->bomb);
}

void game_play_init(GameState* gs) {
    gs->state = STATE_PLAY;
    gs->score = 0;
    gs->bg_type = BG_DAY;
    gs->item_cnt = 0;
    player_init(&gs->player);
    cats_init(gs->cats);
    item_init(&gs->item);
    bomb_init(&gs->bomb);
}

void game_play_update(GameState* gs, u16 keys_held, u16 keys_pressed) {
    if (gs->state != STATE_PLAY) return;

    /* 폭탄 입력 체크 */
    bomb_try_use(&gs->bomb, keys_pressed);
    /* 폭탄 타이머 */
    bomb_update(&gs->bomb);

    bool ba = bomb_is_active(&gs->bomb);

    /* 플레이어 이동 */
    player_update(&gs->player, keys_held, ba);

    /* 고양이 업데이트 */
    s16 score_add = 0;
    u8 damage = cats_update(gs->cats, (u16)gs->score, gs->player.x,
                            ba, &score_add);
    gs->score += score_add;

    /* 피격 처리 */
    if (damage > 0) {
        if (gs->player.life > damage)
            gs->player.life -= damage;
        else
            gs->player.life = 0;
    }

    /* 아이템 업데이트 */
    if (!ba) {
        u8 item_type = item_update(&gs->item, &gs->item_cnt, gs->player.x);
        if (item_type > 0) {
            s16 delta = 0;
            item_apply_effect(item_type, &gs->player.life,
                              &gs->bomb.have, &gs->player.player_accel,
                              &delta);
            gs->score += delta;
        }
    }

    /* 배경 전환 */
    gs->bg_type = difficulty_get_bg_type((u16)gs->score);

    /* 게임오버 체크 */
    game_check_gameover(gs);
}

bool game_check_gameover(GameState* gs) {
    if (gs->player.life < 1) {
        gs->player.state = 1; /* dead */
        gs->state = STATE_GAMEOVER;
        if (gs->score > gs->hiscore)
            gs->hiscore = gs->score;
        return true;
    }
    return false;
}
