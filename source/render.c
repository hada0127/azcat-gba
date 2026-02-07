#ifdef PLATFORM_GBA

#include <tonc.h>
#include <string.h>
#include "render.h"
#include "hud.h"

/* 에셋 데이터 */
#include "bg_title.h"
#include "bg_day.h"
#include "bg_night.h"
#include "bg_matrix.h"
#include "spr_player_walk0.h"
#include "spr_player_walk1.h"
#include "spr_player_walk2.h"
#include "spr_player_dead.h"
#include "spr_cat_white.h"
#include "spr_cat_brown.h"
#include "spr_item_hp.h"
#include "spr_item_bomb.h"
#include "spr_explosion.h"
#include "spr_face_happy.h"
#include "spr_face_normal.h"
#include "spr_face_hurt.h"
#include "spr_face_dead.h"
#include "font_numbers.h"
#include "grade_images.h"

/* OAM 섀도우 버퍼 */
static OBJ_ATTR obj_buffer[128];

/* ── 초기화: 스프라이트 타일+팔레트 → VRAM ── */
void render_init(void) {
    oam_init(obj_buffer, 128);

    /* Mode 4에서 OBJ 타일은 tile_mem[5] (ID 512~)부터 안전 */

    /* 타일 로드 (TID 오프셋 = tile_mem[5][TID-512]) */
    memcpy32(&tile_mem[5][0],   spr_player_walk0Tiles, spr_player_walk0TilesLen / 4); /* 512: 16타일 */
    memcpy32(&tile_mem[5][16],  spr_player_walk1Tiles, spr_player_walk1TilesLen / 4); /* 528: 16타일 */
    memcpy32(&tile_mem[5][32],  spr_player_walk2Tiles, spr_player_walk2TilesLen / 4); /* 544: 16타일 */
    memcpy32(&tile_mem[5][48],  spr_player_deadTiles,  spr_player_deadTilesLen / 4);  /* 560: 32타일 */
    memcpy32(&tile_mem[5][80],  spr_cat_whiteTiles,    spr_cat_whiteTilesLen / 4);    /* 592: 8타일 */
    memcpy32(&tile_mem[5][88],  spr_cat_brownTiles,    spr_cat_brownTilesLen / 4);    /* 600: 8타일 */
    memcpy32(&tile_mem[5][96],  spr_item_hpTiles,      spr_item_hpTilesLen / 4);     /* 608: 4타일 */
    memcpy32(&tile_mem[5][100], spr_item_bombTiles,    spr_item_bombTilesLen / 4);    /* 612: 4타일 */
    memcpy32(&tile_mem[5][104], spr_explosionTiles,    spr_explosionTilesLen / 4);    /* 616: 16타일 */
    memcpy32(&tile_mem[5][120], spr_face_happyTiles,   spr_face_happyTilesLen / 4);  /* 632: 4타일 */
    memcpy32(&tile_mem[5][124], spr_face_normalTiles,  spr_face_normalTilesLen / 4);  /* 636 */
    memcpy32(&tile_mem[5][128], spr_face_hurtTiles,    spr_face_hurtTilesLen / 4);    /* 640 */
    memcpy32(&tile_mem[5][132], spr_face_deadTiles,    spr_face_deadTilesLen / 4);    /* 644 */
    memcpy32(&tile_mem[5][136], font_numbersTiles,     font_numbersTilesLen / 4);     /* 648 */

    /* OBJ 팔레트 로드 (walk0 팔레트 = 공유 팔레트) */
    memcpy16(pal_obj_bank[PB_PLAYER],      spr_player_walk0Pal, spr_player_walk0PalLen / 2);
    memcpy16(pal_obj_bank[PB_CAT_WHITE],   spr_cat_whitePal,   spr_cat_whitePalLen / 2);
    memcpy16(pal_obj_bank[PB_CAT_BROWN],   spr_cat_brownPal,   spr_cat_brownPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_HP],     spr_item_hpPal,     spr_item_hpPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_BOMB],   spr_item_bombPal,   spr_item_bombPalLen / 2);
    memcpy16(pal_obj_bank[PB_EXPLOSION],   spr_explosionPal,   spr_explosionPalLen / 2);
    memcpy16(pal_obj_bank[PB_FACE_HAPPY],  spr_face_happyPal,  spr_face_happyPalLen / 2);
    memcpy16(pal_obj_bank[PB_FACE_NORMAL], spr_face_normalPal, spr_face_normalPalLen / 2);
    memcpy16(pal_obj_bank[PB_FACE_HURT],   spr_face_hurtPal,   spr_face_hurtPalLen / 2);
    memcpy16(pal_obj_bank[PB_FACE_DEAD],   spr_face_deadPal,   spr_face_deadPalLen / 2);
    memcpy16(pal_obj_bank[PB_FONT],        font_numbersPal,    font_numbersPalLen / 2);
}

