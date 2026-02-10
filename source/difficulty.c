#include "difficulty.h"

/* 원본 ActionScript DefineSprite_77/frame_2 기반 룩업 테이블 */
/* {점수 경계, 값} 쌍. score > threshold일 때 해당 값 적용 */

typedef struct {
    u16 threshold;
    u8  value;
} DiffEntry;

/* 고양이 수 테이블 (내림차순 검색) */
static const DiffEntry cat_qty_table[] = {
    {1040, 44},
    {1030, 43},
    {1020, 42},
    {1010, 41},
    {1000, 40},
    { 960, 36},
    { 900, 31},
    { 850, 29},
    { 750, 26},
    { 650, 24},
    { 600, 22},
    { 550, 20},
    { 500, 18},
    { 450, 15},
    { 350, 14},
    { 280, 13},
    { 250, 12},
    { 220, 11},
    { 150, 10},
    {  80,  8},
    {  30,  7},
    {  20,  6},
    {   5,  5},
    {   2,  4},
    {   1,  2},
};
#define CAT_QTY_TABLE_SIZE (sizeof(cat_qty_table) / sizeof(cat_qty_table[0]))

/* 낙하 속도 테이블 (내림차순 검색) */
static const DiffEntry cat_accel_table[] = {
    {990, 17},
    {980, 16},
    {940, 14},
    {920, 13},
    {800, 12},
    {700, 11},
    {450, 10},
    {400,  9},
    {360,  8},
    {280,  7},
    {240,  6},
    {200,  5},
    {100,  4},
    { 50,  3},
    { 10,  2},
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
