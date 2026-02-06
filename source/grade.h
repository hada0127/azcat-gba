#ifndef GRADE_H
#define GRADE_H

#include "types.h"

/* 등급 인덱스 반환 (0~20) */
u8 grade_get_index(u16 score);

/* 등급 코드 문자열 반환 ("g10"~"d100") */
const char* grade_get_code(u16 score);

#endif /* GRADE_H */
