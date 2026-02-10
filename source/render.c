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
#include "bg_bomb.h"
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
#include "font_numbers.h"
#include "spr_hud_heart.h"
#include "spr_hud_bomb.h"
#include "spr_highscore_text.h"
#include "font_numbers_small.h"
#include "grade_images.h"
#include "nav_text.h"

/* OAM 섀도우 버퍼 */
static OBJ_ATTR obj_buffer[128];

/* ── 초기화: 스프라이트 타일+팔레트 → VRAM ── */
void render_init(void) {
    oam_init(obj_buffer, 128);

    /* OBJ Affine 매트릭스: 2/3 스케일링 (0=normal, 1=hflip) */
    {
        OBJ_AFFINE *oa = (OBJ_AFFINE*)obj_buffer;
        oa[AFFINE_IDX].pa = AFFINE_SCALE;
        oa[AFFINE_IDX].pb = 0;
        oa[AFFINE_IDX].pc = 0;
        oa[AFFINE_IDX].pd = AFFINE_SCALE;
        /* affine에서 HFLIP 비트는 인덱스로 재해석되므로 별도 매트릭스 필요 */
        oa[AFFINE_IDX_FLIP].pa = -AFFINE_SCALE;
        oa[AFFINE_IDX_FLIP].pb = 0;
        oa[AFFINE_IDX_FLIP].pc = 0;
        oa[AFFINE_IDX_FLIP].pd = AFFINE_SCALE;

        /* EXIT 확대 애니메이션 affine (4단계: 384→320 점진적 확대) */
        {
            static const s16 exit_scales[] = {348, 316, 284, 256};
            int j;
            for (j = 0; j < AFFINE_EXIT_STEPS; j++) {
                oa[AFFINE_IDX_EXIT_BASE + j].pa = exit_scales[j];
                oa[AFFINE_IDX_EXIT_BASE + j].pb = 0;
                oa[AFFINE_IDX_EXIT_BASE + j].pc = 0;
                oa[AFFINE_IDX_EXIT_BASE + j].pd = exit_scales[j];
            }
        }
    }

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
    memcpy32(&tile_mem[5][384], font_numbersTiles,     font_numbersTilesLen / 4);     /* 896: 40타일 16x16 */
    memcpy32(&tile_mem[5][424], spr_hud_heartTiles,   spr_hud_heartTilesLen / 4);    /* 936: 4타일 16x16 */
    memcpy32(&tile_mem[5][428], spr_hud_bombTiles,    spr_hud_bombTilesLen / 4);     /* 940: 4타일 16x16 */
    memcpy32(&tile_mem[5][432], spr_highscore_textTiles, spr_highscore_textTilesLen / 4); /* 944: 32타일 64x32 */
    memcpy32(&tile_mem[5][464], font_numbers_smallTiles, font_numbers_smallTilesLen / 4); /* 976: 10타일 8x8 */

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
    memcpy16(pal_obj_bank[PB_FONT],        font_numbersPal,    font_numbersPalLen / 2);
    memcpy16(pal_obj_bank[PB_HUD_HEART],   spr_hud_heartPal,   spr_hud_heartPalLen / 2);
    memcpy16(pal_obj_bank[PB_HUD_BOMB],    spr_hud_bombPal,    spr_hud_bombPalLen / 2);
    memcpy16(pal_obj_bank[PB_HISCORE_TXT], spr_highscore_textPal, spr_highscore_textPalLen / 2);
    memcpy16(pal_obj_bank[PB_FONT_SMALL], font_numbers_smallPal, font_numbers_smallPalLen / 2);
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

void render_set_bomb_bg(void) {
    memcpy16(pal_bg_mem, bg_bombPal, 256);
    memcpy32((void*)0x06000000, bg_bombBitmap, bg_bombBitmapLen / 4);
    memcpy32((void*)0x0600A000, bg_bombBitmap, bg_bombBitmapLen / 4);
}

void render_set_title_bg(void) {
    memcpy16(pal_bg_mem, bg_titlePal, 256);
    memcpy32((void*)0x06000000, bg_titleBitmap, bg_titleBitmapLen / 4);
    memcpy32((void*)0x0600A000, bg_titleBitmap, bg_titleBitmapLen / 4);
}

/* ── 스프라이트 렌더링 ── */
void render_sprites(const GameState* gs) {
    int i;

    /* 폭탄 활성: 게임 스프라이트 전부 숨김 (폭탄 BG가 전체화면) */
    if (bomb_is_active(&gs->bomb)) {
        obj_hide(&obj_buffer[OAM_PLAYER]);
        for (i = 0; i < MAX_CATS; i++)
            obj_hide(&obj_buffer[OAM_CAT_START + i]);
        obj_hide(&obj_buffer[OAM_ITEM]);
        for (i = 0; i < OAM_HIT_COUNT; i++)
            obj_hide(&obj_buffer[OAM_HIT_START + i]);
        return;
    }

    /* 플레이어 */
    if (gs->player.state == 0) {
        int px = FP_TO_INT(gs->player.x);
        u16 aff = (gs->player.direction == DIR_RIGHT) ? AFFINE_IDX_FLIP : AFFINE_IDX;

        /* 걷기 애니메이션: 정지=W0, 이동=W0→W1→W2 순환 (6프레임마다 전환) */
        u16 tid;
        if (gs->player.anim_counter == 0) {
            tid = TID_PLAYER_W0;
        } else {
            static const u16 walk_tids[] = {TID_PLAYER_W0, TID_PLAYER_W1, TID_PLAYER_W2};
            u8 frame = (gs->player.anim_counter / 6) % 3;
            tid = walk_tids[frame];
        }

        /* 32x64 TALL OAM + affine 2/3 스케일링 */
        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_TALL | ATTR0_4BPP | ATTR0_AFF,
            ATTR1_SIZE_64 | ATTR1_AFF_ID(aff),
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(tid));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, PLAYER_RENDER_Y);
    } else {
        /* 사망: 64x32 WIDE + affine 2/3 스케일링 */
        int px = FP_TO_INT(gs->player.x) - 16;  /* 중앙 보정 */
        u16 aff = (gs->player.direction == DIR_RIGHT) ? AFFINE_IDX_FLIP : AFFINE_IDX;
        obj_set_attr(&obj_buffer[OAM_PLAYER],
            ATTR0_WIDE | ATTR0_4BPP | ATTR0_AFF,
            ATTR1_SIZE_64 | ATTR1_AFF_ID(aff),
            ATTR2_PALBANK(PB_PLAYER) | ATTR2_ID(TID_PLAYER_DEAD));
        obj_set_pos(&obj_buffer[OAM_PLAYER], px, 131);
    }

    /* 충돌 이펙트 슬롯 초기화 */
    int hit_slot = 0;
    for (i = 0; i < OAM_HIT_COUNT; i++)
        obj_hide(&obj_buffer[OAM_HIT_START + i]);

    /* 고양이 (32x32 SQUARE, 원본 17x32 → x 오프셋 보정) */
    for (i = 0; i < MAX_CATS; i++) {
        int oam = OAM_CAT_START + i;
        if (gs->cats[i].state == CAT_STATE_SIT) {
            int cx = FP_TO_INT(gs->cats[i].x) - CAT_RENDER_OX;
            int cy = FP_TO_INT(gs->cats[i].y);
            obj_set_attr(&obj_buffer[oam],
                ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF,
                ATTR1_SIZE_32 | ATTR1_AFF_ID(AFFINE_IDX),
                ATTR2_PALBANK(PB_CAT_SIT) | ATTR2_ID(TID_CAT_SIT));
            obj_set_pos(&obj_buffer[oam], cx, cy);
        } else if (gs->cats[i].state == CAT_STATE_FALLING ||
                   gs->cats[i].state == CAT_STATE_EXIT) {
            int cx = FP_TO_INT(gs->cats[i].x) - CAT_RENDER_OX;
            int cy = FP_TO_INT(gs->cats[i].y);
            u16 tid = (i & 1) ? TID_CAT_BROWN : TID_CAT_WHITE;
            u16 pb  = (i & 1) ? PB_CAT_BROWN  : PB_CAT_WHITE;
            u16 aff_idx = AFFINE_IDX;
            if (gs->cats[i].state == CAT_STATE_EXIT) {
                /* 확대 애니메이션: exit_anim 값으로 affine 단계 선택 */
                int step = gs->cats[i].exit_anim * AFFINE_EXIT_STEPS / CAT_EXIT_ANIM_LEN;
                if (step >= AFFINE_EXIT_STEPS) step = AFFINE_EXIT_STEPS - 1;
                aff_idx = AFFINE_IDX_EXIT_BASE + step;
            }
            obj_set_attr(&obj_buffer[oam],
                ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF,
                ATTR1_SIZE_32 | ATTR1_AFF_ID(aff_idx),
                ATTR2_PALBANK(pb) | ATTR2_ID(tid));
            obj_set_pos(&obj_buffer[oam], cx, cy);
        } else if (gs->cats[i].state == CAT_STATE_HIT) {
            /* 고양이 슬롯 숨김, 전용 히트 슬롯에 폭발 표시 */
            obj_hide(&obj_buffer[oam]);
            if (hit_slot < OAM_HIT_COUNT) {
                int cx = FP_TO_INT(gs->cats[i].x) - CAT_RENDER_OX;
                int cy = FP_TO_INT(gs->cats[i].y);
                obj_set_attr(&obj_buffer[OAM_HIT_START + hit_slot],
                    ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF,
                    ATTR1_SIZE_32 | ATTR1_AFF_ID(AFFINE_IDX),
                    ATTR2_PALBANK(PB_EXPLOSION) | ATTR2_ID(TID_EXPLOSION));
                obj_set_pos(&obj_buffer[OAM_HIT_START + hit_slot], cx, cy);
                hit_slot++;
            }
        } else {
            obj_hide(&obj_buffer[oam]);
        }
    }

    /* 아이템 (32x32 SQUARE, 원본 20x22 → x/y 오프셋 보정) */
    if (gs->item.active) {
        int ix = FP_TO_INT(gs->item.x) - ITEM_RENDER_OX;
        int iy = FP_TO_INT(gs->item.y);  /* affine 센터링이 OY 보정 대체 */
        u16 tid, pb;
        switch (gs->item.type) {
        case ITEM_TYPE_BOMB:   tid = TID_ITEM_BOMB;   pb = PB_ITEM_BOMB;   break;
        case ITEM_TYPE_POISON: tid = TID_ITEM_POISON; pb = PB_ITEM_POISON; break;
        case ITEM_TYPE_SPEED:  tid = TID_ITEM_SPEED;  pb = PB_ITEM_SPEED;  break;
        default:               tid = TID_ITEM_HP;     pb = PB_ITEM_HP;     break;
        }
        obj_set_attr(&obj_buffer[OAM_ITEM],
            ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF,
            ATTR1_SIZE_32 | ATTR1_AFF_ID(AFFINE_IDX),
            ATTR2_PALBANK(pb) | ATTR2_ID(tid));
        obj_set_pos(&obj_buffer[OAM_ITEM], ix, iy);
    } else {
        obj_hide(&obj_buffer[OAM_ITEM]);
    }

}

