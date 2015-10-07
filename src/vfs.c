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
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

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

static int vfs_access(const char *path, char *buf, int mask)
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

static int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
  off_t offset, struct fuse_file_info *fi)
{
  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;

  dp = opendir(path);
  if(dp == NULL)
    return -errno;

  while( (de = readdir(dp)) != NULL ) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    if(filler (buf, de->d_name, &st,0))
      break;
  }

  closedir(dp);
  return 0;
}

static vfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
  int res;

  if(S_ISREG(mode)) {
    res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (res >= 0)
      res = close(res);
  } else if (S_ISFIFO(mode))
      res = mkfifo(path, mode);
    else
      res = mknod(path, mode, rdev);
    if(res == -1)
      return -errno;

    return 0;
}

static int vfs_mkdir(const char *path, mode_t mode)
{
  int res;

  res = mkdir(path, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_unlink(const char *path)
{
  int res;

  res = unlink(path);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_rmdir(const char *path)
{
  int res;

  res = rmdir(path);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_symlink(const char *from, const char *to)
{
  int res;

  res = symlink(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_rename(const char *from, const char *to)
{
  int res;

  res = rename(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_link(const char *from, const char *to)
{
  int res;

  res = link(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_chmod(const char *path, mode_t mode)
{
  int res;

  res = chmod(path, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_chown(const char *path, uid_t uid, gid_t gid)
{
  int res;

  res = lchown(path, uid, gid);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_truncate(const char *path, off_t size)
{
  int res;

  res = truncate(path, size);
  if (res == -1)
    return -errno;

  return 0;
}

#ifdef HAVE_UTIMENSAT
static int vfs_utimens(const char *path, const struct timespec ts[2])
{
  int res;

  /* don't use utime/utimes since they follow symlinks */
  res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
  if (res == -1)
    return -errno;

  return 0;
}
#endif

static int vfs_open(const char *path, struct fuse_file_info *fi)
{
  int res;

  res = open(path, fi->flags);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
}

static int vfs_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
  int fd;
  int res;

  (void) fi;
  fd = open(path, O_RDONLY);
  if (fd == -1)
    return -errno;

  res = pread(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static int vfs_write(const char *path, const char *buf, size_t size,
         off_t offset, struct fuse_file_info *fi)
{
  int fd;
  int res;

  (void) fi;
  fd = open(path, O_WRONLY);
  if (fd == -1)
    return -errno;

  res = pwrite(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static int vfs_statfs(const char *path, struct statvfs *stbuf)
{
  int res;

  res = statvfs(path, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int vfs_release(const char *path, struct fuse_file_info *fi)
{
  /* Just a stub.  This method is optional and can safely be left
     unimplemented */

  (void) path;
  (void) fi;
  return 0;
}

static int vfs_fsync(const char *path, int isdatasync,
         struct fuse_file_info *fi)
{
  /* Just a stub.  This method is optional and can safely be left
     unimplemented */

  (void) path;
  (void) isdatasync;
  (void) fi;
  return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int vfs_fallocate(const char *path, int mode,
      off_t offset, off_t length, struct fuse_file_info *fi)
{
  int fd;
  int res;

  (void) fi;

  if (mode)
    return -EOPNOTSUPP;

  fd = open(path, O_WRONLY);
  if (fd == -1)
    return -errno;

  res = -posix_fallocate(fd, offset, length);

  close(fd);
  return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int vfs_setxattr(const char *path, const char *name, const char *value,
      size_t size, int flags)
{
  int res = lsetxattr(path, name, value, size, flags);
  if (res == -1)
    return -errno;
  return 0;
}

static int vfs_getxattr(const char *path, const char *name, char *value,
      size_t size)
{
  int res = lgetxattr(path, name, value, size);
  if (res == -1)
    return -errno;
  return res;
}

static int vfs_listxattr(const char *path, char *list, size_t size)
{
  int res = llistxattr(path, list, size);
  if (res == -1)
    return -errno;
  return res;
}

static int vfs_removexattr(const char *path, const char *name)
{
  int res = lremovexattr(path, name);
  if (res == -1)
    return -errno;
  return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations vfs_oper = {
  .getattr  = vfs_getattr,
  .access   = vfs_access,
  .readlink = vfs_readlink,
  .readdir  = vfs_readdir,
  .mknod    = vfs_mknod,
  .mkdir    = vfs_mkdir,
  .symlink  = vfs_symlink,
  .unlink   = vfs_unlink,
  .rmdir    = vfs_rmdir,
  .rename   = vfs_rename,
  .link   = vfs_link,
  .chmod    = vfs_chmod,
  .chown    = vfs_chown,
  .truncate = vfs_truncate,
#ifdef HAVE_UTIMENSAT
  .utimens  = vfs_utimens,
#endif
  .open   = vfs_open,
  .read   = vfs_read,
  .write    = vfs_write,
  .statfs   = vfs_statfs,
  .release  = vfs_release,
  .fsync    = vfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
  .fallocate  = vfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
  .setxattr = vfs_setxattr,
  .getxattr = vfs_getxattr,
  .listxattr  = vfs_listxattr,
  .removexattr  = vfs_removexattr,
#endif
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &vfs_oper, NULL);
}

