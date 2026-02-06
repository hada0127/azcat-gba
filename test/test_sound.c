#include "unity.h"
#include "../source/sound.h"

void test_sound_no_crash(void) {
    sound_init();
    sound_play_bgm(0);
    sound_play_sfx(1);
    sound_stop();
    /* 크래시 없으면 통과 */
    TEST_ASSERT_TRUE(1);
}

int run_sound_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sound_no_crash);
    return UNITY_END();
}
