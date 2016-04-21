#include "timer.h"

time_t last_time_written;

    int
ready_to_remount ()
{
    int res;
    res = is_last_written_time_over_limit() ;
    return res;
}

/**
 * We check the difference between the last time we wrote to the fs.
 * If the difference exceeds the allocated limit (default_time=5s)
 * then we return 1. Else return 0
 *
 * Ready to remount:        1
 * Not ready to remount:    0
 *
 */

    int
is_last_written_time_over_limit ()
{
    int res;
    time_t current_rawtime;
    time(&current_rawtime);
    if (compare_times_to_limit (&last_time_written, &current_rawtime) > 0)
    {
        res = 1;
    }
    else
    {
        res = 0;
    }

    return res;
}

    int
compare_times_to_limit (time_t * last_read, time_t * current_time)
{
    int res;
    time_t last_read_in_seconds = *last_read;
    time_t current_time_in_seconds = *current_time;
    double time_difference;

    time_difference = difftime (current_time_in_seconds, last_read_in_seconds);
    res = (time_difference > TIME_LIMIT) ? 1 : 0;
    return res;
}

    extern time_t
get_last_time_written ()
{
    return last_time_written;
}

    extern void
set_last_time_written (time_t * new_time)
{
    last_time_written = *new_time;
    return;
}
