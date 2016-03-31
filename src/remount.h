/** @file
* Remount.h handles all logic regarding remounting our deviice
*/

#ifndef SFARRO_REMOUNT_H
#define SFARRO_REMOUNT_H

#include <sys/mount.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "vfs.h"

/**
* Remounts device using the given remount path to
* the desired mode 
*  
* @param remount_path Path to device to be remounted
* @param remount_opt Remount options
* @return 1 if ready to remount, otherwise return 0
*/

extern int remount_device(const char * remount_path, const char * remount_opt);

#endif //SFARRO_REMOUNT_H
