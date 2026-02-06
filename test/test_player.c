#include "unity.h"
#include "gba_stubs.h"
#include "../source/player.h"

static Player p;

void test_player_init(void) {
    player_init(&p);
    TEST_ASSERT_EQUAL_INT32(PLAYER_INIT_X, p.x);
    TEST_ASSERT_EQUAL_INT(PLAYER_LIFE_INIT, p.life);
    TEST_ASSERT_EQUAL_INT(DIR_LEFT, p.direction);
    TEST_ASSERT_EQUAL_INT32(0, p.v_accel);
    TEST_ASSERT_EQUAL_INT(0, p.player_accel);
}

void test_player_move_right(void) {
    player_init(&p);
    s32 prev_x = p.x;
    player_update(&p, KEY_RIGHT, false);
    TEST_ASSERT_TRUE(p.x > prev_x);
    TEST_ASSERT_EQUAL_INT(DIR_RIGHT, p.direction);
}

void test_player_move_left(void) {
    player_init(&p);
    s32 prev_x = p.x;
    player_update(&p, KEY_LEFT, false);
    TEST_ASSERT_TRUE(p.x < prev_x);
    TEST_ASSERT_EQUAL_INT(DIR_LEFT, p.direction);
}

void test_player_stop_on_release(void) {
    player_init(&p);
    /* 이동 후 키 릴리스 */
    player_update(&p, KEY_RIGHT, false);
    s32 after_move = p.x;
    player_update(&p, 0, false);
    TEST_ASSERT_EQUAL_INT32(after_move, p.x);
    TEST_ASSERT_EQUAL_INT32(0, p.v_accel);
}

void test_player_acceleration(void) {
    player_init(&p);
    player_update(&p, KEY_RIGHT, false);
    s32 first_speed = p.v_accel;
    player_update(&p, KEY_RIGHT, false);
    TEST_ASSERT_TRUE(p.v_accel > first_speed);
}

void test_player_clamp_right(void) {
    player_init(&p);
    p.x = PLAYER_X_MAX;
    player_update(&p, KEY_RIGHT, false);
    TEST_ASSERT_EQUAL_INT32(PLAYER_X_MAX, p.x);
}

void test_player_clamp_left(void) {
    player_init(&p);
    p.x = PLAYER_X_MIN;
    player_update(&p, KEY_LEFT, false);
    TEST_ASSERT_EQUAL_INT32(PLAYER_X_MIN, p.x);
}

void test_player_no_move_during_bomb(void) {
    player_init(&p);
    s32 prev_x = p.x;
    player_update(&p, KEY_RIGHT, true);
    TEST_ASSERT_EQUAL_INT32(prev_x, p.x);
}

void test_player_no_move_when_dead(void) {
    player_init(&p);
    p.state = 1;
    s32 prev_x = p.x;
    player_update(&p, KEY_RIGHT, false);
    TEST_ASSERT_EQUAL_INT32(prev_x, p.x);
}

void test_player_accel_boost(void) {
    player_init(&p);
    p.player_accel = 2;
    s32 prev_x = p.x;
    player_update(&p, KEY_RIGHT, false);
    /* speed = FP(5) + 0 + FP(2) = FP(7) */
    TEST_ASSERT_EQUAL_INT32(prev_x + FP(7), p.x);
}

void test_player_continuous_accel(void) {
    player_init(&p);
    int i;
    /* 10프레임 연속 이동 */
    for (i = 0; i < 10; i++) {
        player_update(&p, KEY_RIGHT, false);
    }
    /* v_accel = 51 * 10 = 510 */
    TEST_ASSERT_EQUAL_INT32(510, p.v_accel);
}

int run_player_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_player_init);
    RUN_TEST(test_player_move_right);
    RUN_TEST(test_player_move_left);
    RUN_TEST(test_player_stop_on_release);
    RUN_TEST(test_player_acceleration);
    RUN_TEST(test_player_clamp_right);
    RUN_TEST(test_player_clamp_left);
    RUN_TEST(test_player_no_move_during_bomb);
    RUN_TEST(test_player_no_move_when_dead);
    RUN_TEST(test_player_accel_boost);
    RUN_TEST(test_player_continuous_accel);
    return UNITY_END();
}
