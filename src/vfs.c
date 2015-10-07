#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#include <fuse.h>
#include <unlockmgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include <sys/file.h>

static int vfs_getattr(const char *path, struct stat *stbuf)
{
  int res;

  res = lstat(path, stbuf);

  if (res == -1 ) {
    return -errno;
  }
  return 0;
}

static int vfs_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
  int res;

  (void) path;

  res = fstat(fi -> fh, stbuf);

  if(res == -1)
    return -errno;

  return 0;
}

static int vfs_access(const char *path, char *buf, size_t size)
{
  int res;
  res = access(path, mask); 
  
  if(res == -1) 
    return -errno; 

  buf[res] = '\0';
  return 0;
}

static int vfs_readlink(const char *path, char *buf, size_t size)
{
  int res;
  res = readlink(path, buf, size - 1);
  if (res == -1 ) {
    return -errno;
  }
}
