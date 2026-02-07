#ifndef RENDER_H
#define RENDER_H

#ifdef PLATFORM_GBA

#include "types.h"
#include "game.h"
#include "gameover.h"

/* ── OAM 슬롯 할당 ── */
#define OAM_PLAYER      0
#define OAM_CAT_START   1
/* OAM_CAT_END = OAM_CAT_START + MAX_CATS - 1 = 33 */
#define OAM_ITEM        34
#define OAM_EXPLOSION   35
#define OAM_FACE        36
#define OAM_SCORE_START 37  /* 5자리 점수 */
#define OAM_BOMB_ICON   42  /* 폭탄 보유 표시 */
#define OAM_COUNT       43

/* ── OBJ 타일 ID (Mode 4: 512부터) ── */
#define TID_PLAYER_W0   512  /* 16타일 32x32 걷기0 */
#define TID_PLAYER_W1   528  /* 16타일 32x32 걷기1 */
#define TID_PLAYER_W2   544  /* 16타일 32x32 걷기2 */
#define TID_PLAYER_DEAD 560  /* 32타일 64x32 사망  */
#define TID_CAT_WHITE   592  /*  8타일 16x32 */
#define TID_CAT_BROWN   600  /*  8타일 16x32 */
#define TID_ITEM_HP     608  /*  4타일 16x16 */
#define TID_ITEM_BOMB   612  /*  4타일 16x16 */
#define TID_EXPLOSION   616  /* 16타일 32x32 */
#define TID_FACE_HAPPY  632  /*  4타일 16x16 */
#define TID_FACE_NORMAL 636
#define TID_FACE_HURT   640
#define TID_FACE_DEAD   644
#define TID_FONT        648  /* 0~9 숫자 (10타일) */

/* ── OBJ 팔레트 뱅크 ── */
#define PB_PLAYER       0
#define PB_CAT_WHITE    1
#define PB_CAT_BROWN    2
#define PB_ITEM_HP      3
#define PB_ITEM_BOMB    4
#define PB_EXPLOSION    5
#define PB_FACE_HAPPY   6
#define PB_FACE_NORMAL  7
#define PB_FACE_HURT    8
#define PB_FACE_DEAD    9
#define PB_FONT         10

/* 배경 타입 상수 (title 추가) */
#define BG_TITLE_SCREEN 0xFF

/* 플레이어 렌더 Y (화면 하단) */
#define PLAYER_RENDER_Y (SCREEN_H - 32)

/* UI 패널 좌표 (오른쪽 60px 영역) */
#define UI_PANEL_X      (PLAY_AREA_W + 2)  /* 182 */

void render_init(void);
void render_set_bg(u8 bg_type);
void render_set_title_bg(void);
void render_sprites(const GameState* gs);
void render_hud(const GameState* gs);
void render_title_hud(s16 hiscore);
void render_gameover_screen(const GameState* gs, const GameOverResult* result);
void render_gameover_grade(u8 grade_index);
void render_hide_all(void);
void render_oam_update(void);

#endif /* PLATFORM_GBA */
#endif /* RENDER_H */
