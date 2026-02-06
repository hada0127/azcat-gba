#include "unity.h"
#include "../source/collision.h"

void test_aabb_overlap(void) {
    HitBox a = {0, 0, 10, 10};
    HitBox b = {5, 5, 10, 10};
    TEST_ASSERT_TRUE(collision_check_aabb(&a, &b));
}

void test_aabb_no_overlap(void) {
    HitBox a = {0, 0, 10, 10};
    HitBox b = {20, 20, 10, 10};
    TEST_ASSERT_FALSE(collision_check_aabb(&a, &b));
}

void test_aabb_edge_touch(void) {
    /* 경계 접촉 = 겹침 아님 (<=) */
    HitBox a = {0, 0, 10, 10};
    HitBox b = {10, 0, 10, 10};
    TEST_ASSERT_FALSE(collision_check_aabb(&a, &b));
}

void test_cat_player_hit(void) {
    /* 고양이 Y가 충돌 구간 내, X 겹침 */
    TEST_ASSERT_TRUE(collision_check_cat_player(FP(100), FP(120), FP(100)));
}

void test_cat_player_miss_y_too_high(void) {
    /* Y가 충돌 구간 위 */
    TEST_ASSERT_FALSE(collision_check_cat_player(FP(100), FP(50), FP(100)));
}

void test_cat_player_miss_y_too_low(void) {
    /* Y가 충돌 구간 아래 */
    TEST_ASSERT_FALSE(collision_check_cat_player(FP(100), FP(140), FP(100)));
}

void test_cat_player_miss_x(void) {
    /* Y는 구간 내, X가 떨어져 있음 */
    TEST_ASSERT_FALSE(collision_check_cat_player(FP(200), FP(120), FP(10)));
}

void test_cat_player_boundary_y(void) {
    /* Y == 116 (최소 경계) */
    TEST_ASSERT_TRUE(collision_check_cat_player(FP(100), CAT_HIT_Y_MIN, FP(100)));
    /* Y == 136 (최대 경계) */
    TEST_ASSERT_TRUE(collision_check_cat_player(FP(100), CAT_HIT_Y_MAX, FP(100)));
}

void test_item_player_hit(void) {
    TEST_ASSERT_TRUE(collision_check_item_player(FP(100), FP(130), FP(100)));
}

void test_item_player_miss(void) {
    TEST_ASSERT_FALSE(collision_check_item_player(FP(100), FP(50), FP(100)));
}

int run_collision_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_aabb_overlap);
    RUN_TEST(test_aabb_no_overlap);
    RUN_TEST(test_aabb_edge_touch);
    RUN_TEST(test_cat_player_hit);
    RUN_TEST(test_cat_player_miss_y_too_high);
    RUN_TEST(test_cat_player_miss_y_too_low);
    RUN_TEST(test_cat_player_miss_x);
    RUN_TEST(test_cat_player_boundary_y);
    RUN_TEST(test_item_player_hit);
    RUN_TEST(test_item_player_miss);
    return UNITY_END();
}