/* ── 배경 전환 (양쪽 페이지에 복사) ── */
void render_set_bg(u8 bg_type) {
    const unsigned int*   bmp;
    const unsigned short* pal;
    u32 bmp_len;

    switch (bg_type) {
    case BG_NIGHT:
        bmp = bg_nightBitmap;  pal = bg_nightPal;  bmp_len = bg_nightBitmapLen;  break;
    case BG_MATRIX:
        bmp = bg_matrixBitmap; pal = bg_matrixPal;  bmp_len = bg_matrixBitmapLen; break;
    default: /* BG_DAY */
        bmp = bg_dayBitmap;    pal = bg_dayPal;     bmp_len = bg_dayBitmapLen;    break;
    }

    memcpy16(pal_bg_mem, pal, 256);
    /* 양쪽 페이지에 복사 (Mode 4 더블버퍼) */
    memcpy32((void*)0x06000000, bmp, bmp_len / 4);
    memcpy32((void*)0x0600A000, bmp, bmp_len / 4);
}

void render_set_title_bg(void) {
    memcpy16(pal_bg_mem, bg_titlePal, 256);
    memcpy32((void*)0x06000000, bg_titleBitmap, bg_titleBitmapLen / 4);
    memcpy32((void*)0x0600A000, bg_titleBitmap, bg_titleBitmapLen / 4);
}