/* ── 숫자 5자리를 OAM으로 표시 (16x16 테두리 폰트) ── */
static void render_digits(s16 value, int oam_start, int x, int y) {
    char buf[6];
    int i;
    hud_format_score(value, buf);
    for (i = 0; i < 5; i++) {
        int digit = buf[i] - '0';
        obj_set_attr(&obj_buffer[oam_start + i],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_16,
            ATTR2_PALBANK(PB_FONT) | ATTR2_ID(TID_FONT + digit * 4));
        obj_set_pos(&obj_buffer[oam_start + i], x + i * HUD_DIGIT_SPACE, y);
    }
}

/* ── 숫자 5자리를 OAM으로 표시 (8x8 작은 폰트) ── */
static void render_digits_small(s16 value, int oam_start, int x, int y) {
    char buf[6];
    int i;
    hud_format_score(value, buf);
    for (i = 0; i < 5; i++) {
        int digit = buf[i] - '0';
        obj_set_attr(&obj_buffer[oam_start + i],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_8,
            ATTR2_PALBANK(PB_FONT_SMALL) | ATTR2_ID(TID_FONT_SMALL + digit));
        obj_set_pos(&obj_buffer[oam_start + i], x + i * TITLE_DIGIT_SPACE, y);
    }
}

/* ── HUD 렌더링 (플레이 중) ── */
void render_hud(const GameState* gs) {
    int i;

    /* 하트 아이콘 (16x16, 좌측 상단) */
    for (i = 0; i < OAM_HEART_COUNT; i++) {
        if (i < gs->player.life) {
            obj_set_attr(&obj_buffer[OAM_HEART_START + i],
                ATTR0_SQUARE | ATTR0_4BPP,
                ATTR1_SIZE_16,
                ATTR2_PALBANK(PB_HUD_HEART) | ATTR2_ID(TID_HUD_HEART));
            obj_set_pos(&obj_buffer[OAM_HEART_START + i],
                HUD_HEART_X + i * HUD_HEART_SPACE, HUD_HEART_Y);
        } else {
            obj_hide(&obj_buffer[OAM_HEART_START + i]);
        }
    }

    /* 점수 (중앙 상단, 16x16 테두리 폰트) */
    render_digits(gs->score, OAM_SCORE_START, HUD_SCORE_X, HUD_SCORE_Y);

    /* 폭탄 보유 아이콘 (16x16, 하트 아래줄) */
    if (gs->bomb.have) {
        obj_set_attr(&obj_buffer[OAM_HUD_BOMB],
            ATTR0_SQUARE | ATTR0_4BPP,
            ATTR1_SIZE_16,
            ATTR2_PALBANK(PB_HUD_BOMB) | ATTR2_ID(TID_HUD_BOMB));
        obj_set_pos(&obj_buffer[OAM_HUD_BOMB], HUD_HBOMB_X, HUD_HBOMB_Y);
    } else {
        obj_hide(&obj_buffer[OAM_HUD_BOMB]);
    }
}

