#include "unity.h"
#include "../source/save.h"

void test_save_checksum_deterministic(void) {
    SaveData sd;
    save_prepare(&sd, 100);
    u16 cs1 = sd.checksum;
    save_prepare(&sd, 100);
    u16 cs2 = sd.checksum;
    TEST_ASSERT_EQUAL_INT(cs1, cs2);
}

void test_save_valid_after_prepare(void) {
    SaveData sd;
    save_prepare(&sd, 500);
    TEST_ASSERT_TRUE(save_is_valid(&sd));
}

void test_save_invalid_wrong_magic(void) {
    SaveData sd;
    save_prepare(&sd, 500);
    sd.magic = 0xDEADBEEF;
    TEST_ASSERT_FALSE(save_is_valid(&sd));
}

void test_save_invalid_wrong_checksum(void) {
    SaveData sd;
    save_prepare(&sd, 500);
    sd.checksum ^= 0xFFFF;
    TEST_ASSERT_FALSE(save_is_valid(&sd));
}

void test_save_write_read_roundtrip(void) {
    SaveData write_sd, read_sd;
    save_prepare(&write_sd, 1234);
    save_write(&write_sd);
    save_read(&read_sd);
    TEST_ASSERT_TRUE(save_is_valid(&read_sd));
    TEST_ASSERT_EQUAL_INT(1234, read_sd.hiscore);
}

void test_save_different_scores_different_checksums(void) {
    SaveData sd1, sd2;
    save_prepare(&sd1, 100);
    save_prepare(&sd2, 200);
    TEST_ASSERT_NOT_EQUAL(sd1.checksum, sd2.checksum);
}

int run_save_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_save_checksum_deterministic);
    RUN_TEST(test_save_valid_after_prepare);
    RUN_TEST(test_save_invalid_wrong_magic);
    RUN_TEST(test_save_invalid_wrong_checksum);
    RUN_TEST(test_save_write_read_roundtrip);
    RUN_TEST(test_save_different_scores_different_checksums);
    return UNITY_END();
}
