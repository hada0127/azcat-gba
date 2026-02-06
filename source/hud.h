#ifndef HUD_H
#define HUD_H

#include "types.h"

#define FACE_HAPPY  0   /* life == 3 */
#define FACE_NORMAL 1   /* life == 2 */
#define FACE_HURT   2   /* life == 1 */
#define FACE_DEAD   3   /* life == 0 */

/* HP에 따른 얼굴 인덱스 */
u8 hud_get_face_index(u8 life);

/* 점수를 5자리 문자열로 포맷 (buf는 최소 6바이트) */
void hud_format_score(s16 score, char* buf);

#endif /* HUD_H */
