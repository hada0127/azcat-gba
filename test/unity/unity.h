#ifndef UNITY_H
#define UNITY_H

#include <stdio.h>
#include <string.h>

/* Unity 경량 테스트 프레임워크 (최소 구현) */

extern int unity_test_failures;
extern int unity_test_count;
extern const char* unity_current_test;
extern const char* unity_current_file;
extern int unity_current_line;

void UnityBegin(const char* filename);
int UnityEnd(void);
void UnityDefaultTestRun(void (*func)(void), const char* name,
                         const char* file, int line);

#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #condition); \
            unity_test_failures++; \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    do { \
        int _e = (expected); int _a = (actual); \
        if (_e != _a) { \
            printf("  FAIL: %s:%d: Expected %d, got %d\n", \
                   __FILE__, __LINE__, _e, _a); \
            unity_test_failures++; \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL_INT32(expected, actual) \
    do { \
        long _e = (long)(expected); long _a = (long)(actual); \
        if (_e != _a) { \
            printf("  FAIL: %s:%d: Expected %ld, got %ld\n", \
                   __FILE__, __LINE__, _e, _a); \
            unity_test_failures++; \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    do { \
        const char* _e = (expected); const char* _a = (actual); \
        if (strcmp(_e, _a) != 0) { \
            printf("  FAIL: %s:%d: Expected \"%s\", got \"%s\"\n", \
                   __FILE__, __LINE__, _e, _a); \
            unity_test_failures++; \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
    TEST_ASSERT((expected) != (actual))

#define TEST_ASSERT_GREATER_THAN(threshold, actual) \
    TEST_ASSERT((actual) > (threshold))

#define TEST_ASSERT_LESS_THAN(threshold, actual) \
    TEST_ASSERT((actual) < (threshold))

#define TEST_ASSERT_GREATER_OR_EQUAL(threshold, actual) \
    TEST_ASSERT((actual) >= (threshold))

#define TEST_ASSERT_LESS_OR_EQUAL(threshold, actual) \
    TEST_ASSERT((actual) <= (threshold))

#define RUN_TEST(func) \
    UnityDefaultTestRun(func, #func, __FILE__, __LINE__)

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END()   UnityEnd()

/* setUp/tearDown 스텁 (필요시 사용자 정의) */
void setUp(void);
void tearDown(void);

#endif /* UNITY_H */
