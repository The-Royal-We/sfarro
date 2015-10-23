#include "timer.h"

time_t get_current_time() {
    time_t rawtime;
    time(&rawtime);
    return rawtime;
}

int is_last_read_time_over_limit(time_t *last_read) {

    int res;
    time_t current_rawtime = get_current_time();
    if (compare_times_to_limit(last_read, current_rawtime) > 0) {
//        Then we start to remount the system
        res = 1;
    } else {
//         It's not time yet
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