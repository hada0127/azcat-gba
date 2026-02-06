#include <stdio.h>

/* 각 테스트 모듈의 러너 함수 선언 */
extern int run_types_tests(void);
extern int run_difficulty_tests(void);
extern int run_grade_tests(void);
extern int run_collision_tests(void);
extern int run_bomb_tests(void);
extern int run_player_tests(void);
extern int run_item_tests(void);
extern int run_cat_tests(void);
extern int run_game_tests(void);

int main(void) {
    int failures = 0;

    printf("=== azcat-gba Test Runner ===\n\n");

    failures += run_types_tests();
    failures += run_difficulty_tests();
    failures += run_grade_tests();
    failures += run_collision_tests();
    failures += run_bomb_tests();
    failures += run_player_tests();
    failures += run_item_tests();
    failures += run_cat_tests();
    failures += run_game_tests();

    printf("=== Total Failures: %d ===\n", failures);
    return failures ? 1 : 0;
}
