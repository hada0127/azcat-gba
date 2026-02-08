#include "sound.h"

#ifdef PLATFORM_GBA

#include <tonc.h>

/* ── 음계 (rate = 2048 - 131072/Hz) ── */
#define NOTE_C3  1048
#define NOTE_G3  1379
#define NOTE_C4  1548
#define NOTE_D4  1602
#define NOTE_E4  1651
#define NOTE_F4  1672
#define NOTE_G4  1714
#define NOTE_A4  1750
#define NOTE_B4  1783
#define NOTE_C5  1797
#define NOTE_D5  1825
#define NOTE_REST 0

/* BGM 노트 간격 (프레임) */
#define BGM_STEP 10

/* BGM 멜로디 (채널 1) */
static const u16 bgm_melody[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_G4,
    NOTE_E4, NOTE_C4, NOTE_D4, NOTE_E4,
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_C5,
    NOTE_A4, NOTE_G4, NOTE_E4, NOTE_REST,
};
#define BGM_MELODY_LEN (sizeof(bgm_melody) / sizeof(bgm_melody[0]))

/* BGM 베이스 (채널 2) */
static const u16 bgm_bass[] = {
    NOTE_C3, NOTE_REST, NOTE_C3, NOTE_REST,
    NOTE_C3, NOTE_REST, NOTE_G3, NOTE_REST,
    NOTE_C3, NOTE_REST, NOTE_C3, NOTE_REST,
    NOTE_G3, NOTE_REST, NOTE_C3, NOTE_REST,
};
#define BGM_BASS_LEN (sizeof(bgm_bass) / sizeof(bgm_bass[0]))

/* 상태 */
static u8  bgm_on;
static u16 bgm_tick;
static u8  bgm_idx;
static u8  sfx_ch1_lock;   /* SFX가 채널1 점유 중이면 >0 */

void sound_init(void) {
    /* 마스터 사운드 활성화 */
    REG_SNDSTAT = SSTAT_ENABLE;

    /* DMG 채널 볼륨 최대, 채널 1+2+4 양쪽 스피커 */
    REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1 | SDMG_SQR2 | SDMG_NOISE, 7);

    /* DMG 채널 100% 볼륨 */
    REG_SNDDSCNT = SDS_DMG100;

    bgm_on = 0;
    bgm_tick = 0;
    bgm_idx = 0;
    sfx_ch1_lock = 0;
}

static void play_note_ch1(u16 rate) {
    if (rate == NOTE_REST) {
        REG_SND1CNT = SSQR_DUTY1_2 | SSQR_IVOL(0);
        return;
    }
    REG_SND1SWEEP = 0;
    REG_SND1CNT = SSQR_DUTY1_2 | SSQR_IVOL(8);
    REG_SND1FREQ = SFREQ_RESET | rate;
}

static void play_note_ch2(u16 rate) {
    if (rate == NOTE_REST) {
        REG_SND2CNT = SSQR_DUTY1_4 | SSQR_IVOL(0);
        return;
    }
    REG_SND2CNT = SSQR_DUTY1_4 | SSQR_IVOL(5);
    REG_SND2FREQ = SFREQ_RESET | rate;
}

void sound_play_bgm(u8 track) {
    (void)track;
    bgm_on = 1;
    bgm_tick = 0;
    bgm_idx = 0;
}

void sound_play_sfx(u8 sfx_id) {
    switch (sfx_id) {
    case SFX_HIT:
        /* 노이즈 버스트 (채널4) */
        REG_SND4CNT = SSQR_IVOL(15) | SSQR_DEC | SSQR_TIME(3);
        REG_SND4FREQ = SFREQ_RESET | (3 << 4);
        break;

    case SFX_GAMEOVER:
        /* 하강 스윕 (채널1) — BGM 중지 */
        bgm_on = 0;
        REG_SND1SWEEP = SSW_BUILD(4, 1, 3);
        REG_SND1CNT = SSQR_DUTY1_2 | SSQR_IVOL(12) | SSQR_DEC | SSQR_TIME(1);
        REG_SND1FREQ = SFREQ_RESET | NOTE_C5;
        sfx_ch1_lock = 90;
        /* 베이스도 정지 */
        REG_SND2CNT = SSQR_IVOL(0);
        break;

    case SFX_BOMB:
        /* 폭발 노이즈 (채널4) */
        REG_SND4CNT = SSQR_IVOL(12) | SSQR_DEC | SSQR_TIME(4);
        REG_SND4FREQ = SFREQ_RESET | (6 << 4) | 3;
        break;

    case SFX_ITEM:
        /* 짧은 비프 (채널1) */
        REG_SND1SWEEP = 0;
        REG_SND1CNT = SSQR_DUTY1_4 | SSQR_IVOL(10) | SSQR_DEC | SSQR_TIME(2);
        REG_SND1FREQ = SFREQ_RESET | NOTE_A4;
        sfx_ch1_lock = 8;
        break;
    }
}

void sound_update(void) {
    /* SFX 채널1 잠금 카운트 */
    if (sfx_ch1_lock > 0)
        sfx_ch1_lock--;

    if (!bgm_on) return;

    bgm_tick++;
    if (bgm_tick >= BGM_STEP) {
        bgm_tick = 0;

        /* 멜로디 (채널1) — SFX 점유 중이 아니면 */
        if (sfx_ch1_lock == 0) {
            play_note_ch1(bgm_melody[bgm_idx % BGM_MELODY_LEN]);
        }

        /* 베이스 (채널2) */
        play_note_ch2(bgm_bass[bgm_idx % BGM_BASS_LEN]);

        bgm_idx++;
        if (bgm_idx >= BGM_MELODY_LEN)
            bgm_idx = 0;
    }
}

void sound_stop(void) {
    bgm_on = 0;
    REG_SND1CNT = 0;
    REG_SND2CNT = 0;
    REG_SND4CNT = 0;
}

#else
/* ── PC 스텁 ── */
void sound_init(void) { (void)0; }
void sound_play_bgm(u8 track) { (void)track; }
void sound_play_sfx(u8 sfx_id) { (void)sfx_id; }
void sound_update(void) { (void)0; }
void sound_stop(void) { (void)0; }
#endif
