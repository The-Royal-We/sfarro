#include "vfs_ro.h"

static void vfs_ro_fullpath(char fpath[PATH_MAX], const char *path)
{
    //TODO: Refactor this to use RO specific path
    strcpy(fpath, VFS_DATA->rootdir);
    strncat(fpath, path, PATH_MAX);
}

    static int
vfs_ro_getattr(const char *path, struct stat *st_data)
{
    int res;
    char fpath[PATH_MAX] ;
    vfs_ro_fullpath(fpath, path);

    res=lstat(fpath);
    free(fpath);
    if (res < 0)
        return -errno
            return res;
}

    static int
vfs_ro_fgetattr (const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    int res;

    if(!strcmp(path,"/"))
        return vfs_ro_getattr(path, stbuf);
    res = fstat (fi->fh, stbuf);

    if (res < 0)
        return -ENOENT;

    return res;
}

    static int
vfs_ro_access(const char *path, int mask)
{
    int res;
    char fpath[PATH_MAX] ;
    vfs_ro_fullpath(fpath, path);

    // Make sure we block writing
    if (mask & W_OK)
        return -EROFS;
    res = access(fpath);
    free(fpath);
    if (res == -1)
        return -errno;

    return res;
}

    static int
vfs_ro_readlink(const char *path, char *buf, size_t size)
{
    int res;
    char fpath[PATH_MAX] ;
    vfs_ro_fullpath(fpath, path);

    res = readlink(fpath, buf, size - 1);
    free(fpath);
    if(res < 0)
        return -errno;
    buf[res] == '\0';
    return res;

}

    static int
vfs_ro_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;

    (void) offset;

    dp = (DIR *) (uintptr_t) fi->fh;

    de = readdir(dp);

    if (de == NULL)
        return -errno;

    do{
        if(filler(buf, de->d_name, NULL, 0)!=0) {
            return -ENOMEM;
        }
    } while ((de = readdir (dp)) != NULL);
    closedir(dp);
    return res;
}

    static int
vfs_ro_mknod(const char *path, mode_t mode, dev_t rdev)
{
    (void) path;
    (void) node;

    return -EROFS;
}

    static int
vfs_ro_mkdir (const char *path, mode_t mode)
{
    (void) path;
    (void) mode;

    return -EROFS;
}

    static int
vfs_ro_unlink (const char *path)
{
    (void) path;
    return -EROFS;
}

    static int
vfs_ro_rmdir (const char *path)
{
    (void) path;
    return -EROFS;
}

    static int
vfs_ro_symlink (const char * from, const char * to)
{
    (void) from;
    (void) to;

    return -EROFS
}

    static int
vfs_ro_chmod (const char * path, mode_t mode)
{
    (void) path;
    (void) mode;

    return -EROFS;
}

    static int
vfs_ro_chown (const char * path, uid_t uid, gid_t gid)
{
    (void) path;
    (void) uid;
    (void) gid;

    return -EROFS;
}

    static int
vfs_ro_truncate (const char * path, off_t size)
{

    (void) path;
    (void) size;

    return -EROFS;
}

    static int
vfs_ro_utime (const char *path, struct utimbuf *buf)
{
    (void) path;
    (void) buf;

    return -EROFS;
}

    static int
vfs_ro_open (const char *path, struct fuse_file_info *finfo)
{
    int res;

    /* Allow file opens, unless writes are being made */

    int flags = finfo -> flags;

    if((flags & O_WRONLY) || (flags & O_RDWR) || (flags & O_CREAT) || (flags & O_EXCL) || (flags & O_TRUNC) || (flags &
                O_APPEND))
    {
        return -EROFS
    }

    char fpath[PATH_MAX];
    vfs_ro_fullpath(fpath, path);
    res = open(fpath, flags);

    free(fpath);
    if (res < 0)
        return -errno;
    close(res);
    return 0;
}

    static int
vfs_ro_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
    int fd;
    int res;
    (void) finfo;
    char fpath[PATH_MAX];
    vfs_ro_fullpath(fpath, path);
    fd = open(fpath, O_RDONLY);
    free(fpath);
    if (fd < 0)
    {
        res = -errno;
        return res;
    }
    res = pread(fd, buf, size, offset);

    if(res < 0)
    {
        res = -errno;
    }
    close(fd);
    return res;
}

    static int
vfs_ro_statfs (const char * path, struct statvfs * st_buf )
{
    int res;
    char fpath[PATH_MAX];
    vfs_ro_fullpath(fpath, path);

    res = statvfs(fpath, st_buf);
    if(res < 0)
        return -errno;

    return 0
}

    static int
vfs_ro_release (const char *path, struct fuse_file_info *finfo)
{
    (void) path;
    (void) finfo;

    return 0;
}

    static int
vfs_ro_setxattr (const char *path, const char *name, const char *value, size_t size, int flags)
{
    (void)path;
    (void)name;
    (void)value;
    (void)size;
    (void)flags;
    return -EROFS;
}

    static int
vfs_ro_getxattr (const char *path, const char *name, char *value, size_t size)
{
    int res;
    char fpath[PATH_MAX];
    vfs_ro_fullpath(fpath, path);
    res = lgetxattr(fpath, name, value, size);
    free(fpath);
    if(res < 0)
        return -errno;
    return res;
}

    static int 
vfs_ro_listxattr (const char *path, char *list, size_t size)
{
    int res;
    char fpath[PATH_MAX];
    vfs_ro_fullpath(fpath, path);

    res = llistxattr(fpath, list, size);

    if(res < 0)
        return -errno;
    return res;
}

    static int
vfs_ro_removexattr(const char *path, const char *name)
{
    (void)path;
    (void)name;

    return -EROFS;
}

static struct fuse_operations vfs_oper = {
    .getattr = vfs_ro_getattr,
    .opendir = vfs_ro_opendir,
    .fgetattr = vfs_ro_fgetattr,
    .access = vfs_ro_access,
    .readlink = vfs_ro_readlink,
    .readdir = vfs_ro_readdir,
    .mknod = vfs_ro_mknod,
    .mkdir = vfs_ro_mkdir,
    .symlink = vfs_ro_symlink,
    .unlink = vfs_ro_unlink,
    .rmdir = vfs_ro_rmdir,
    .rename = vfs_ro_rename,
    .link = vfs_ro_link,
    .chmod = vfs_ro_chmod,
    .chown = vfs_ro_chown,
    .truncate = vfs_ro_truncate,
    .utime = vfs_ro_utime,
#ifdef HAVE_UTIMENSAT
    .utimens = vfs_ro_utimens,
#endif
    .open = vfs_ro_open,
    .read = vfs_ro_read,
    .write = vfs_ro_write,
    .statfs = vfs_ro_statfs,
    .release = vfs_ro_release,
    .fsync = vfs_ro_fsync,
#ifdef HAVE_SETXATTR
    .setxattr = vfs_setxattr,
    .getxattr = vfs_getxattr,
    .listxattr = vfs_listxattr,
    .removexattr = vfs_removexattr
#endif
};

int vfs_ro_main (struct vfs_state *vfs_data, )
{
    int fuse_status;
    char *dirs[] = {vfs_data->mountdir, vfs_data->rootdir};

    fprintf (stderr, "Calling vfs_ro_main\n");
    umask(0);
    fuse_status = (sizeof(dirs), dirs, &vfs_oper, vfs_data);
    return fuse_status;


}

