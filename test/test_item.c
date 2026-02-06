#include "unity.h"
#include "../source/item.h"
#include <stdlib.h>

static Item item;

void test_item_init(void) {
    item_init(&item);
    TEST_ASSERT_EQUAL_INT(0, item.active);
    TEST_ASSERT_EQUAL_INT(0, item.type);
}

void test_item_spawn_after_interval(void) {
    item_init(&item);
    u16 cnt = ITEM_SPAWN_INTERVAL + 1;
    item_update(&item, &cnt, FP(100));
    TEST_ASSERT_EQUAL_INT(1, item.active);
    TEST_ASSERT_TRUE(item.type >= 1 && item.type <= 4);
    TEST_ASSERT_EQUAL_INT(0, cnt);
}

void test_item_no_spawn_before_interval(void) {
    item_init(&item);
    u16 cnt = 50;
    item_update(&item, &cnt, FP(100));
    TEST_ASSERT_EQUAL_INT(0, item.active);
    TEST_ASSERT_EQUAL_INT(51, cnt);
}

void test_item_falls(void) {
    item_init(&item);
    item.active = 1;
    item.y = ITEM_SPAWN_Y;
    item.v_accel = 0;
    item.type = 1;
    u16 cnt = 0;
    s32 prev_y = item.y;
    item_update(&item, &cnt, FP(-100)); /* 플레이어 먼 곳 */
    TEST_ASSERT_TRUE(item.y > prev_y);
}

void test_item_lands(void) {
    item_init(&item);
    item.active = 1;
    item.y = ITEM_LAND_Y - FP(1);
    item.v_accel = FP(5);
    item.type = 1;
    u16 cnt = 0;
    item_update(&item, &cnt, FP(-100));
    TEST_ASSERT_EQUAL_INT(1, item.landed);
    TEST_ASSERT_EQUAL_INT32(ITEM_LAND_Y, item.y);
}

void test_item_expires_after_sit(void) {
    item_init(&item);
    item.active = 1;
    item.landed = 1;
    item.y = ITEM_LAND_Y;
    item.type = 1;
    item.sit_cnt = ITEM_SIT_TIMEOUT - 1;
    u16 cnt = 0;
    item_update(&item, &cnt, FP(-100));
    TEST_ASSERT_EQUAL_INT(0, item.active);
}

void test_item_effect_hp(void) {
    u8 life = 2;
    u8 bomb = 0;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_HP, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(3, life);
    TEST_ASSERT_EQUAL_INT(0, delta);
}

void test_item_effect_hp_overflow(void) {
    u8 life = 3;
    u8 bomb = 0;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_HP, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(3, life);
    TEST_ASSERT_EQUAL_INT(5, delta);
}

void test_item_effect_bomb(void) {
    u8 life = 3;
    u8 bomb = 0;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_BOMB, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(1, bomb);
}

void test_item_effect_bomb_overflow(void) {
    u8 life = 3;
    u8 bomb = 1;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_BOMB, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(1, bomb);
    TEST_ASSERT_EQUAL_INT(5, delta);
}

void test_item_effect_poison(void) {
    u8 life = 3;
    u8 bomb = 0;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_POISON, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(2, life);
}

void test_item_effect_poison_min(void) {
    u8 life = 1;
    u8 bomb = 0;
    u8 accel = 0;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_POISON, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(1, life);
    TEST_ASSERT_EQUAL_INT(-5, delta);
}

void test_item_effect_speed(void) {
    u8 life = 3;
    u8 bomb = 0;
    u8 accel = 2;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_SPEED, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(3, accel);
}

void test_item_effect_speed_overflow(void) {
    u8 life = 3;
    u8 bomb = 0;
    u8 accel = 4;
    s16 delta = 0;
    item_apply_effect(ITEM_TYPE_SPEED, &life, &bomb, &accel, &delta);
    TEST_ASSERT_EQUAL_INT(4, accel);
    TEST_ASSERT_EQUAL_INT(5, delta);
}

int run_item_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_item_init);
    RUN_TEST(test_item_spawn_after_interval);
    RUN_TEST(test_item_no_spawn_before_interval);
    RUN_TEST(test_item_falls);
    RUN_TEST(test_item_lands);
    RUN_TEST(test_item_expires_after_sit);
    RUN_TEST(test_item_effect_hp);
    RUN_TEST(test_item_effect_hp_overflow);
    RUN_TEST(test_item_effect_bomb);
    RUN_TEST(test_item_effect_bomb_overflow);
    RUN_TEST(test_item_effect_poison);
    RUN_TEST(test_item_effect_poison_min);
    RUN_TEST(test_item_effect_speed);
    RUN_TEST(test_item_effect_speed_overflow);
    return UNITY_END();
}
