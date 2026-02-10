#include "sound.h"

#ifdef PLATFORM_GBA

#include <tonc.h>
#include "snd_bgm.h"
#include "snd_hit.h"
#include "snd_bomb.h"
#include "snd_item.h"
#include "snd_gameover.h"

/* 8192Hz 타이머 값: 65536 - (16777216/8192) = 63488 = 0xF800 */
#define TIMER_8192HZ    0xF800

/* BGM 총 프레임 수 */
#define BGM_TOTAL_FRAMES  (SND_BGM_LEN * 60 / 8192)

/* SFX 프레임 계산: 오버런 방지 위해 +1 없음 */
#define SFX_FRAMES(len)   ((len) * 60 / 8192)

/* 상태 */
static u8  bgm_on;
static u32 bgm_frames;
static u16 sfx_frames_left;

/* 채널 A 정지: DMA 멈추고 FIFO에 0 채우기 */
static void stop_channel_a(void) {
    REG_DMA1CNT = 0;
    REG_SNDDSCNT |= SDS_ARESET;
    /* FIFO에 무음 주입 (8 words = FIFO 전체) */
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
    REG_FIFO_A = 0;
}

/* 채널 B 정지: DMA 멈추고 FIFO에 0 채우기 */
static void stop_channel_b(void) {
    REG_DMA2CNT = 0;
    REG_SNDDSCNT |= SDS_BRESET;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
    REG_FIFO_B = 0;
}

void sound_init(void) {
    /* 마스터 사운드 활성화 */
    REG_SNDSTAT = SSTAT_ENABLE;

    /* Direct Sound A+B: DMG 볼륨 0(PSG 차단), DS 100%, 양쪽 스피커, 타이머 0 */
    REG_SNDDSCNT = SDS_A100 | SDS_B100
                 | SDS_AL | SDS_AR | SDS_BL | SDS_BR
                 | SDS_ATMR0 | SDS_BTMR0;

    /* 타이머 0: 8192Hz */
    REG_TM0D = TIMER_8192HZ;
    REG_TM0CNT = TM_ENABLE | TM_FREQ_SYS;

    bgm_on = 0;
    bgm_frames = 0;
    sfx_frames_left = 0;
}

void sound_play_bgm(u8 track) {
    (void)track;

    REG_DMA1CNT = 0;
    REG_SNDDSCNT |= SDS_ARESET;

    REG_DMA1SAD = (u32)snd_bgm_data;
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    bgm_frames = 0;
    bgm_on = 1;
}

void sound_play_sfx(u8 sfx_id) {
    const void *data = NULL;
    u32 len = 0;

    switch (sfx_id) {
    case SFX_HIT:
        data = snd_hit_data;
        len = SND_HIT_LEN;
        break;
    case SFX_GAMEOVER:
        data = snd_gameover_data;
        len = SND_GAMEOVER_LEN;
        bgm_on = 0;
        stop_channel_a();
        break;
    case SFX_BOMB:
        data = snd_bomb_data;
        len = SND_BOMB_LEN;
        break;
    case SFX_ITEM:
        data = snd_item_data;
        len = SND_ITEM_LEN;
        break;
    default:
        return;
    }

    REG_DMA2CNT = 0;
    REG_SNDDSCNT |= SDS_BRESET;

    REG_DMA2SAD = (u32)data;
    REG_DMA2DAD = (u32)&REG_FIFO_B;
    REG_DMA2CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    sfx_frames_left = SFX_FRAMES(len);
}

void sound_update(void) {
    /* BGM 루프 */
    if (bgm_on) {
        bgm_frames++;
        if (bgm_frames >= BGM_TOTAL_FRAMES) {
            REG_DMA1CNT = 0;
            REG_SNDDSCNT |= SDS_ARESET;
            REG_DMA1SAD = (u32)snd_bgm_data;
            REG_DMA1DAD = (u32)&REG_FIFO_A;
            REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                        | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
            bgm_frames = 0;
        }
    }

    /* SFX 끝나면 채널 B 정지 */
    if (sfx_frames_left > 0) {
        sfx_frames_left--;
        if (sfx_frames_left == 0)
            stop_channel_b();
    }
}

void sound_stop(void) {
    bgm_on = 0;
    sfx_frames_left = 0;
    stop_channel_a();
    stop_channel_b();
}

#else
/* ── PC 스텁 ── */
void sound_init(void) { (void)0; }
void sound_play_bgm(u8 track) { (void)track; }
void sound_play_sfx(u8 sfx_id) { (void)sfx_id; }
void sound_update(void) { (void)0; }
void sound_stop(void) { (void)0; }
#endif
