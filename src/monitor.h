#ifndef MAIN_MONITOR_H
#define MAIN_MONITOR_H

#include <stdbool.h>
#include <fuse.h>
#include <pthread.h>
#include <unistd.h>

#include "constants.h"
#include "timer.h"
#include "params.h"
#include "remount.h"

extern void init_sfarro_monitor();

bool periodic_remount_check();

#endif //MAIN_MONITOR_H
