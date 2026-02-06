#include "grade.h"

typedef struct {
    u16 min_score;
    const char* code;
} GradeEntry;

/* 내림차순 검색용 (score > threshold) */
static const GradeEntry grade_table[GRADE_COUNT] = {
    {3000, "d100"},
    {1000, "d10"},
    { 900, "d9"},
    { 800, "d8"},
    { 700, "d7"},
    { 600, "d6"},
    { 500, "d5"},
    { 400, "d4"},
    { 300, "d3"},
    { 200, "d2"},
    { 100, "d1"},
    {  90, "g1"},
    {  80, "g2"},
    {  70, "g3"},
    {  60, "g4"},
    {  50, "g5"},
    {  40, "g6"},
    {  30, "g7"},
    {  20, "g8"},
    {  10, "g9"},
    {   0, "g10"},  /* 기본값 (score <= 10) */
};

u8 grade_get_index(u16 score) {
    u8 i;
    for (i = 0; i < GRADE_COUNT - 1; i++) {
        if (score > grade_table[i].min_score)
            return i;
    }
    return GRADE_COUNT - 1;
}

const char* grade_get_code(u16 score) {
    return grade_table[grade_get_index(score)].code;
}
