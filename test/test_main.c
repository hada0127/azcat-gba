#include <stdio.h>

/* 각 테스트 모듈의 러너 함수 선언 */
extern int run_types_tests(void);

int main(void) {
    int failures = 0;

    printf("=== azcat-gba Test Runner ===\n\n");

    failures += run_types_tests();

    printf("=== Total Failures: %d ===\n", failures);
    return failures ? 1 : 0;
}
