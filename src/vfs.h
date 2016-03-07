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
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#include <fuse/fuse_lowlevel.h>
#include "monitor.h"
#include "params.h"
#include "timer.h"

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

int read_only;
int vfs(int argc, char *argv[], struct vfs_state *vfs_data);

void sfarro_usage();
void set_new_written_time_to_current_time();
extern int set_vfs_to_read_only();
extern int get_read_only();
