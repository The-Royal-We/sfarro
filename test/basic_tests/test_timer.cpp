#include "test_timer.h"

TEST(test_timer, set_last_time_written_test) {
    time_t testTime = LONG_MAX;
    set_last_time_written(&testTime);
    EXPECT_EQ(testTime, get_last_time_written());
}