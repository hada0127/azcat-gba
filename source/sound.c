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
 * 더블 버퍼 오디오 시스템
 *
 * 타이머 주기 = 65536 - 0xF800 = 2048 CPU cycles
 * VBlank 주기 = 228 × 308 × 4 = 280896 CPU cycles
 * 프레임당 샘플 = 280896 / 2048 = 137.15625 = 137 + 5/32
 *
 * 분수 누적으로 137/138 교대 배분하여 드리프트 없이 정확한 재생.
 * 버퍼는 BUF_SIZE(144)로 실제 소비량보다 약간 크게 잡아
 * DMA가 타이밍 오차로 1~2 샘플 더 읽어도 유효 데이터를 읽도록 함.
 */

#define TIMER_VAL      0xF800

#define SPF_WHOLE      137   /* 프레임당 샘플 정수부 */
#define SPF_FRAC_NUM   5     /* 분수부 분자 (5/32) */
#define SPF_FRAC_DEN   32    /* 분수부 분모 */
#define BUF_SIZE       144   /* 버퍼 크기: 138 + 여유, 16의 배수 */

/* 더블 버퍼 */
static s8 buf_a[2][BUF_SIZE] ALIGN(4);
static s8 buf_b[2][BUF_SIZE] ALIGN(4);
static u8 cur_buf;
static u8 frac_acc;

/* BGM 상태 */
static const s8 *bgm_src;
static u32 bgm_pos;
static u32 bgm_len;
static u8  bgm_on;

/* SFX 상태 */
static const s8 *sfx_src;
static u32 sfx_pos;
static u32 sfx_len;

void sound_init(void) {
    REG_SNDSTAT = SSTAT_ENABLE;

    REG_SNDDSCNT = SDS_A100 | SDS_B100
                 | SDS_AL | SDS_AR | SDS_BL | SDS_BR
                 | SDS_ATMR0 | SDS_BTMR0;

    REG_TM0D = TIMER_VAL;
    REG_TM0CNT = TM_ENABLE | TM_FREQ_SYS;

    memset(buf_a, 0, sizeof(buf_a));
    memset(buf_b, 0, sizeof(buf_b));
    cur_buf = 0;
    frac_acc = 0;

    /* 무음 버퍼로 DMA 시작 */
    REG_DMA1SAD = (u32)buf_a[0];
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    REG_DMA2SAD = (u32)buf_b[0];
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
    /* 이번 프레임 소비 샘플 수 계산 (분수 누적) */
    u32 spf = SPF_WHOLE;
    frac_acc += SPF_FRAC_NUM;
    if (frac_acc >= SPF_FRAC_DEN) {
        frac_acc -= SPF_FRAC_DEN;
        spf++;
    }

    u8 next = cur_buf ^ 1;

    /* ── BGM 버퍼 채우기 ── */
    if (bgm_on) {
        /*
         * BUF_SIZE 샘플을 복사 (DMA 안전 마진 확보)
         * 위치는 spf만큼만 전진 (실제 재생량에 맞춤)
         */
        u32 left = bgm_len - bgm_pos;
        if (left >= BUF_SIZE) {
            memcpy(buf_a[next], bgm_src + bgm_pos, BUF_SIZE);
        } else {
            /* 루프 경계: 끝부분 + 처음부터 이어 복사 */
            memcpy(buf_a[next], bgm_src + bgm_pos, left);
            memcpy(buf_a[next] + left, bgm_src, BUF_SIZE - left);
        }
        bgm_pos += spf;
        if (bgm_pos >= bgm_len)
            bgm_pos -= bgm_len;
    } else {
        memset(buf_a[next], 0, BUF_SIZE);
    }

    /* ── SFX 버퍼 채우기 ── */
    if (sfx_len > 0) {
        u32 left = sfx_len - sfx_pos;
        u32 n = (left < BUF_SIZE) ? left : BUF_SIZE;
        memcpy(buf_b[next], sfx_src + sfx_pos, n);
        if (n < BUF_SIZE)
            memset(buf_b[next] + n, 0, BUF_SIZE - n);
        sfx_pos += spf;
        if (sfx_pos >= sfx_len)
            sfx_len = 0;
    } else {
        memset(buf_b[next], 0, BUF_SIZE);
    }

    /* ── DMA 스왑 ── */
    REG_DMA1CNT = 0;
    REG_DMA1SAD = (u32)buf_a[next];
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    REG_DMA2CNT = 0;
    REG_DMA2SAD = (u32)buf_b[next];
    REG_DMA2DAD = (u32)&REG_FIFO_B;
    REG_DMA2CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT
                | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

    cur_buf = next;
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
