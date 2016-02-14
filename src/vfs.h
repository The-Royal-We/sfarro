#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

#include "constants.h"
#include "timer.h"
#include "params.h"

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

int vfs(int argc, char *argv[]);

void sfarro_usage();
void set_new_written_time_to_current_time();
