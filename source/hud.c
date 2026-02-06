#include "hud.h"

u8 hud_get_face_index(u8 life) {
    if (life >= 3) return FACE_HAPPY;
    if (life == 2) return FACE_NORMAL;
    if (life == 1) return FACE_HURT;
    return FACE_DEAD;
}

void hud_format_score(s16 score, char* buf) {
    int s = score;
    if (s < 0) s = 0;
    if (s > 99999) s = 99999;

    buf[0] = '0' + (s / 10000);
    buf[1] = '0' + ((s / 1000) % 10);
    buf[2] = '0' + ((s / 100) % 10);
    buf[3] = '0' + ((s / 10) % 10);
    buf[4] = '0' + (s % 10);
    buf[5] = '\0';
}
