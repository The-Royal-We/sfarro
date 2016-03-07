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

int remount(const char *remount_directory);

#endif //SFARRO_REMOUNT_H
