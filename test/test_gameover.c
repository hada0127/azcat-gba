#include "unity.h"
#include "gba_stubs.h"
#include "../source/gameover.h"

void test_gameover_grade(void) {
    GameOverResult r;
    gameover_init(&r, 501, 0);  /* score > 500 → d5 */
    TEST_ASSERT_EQUAL_STRING("d5", r.grade_code);
}

void test_gameover_new_hiscore(void) {
    GameOverResult r;
    gameover_init(&r, 100, 50);
    TEST_ASSERT_TRUE(r.new_hiscore);
}

void test_gameover_no_hiscore(void) {
    GameOverResult r;
    gameover_init(&r, 50, 100);
    TEST_ASSERT_FALSE(r.new_hiscore);
}

void test_gameover_left_to_title(void) {
    TEST_ASSERT_EQUAL_INT(STATE_TITLE, gameover_update(KEY_B));
}

void test_gameover_right_to_retry(void) {
    TEST_ASSERT_EQUAL_INT(STATE_PLAY, gameover_update(KEY_A));
}

void test_gameover_no_input(void) {
    TEST_ASSERT_EQUAL_INT(0, gameover_update(0));
}

int run_gameover_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_gameover_grade);
    RUN_TEST(test_gameover_new_hiscore);
    RUN_TEST(test_gameover_no_hiscore);
    RUN_TEST(test_gameover_left_to_title);
    RUN_TEST(test_gameover_right_to_retry);
    RUN_TEST(test_gameover_no_input);
    return UNITY_END();
}
