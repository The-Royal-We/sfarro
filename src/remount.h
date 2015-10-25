//
// Created by brendan on 25/10/15.
//

#include <stdbool.h>
#include <unistd.h>
#include <fuse.h>
#include <pthread.h>
#include <sys/mount.h>

#include "constants.h"
#include "timer.h"
#include "params.h"

#ifndef SFARRO_REMOUNT_H
#define SFARRO_REMOUNT_H
extern void init_sfarro_monitor();
int remount();
bool periodic_remount_check();

#endif //SFARRO_REMOUNT_H
