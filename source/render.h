#ifndef RENDER_H
#define RENDER_H

#ifdef PLATFORM_GBA

#include "types.h"
#include "game.h"
#include "gameover.h"

/* ── OAM 슬롯 할당 (낮은 인덱스 = 앞에 표시) ── */
/* 레이어: UI > 히트이펙트 > 아이템 > 고양이 > 플레이어 */
#define OAM_HEART_START 0   /* 0-2: HP 하트 아이콘 (최전면) */
#define OAM_HEART_COUNT 3
#define OAM_SCORE_START 3   /* 3-7: 5자리 점수 */
#define OAM_HUD_BOMB    8   /* 폭탄 보유 표시 */
#define OAM_HIT_START   9   /* 9-12: 충돌 이펙트 (최대 4개) */
#define OAM_HIT_COUNT   4
#define OAM_ITEM        13
#define OAM_CAT_START   14  /* 14-61: 고양이 48마리 */
#define OAM_PLAYER      62
#define OAM_PAUSE       63  /* PAUSE 텍스트 */
#define OAM_COUNT       64

/* ── OBJ 타일 ID (Mode 4: 512부터) ── */
/* 원본 스프라이트 사이즈: 스케일링 없이 OAM 캔버스에 패딩 */
#define TID_PLAYER_W0   512  /* 32타일 32x64 걷기0 (원본 30x53) */
#define TID_PLAYER_W1   544  /* 32타일 32x64 걷기1 (원본 27x56) */
#define TID_PLAYER_W2   576  /* 32타일 32x64 걷기2 (원본 27x58) */
#define TID_PLAYER_DEAD 608  /* 32타일 64x32 사망  (원본 54x25) */
#define TID_CAT_WHITE   640  /* 16타일 32x32 (원본 17x32) */
#define TID_CAT_BROWN   656  /* 16타일 32x32 */
#define TID_CAT_SIT     672  /* 16타일 32x32 (원본 17x32) */
#define TID_ITEM_HP     688  /* 16타일 32x32 (원본 20x22) */
#define TID_ITEM_BOMB   704  /* 16타일 32x32 */
#define TID_ITEM_POISON 720  /* 16타일 32x32 */
#define TID_ITEM_SPEED  736  /* 16타일 32x32 */
#define TID_EXPLOSION   752  /* 16타일 32x32 (원본 29x26) */
#define TID_FONT        896  /* 0~9 숫자 (4타일x10=40타일, 16x16) */
#define TID_HUD_HEART   936  /* 4타일 16x16 (하트 HUD) */
#define TID_HUD_BOMB    940  /* 4타일 16x16 (폭탄 HUD) */
#define TID_HISCORE_TXT 944  /* 32타일 64x32 (High Score 텍스트) */
#define TID_FONT_SMALL  976  /* 10타일 8x8 (작은 숫자 0~9) */
#define TID_PAUSE       988  /* 32타일 64x32 (PAUSE 텍스트) */

/* ── OBJ 팔레트 뱅크 ── */
#define PB_PLAYER       0
#define PB_CAT_WHITE    1
#define PB_CAT_BROWN    2
#define PB_CAT_SIT      3
#define PB_ITEM_HP      4
#define PB_ITEM_BOMB    5
#define PB_ITEM_POISON  6
#define PB_ITEM_SPEED   7
#define PB_EXPLOSION    8
#define PB_HUD_HEART    9
#define PB_HUD_BOMB     10
#define PB_HISCORE_TXT  11
#define PB_FONT_SMALL   12
#define PB_FONT         13
#define PB_PAUSE        15  /* PAUSE 텍스트: 0=투명, 1=흰색, 2=검정 */

/* 배경 타입 상수 (title 추가) */
#define BG_TITLE_SCREEN 0xFF

/* 게임오버 레이아웃 좌표 */
#define GO_GRADE_Y      45
#define GO_SCORE_Y      68
#define GO_NAV_Y        100

/* 플레이어 렌더 Y (affine 2/3 스케일링 보정) */
#define PLAYER_RENDER_Y 104

/* 렌더 오프셋 (원본→OAM 패딩 보정) */
#define CAT_RENDER_OX   7   /* (32-17)/2 = 7.5 → 7 */
#define ITEM_RENDER_OX  6   /* (32-20)/2 = 6 */
#define ITEM_RENDER_OY  0   /* affine 센터링이 대체 */

/* OBJ Affine 스케일링 (게임 스프라이트 2/3 표시) */
#define AFFINE_SCALE    384  /* 3/2 in .8 fixed = 2/3 display size */
#define AFFINE_IDX      0    /* 게임 스프라이트 공용 affine 매트릭스 인덱스 */
#define AFFINE_IDX_FLIP 1    /* 좌우반전 affine (HFLIP 대체) */
#define AFFINE_IDX_EXIT_BASE  2    /* EXIT 확대 affine 시작 인덱스 */
#define AFFINE_EXIT_STEPS     4    /* 확대 단계 수 (인덱스 2~5) */

/* HUD 좌표 */
#define HUD_HEART_X      2   /* 하트 시작 X (좌측 2px 여백) */
#define HUD_HEART_Y      2   /* 하트 Y (상단 2px 여백) */
#define HUD_HEART_SPACE  12  /* 하트 간격 (11px 콘텐츠 + 1px 간격) */
#define HUD_HBOMB_X      2   /* 폭탄 아이콘 X */
#define HUD_HBOMB_Y      14  /* 하트 아래줄 (2+11+1) */
#define HUD_SCORE_X     88  /* 중앙 (240-5*12-4)/2 */
#define HUD_SCORE_Y     0   /* 상단 */
#define HUD_DIGIT_SPACE 12  /* 16x16 숫자 간격 */

/* 타이틀 HUD 좌표 (한 줄 배치) */
#define TITLE_HS_TEXT_X    1   /* High Score 텍스트 X */
#define TITLE_HS_TEXT_Y    0   /* High Score 텍스트 Y */
#define TITLE_HS_NUM_X     46  /* 하이스코어 숫자 X (텍스트 옆) */
#define TITLE_HS_NUM_Y     3   /* 하이스코어 숫자 Y (텍스트 세로 중앙 맞춤) */
#define TITLE_DIGIT_SPACE  7   /* 8x8 숫자 간격 */

void render_init(void);
void render_set_bg(u8 bg_type);
void render_set_bomb_bg(void);
void render_set_title_bg(void);
void render_sprites(const GameState* gs);
void render_hud(const GameState* gs);
void render_title_hud(s16 hiscore);
void render_gameover_screen(const GameState* gs, const GameOverResult* result);
void render_gameover_load_ui(u8 grade_index);
void render_hide_all(void);
void render_oam_update(void);
void render_pause_init(void);
void render_pause_show(void);
void render_pause_hide(void);

#endif /* PLATFORM_GBA */
#endif /* RENDER_H */
