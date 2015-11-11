//
// Created by brendan on 25/10/15.
//

#ifndef SFARRO_REMOUNT_H
#define SFARRO_REMOUNT_H

#include <sys/mount.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

int remount(char *remount_directory);

#endif //SFARRO_REMOUNT_H
