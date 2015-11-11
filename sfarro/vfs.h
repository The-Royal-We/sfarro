#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#define PATH_MAX 300

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include "constants.h"
#include "timer.h"
#include "params.h"

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif



int vfs(int argc, char *argv[]);

void sfarro_usage();
static int vfs_chmod(const char *path, mode_t mode);
static int vfs_statfs(const char *path, struct statvfs *stbuf);

void set_new_written_time_to_current_time();
