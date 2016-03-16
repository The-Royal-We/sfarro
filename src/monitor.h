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


int periodic_remount_check();

extern void init_sfarro_monitor (struct vfs_state *vfs_d);
extern int get_device_is_readonly();
extern void set_device_is_readonly(int val);

int device_is_ro;

#endif //MAIN_MONITOR_H
