#ifdef PLATFORM_GBA

#include <tonc.h>
#include "types.h"
#include "game.h"
#include "gameover.h"
#include "save.h"
#include "sound.h"
#include "render.h"

int main(void) {
    /* IRQ 초기화 */
    irq_init(NULL);
    irq_enable(II_VBLANK);

    /* Mode 4 + BG2(비트맵) + OBJ + 1D 매핑 */
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

    /* 렌더링 초기화 (타일+팔레트 로드) */
    render_init();
    sound_init();

    /* 게임 상태 */
    GameState gs;
    game_init(&gs);

    /* 세이브 로드 */
    SaveData save;
    save_read(&save);
    if (save_is_valid(&save))
        gs.hiscore = save.hiscore;

    GameOverResult go_result;
    u8 prev_bg = 0xFF;
    u8 prev_state = 0xFF;
    u8 go_input_delay = 0;  /* 게임오버 입력 딜레이 */

    /* 타이틀 배경 세팅 */
    render_set_title_bg();
    render_hide_all();

    while (1) {
        VBlankIntrWait();

        /* VBlank 중: OAM 갱신 + 페이지 전환 */
        render_oam_update();

        /* 입력 폴링 */
        key_poll();
        u16 kd = key_curr_state();
        u16 kp = key_hit(KEY_MASK);

        switch (gs.state) {

        /* ── 타이틀 ── */
        case STATE_TITLE:
            if (prev_state != STATE_TITLE) {
                render_set_title_bg();
                render_hide_all();
                prev_state = STATE_TITLE;
            }
            render_title_hud(gs.hiscore);
            render_oam_update();

            if (kp & (KEY_START | KEY_A)) {
                game_play_init(&gs);
                prev_bg = 0xFF; /* 강제 BG 로드 */
                prev_state = STATE_PLAY;
            }
            break;

        /* ── 플레이 ── */
        case STATE_PLAY:
            game_play_update(&gs, kd, kp);

            /* 배경 전환 감지 */
            if (gs.bg_type != prev_bg) {
                render_set_bg(gs.bg_type);
                prev_bg = gs.bg_type;
            }

            /* 게임오버 진입 감지 */
            if (gs.state == STATE_GAMEOVER) {
                gameover_init(&go_result, gs.score, gs.hiscore);
                if (go_result.new_hiscore) {
                    save_prepare(&save, gs.hiscore);
                    save_write(&save);
                }
                prev_state = STATE_GAMEOVER;
                go_input_delay = 30; /* 0.5초 입력 무시 */
                render_darken_bg_palette();
                render_gameover_grade(go_result.grade_index);
                render_gameover_nav();
                render_gameover_screen(&gs, &go_result);
                break;
            }

            render_sprites(&gs);
            render_hud(&gs);
            break;

        /* ── 게임오버 ── */
        case STATE_GAMEOVER:
            render_gameover_screen(&gs, &go_result);

            if (go_input_delay > 0) {
                go_input_delay--;
                break;
            }
            {
                u8 next = gameover_update(kp);
                if (next == STATE_TITLE) {
                    gs.state = STATE_TITLE;
                    prev_state = 0xFF; /* 타이틀 재진입 트리거 */
                } else if (next == STATE_PLAY) {
                    game_play_init(&gs);
                    prev_bg = 0xFF;
                    prev_state = STATE_PLAY;
                    render_hide_all();
                }
            }
            break;
        }
    }

    return 0;
}

#endif /* PLATFORM_GBA */
