/** @file
* The timer handles the logic required to track when the last write was made
* and determines whether or not SFARRO is ready to remount our mountpoint
* to readonly or not
*/

#ifndef SFARRO_TIMER_H
#define SFARRO_TIMER_H
#include <stdio.h>
#include <time.h>

#include "constants.h"
/**
* Gets the current time of the system as a Unix timestamp
* @return Current timestamp of system on invocation
*/
time_t* get_current_time();

/**
* Compares the last_time_written with the system current time
* @return 1 if time difference exceeds the limit, else 0
*/
int is_last_written_time_over_limit();

/**
* Gets the current time of the system as a Unix timestamp
* @param last_written time_t containing last time written
* @param current_time The current running time on method invocation
* @return 1 if time difference between last_written and current
* time exceeds time limit, else return 0
*/
int compare_times_to_limit(time_t *last_written, time_t *current_time);

/**
* Mutat
* @param last_written time_t containing last time written
*/
void set_new_written_time_to_current_time ();

/**
* Checks if SFARRO should remount the mountpoint
* @return 1 if ready to remount, otherwise return 0
*/
extern int ready_to_remount();

/**
* Accessor for LAST_TIME_WRITTEN method
* @return 1 if ready to remount, otherwise return 0
*/
extern time_t get_last_time_written();
/**
* Mutator for LAST_TIME_WRITTEN method
* @param new_time Value to set last_time_written to
* @return 1 if ready to remount, otherwise return 0
*/
extern void set_last_time_written(time_t* new_time);

#endif //SFARRO_TIMER_H
