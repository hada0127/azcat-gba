#include "unity.h"
#include "../source/difficulty.h"

void test_cat_qty_zero_at_start(void) {
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_qty(0));
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_qty(1));
}

void test_cat_qty_low_scores(void) {
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_qty(2));
    TEST_ASSERT_EQUAL_INT(2, difficulty_get_cat_qty(3));
    TEST_ASSERT_EQUAL_INT(2, difficulty_get_cat_qty(5));
    TEST_ASSERT_EQUAL_INT(3, difficulty_get_cat_qty(6));
    TEST_ASSERT_EQUAL_INT(3, difficulty_get_cat_qty(20));
    TEST_ASSERT_EQUAL_INT(4, difficulty_get_cat_qty(21));
}

void test_cat_qty_mid_scores(void) {
    TEST_ASSERT_EQUAL_INT(5, difficulty_get_cat_qty(31));
    TEST_ASSERT_EQUAL_INT(6, difficulty_get_cat_qty(81));
    TEST_ASSERT_EQUAL_INT(8, difficulty_get_cat_qty(151));
    TEST_ASSERT_EQUAL_INT(10, difficulty_get_cat_qty(221));
}

void test_cat_qty_high_scores(void) {
    TEST_ASSERT_EQUAL_INT(17, difficulty_get_cat_qty(501));
    TEST_ASSERT_EQUAL_INT(19, difficulty_get_cat_qty(551));
    TEST_ASSERT_EQUAL_INT(21, difficulty_get_cat_qty(601));
    TEST_ASSERT_EQUAL_INT(25, difficulty_get_cat_qty(751));
    TEST_ASSERT_EQUAL_INT(30, difficulty_get_cat_qty(901));
}

void test_cat_qty_max(void) {
    TEST_ASSERT_EQUAL_INT(43, difficulty_get_cat_qty(1041));
    TEST_ASSERT_EQUAL_INT(43, difficulty_get_cat_qty(5000));
}

void test_cat_qty_boundary_exact(void) {
    /* score > threshold 방식이므로 threshold 자체는 이전 단계 */
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_qty(1));
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_qty(2));
    TEST_ASSERT_EQUAL_INT(4, difficulty_get_cat_qty(21));
    TEST_ASSERT_EQUAL_INT(5, difficulty_get_cat_qty(31));
}

void test_cat_accel_default(void) {
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_accel(0));
    TEST_ASSERT_EQUAL_INT(1, difficulty_get_cat_accel(10));
}

void test_cat_accel_progression(void) {
    TEST_ASSERT_EQUAL_INT(2, difficulty_get_cat_accel(11));
    TEST_ASSERT_EQUAL_INT(3, difficulty_get_cat_accel(51));
    TEST_ASSERT_EQUAL_INT(4, difficulty_get_cat_accel(101));
    TEST_ASSERT_EQUAL_INT(5, difficulty_get_cat_accel(201));
}

void test_cat_accel_max(void) {
    TEST_ASSERT_EQUAL_INT(17, difficulty_get_cat_accel(991));
    TEST_ASSERT_EQUAL_INT(17, difficulty_get_cat_accel(5000));
}

void test_bg_type_day(void) {
    TEST_ASSERT_EQUAL_INT(BG_DAY, difficulty_get_bg_type(0));
    TEST_ASSERT_EQUAL_INT(BG_DAY, difficulty_get_bg_type(500));
}

void test_bg_type_night(void) {
    TEST_ASSERT_EQUAL_INT(BG_NIGHT, difficulty_get_bg_type(501));
    TEST_ASSERT_EQUAL_INT(BG_NIGHT, difficulty_get_bg_type(900));
}

void test_bg_type_matrix(void) {
    TEST_ASSERT_EQUAL_INT(BG_MATRIX, difficulty_get_bg_type(901));
    TEST_ASSERT_EQUAL_INT(BG_MATRIX, difficulty_get_bg_type(5000));
}

int run_difficulty_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_cat_qty_zero_at_start);
    RUN_TEST(test_cat_qty_low_scores);
    RUN_TEST(test_cat_qty_mid_scores);
    RUN_TEST(test_cat_qty_high_scores);
    RUN_TEST(test_cat_qty_max);
    RUN_TEST(test_cat_qty_boundary_exact);
    RUN_TEST(test_cat_accel_default);
    RUN_TEST(test_cat_accel_progression);
    RUN_TEST(test_cat_accel_max);
    RUN_TEST(test_bg_type_day);
    RUN_TEST(test_bg_type_night);
    RUN_TEST(test_bg_type_matrix);
    return UNITY_END();
}