/* ── 타이틀 HUD (하이스코어 표시) ── */
void render_title_hud(s16 hiscore) {
    int i;

    /* "High Score" 텍스트 (64x32 WIDE, 하트 슬롯 0 재활용) */
    obj_set_attr(&obj_buffer[OAM_HEART_START],
        ATTR0_WIDE | ATTR0_4BPP,
        ATTR1_SIZE_64,
        ATTR2_PALBANK(PB_HISCORE_TXT) | ATTR2_ID(TID_HISCORE_TXT));
    obj_set_pos(&obj_buffer[OAM_HEART_START], TITLE_HS_TEXT_X, TITLE_HS_TEXT_Y);

    /* 하이스코어 숫자 (8x8 작은 폰트, 텍스트 옆) */
    render_digits_small(hiscore, OAM_SCORE_START, TITLE_HS_NUM_X, TITLE_HS_NUM_Y);

    /* 사용하지 않는 HUD 슬롯 숨기기 */
    for (i = 1; i < OAM_HEART_COUNT; i++)
        obj_hide(&obj_buffer[OAM_HEART_START + i]);
    obj_hide(&obj_buffer[OAM_HUD_BOMB]);
}

/* ── 게임오버 UI: OAM 스프라이트 기반 (투명 배경, 스프라이트 위에 표시) ── */

