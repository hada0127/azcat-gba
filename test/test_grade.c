#include "unity.h"
#include "../source/grade.h"

void test_grade_lowest(void) {
    TEST_ASSERT_EQUAL_STRING("g10", grade_get_code(0));
    TEST_ASSERT_EQUAL_STRING("g10", grade_get_code(10));
}

void test_grade_kyu(void) {
    TEST_ASSERT_EQUAL_STRING("g9", grade_get_code(11));
    TEST_ASSERT_EQUAL_STRING("g8", grade_get_code(21));
    TEST_ASSERT_EQUAL_STRING("g5", grade_get_code(51));
    TEST_ASSERT_EQUAL_STRING("g1", grade_get_code(91));
}

void test_grade_dan(void) {
    TEST_ASSERT_EQUAL_STRING("d1", grade_get_code(101));
    TEST_ASSERT_EQUAL_STRING("d2", grade_get_code(201));
    TEST_ASSERT_EQUAL_STRING("d5", grade_get_code(501));
    TEST_ASSERT_EQUAL_STRING("d9", grade_get_code(901));
}

void test_grade_d10(void) {
    TEST_ASSERT_EQUAL_STRING("d10", grade_get_code(1001));
    TEST_ASSERT_EQUAL_STRING("d10", grade_get_code(2999));
}

void test_grade_d100_max(void) {
    TEST_ASSERT_EQUAL_STRING("d100", grade_get_code(3001));
    TEST_ASSERT_EQUAL_STRING("d100", grade_get_code(10000));
}

void test_grade_boundary_exact(void) {
    /* score > threshold 검사 */
    TEST_ASSERT_EQUAL_STRING("g10", grade_get_code(10));
    TEST_ASSERT_EQUAL_STRING("g9",  grade_get_code(11));
    TEST_ASSERT_EQUAL_STRING("g1",  grade_get_code(100));  /* score > 90 → g1 */
    TEST_ASSERT_EQUAL_STRING("d1",  grade_get_code(101));   /* score > 100 → d1 */
}

void test_grade_index_range(void) {
    TEST_ASSERT_EQUAL_INT(GRADE_COUNT - 1, grade_get_index(0));
    TEST_ASSERT_EQUAL_INT(0, grade_get_index(3001));
}

int run_grade_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_grade_lowest);
    RUN_TEST(test_grade_kyu);
    RUN_TEST(test_grade_dan);
    RUN_TEST(test_grade_d10);
    RUN_TEST(test_grade_d100_max);
    RUN_TEST(test_grade_boundary_exact);
    RUN_TEST(test_grade_index_range);
    return UNITY_END();
}
