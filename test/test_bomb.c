#include "unity.h"
#include "gba_stubs.h"
#include "../source/bomb.h"

static Bomb bomb;

void test_bomb_init(void) {
    bomb_init(&bomb);
    TEST_ASSERT_EQUAL_INT(0, bomb.have);
    TEST_ASSERT_EQUAL_INT(0, bomb.use);
    TEST_ASSERT_EQUAL_INT(0, bomb.timer);
}

void test_bomb_use_with_have(void) {
    bomb_init(&bomb);
    bomb.have = 1;
    bomb_try_use(&bomb, KEY_L);
    TEST_ASSERT_EQUAL_INT(1, bomb.use);
    TEST_ASSERT_EQUAL_INT(0, bomb.have);
}

void test_bomb_use_without_have(void) {
    bomb_init(&bomb);
    bomb_try_use(&bomb, KEY_L);
    TEST_ASSERT_EQUAL_INT(0, bomb.use);
}

void test_bomb_use_wrong_key(void) {
    bomb_init(&bomb);
    bomb.have = 1;
    bomb_try_use(&bomb, KEY_A);
    TEST_ASSERT_EQUAL_INT(0, bomb.use);
    TEST_ASSERT_EQUAL_INT(1, bomb.have);
}

void test_bomb_timer_expires(void) {
    bomb_init(&bomb);
    bomb.have = 1;
    bomb_try_use(&bomb, KEY_L);
    TEST_ASSERT_TRUE(bomb_is_active(&bomb));

    int i;
    for (i = 0; i < BOMB_DURATION; i++) {
        bomb_update(&bomb);
    }
    TEST_ASSERT_FALSE(bomb_is_active(&bomb));
}

void test_bomb_no_double_use(void) {
    bomb_init(&bomb);
    bomb.have = 1;
    bomb_try_use(&bomb, KEY_L);
    /* 이미 사용중, 다시 시도해도 변화 없음 */
    bomb.have = 1; /* 강제로 다시 줘도 */
    bomb_try_use(&bomb, KEY_L);
    TEST_ASSERT_EQUAL_INT(1, bomb.use);
}

int run_bomb_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bomb_init);
    RUN_TEST(test_bomb_use_with_have);
    RUN_TEST(test_bomb_use_without_have);
    RUN_TEST(test_bomb_use_wrong_key);
    RUN_TEST(test_bomb_timer_expires);
    RUN_TEST(test_bomb_no_double_use);
    return UNITY_END();
}