/* 게임오버 UI OBJ 팔레트 뱅크 */
#define PB_GO_UI        14

/* 게임오버 타일 ID (얼굴 타일 영역 재사용) */
#define TID_GO_GRADE1   768   /* 64x32, 32 tiles (face_happy 덮어씀) */
#define TID_GO_GRADE2   800   /* 16x16, 4 tiles */
#define TID_GO_TITLE1   804   /* 64x32, 32 tiles */
#define TID_GO_TITLE2   836   /* 16x16, 4 tiles */
#define TID_GO_RETRY1   840   /* 64x32, 32 tiles */
#define TID_GO_RETRY2   872   /* 16x16, 4 tiles */

/* 비트맵(0/1/2) → 4bpp OBJ 타일 변환 (영역 지정) */
static void bitmap_to_obj_tiles(const unsigned char* src, int src_w, int src_h,
                                int ox, int oy, u32* dst, int spr_w, int spr_h) {
    int cols = spr_w / 8;
    int rows = spr_h / 8;
    int tr, tc, py, px;

    for (tr = 0; tr < rows; tr++) {
        for (tc = 0; tc < cols; tc++) {
            u32* tile = &dst[(tr * cols + tc) * 8];
            for (py = 0; py < 8; py++) {
                int img_y = oy + tr * 8 + py;
                u32 row_val = 0;
                for (px = 0; px < 8; px++) {
                    int img_x = ox + tc * 8 + px;
                    u8 pix = (img_x >= 0 && img_x < src_w &&
                              img_y >= 0 && img_y < src_h) ?
                             src[img_y * src_w + img_x] : 0;
                    row_val |= (pix & 0xF) << (px * 4);
                }
                tile[py] = row_val;
            }
        }
    }
}

