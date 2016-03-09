#ifndef MAIN_MONITOR_H
#define MAIN_MONITOR_H

#include <fuse.h>
#include <pthread.h>
#include <unistd.h>

#include "vfs.h"
#include "constants.h"
#include "timer.h"
#include "monitor.h"
#include "params.h"

extern void init_sfarro_monitor (struct vfs_state *vfs_d);
int periodic_remount_check();

#endif //MAIN_MONITOR_H
