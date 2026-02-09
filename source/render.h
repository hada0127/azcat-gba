#ifndef RENDER_H
#define RENDER_H

#ifdef PLATFORM_GBA

#include "types.h"
#include "game.h"
#include "gameover.h"

/* ── OAM 슬롯 할당 (낮은 인덱스 = 앞에 표시) ── */
/* 레이어: UI > 히트이펙트 > 아이템 > 고양이 > 플레이어 */
#define OAM_FACE        0
#define OAM_SCORE_START 1   /* 1-5: 5자리 점수 */
#define OAM_BOMB_ICON   6   /* 폭탄 보유 표시 */
#define OAM_HIT_START   7   /* 7-10: 충돌 이펙트 (최대 4개) */
#define OAM_HIT_COUNT   4
#define OAM_ITEM        11
#define OAM_CAT_START   12  /* 12-44: 고양이 33마리 */
#define OAM_PLAYER      45
#define OAM_COUNT       46

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
#define TID_FACE_HAPPY  768  /* 32타일 32x64 */
#define TID_FACE_NORMAL 800  /* 32타일 32x64 */
#define TID_FACE_HURT   832  /* 32타일 32x64 */
#define TID_FACE_DEAD   864  /* 32타일 32x64 */
#define TID_FONT        896  /* 0~9 숫자 (4타일x10=40타일, 16x16) */
#define TID_BOMB_ICON   936  /* 16타일 32x32 (원본 18x24) */

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
#define PB_FACE_HAPPY   9
#define PB_FACE_NORMAL  10
#define PB_FACE_HURT    11
#define PB_FACE_DEAD    12
#define PB_FONT         13
#define PB_BOMB_ICON    15

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

/* HUD 좌표 */
#define HUD_FACE_X      207  /* 240-32-1 = 오른쪽 1px 여백 */
#define HUD_FACE_Y      1    /* 위쪽 1px 여백 */
#define HUD_BOMB_X      207  /* 얼굴과 같은 X */
#define HUD_BOMB_Y      25   /* 얼굴 하단부 겹침 */
#define HUD_SCORE_X     88  /* 중앙 (240-5*12-4)/2 */
#define HUD_SCORE_Y     0   /* 상단 */
#define HUD_DIGIT_SPACE 12  /* 16x16 숫자 간격 */

void render_init(void);
void render_set_bg(u8 bg_type);
void render_set_bomb_bg(void);
void render_set_title_bg(void);
void render_sprites(const GameState* gs);
void render_hud(const GameState* gs);
void render_title_hud(s16 hiscore);
void render_gameover_screen(const GameState* gs, const GameOverResult* result);
void render_gameover_load_ui(u8 grade_index);
void render_restore_face_tiles(void);
void render_hide_all(void);
void render_oam_update(void);

#endif /* PLATFORM_GBA */
#endif /* RENDER_H */
