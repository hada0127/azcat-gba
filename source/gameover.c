#include "gameover.h"
#include "grade.h"
#include "gba_stubs.h"

void gameover_init(GameOverResult* result, s16 score, s16 hiscore) {
    result->grade_index = grade_get_index((u16)score);
    result->grade_code = grade_get_code((u16)score);
    result->new_hiscore = (score > hiscore);
}

u8 gameover_update(u16 keys_pressed) {
    if (keys_pressed & KEY_LEFT)  return STATE_TITLE;
    if (keys_pressed & KEY_RIGHT) return STATE_PLAY;
    return 0;
}