/* 게임오버 UI 타일+팔레트 로드 (얼굴 VRAM 영역에 덮어씀) */
void render_gameover_load_ui(u8 grade_index) {
    /* UI 전용 OBJ 팔레트: 0=투명, 1=흰색, 2=검정 */
    pal_obj_bank[PB_GO_UI][1] = 0x7FFF;
    pal_obj_bank[PB_GO_UI][2] = 0x0000;

    /* 등급 → 64x32 + 16x16 타일 */
    if (grade_index < GRADE_COUNT) {
        const unsigned char* gsrc = grade_image_data[grade_index];
        bitmap_to_obj_tiles(gsrc, GRADE_IMG_W, GRADE_IMG_H, 0, 0,
            (u32*)&tile_mem[5][TID_GO_GRADE1 - 512], 64, 32);
        bitmap_to_obj_tiles(gsrc, GRADE_IMG_W, GRADE_IMG_H, 64, 0,
            (u32*)&tile_mem[5][TID_GO_GRADE2 - 512], 16, 16);
    }

    /* 타이틀 네비 → 64x32 + 16x16 타일 */
    bitmap_to_obj_tiles(nav_title_data, NAV_TEXT_W, NAV_TEXT_HEIGHT, 0, 0,
        (u32*)&tile_mem[5][TID_GO_TITLE1 - 512], 64, 32);
    bitmap_to_obj_tiles(nav_title_data, NAV_TEXT_W, NAV_TEXT_HEIGHT, 64, 0,
        (u32*)&tile_mem[5][TID_GO_TITLE2 - 512], 16, 16);

    /* 재도전 네비 → 64x32 + 16x16 타일 */
    bitmap_to_obj_tiles(nav_retry_data, NAV_TEXT_W, NAV_TEXT_HEIGHT, 0, 0,
        (u32*)&tile_mem[5][TID_GO_RETRY1 - 512], 64, 32);
    bitmap_to_obj_tiles(nav_retry_data, NAV_TEXT_W, NAV_TEXT_HEIGHT, 64, 0,
        (u32*)&tile_mem[5][TID_GO_RETRY2 - 512], 16, 16);
}

