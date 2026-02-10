#include "difficulty.h"

/* 원본 ActionScript DefineSprite_77/frame_2 기반 룩업 테이블 */
/* {점수 경계, 값} 쌍. score > threshold일 때 해당 값 적용 */

typedef struct {
    u16 threshold;
    u8  value;
} DiffEntry;

/* 고양이 수 테이블 (내림차순 검색) */
static const DiffEntry cat_qty_table[] = {
    {1040, 48},
    {1030, 47},
    {1020, 46},
    {1010, 45},
    {1000, 44},
    { 960, 40},
    { 900, 35},
    { 850, 33},
    { 750, 30},
    { 650, 28},
    { 600, 26},
    { 550, 24},
    { 500, 22},
    { 450, 19},
    { 350, 17},
    { 280, 16},
    { 250, 15},
    { 220, 14},
    { 150, 12},
    {  80, 10},
    {  30,  9},
    {  20,  8},
    {   5,  7},
    {   2,  5},
    {   1,  3},
};
#define CAT_QTY_TABLE_SIZE (sizeof(cat_qty_table) / sizeof(cat_qty_table[0]))

/* 낙하 속도 테이블 (내림차순 검색) */
static const DiffEntry cat_accel_table[] = {
    {990, 13},
    {980, 12},
    {940, 11},
    {920, 10},
    {800,  9},
    {700,  8},
    {450,  7},
    {400,  6},
    {360,  6},
    {280,  5},
    {240,  4},
    {200,  4},
    {100,  3},
    { 50,  2},
    { 10,  1},
};
#define CAT_ACCEL_TABLE_SIZE (sizeof(cat_accel_table) / sizeof(cat_accel_table[0]))

u8 difficulty_get_cat_qty(u16 score) {
    u32 i;
    for (i = 0; i < CAT_QTY_TABLE_SIZE; i++) {
        if (score > cat_qty_table[i].threshold)
            return cat_qty_table[i].value;
    }
    return 1; /* score 0에서도 최소 1마리 스폰 보장 */
}

u8 difficulty_get_cat_accel(u16 score) {
    u32 i;
    for (i = 0; i < CAT_ACCEL_TABLE_SIZE; i++) {
        if (score > cat_accel_table[i].threshold)
            return cat_accel_table[i].value;
    }
    return 1; /* 기본값 */
}

u8 difficulty_get_bg_type(u16 score) {
    if (score > BG_THRESHOLD_MATRIX) return BG_MATRIX;
    if (score > BG_THRESHOLD_NIGHT)  return BG_NIGHT;
    return BG_DAY;
}
