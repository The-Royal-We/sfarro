#include <iostream>
#include "gtest/gtest.h"

extern "C" {
#include "timer.h"
}

TEST(test_timer, set_last_time_written_test) {
    time_t testTime = LONG_MAX;
    set_last_time_written(&testTime);
    EXPECT_EQ(testTime, get_last_time_written());
}

/*
 * #define TIME_LIMIT 5000l
 */

TEST(test_timer, compare_times_to_limit_test) {
    time_t test_time_last_read = (time_t) 1447157520;
    time_t test_current_time = *get_current_time();

    int res = compare_times_to_limit(&test_time_last_read, &test_current_time);

    printf("\nCurrent Time: %ld\n", (long) test_current_time);
    printf("\nResult: %d\n", res);
    EXPECT_EQ(res, -1);
}
