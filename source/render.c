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
#include "spr_cat_sit.h"
#include "spr_item_hp.h"
#include "spr_item_bomb.h"
#include "spr_item_poison.h"
#include "spr_item_speed.h"
#include "spr_explosion.h"
#include "spr_face_happy.h"
#include "spr_face_normal.h"
#include "spr_face_hurt.h"
#include "spr_face_dead.h"
#include "font_numbers.h"
#include "grade_images.h"
#include "nav_text.h"

/* OAM 섀도우 버퍼 */
static OBJ_ATTR obj_buffer[128];

/* ── 초기화: 스프라이트 타일+팔레트 → VRAM ── */
void render_init(void) {
    oam_init(obj_buffer, 128);

    /* Mode 4에서 OBJ 타일은 tile_mem[5] (ID 512~)부터 안전 */

    /* 타일 로드 (TID 오프셋 = tile_mem[5][TID-512]) */
    memcpy32(&tile_mem[5][0],   spr_player_walk0Tiles, spr_player_walk0TilesLen / 4); /* 512: 32타일 32x64 */
    memcpy32(&tile_mem[5][32],  spr_player_walk1Tiles, spr_player_walk1TilesLen / 4); /* 544: 32타일 */
    memcpy32(&tile_mem[5][64],  spr_player_walk2Tiles, spr_player_walk2TilesLen / 4); /* 576: 32타일 */
    memcpy32(&tile_mem[5][96],  spr_player_deadTiles,  spr_player_deadTilesLen / 4);  /* 608: 32타일 64x32 */
    memcpy32(&tile_mem[5][128], spr_cat_whiteTiles,    spr_cat_whiteTilesLen / 4);    /* 640: 16타일 32x32 */
    memcpy32(&tile_mem[5][144], spr_cat_brownTiles,    spr_cat_brownTilesLen / 4);    /* 656: 16타일 */
    memcpy32(&tile_mem[5][160], spr_cat_sitTiles,      spr_cat_sitTilesLen / 4);      /* 672: 16타일 */
    memcpy32(&tile_mem[5][176], spr_item_hpTiles,      spr_item_hpTilesLen / 4);     /* 688: 16타일 32x32 */
    memcpy32(&tile_mem[5][192], spr_item_bombTiles,    spr_item_bombTilesLen / 4);    /* 704: 16타일 */
    memcpy32(&tile_mem[5][208], spr_item_poisonTiles,  spr_item_poisonTilesLen / 4);  /* 720: 16타일 */
    memcpy32(&tile_mem[5][224], spr_item_speedTiles,   spr_item_speedTilesLen / 4);   /* 736: 16타일 */
    memcpy32(&tile_mem[5][240], spr_explosionTiles,    spr_explosionTilesLen / 4);    /* 752: 16타일 */
    memcpy32(&tile_mem[5][256], spr_face_happyTiles,   spr_face_happyTilesLen / 4);  /* 768: 16타일 */
    memcpy32(&tile_mem[5][272], spr_face_normalTiles,  spr_face_normalTilesLen / 4);  /* 784: 16타일 */
    memcpy32(&tile_mem[5][288], spr_face_hurtTiles,    spr_face_hurtTilesLen / 4);    /* 800: 16타일 */
    memcpy32(&tile_mem[5][304], spr_face_deadTiles,    spr_face_deadTilesLen / 4);    /* 816: 16타일 */
    memcpy32(&tile_mem[5][320], font_numbersTiles,     font_numbersTilesLen / 4);     /* 832 */

    /* OBJ 팔레트 로드 (walk0 팔레트 = 공유 팔레트) */
    memcpy16(pal_obj_bank[PB_PLAYER],      spr_player_walk0Pal, spr_player_walk0PalLen / 2);
    memcpy16(pal_obj_bank[PB_CAT_WHITE],   spr_cat_whitePal,   spr_cat_whitePalLen / 2);
    memcpy16(pal_obj_bank[PB_CAT_BROWN],   spr_cat_brownPal,   spr_cat_brownPalLen / 2);
    memcpy16(pal_obj_bank[PB_CAT_SIT],     spr_cat_sitPal,     spr_cat_sitPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_HP],     spr_item_hpPal,     spr_item_hpPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_BOMB],   spr_item_bombPal,   spr_item_bombPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_POISON], spr_item_poisonPal, spr_item_poisonPalLen / 2);
    memcpy16(pal_obj_bank[PB_ITEM_SPEED],  spr_item_speedPal,  spr_item_speedPalLen / 2);
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

        /* 32x64 TALL OAM (원본 ~30x53) */
        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_TALL | ATTR0_4BPP,
            ATTR1_SIZE_64 | flip,
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(tid));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, PLAYER_RENDER_Y);
    } else {
        /* 사망: 64x32 WIDE 스프라이트 (원본 54x25) */
        int px = FP_TO_INT(gs->player.x) - 16;  /* 중앙 보정 */
        u16 flip = (gs->player.direction == DIR_RIGHT) ? ATTR1_HFLIP : 0;
        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_WIDE | ATTR0_4BPP,
            ATTR1_SIZE_64 | flip,
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(TID_PLAYER_DEAD));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, PLAYER_RENDER_Y);
    }

    /* 고양이 (32x32 SQUARE, 원본 17x32 → x 오프셋 보정) */
    for (i = 0; i < MAX_CATS; i++) {
        int oam = OAM_CAT_START + i;
        if (gs->cats[i].state == CAT_STATE_SIT) {
            int cx = FP_TO_INT(gs->cats[i].x) - CAT_RENDER_OX;
            int cy = FP_TO_INT(gs->cats[i].y);
            obj_set_attr(&obj_buffer[oam],
                ATTR0_SQUARE | ATTR0_4BPP,
                ATTR1_SIZE_32,
                ATTR2_PALBANK(PB_CAT_SIT) | ATTR2_ID(TID_CAT_SIT));
            obj_set_pos(&obj_buffer[oam], cx, cy);
        } else if (gs->cats[i].state == CAT_STATE_FALLING ||
                   gs->cats[i].state == CAT_STATE_EXIT) {
            int cx = FP_TO_INT(gs->cats[i].x) - CAT_RENDER_OX;
            int cy = FP_TO_INT(gs->cats[i].y);
            u16 tid = (i & 1) ? TID_CAT_BROWN : TID_CAT_WHITE;
            u16 pb  = (i & 1) ? PB_CAT_BROWN  : PB_CAT_WHITE;
            obj_set_attr(&obj_buffer[oam],
                ATTR0_SQUARE | ATTR0_4BPP,
                ATTR1_SIZE_32,
                ATTR2_PALBANK(pb) | ATTR2_ID(tid));
            obj_set_pos(&obj_buffer[oam], cx, cy);
        } else {
            obj_hide(&obj_buffer[oam]);
        }
    }

    /* 아이템 (32x32 SQUARE, 원본 20x22 → x/y 오프셋 보정) */
    if (gs->item.active) {
        int ix = FP_TO_INT(gs->item.x) - ITEM_RENDER_OX;
        int iy = FP_TO_INT(gs->item.y) - ITEM_RENDER_OY;
        u16 tid, pb;
        switch (gs->item.type) {
        case ITEM_TYPE_BOMB:   tid = TID_ITEM_BOMB;   pb = PB_ITEM_BOMB;   break;
        case ITEM_TYPE_POISON: tid = TID_ITEM_POISON; pb = PB_ITEM_POISON; break;
        case ITEM_TYPE_SPEED:  tid = TID_ITEM_SPEED;  pb = PB_ITEM_SPEED;  break;
        default:               tid = TID_ITEM_HP;     pb = PB_ITEM_HP;     break;
        }
        obj_set_attr(&obj_buffer[OAM_ITEM],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_32,
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
        obj_set_pos(&obj_buffer[OAM_EXPLOSION], (SCREEN_W - 32) / 2, 64);
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

/* ── HUD용 얼굴 설정 ── */
static void render_face(u8 life) {
    u8 face = hud_get_face_index(life);
    u16 face_tid, face_pb;
    switch (face) {
    case FACE_HAPPY:  face_tid = TID_FACE_HAPPY;  face_pb = PB_FACE_HAPPY;  break;
    case FACE_NORMAL: face_tid = TID_FACE_NORMAL; face_pb = PB_FACE_NORMAL; break;
    case FACE_HURT:   face_tid = TID_FACE_HURT;   face_pb = PB_FACE_HURT;   break;
    default:          face_tid = TID_FACE_DEAD;   face_pb = PB_FACE_DEAD;   break;
    }
    obj_set_attr(&obj_buffer[OAM_FACE],
        ATTR0_SQUARE | ATTR0_4BPP,
        ATTR1_SIZE_32,
        ATTR2_PALBANK(face_pb) | ATTR2_ID(face_tid));
    obj_set_pos(&obj_buffer[OAM_FACE], HUD_FACE_X, HUD_FACE_Y);
}

/* ── HUD 렌더링 (플레이 중) ── */
void render_hud(const GameState* gs) {
    /* 얼굴 아이콘 (32x32, 상단 좌측) */
    render_face(gs->player.life);

    /* 점수 (얼굴 옆) */
    render_digits(gs->score, OAM_SCORE_START, HUD_SCORE_X, HUD_SCORE_Y);

    /* 폭탄 보유: 얼굴 위에 겹침 */
    if (gs->bomb.have) {
        obj_set_attr(&obj_buffer[OAM_BOMB_ICON],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_16,
            ATTR2_PALBANK(PB_ITEM_BOMB) | ATTR2_ID(TID_ITEM_BOMB));
        obj_set_pos(&obj_buffer[OAM_BOMB_ICON], HUD_BOMB_X, HUD_BOMB_Y);
    } else {
        obj_hide(&obj_buffer[OAM_BOMB_ICON]);
    }
}

/* ── 타이틀 HUD (하이스코어 표시) ── */
void render_title_hud(s16 hiscore) {
    /* 하이스코어 숫자를 화면 중앙 하단에 표시 */
    render_digits(hiscore, OAM_SCORE_START, (SCREEN_W - 40) / 2, SCREEN_H - 16);

    /* 나머지 HUD 슬롯 숨기기 */
    obj_hide(&obj_buffer[OAM_FACE]);
    obj_hide(&obj_buffer[OAM_BOMB_ICON]);
}

/* ── 배경 팔레트 어둡게 (밝기 50%) ── */
void render_darken_bg_palette(void) {
    int i;
    for (i = 0; i < 256; i++) {
        u16 c = pal_bg_mem[i];
        u16 r = (c & 0x1F) >> 1;
        u16 g = ((c >> 5) & 0x1F) >> 1;
        u16 b = ((c >> 10) & 0x1F) >> 1;
        pal_bg_mem[i] = r | (g << 5) | (b << 10);
    }
}

/* ── 등급 이미지를 프레임버퍼에 블릿 (Mode 4, 8bpp) ── */
#define GRADE_PAL_IDX 255  /* 등급 텍스트용 팔레트 인덱스 */

void render_gameover_grade(u8 grade_index) {
    if (grade_index >= GRADE_COUNT) return;

    /* 팔레트 설정: 코어=흰색, 엣지=반투명 회색 */
    pal_bg_mem[GRADE_PAL_IDX] = RGB15(31, 31, 31);       /* 코어 (흰) */
    pal_bg_mem[GRADE_PAL_IDX - 1] = RGB15(18, 18, 20);   /* 엣지 (회색) */

    const unsigned char* src = grade_image_data[grade_index];
    int ox = (SCREEN_W - GRADE_IMG_W) / 2;
    int oy = GO_GRADE_Y;

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
            u8 v_lo = src[y * GRADE_IMG_W + x];
            u8 v_hi = src[y * GRADE_IMG_W + x + 1];
            if (v_lo || v_hi) {
                int idx = px / 2;
                u16 old = dst0[idx];
                u8 lo = (v_lo == 2) ? GRADE_PAL_IDX :
                         (v_lo == 1) ? (GRADE_PAL_IDX - 1) : (u8)(old & 0xFF);
                u8 hi = (v_hi == 2) ? GRADE_PAL_IDX :
                         (v_hi == 1) ? (GRADE_PAL_IDX - 1) : (u8)(old >> 8);
                u16 val = (hi << 8) | lo;
                dst0[idx] = val;
                dst1[idx] = val;
            }
        }
    }
}

