//
// Created by brendan on 23/10/15.
//

#ifndef SFARRO_TIMER_H
#define SFARRO_TIMER_H
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

#include "constants.h"

time_t LAST_TIME_WRITTEN = LONG_MAX; // Initialize with (practically) infinity. Represents no write being made

extern bool ready_to_remount();
time_t* get_current_time();
int is_last_written_time_over_limit();
int compare_times_to_limit(time_t *last_read, time_t *current_time);

#endif //SFARRO_TIMER_H
