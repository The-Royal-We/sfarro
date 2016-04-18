/** @file
* The monitor observes the safe file system and triggers
* the remount function when we are ready to remount
*/

#ifndef MAIN_MONITOR_H
#define MAIN_MONITOR_H

#include <fuse.h>
#include <pthread.h>
#include <unistd.h>

#include "vfs.h"
#include "constants.h"
#include "timer.h"
#include "params.h"
#include "remount.h"

/**
* This function is passed into a pthread. Constantly monitors
* the safe filesystem and remounts device when ready
*/
void * periodic_remount_check();

/**
* Initalizes the monitor function
* @param *vfs_d Struct carrying both our root and mount directories
*/
extern void init_sfarro_monitor (struct vfs_state *vfs_d);


/**
* Accessor for device_is_ro flag
*/
extern int get_device_is_readonly();

/**
* Mutator for device_is_ro flag
* @param Value to set device_is_ro
*/
extern void set_device_is_readonly(int val);

extern volatile int device_is_ro;

#endif //MAIN_MONITOR_H
