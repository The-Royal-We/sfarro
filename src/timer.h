#ifndef SFARRO_TIMER_H
#define SFARRO_TIMER_H
#include <stdio.h>
#include <time.h>

#include "constants.h"
extern int ready_to_remount();

time_t* get_current_time();
int is_last_written_time_over_limit();
int compare_times_to_limit(time_t *last_read, time_t *current_time);
void set_new_written_time_to_current_time ();

extern time_t get_last_time_written();
extern void set_last_time_written(time_t* new_time);

#endif //SFARRO_TIMER_H
