#ifndef TYPES_H
#define TYPES_H

#include "platform.h"

/* ── 고정소수점 (.8 fixed point) ── */
#define FP_SHIFT 8
#define FP(x)              ((s32)(x) << FP_SHIFT)
#define FP_TO_INT(x)       ((x) >> FP_SHIFT)
#define FP_MUL(a, b)       (((a) * (b)) >> FP_SHIFT)
#define FP_FROM_FLOAT_CONST(f) ((s32)((f) * (1 << FP_SHIFT)))

/* ── 화면 / 레이아웃 ── */
#define SCREEN_W            240
#define SCREEN_H            160
#define PLAY_AREA_W         SCREEN_W  /* 전체 화면 사용 */
#define HUD_HEIGHT               16
#define GAME_AREA_H         (SCREEN_H - HUD_HEIGHT)

/* ── 60fps 보정 계수 (원본 ~12fps → GBA 60fps) ── */
#define FRAME_RATE_RATIO    5

/* ── 플레이어 ── */
#define PLAYER_INIT_X       FP(104)  /* (SCREEN_W - 32) / 2 */
#define PLAYER_X_MIN        FP(0)
#define PLAYER_X_MAX        FP(214)  /* SCREEN_W - 26 (affine 2/3 시각 폭) */
#define PLAYER_BASE_SPEED   FP_FROM_FLOAT_CONST(1.0f)    /* FP(5)/5 */
#define PLAYER_ACCEL_INC    FP_FROM_FLOAT_CONST(0.04f)   /* 0.2/5 */
#define PLAYER_ACCEL_MAX    4
#define PLAYER_LIFE_MAX     3
#define PLAYER_LIFE_INIT    3

/* ── 고양이 ── */
#define MAX_CATS            33
#define CAT_SPAWN_X_MIN     1
#define CAT_SPAWN_X_MAX     214  /* SCREEN_W - 26 (affine 2/3) */
#define CAT_SPAWN_Y_MIN     (-120)
#define CAT_SPAWN_Y_MAX     (-20)
#define CAT_LAND_Y          FP(131)  /* 바닥=157, OAM bottom=131+26=157 */
#define CAT_HIT_Y_MIN       FP(108)  /* 충돌 시작 Y를 아래로 */
#define CAT_HIT_Y_MAX       FP(131)  /* = CAT_LAND_Y */
#define CAT_GRAVITY         FP_FROM_FLOAT_CONST(0.04f)   /* 0.2/5 */
#define CAT_BASE_FALL       FP_FROM_FLOAT_CONST(0.08f)   /* 0.4/5 */
#define CAT_EXIT_Y_OFFSET   FP(6)   /* SIT→EXIT 스프라이트 전환 시 Y 보정 */
#define CAT_EXIT_ANIM_LEN   8   /* EXIT 확대 애니메이션 프레임 수 */
#define CAT_SIT_FRAMES      15  /* 착지 체류 프레임 (0.25초@60fps) */
#define CAT_HIT_FRAMES      15  /* 충돌 이펙트 지속 프레임 */

/* ── 아이템 ── */
#define ITEM_SPAWN_X_MIN    1
#define ITEM_SPAWN_X_MAX    214  /* SCREEN_W - 26 (affine 2/3) */
#define ITEM_SPAWN_Y        FP(-24)
#define ITEM_LAND_Y         FP(131)  /* 바닥=157, OAM bottom=131+26=157 */
#define ITEM_GRAVITY        FP_FROM_FLOAT_CONST(0.1f)    /* 0.5/5 */
#define ITEM_BASE_FALL      FP_FROM_FLOAT_CONST(0.2f)    /* FP(1)/5 */
#define ITEM_SIT_TIMEOUT    (45 * FRAME_RATE_RATIO)
#define ITEM_SPAWN_INTERVAL (150 * FRAME_RATE_RATIO)
#define ITEM_TYPE_HP        1
#define ITEM_TYPE_BOMB      2
#define ITEM_TYPE_POISON    3
#define ITEM_TYPE_SPEED     4
#define ITEM_TYPE_COUNT     4

/* ── 폭탄 ── */
#define BOMB_DURATION       (20 * FRAME_RATE_RATIO)

/* ── 배경 전환 점수 ── */
#define BG_THRESHOLD_NIGHT  500
#define BG_THRESHOLD_MATRIX 900

/* ── 배경 타입 ── */
#define BG_DAY              0
#define BG_NIGHT            1
#define BG_MATRIX           2

/* ── 게임 상태 ── */
#define STATE_TITLE         0
#define STATE_PLAY          1
#define STATE_GAMEOVER      2

/* ── 등급 ── */
#define GRADE_COUNT         21

/* ── 히트박스 ── */
typedef struct {
    s32 x, y, w, h;
} HitBox;

#endif /* TYPES_H */
