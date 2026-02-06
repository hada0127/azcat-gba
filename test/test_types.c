#include "unity.h"
#include "../source/types.h"

void setUp(void) {}
void tearDown(void) {}

void test_fp_converts_integer(void) {
    TEST_ASSERT_EQUAL_INT32(256, FP(1));
    TEST_ASSERT_EQUAL_INT32(1280, FP(5));
    TEST_ASSERT_EQUAL_INT32(-256, FP(-1));
}

void test_fp_to_int(void) {
    TEST_ASSERT_EQUAL_INT32(1, FP_TO_INT(FP(1)));
    TEST_ASSERT_EQUAL_INT32(5, FP_TO_INT(FP(5)));
    TEST_ASSERT_EQUAL_INT32(108, FP_TO_INT(FP(108)));
}

void test_fp_mul(void) {
    /* 2 * 3 = 6 */
    TEST_ASSERT_EQUAL_INT32(FP(6), FP_MUL(FP(2), FP(3)));
    /* 10 * 0 = 0 */
    TEST_ASSERT_EQUAL_INT32(0, FP_MUL(FP(10), FP(0)));
}

void test_fp_from_float(void) {
    /* 0.2 * 256 = 51.2 → 51 (truncated) */
    s32 val = FP_FROM_FLOAT_CONST(0.2f);
    TEST_ASSERT_EQUAL_INT32(51, val);
    /* 0.5 * 256 = 128 */
    TEST_ASSERT_EQUAL_INT32(128, FP_FROM_FLOAT_CONST(0.5f));
}

void test_game_constants(void) {
    TEST_ASSERT_EQUAL_INT32(240, SCREEN_W);
    TEST_ASSERT_EQUAL_INT32(160, SCREEN_H);
    TEST_ASSERT_EQUAL_INT32(16, HUD_H);
    TEST_ASSERT_EQUAL_INT32(33, MAX_CATS);
    TEST_ASSERT_EQUAL_INT32(FP(108), PLAYER_INIT_X);
    TEST_ASSERT_EQUAL_INT32(FP(216), PLAYER_X_MAX);
    TEST_ASSERT_EQUAL_INT32(FP(136), CAT_LAND_Y);
    TEST_ASSERT_EQUAL_INT32(51, PLAYER_ACCEL_INC);
    TEST_ASSERT_EQUAL_INT32(128, CAT_GRAVITY);
    TEST_ASSERT_EQUAL_INT32(20, BOMB_DURATION);
}

void test_fp_player_speed(void) {
    /* 5 + 0.2(누적) + player_accel 시뮬레이션 */
    s32 speed = PLAYER_BASE_SPEED + PLAYER_ACCEL_INC + FP(1);
    TEST_ASSERT_EQUAL_INT32(FP(5) + 51 + FP(1), speed);
    /* 정수 변환 시 5+0+1=6 (0.2는 소수라 정수부에 미포함) */
    TEST_ASSERT_EQUAL_INT32(6, FP_TO_INT(PLAYER_BASE_SPEED + PLAYER_ACCEL_INC + FP(1)));
}

int run_types_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fp_converts_integer);
    RUN_TEST(test_fp_to_int);
    RUN_TEST(test_fp_mul);
    RUN_TEST(test_fp_from_float);
    RUN_TEST(test_game_constants);
    RUN_TEST(test_fp_player_speed);
    return UNITY_END();
}