/* ── 스프라이트 렌더링 ── */
void render_sprites(const GameState* gs) {
    int i;

    /* 플레이어 */
    if (gs->player.state == 0) {
        int px = FP_TO_INT(gs->player.x);
        u16 flip = (gs->player.direction == DIR_RIGHT) ? ATTR1_HFLIP : 0;

        /* 걷기 애니메이션: 정지=W0, 이동=W0→W1→W2 순환 (6프레임마다 전환) */
        u16 tid;
        if (gs->player.anim_counter == 0) {
            tid = TID_PLAYER_W0;
        } else {
            static const u16 walk_tids[] = {TID_PLAYER_W0, TID_PLAYER_W1, TID_PLAYER_W2};
            u8 frame = (gs->player.anim_counter / 6) % 3;
            tid = walk_tids[frame];
        }

        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_32 | flip,
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(tid));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, PLAYER_RENDER_Y);
    } else {
        /* 사망: 64x32 WIDE 스프라이트 */
        int px = FP_TO_INT(gs->player.x) - 16;  /* 중앙 보정 */
        u16 flip = (gs->player.direction == DIR_RIGHT) ? ATTR1_HFLIP : 0;
        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_WIDE | ATTR0_4BPP,
            ATTR1_SIZE_64 | flip,
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(TID_PLAYER_DEAD));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, PLAYER_RENDER_Y);
    }

    /* 고양이 */
    for (i = 0; i < MAX_CATS; i++) {
        int oam = OAM_CAT_START + i;
        if (gs->cats[i].state != CAT_STATE_INACTIVE) {
            int cx = FP_TO_INT(gs->cats[i].x);
            int cy = FP_TO_INT(gs->cats[i].y);
            u16 tid = (i & 1) ? TID_CAT_BROWN : TID_CAT_WHITE;
            u16 pb  = (i & 1) ? PB_CAT_BROWN  : PB_CAT_WHITE;
            obj_set_attr(&obj_buffer[oam],
                ATTR0_TALL | ATTR0_4BPP,
                ATTR1_SIZE_32,
                ATTR2_PALBANK(pb) | ATTR2_ID(tid));
            obj_set_pos(&obj_buffer[oam], cx, cy);
        } else {
            obj_hide(&obj_buffer[oam]);
        }
    }

    /* 아이템 */
    if (gs->item.active) {
        int ix = FP_TO_INT(gs->item.x);
        int iy = FP_TO_INT(gs->item.y);
        u16 tid, pb;
        if (gs->item.type == ITEM_TYPE_BOMB) {
            tid = TID_ITEM_BOMB; pb = PB_ITEM_BOMB;
        } else {
            tid = TID_ITEM_HP; pb = PB_ITEM_HP;
        }
        obj_set_attr(&obj_buffer[OAM_ITEM],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_16,
            ATTR2_PALBANK(pb) | ATTR2_ID(tid));
        obj_set_pos(&obj_buffer[OAM_ITEM], ix, iy);
    } else {
        obj_hide(&obj_buffer[OAM_ITEM]);
    }

    /* 폭탄 이펙트 */
    if (bomb_is_active(&gs->bomb)) {
        obj_set_attr(&obj_buffer[OAM_EXPLOSION],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_32,
            ATTR2_PALBANK(PB_EXPLOSION) | ATTR2_ID(TID_EXPLOSION));
        obj_set_pos(&obj_buffer[OAM_EXPLOSION], 104, 64);
    } else {
        obj_hide(&obj_buffer[OAM_EXPLOSION]);
    }
}

/* ── 숫자 5자리를 OAM으로 표시 ── */
static void render_digits(s16 value, int oam_start, int x, int y) {
    char buf[6];
    int i;
    hud_format_score(value, buf);
    for (i = 0; i < 5; i++) {
        int digit = buf[i] - '0';
        obj_set_attr(&obj_buffer[oam_start + i],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_8,
            ATTR2_PALBANK(PB_FONT) | ATTR2_ID(TID_FONT + digit));
        obj_set_pos(&obj_buffer[oam_start + i], x + i * 8, y);
    }
}

/* ── HUD 렌더링 (플레이 중) ── */
void render_hud(const GameState* gs) {
    /* 얼굴 아이콘 */
    u8 face = hud_get_face_index(gs->player.life);
    u16 face_tid, face_pb;
    switch (face) {
    case FACE_HAPPY:  face_tid = TID_FACE_HAPPY;  face_pb = PB_FACE_HAPPY;  break;
    case FACE_NORMAL: face_tid = TID_FACE_NORMAL; face_pb = PB_FACE_NORMAL; break;
    case FACE_HURT:   face_tid = TID_FACE_HURT;   face_pb = PB_FACE_HURT;   break;
    default:          face_tid = TID_FACE_DEAD;   face_pb = PB_FACE_DEAD;   break;
    }
    obj_set_attr(&obj_buffer[OAM_FACE],
        ATTR0_SQUARE | ATTR0_4BPP,
        ATTR1_SIZE_16,
        ATTR2_PALBANK(face_pb) | ATTR2_ID(face_tid));
    obj_set_pos(&obj_buffer[OAM_FACE], 2, 0);

    /* 점수 (얼굴 옆) */
    render_digits(gs->score, OAM_SCORE_START, 20, 4);

    /* 폭탄 보유 아이콘 */
    if (gs->bomb.have) {
        obj_set_attr(&obj_buffer[OAM_BOMB_ICON],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_16,
            ATTR2_PALBANK(PB_ITEM_BOMB) | ATTR2_ID(TID_ITEM_BOMB));
        obj_set_pos(&obj_buffer[OAM_BOMB_ICON], SCREEN_W - 18, 0);
    } else {
        obj_hide(&obj_buffer[OAM_BOMB_ICON]);
    }
}

