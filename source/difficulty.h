#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include "types.h"

/* 점수 기반 활성 고양이 수 (0~43) */
u8 difficulty_get_cat_qty(u16 score);

/* 점수 기반 고양이 낙하 추가속도 (1~17) */
u8 difficulty_get_cat_accel(u16 score);

/* 점수 기반 배경 타입 (BG_DAY/NIGHT/MATRIX) */
u8 difficulty_get_bg_type(u16 score);

#endif /* DIFFICULTY_H */
