#include "sound.h"

#ifdef PLATFORM_GBA

#include <string.h>
#include <tonc.h>
#include "snd_bgm.h"
#include "snd_hit.h"
#include "snd_bomb.h"
#include "snd_item.h"
#include "snd_gameover.h"

/*
 * 연속 더블 버퍼 오디오 시스템 (Deku 방식)
 *
 * 10512Hz = 280896 cycles / 1596 timer_period
 * 타이머값 = 65536 - 1596 = 63940
 * 버퍼크기 = 280896 / 1596 = 176 (정확히 나눠떨어짐!)
 *
 * 두 버퍼가 메모리상 연속 배치 → DMA가 buf[0]→buf[1] 자연스럽게 흐름.
 * DMA 리스타트는 2프레임에 1번만 → 클릭/팝 최소화.
 */

#define TIMER_10512HZ  63940
#define BUF_SIZE       176

/* 연속 더블 버퍼: buf[0..175] = A, buf[176..351] = B */
static s8 mix_a[BUF_SIZE * 2] ALIGN(4);   /* BGM 채널 */
static s8 mix_b[BUF_SIZE * 2] ALIGN(4);   /* SFX 채널 */
static u8 active_buf;  /* 현재 DMA가 읽는 버퍼 (0 or 1) */

/* BGM 상태 */
static const s8 *bgm_src;
static u32 bgm_pos;
static u32 bgm_len;
static u8  bgm_on;

/* SFX 상태 */
static const s8 *sfx_src;
static u32 sfx_pos;
static u32 sfx_len;

/* 믹서: dst에 BUF_SIZE 샘플 채우기 */
static void fill_bgm(s8 *dst) {
    if (!bgm_on) {
        memset(dst, 0, BUF_SIZE);
        return;
    }
    u32 left = bgm_len - bgm_pos;
    if (left >= BUF_SIZE) {
        memcpy(dst, bgm_src + bgm_pos, BUF_SIZE);
        bgm_pos += BUF_SIZE;
    } else {
        /* 루프: 끝 + 처음 이어 복사 */
        memcpy(dst, bgm_src + bgm_pos, left);
        memcpy(dst + left, bgm_src, BUF_SIZE - left);
        bgm_pos = BUF_SIZE - left;
    }
}

static void fill_sfx(s8 *dst) {
    if (sfx_len == 0) {
        memset(dst, 0, BUF_SIZE);
        return;
    }
    u32 left = sfx_len - sfx_pos;
    u32 n = (left < BUF_SIZE) ? left : BUF_SIZE;
    memcpy(dst, sfx_src + sfx_pos, n);
    if (n < BUF_SIZE)
        memset(dst + n, 0, BUF_SIZE - n);
    sfx_pos += n;
    if (sfx_pos >= sfx_len)
        sfx_len = 0;
}

void sound_init(void) {
    REG_SNDSTAT = SSTAT_ENABLE;

    REG_SNDDSCNT = SDS_A100 | SDS_B100
                 | SDS_AL | SDS_AR | SDS_BL | SDS_BR
                 | SDS_ATMR0 | SDS_BTMR0;

    REG_TM0D = TIMER_10512HZ;
    REG_TM0CNT = TM_ENABLE | TM_FREQ_SYS;

    memset(mix_a, 0, sizeof(mix_a));
    memset(mix_b, 0, sizeof(mix_b));
    active_buf = 0;

    /* DMA: 연속 버퍼 시작 */
    REG_DMA1SAD = (u32)mix_a;
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    REG_DMA2SAD = (u32)mix_b;
    REG_DMA2DAD = (u32)&REG_FIFO_B;
    REG_DMA2CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    bgm_on = 0;
    bgm_pos = 0;
    bgm_len = 0;
    sfx_len = 0;
    sfx_pos = 0;
}

void sound_play_bgm(u8 track) {
    (void)track;
    bgm_src = (const s8 *)snd_bgm_data;
    bgm_len = SND_BGM_LEN;
    bgm_pos = 0;
    bgm_on = 1;
}

void sound_play_sfx(u8 sfx_id) {
    switch (sfx_id) {
    case SFX_HIT:
        sfx_src = (const s8 *)snd_hit_data;
        sfx_len = SND_HIT_LEN;
        break;
    case SFX_GAMEOVER:
        sfx_src = (const s8 *)snd_gameover_data;
        sfx_len = SND_GAMEOVER_LEN;
        bgm_on = 0;
        break;
    case SFX_BOMB:
        sfx_src = (const s8 *)snd_bomb_data;
        sfx_len = SND_BOMB_LEN;
        break;
    case SFX_ITEM:
        sfx_src = (const s8 *)snd_item_data;
        sfx_len = SND_ITEM_LEN;
        break;
    default:
        return;
    }
    sfx_pos = 0;
}

void sound_update(void) {
    if (active_buf == 1) {
        /*
         * DMA가 buf[1]을 읽는 중 → buf[0]을 채움.
         * VBlank에서 DMA를 처음으로 되감기.
         */
        fill_bgm(mix_a);
        fill_sfx(mix_b);

        /* DMA 리스타트: 버퍼 시작으로 */
        REG_DMA1CNT = 0;
        REG_DMA1SAD = (u32)mix_a;
        REG_DMA1DAD = (u32)&REG_FIFO_A;
        REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                    | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        REG_DMA2CNT = 0;
        REG_DMA2SAD = (u32)mix_b;
        REG_DMA2DAD = (u32)&REG_FIFO_B;
        REG_DMA2CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                    | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        active_buf = 0;
    } else {
        /*
         * DMA가 buf[0]을 읽는 중 → buf[1]을 채움.
         * DMA는 자연스럽게 buf[1]로 흘러감 (연속 메모리).
         * DMA 리스타트 불필요!
         */
        fill_bgm(mix_a + BUF_SIZE);
        fill_sfx(mix_b + BUF_SIZE);

        active_buf = 1;
    }
}

void sound_stop(void) {
    bgm_on = 0;
    sfx_len = 0;
}

#else
/* ── PC 스텁 ── */
void sound_init(void) { (void)0; }
void sound_play_bgm(u8 track) { (void)track; }
void sound_play_sfx(u8 sfx_id) { (void)sfx_id; }
void sound_update(void) { (void)0; }
void sound_stop(void) { (void)0; }
#endif