/* ── 타이틀 HUD (하이스코어 표시) ── */
void render_title_hud(s16 hiscore) {
    /* 하이스코어 숫자를 화면 중앙 하단에 표시 */
    render_digits(hiscore, OAM_SCORE_START, 100, SCREEN_H - 16);

    /* 나머지 HUD 슬롯 숨기기 */
    obj_hide(&obj_buffer[OAM_FACE]);
    obj_hide(&obj_buffer[OAM_BOMB_ICON]);
}

/* ── 등급 이미지를 프레임버퍼에 블릿 (Mode 4, 8bpp) ── */
#define GRADE_PAL_IDX 255  /* 등급 텍스트용 팔레트 인덱스 */

void render_gameover_grade(u8 grade_index) {
    if (grade_index >= GRADE_COUNT) return;

    /* 팔레트 인덱스 255에 흰색 설정 */
    pal_bg_mem[GRADE_PAL_IDX] = RGB15(31, 31, 31);

    const unsigned char* src = grade_image_data[grade_index];
    /* 화면 중앙 배치 */
    int ox = (SCREEN_W - GRADE_IMG_W) / 2;
    int oy = SCREEN_H / 2 + 10;

    u16* page0 = (u16*)0x06000000;
    u16* page1 = (u16*)0x0600A000;

    int y, x;
    for (y = 0; y < GRADE_IMG_H; y++) {
        int row = oy + y;
        if (row >= SCREEN_H) break;
        u16* dst0 = &page0[row * (SCREEN_W / 2)];
        u16* dst1 = &page1[row * (SCREEN_W / 2)];
        for (x = 0; x < GRADE_IMG_W; x += 2) {
            int px = ox + x;
            if (px >= SCREEN_W - 1) break;
            u8 lo = src[y * GRADE_IMG_W + x] ? GRADE_PAL_IDX : 0;
            u8 hi = src[y * GRADE_IMG_W + x + 1] ? GRADE_PAL_IDX : 0;
            if (lo || hi) {
                int idx = px / 2;
                u16 old = dst0[idx];
                /* 투명(0)인 부분은 배경 유지 */
                u8 final_lo = lo ? lo : (u8)(old & 0xFF);
                u8 final_hi = hi ? hi : (u8)(old >> 8);
                u16 val = (final_hi << 8) | final_lo;
                dst0[idx] = val;
                dst1[idx] = val;
            }
        }
    }
}

/* ── 게임오버 화면 ── */
void render_gameover_screen(const GameState* gs, const GameOverResult* result) {
    /* 게임 스프라이트 유지 (프리즈 상태) */
    /* 얼굴: 사망 */
    obj_set_attr(&obj_buffer[OAM_FACE],
        ATTR0_SQUARE | ATTR0_4BPP,
        ATTR1_SIZE_16,
        ATTR2_PALBANK(PB_FACE_DEAD) | ATTR2_ID(TID_FACE_DEAD));
    obj_set_pos(&obj_buffer[OAM_FACE], 2, 0);

    /* 최종 점수 */
    render_digits(gs->score, OAM_SCORE_START, 20, 4);

    /* 폭탄 아이콘 숨김 */
    obj_hide(&obj_buffer[OAM_BOMB_ICON]);
}

/* ── 모든 스프라이트 숨기기 ── */
void render_hide_all(void) {
    int i;
    for (i = 0; i < 128; i++) {
        obj_hide(&obj_buffer[i]);
    }
}

/* ── OAM 섀도우 → 하드웨어 복사 ── */
void render_oam_update(void) {
    oam_copy(oam_mem, obj_buffer, 128);
}

#endif /* PLATFORM_GBA */
