#include "unity.h"
#include "../source/cat.h"
#include <stdlib.h>

static Cat cats[MAX_CATS];

void test_cats_init(void) {
    cats_init(cats);
    int i;
    for (i = 0; i < MAX_CATS; i++) {
        TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[i].state);
    }
}

void test_cats_spawn_at_zero(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* score=0 → cat_qty=1 (최소 1마리 보장) */
    cats_update(cats, 0, FP(-100), false, &score_add);
    TEST_ASSERT_EQUAL_INT(CAT_STATE_FALLING, cats[0].state);
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[1].state);
}

void test_cats_spawn_at_score2(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* score=2 → cat_qty=1 (score > 1) */
    cats_update(cats, 2, FP(-100), false, &score_add);
    TEST_ASSERT_EQUAL_INT(CAT_STATE_FALLING, cats[0].state);
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[1].state);
}

void test_cats_fall(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* 스폰 */
    cats_update(cats, 10, FP(-100), false, &score_add);
    s32 prev_y = cats[0].y;
    /* 한 프레임 더 */
    cats_update(cats, 10, FP(-100), false, &score_add);
    TEST_ASSERT_TRUE(cats[0].y > prev_y);
}

void test_cats_land_gives_score(void) {
    srand(42);
    cats_init(cats);
    /* 고양이를 착지 직전에 놓기 */
    cats[0].state = CAT_STATE_FALLING;
    cats[0].x = FP(100);
    cats[0].y = CAT_LAND_Y - FP(1);
    cats[0].v_accel = FP(10); /* 바로 착지할 정도 */
    s16 score_add = 0;
    cats_update(cats, 10, FP(-100), false, &score_add);
    TEST_ASSERT_TRUE(score_add > 0);
}

void test_cats_collision_damage(void) {
    cats_init(cats);
    /* 고양이를 충돌 구간에 배치 */
    cats[0].state = CAT_STATE_FALLING;
    cats[0].x = FP(100);
    cats[0].y = FP(120); /* 충돌 구간 내 */
    cats[0].v_accel = 0;
    s16 score_add = 0;
    /* 플레이어를 같은 X에 */
    u8 damage = cats_update(cats, 10, FP(100), false, &score_add);
    TEST_ASSERT_TRUE(damage > 0);
    /* 충돌 후 HIT 상태 (이펙트 표시) */
    TEST_ASSERT_EQUAL_INT(CAT_STATE_HIT, cats[0].state);
}

void test_cats_hit_to_inactive(void) {
    int i;
    cats_init(cats);
    /* HIT 상태, 타이머 거의 완료 */
    cats[0].state = CAT_STATE_HIT;
    cats[0].x = FP(100);
    cats[0].y = FP(120);
    cats[0].v_accel = CAT_HIT_FRAMES - 1;
    s16 score_add = 0;
    cats_update(cats, 10, FP(-100), false, &score_add);
    /* HIT → INACTIVE 전환 */
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[0].state);
}

void test_cats_no_collision_far_away(void) {
    cats_init(cats);
    cats[0].state = CAT_STATE_FALLING;
    cats[0].x = FP(200);
    cats[0].y = FP(120);
    cats[0].v_accel = 0;
    s16 score_add = 0;
    u8 damage = cats_update(cats, 10, FP(10), false, &score_add);
    TEST_ASSERT_EQUAL_INT(0, damage);
}

void test_cats_bomb_clears_all(void) {
    cats_init(cats);
    int i;
    /* 여러 고양이 낙하 중 */
    for (i = 0; i < 5; i++) {
        cats[i].state = CAT_STATE_FALLING;
        cats[i].x = FP(50 + i * 20);
        cats[i].y = FP(50);
        cats[i].v_accel = 0;
    }
    s16 score_add = 0;
    cats_update(cats, 100, FP(100), true, &score_add);
    for (i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[i].state);
    }
}

void test_cats_qty_increases_with_score(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* score=100 → cat_qty=6 */
    cats_update(cats, 100, FP(-100), false, &score_add);
    int active = 0;
    int i;
    for (i = 0; i < MAX_CATS; i++) {
        if (cats[i].state != CAT_STATE_INACTIVE) active++;
    }
    TEST_ASSERT_TRUE(active > 0);
    TEST_ASSERT_TRUE(active <= 6);
}

void test_cats_sit_to_exit(void) {
    srand(42);
    cats_init(cats);
    /* 고양이를 착지 상태로 (체류 거의 완료) */
    cats[0].state = CAT_STATE_SIT;
    cats[0].x = FP(100);
    cats[0].y = CAT_LAND_Y;
    cats[0].v_accel = CAT_SIT_FRAMES - 1;
    s16 score_add = 0;
    cats_update(cats, 10, FP(-100), false, &score_add);
    /* SIT → EXIT 전환 */
    TEST_ASSERT_EQUAL_INT(CAT_STATE_EXIT, cats[0].state);
}

void test_cats_exit_falls_off(void) {
    cats_init(cats);
    /* 고양이를 EXIT 상태로, 화면 밖 직전 */
    cats[0].state = CAT_STATE_EXIT;
    cats[0].x = FP(100);
    cats[0].y = FP(199);
    cats[0].v_accel = FP(10);
    s16 score_add = 0;
    cats_update(cats, 10, FP(-100), false, &score_add);
    /* 화면 밖으로 나가면 INACTIVE */
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[0].state);
}

void test_cats_max_count(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* score=1050 → cat_qty=43=MAX_CATS */
    cats_update(cats, 1050, FP(-100), false, &score_add);
    int active = 0;
    int i;
    for (i = 0; i < MAX_CATS; i++) {
        if (cats[i].state != CAT_STATE_INACTIVE) active++;
    }
    /* 모든 33마리 활성 */
    TEST_ASSERT_EQUAL_INT(MAX_CATS, active);
}

void test_cats_inactive_above_qty(void) {
    srand(42);
    cats_init(cats);
    s16 score_add = 0;
    /* score=2 → cat_qty=1 */
    cats_update(cats, 2, FP(-100), false, &score_add);
    /* 인덱스 1 이상은 비활성이어야 함 */
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[1].state);
    TEST_ASSERT_EQUAL_INT(CAT_STATE_INACTIVE, cats[2].state);
}

int run_cat_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_cats_init);
    RUN_TEST(test_cats_spawn_at_zero);
    RUN_TEST(test_cats_spawn_at_score2);
    RUN_TEST(test_cats_fall);
    RUN_TEST(test_cats_land_gives_score);
    RUN_TEST(test_cats_collision_damage);
    RUN_TEST(test_cats_hit_to_inactive);
    RUN_TEST(test_cats_no_collision_far_away);
    RUN_TEST(test_cats_bomb_clears_all);
    RUN_TEST(test_cats_qty_increases_with_score);
    RUN_TEST(test_cats_sit_to_exit);
    RUN_TEST(test_cats_exit_falls_off);
    RUN_TEST(test_cats_max_count);
    RUN_TEST(test_cats_inactive_above_qty);
    return UNITY_END();
}
