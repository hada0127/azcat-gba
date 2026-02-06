#include "unity.h"
#include "../source/hud.h"

void test_hud_face_happy(void) {
    TEST_ASSERT_EQUAL_INT(FACE_HAPPY, hud_get_face_index(3));
}

void test_hud_face_normal(void) {
    TEST_ASSERT_EQUAL_INT(FACE_NORMAL, hud_get_face_index(2));
}

void test_hud_face_hurt(void) {
    TEST_ASSERT_EQUAL_INT(FACE_HURT, hud_get_face_index(1));
}

void test_hud_face_dead(void) {
    TEST_ASSERT_EQUAL_INT(FACE_DEAD, hud_get_face_index(0));
}

void test_hud_face_overflow(void) {
    /* life > 3이면 HAPPY */
    TEST_ASSERT_EQUAL_INT(FACE_HAPPY, hud_get_face_index(5));
}

void test_hud_score_format_zero(void) {
    char buf[6];
    hud_format_score(0, buf);
    TEST_ASSERT_EQUAL_STRING("00000", buf);
}

void test_hud_score_format_normal(void) {
    char buf[6];
    hud_format_score(42, buf);
    TEST_ASSERT_EQUAL_STRING("00042", buf);
}

void test_hud_score_format_large(void) {
    char buf[6];
    hud_format_score(12345, buf);
    TEST_ASSERT_EQUAL_STRING("12345", buf);
}

void test_hud_score_format_negative(void) {
    char buf[6];
    hud_format_score(-5, buf);
    TEST_ASSERT_EQUAL_STRING("00000", buf);
}

int run_hud_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hud_face_happy);
    RUN_TEST(test_hud_face_normal);
    RUN_TEST(test_hud_face_hurt);
    RUN_TEST(test_hud_face_dead);
    RUN_TEST(test_hud_face_overflow);
    RUN_TEST(test_hud_score_format_zero);
    RUN_TEST(test_hud_score_format_normal);
    RUN_TEST(test_hud_score_format_large);
    RUN_TEST(test_hud_score_format_negative);
    return UNITY_END();
}