/* ── 게임오버 화면: UI를 OAM 스프라이트로 표시 ── */
void render_gameover_screen(const GameState* gs, const GameOverResult* result) {
    int i;
    int go_grade_x = (SCREEN_W - GRADE_IMG_W) / 2;
    int go_score_x = (SCREEN_W - (4 * HUD_DIGIT_SPACE + 16)) / 2; /* 실제 시각 너비 기준 */
    int go_title_x = SCREEN_W / 4 - NAV_TEXT_W / 2;
    int go_retry_x = SCREEN_W * 3 / 4 - NAV_TEXT_W / 2;

    /* BG2를 PRIO 2로 내려서 PRIO(1) 게임스프라이트도 배경 앞에 표시 */
    REG_BG2CNT = (REG_BG2CNT & ~BG_PRIO_MASK) | BG_PRIO(2);

    /* 게임플레이 스프라이트 우선순위 1 (PRIO(0) UI 뒤, BG2 앞) */
    obj_buffer[OAM_PLAYER].attr2 = (obj_buffer[OAM_PLAYER].attr2 & ~ATTR2_PRIO_MASK) | ATTR2_PRIO(1);
    obj_buffer[OAM_ITEM].attr2 = (obj_buffer[OAM_ITEM].attr2 & ~ATTR2_PRIO_MASK) | ATTR2_PRIO(1);
    for (i = 0; i < MAX_CATS; i++)
        obj_buffer[OAM_CAT_START + i].attr2 = (obj_buffer[OAM_CAT_START + i].attr2 & ~ATTR2_PRIO_MASK) | ATTR2_PRIO(1);
    for (i = 0; i < OAM_HIT_COUNT; i++)
        obj_buffer[OAM_HIT_START + i].attr2 = (obj_buffer[OAM_HIT_START + i].attr2 & ~ATTR2_PRIO_MASK) | ATTR2_PRIO(1);

    /* 등급 (64x32 + 16x16, 하트 슬롯 재사용) */
    obj_set_attr(&obj_buffer[OAM_HEART_START],
        ATTR0_WIDE | ATTR0_4BPP, ATTR1_SIZE_64,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_GRADE1));
    obj_set_pos(&obj_buffer[OAM_HEART_START], go_grade_x, GO_GRADE_Y);

    obj_set_attr(&obj_buffer[OAM_HEART_START + 1],
        ATTR0_SQUARE | ATTR0_4BPP, ATTR1_SIZE_16,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_GRADE2));
    obj_set_pos(&obj_buffer[OAM_HEART_START + 1], go_grade_x + 64, GO_GRADE_Y);

    /* 나머지 HUD 슬롯 숨김 */
    obj_hide(&obj_buffer[OAM_HEART_START + 2]);
    obj_hide(&obj_buffer[OAM_HUD_BOMB]);

    /* 점수 (기존 폰트 타일 사용) */
    render_digits(gs->score, OAM_SCORE_START, go_score_x, GO_SCORE_Y);

    /* 타이틀 네비 (OAM 48~49) */
    obj_set_attr(&obj_buffer[48],
        ATTR0_WIDE | ATTR0_4BPP, ATTR1_SIZE_64,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_TITLE1));
    obj_set_pos(&obj_buffer[48], go_title_x, GO_NAV_Y);

    obj_set_attr(&obj_buffer[49],
        ATTR0_SQUARE | ATTR0_4BPP, ATTR1_SIZE_16,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_TITLE2));
    obj_set_pos(&obj_buffer[49], go_title_x + 64, GO_NAV_Y);

    /* 재도전 네비 (OAM 50~51) */
    obj_set_attr(&obj_buffer[50],
        ATTR0_WIDE | ATTR0_4BPP, ATTR1_SIZE_64,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_RETRY1));
    obj_set_pos(&obj_buffer[50], go_retry_x, GO_NAV_Y);

    obj_set_attr(&obj_buffer[51],
        ATTR0_SQUARE | ATTR0_4BPP, ATTR1_SIZE_16,
        ATTR2_PALBANK(PB_GO_UI) | ATTR2_ID(TID_GO_RETRY2));
    obj_set_pos(&obj_buffer[51], go_retry_x + 64, GO_NAV_Y);
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
