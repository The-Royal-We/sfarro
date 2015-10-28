#include "timer.h"

time_t LAST_TIME_WRITTEN = LONG_MAX;

time_t *get_current_time() {
    time_t rawtime;
    time(&rawtime);
    return &rawtime;
}

bool ready_to_remount() {
    if (is_last_written_time_over_limit() == 1)
        return true;
    return false;
}

/**
 * We check the difference between the last time we wrote to the fs.
 * If the difference exceeds the allocated limit (default_time=5000ms)
 * then we return 1. Else return 0
 *
 * Ready to remount:        1
 * Not ready to remount:    0
 *
 */

int is_last_written_time_over_limit() {
    int res;
    time_t *current_rawtime = get_current_time();
    if (compare_times_to_limit(&LAST_TIME_WRITTEN, current_rawtime) > 0) {
        res = 1;
    } else {
        res = 0;
    }

    return res;
}

int compare_times_to_limit(time_t *last_read, time_t *current_time) {
    long last_read_in_seconds = (long) last_read;
    long current_time_in_seconds = (long) current_time;
    int res;

    if (current_time_in_seconds - last_read_in_seconds > TIME_LIMIT) {
//        We've exceeded the limit
        res = -1;
    } else {
        res = 0;
    }

    return res;


}

time_t get_last_time_written() {
    return LAST_TIME_WRITTEN;
}

void set_last_time_written(time_t* new_time) {
    LAST_TIME_WRITTEN = *new_time;
    return;
}