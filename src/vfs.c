#include <stdlib.h>
#include "vfs.h"
#include "log.h"

/*
 * Adding in custom error handler
 * Want to report all errors to a logfile instead of stdout
 */



static int vfs_error(char *str) {
    int ret = -errno;
    log_msg("   ERROR %s: %s \n", str, strerror(errno));
    return ret;
}

/*
 * All paths that we pull out will be relative to the root
 * of the mounted filesystem. This method should pull out the
 * mountpoint, so we can construct all paths with the mount in
 * mind
 */

static void vfs_fullpath(char fpath[PATH_MAX], const char *path) {
    strcpy(fpath, VFS_DATA->rootdir);
    strncat(fpath, path, PATH_MAX);

    log_msg("    vfs_fullpath:  rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
            VFS_DATA->rootdir, path, fpath);
}


/*
 * Get object attributes
 */

static int vfs_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[PATH_MAX];

    log_msg("\nvfs_fgetattr(path=\"%s\", stbuf=0x%08x\n",
            path, stbuf);
    vfs_fullpath(fpath, path);

    res = lstat(path, stbuf);

    if (res != 0) {
        res = vfs_error("vfs_gettr lstat");
    }

    log_stat(stbuf);
    return res;
}


/*
 *Get file attributes
 */

static int vfs_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

    int res;
    (void) path;

    log_msg("\nvfs_fgetattr(path=\"%s\", stbuf=0x%08x\n",
            path, stbuf);

    res = fstat(fi->fh, stbuf);

    if (res != -1)
        return vfs_error("vfs_fgettr fstat");

    return res;
}

static int vfs_access(const char *path, char *buf, int mask) {
    int res;
    res = access(path, mask);

    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}

static int vfs_readlink(const char *path, char *buf, size_t size) {
    int res;
    char fpath[PATH_MAX];

    log_msg("bb_readlink(path=\"%s\", link=\"%s\", size=%d)\n",
            path, buf, size);
    vfs_fullpath(fpath, path);

    res = readlink(path, buf, size - 1);
    if (res < 0) {
        res = vfs_error("vfs_readlink readlink");
    } else {
        buf[res] = '\0';
        res = 0;
    }
    return res;
}

static int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int vfs_mknod(const char *path, mode_t mode, dev_t rdev) {
    int res;
    char fpath[PATH_MAX];

    log_msg("\nbb_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",
            path, mode, rdev);
    vfs_fullpath(fpath, path);

    if (S_ISREG(mode)) {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(path, mode);
    else
        res = mknod(path, mode, rdev);
    if (res < 0)
        res = vfs_error("vfs_mknod mknod");

    return res;
}

static int vfs_mkdir(const char *path, mode_t mode) {
    int res;

    res = mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_unlink(const char *path) {
    int res;

    res = unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_rmdir(const char *path) {
    int res;

    res = rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_symlink(const char *from, const char *to) {
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_rename(const char *from, const char *to) {
    int res;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_link(const char *from, const char *to) {
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_chmod(const char *path, mode_t mode) {
    int res;

    res = chmod(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_chown(const char *path, uid_t uid, gid_t gid) {
    int res;

    res = lchown(path, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_truncate(const char *path, off_t size) {
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

static int vfs_open(const char *path, struct fuse_file_info *fi) {
    int res;

    res = open(path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int vfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
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
                     off_t offset, struct fuse_file_info *fi) {
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

static int vfs_statfs(const char *path, struct statvfs *stbuf) {
    int res;

    res = statvfs(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int vfs_release(const char *path, struct fuse_file_info *fi) {
    /* Just a stub.  This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) fi;
    return 0;
}

static int vfs_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi) {
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

int vfs(int argc, const char *argv[]) {
    int fuse_status;
    struct vfs_state *vfs_data;

    if ((getuid() == 0) || (geteuid() == 0)) {
        fprintf(stderr, "Running sfarro as root opens security holes\n");
        fprintf(stderr, "But it's cool brah\n");
//        return 1;
    }
    vfs_data = malloc(sizeof(struct vfs_state));

    if (vfs_data == NULL) {
        perror("main calloc");
        abort();
    }

    /*
     * Pull the root directory from the argument list and save it in my internal data
     */

    vfs_data->rootdir = realpath(argv[1], NULL);

    fprintf(stderr, "Allocated rootdir: %s", vfs_data->rootdir);

//    argv[argc-2] = argv[argc-1];
//    argv[argc-1] = NULL;
//    argc--;

    vfs_data->logfile = log_open();

    FILE* lgs = vfs_data->logfile;

    fprintf(stderr, "Calling fuse_main\n");
    fuse_status = fuse_main(argc, argv, &vfs_oper, vfs_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_status);
    umask(0);
    return fuse_status;
}

