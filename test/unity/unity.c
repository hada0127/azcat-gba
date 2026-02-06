#include "unity.h"

int unity_test_failures = 0;
int unity_test_count = 0;
const char* unity_current_test = "";
const char* unity_current_file = "";
int unity_current_line = 0;

void UnityBegin(const char* filename) {
    unity_test_failures = 0;
    unity_test_count = 0;
    printf("--- %s ---\n", filename);
}

int UnityEnd(void) {
    printf("--- %d Tests, %d Failures ---\n\n",
           unity_test_count, unity_test_failures);
    return unity_test_failures;
}

void UnityDefaultTestRun(void (*func)(void), const char* name,
                         const char* file, int line) {
    int prev_failures = unity_test_failures;
    unity_current_test = name;
    unity_current_file = file;
    unity_current_line = line;
    unity_test_count++;

    setUp();
    func();
    tearDown();

    if (unity_test_failures == prev_failures) {
        printf("  PASS: %s\n", name);
    }
}
