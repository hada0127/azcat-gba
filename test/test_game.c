#include "unity.h"
#include "gba_stubs.h"
#include "../source/game.h"

static GameState gs;

void test_game_init(void) {
    game_init(&gs);
    TEST_ASSERT_EQUAL_INT(STATE_TITLE, gs.state);
    TEST_ASSERT_EQUAL_INT(0, gs.score);
    TEST_ASSERT_EQUAL_INT(PLAYER_LIFE_INIT, gs.player.life);
}

void test_game_play_init(void) {
    game_init(&gs);
    gs.score = 999;
    game_play_init(&gs);
    TEST_ASSERT_EQUAL_INT(STATE_PLAY, gs.state);
    TEST_ASSERT_EQUAL_INT(0, gs.score);
    TEST_ASSERT_EQUAL_INT(PLAYER_LIFE_INIT, gs.player.life);
}

void test_game_gameover_on_zero_life(void) {
    game_play_init(&gs);
    gs.player.life = 0;
    TEST_ASSERT_TRUE(game_check_gameover(&gs));
    TEST_ASSERT_EQUAL_INT(STATE_GAMEOVER, gs.state);
}

void test_game_no_gameover_with_life(void) {
    game_play_init(&gs);
    gs.player.life = 1;
    TEST_ASSERT_FALSE(game_check_gameover(&gs));
    TEST_ASSERT_EQUAL_INT(STATE_PLAY, gs.state);
}

void test_game_hiscore_update(void) {
    game_init(&gs);
    game_play_init(&gs);
    gs.score = 100;
    gs.player.life = 0;
    game_check_gameover(&gs);
    TEST_ASSERT_EQUAL_INT(100, gs.hiscore);
}

void test_game_hiscore_no_downgrade(void) {
    game_init(&gs);
    gs.hiscore = 200;
    game_play_init(&gs);
    gs.score = 50;
    gs.player.life = 0;
    game_check_gameover(&gs);
    TEST_ASSERT_EQUAL_INT(200, gs.hiscore);
}

void test_game_play_update_moves_player(void) {
    game_play_init(&gs);
    s32 prev_x = gs.player.x;
    game_play_update(&gs, KEY_RIGHT, 0);
    TEST_ASSERT_TRUE(gs.player.x > prev_x);
}

void test_game_play_update_not_in_play(void) {
    game_init(&gs); /* STATE_TITLE */
    s32 prev_x = gs.player.x;
    game_play_update(&gs, KEY_RIGHT, 0);
    /* 타이틀 상태에서는 업데이트 안 됨 */
    TEST_ASSERT_EQUAL_INT32(prev_x, gs.player.x);
}

void test_game_simulation_10_frames(void) {
    game_play_init(&gs);
    int i;
    for (i = 0; i < 10; i++) {
        game_play_update(&gs, KEY_RIGHT, 0);
    }
    /* 10프레임 후에도 크래시 없이 동작해야 함 */
    TEST_ASSERT_EQUAL_INT(STATE_PLAY, gs.state);
    TEST_ASSERT_TRUE(gs.player.x > PLAYER_INIT_X);
}

int run_game_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_game_init);
    RUN_TEST(test_game_play_init);
    RUN_TEST(test_game_gameover_on_zero_life);
    RUN_TEST(test_game_no_gameover_with_life);
    RUN_TEST(test_game_hiscore_update);
    RUN_TEST(test_game_hiscore_no_downgrade);
    RUN_TEST(test_game_play_update_moves_player);
    RUN_TEST(test_game_play_update_not_in_play);
    RUN_TEST(test_game_simulation_10_frames);
    return UNITY_END();
}