/* ── 네비 텍스트를 프레임버퍼에 블릿 ── */
#define NAV_PAL_IDX 254  /* 밝은 회색 */

void render_gameover_nav(void) {
    /* 네비 텍스트 팔레트: 코어=밝은 회색, 엣지=중간 회색 */
    pal_bg_mem[NAV_PAL_IDX] = RGB15(25, 25, 27);        /* 코어 */
    pal_bg_mem[NAV_PAL_IDX - 1] = RGB15(14, 14, 16);    /* 엣지 */

    u16* page0 = (u16*)0x06000000;
    u16* page1 = (u16*)0x0600A000;

    const unsigned char* srcs[2] = { nav_title_data, nav_retry_data };
    int offsets_x[2] = {
        SCREEN_W / 4 - NAV_TEXT_W / 2,
        SCREEN_W * 3 / 4 - NAV_TEXT_W / 2
    };

    int n, y, x;
    for (n = 0; n < 2; n++) {
        const unsigned char* src = srcs[n];
        int ox = offsets_x[n];
        for (y = 0; y < NAV_TEXT_HEIGHT; y++) {
            int row = GO_NAV_Y + y;
            if (row >= SCREEN_H) break;
            u16* dst0 = &page0[row * (SCREEN_W / 2)];
            u16* dst1 = &page1[row * (SCREEN_W / 2)];
            for (x = 0; x < NAV_TEXT_W; x += 2) {
                int px = ox + x;
                if (px < 0 || px >= SCREEN_W - 1) continue;
                u8 v_lo = src[y * NAV_TEXT_W + x];
                u8 v_hi = src[y * NAV_TEXT_W + x + 1];
                if (v_lo || v_hi) {
                    int idx = px / 2;
                    u16 old = dst0[idx];
                    u8 lo = (v_lo == 2) ? NAV_PAL_IDX :
                             (v_lo == 1) ? (NAV_PAL_IDX - 1) : (u8)(old & 0xFF);
                    u8 hi = (v_hi == 2) ? NAV_PAL_IDX :
                             (v_hi == 1) ? (NAV_PAL_IDX - 1) : (u8)(old >> 8);
                    u16 val = (hi << 8) | lo;
                    dst0[idx] = val;
                    dst1[idx] = val;
                }
            }
        }
    }
}

/* ── 게임오버 화면 (OAM 설정) ── */
void render_gameover_screen(const GameState* gs, const GameOverResult* result) {
    /* 얼굴 숨김 (GBA에는 사이드패널 없음) */
    obj_hide(&obj_buffer[OAM_FACE]);

    /* 점수를 화면 중앙에 표시 */
    render_digits(gs->score, OAM_SCORE_START, GO_SCORE_X, GO_SCORE_Y);

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
