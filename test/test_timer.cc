#include "gtest/gtest.h"
#include "../src/timer.h"

TEST(test_timer, set_last_time_written_test) {
    time_t testTime = 12000l;
    set_last_time_written(&testTime);
    EXPECT_EQ(testTime, get_last_time_written());
}

//time_t LAST_TIME_WRITTEN = LONG_MAX;
//
//time_t *get_current_time() {
//    time_t rawtime;
//    time(&rawtime);
//    return &rawtime;
//}