#include "vfs.h"

// TODO: Add in user defined remount period, else default to 30 s

/*
 * All paths that we pull out will be relative to the root
 * of the mounted filesystem. This method should pull out the
 * mountpoint, so we can construct all paths with the mount in
 * mind
 */


int set_vfs_to_read_only()
{
    read_only = 1;
    return 0;
}

int get_read_only()
{
    return read_only;
}
static void vfs_fullpath(char fpath[PATH_MAX], const char *path)
{
    strcpy(fpath, VFS_DATA->rootdir);
    strncat(fpath, path, PATH_MAX);
}

/*
 * Get object attributes
 */
    static int
vfs_getattr (const char *path, struct stat *stbuf)
{
    int res;
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);
    res = lstat(fpath, stbuf);

    if (res < 0)
        res = -ENOENT;
    return res;
}


/*
 *Get file attributes
 */

    static int
vfs_fgetattr (const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    int res;

    if(!strcmp(path,"/"))
        return vfs_getattr(path, stbuf);
    res = fstat (fi->fh, stbuf);

    if (res < 0)
        return -ENOENT;

    return res;
}

    static int
vfs_access (const char *path, int mask)
{
    int res = 0;
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);
    if(read_only && (mask & W_OK))
        return -EROFS;

    res = access(fpath, mask);

    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_readlink (const char *path, char *buf, size_t size)
{
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath (fpath, path);

    res = readlink (fpath, buf, size - 1);
    if (res < 0)
    {
        res = -errno;
    }
    else
    {
        buf[res] = '\0';
        res = 0;
    }
    return res;
}

    static int
vfs_readdir (const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
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

    closedir (dp);
    return 0;
}

    static int
vfs_mknod (const char *path, mode_t mode, dev_t rdev)
{
    if(read_only)
    {
        (void) path;
        (void) mode;
        (void) rdev;

        return -EROFS;
    }

    int res;
    char fpath[PATH_MAX];

    vfs_fullpath (fpath, path);

    if (S_ISREG (mode))
    {
        res = open (fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close (res);
    }
    else if (S_ISFIFO (mode))
        res = mkfifo (fpath, mode);
    else
        res = mknod (fpath, mode, rdev);
    if (res < 0)
        res = -errno;

    return res;
}

    static int
vfs_mkdir (const char *path, mode_t mode)
{
    if(read_only)
    {
        (void)path;
        (void)mode;

        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath (fpath, path);

    res = mkdir (fpath, mode);
    if (res < 0)
        return -errno;

    return res;
}

int vfs_releasedir(const char *path, struct fuse_file_info *fi)
{
    int res = 0;
    closedir((DIR *) (uintptr_t) fi->fh);

    return res;
}

    static int
vfs_unlink (const char *path)
{
    if(read_only)
    {
        (void) path;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath (fpath, path);

    res = unlink (fpath);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_rmdir (const char *path)
{
    if(read_only)
    {
        (void) path;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath (fpath, path);

    res = rmdir (fpath);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_symlink (const char *path, const char *link)
{
    if(read_only)
    {
        (void) path;
        (void) link;
        return -EROFS;
    }
    int res;
    char flink[PATH_MAX];

    vfs_fullpath (flink, link);

    res = symlink (path, flink);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_rename (const char *path, const char *newpath)
{
    if(read_only)
    {
        (void) path;
        (void) newpath;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    vfs_fullpath (fpath, path);
    vfs_fullpath (fnewpath, newpath);
    res = rename (fpath, fnewpath);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_link (const char *path, const char *newpath)
{
    if(read_only)
    {
        (void) path;
        (void) newpath;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    vfs_fullpath (fpath, path);
    vfs_fullpath (fnewpath, newpath);

    res = link (fpath, fnewpath);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_chmod (const char *path, mode_t mode)
{
    if(read_only)
    {
        (void) path;
        (void) mode;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);
    res = chmod (fpath, mode);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_chown (const char *path, uid_t uid, gid_t gid)
{
    if(read_only)
    {
        (void) path;
        (void) uid;
        (void) gid;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);

    res = lchown (fpath, uid, gid);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_truncate (const char *path, off_t size)
{
    if(read_only)
    {
        (void) path;
        (void) size;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);

    res = truncate (fpath, size);
    if (res < 0)
        return -errno;

    return res;
}

    static int
vfs_utime(const char *path, struct utimbuf *ubuf)
{
    if(read_only)
    {
        (void) path;
        (void) ubuf;
        return -EROFS;
    }
    int res = 0;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);
    res = utime(fpath, ubuf);
    if(res < 0)
        return -errno;
    return res;

}

#ifdef HAVE_UTIMENSAT
    static int
vfs_utimens (const char *path, const struct timespec ts[2])
{
    if(read_only)
    {
        (void) path;
        (void) ts;
        return -EROFS;
    }
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);

    /* don't use utime/utimes since they follow symlinks */
    res = utimensat (0, fpath, ts, AT_SYMLINK_NOFOLLOW);
    if (res < 0)
        return -errno;

    return 0;
}
#endif

/* We will allow file opens, unless writes are being made */

    static int
vfs_open (const char *path, struct fuse_file_info *fi)
{
    int res = 0;
    int fd;
    int flags = fi -> flags; 
    if(read_only)
    {
        if((flags & O_WRONLY) || (flags & O_RDWR) || (flags & O_CREAT) || (flags & O_EXCL) || (flags & O_TRUNC) || (flags &
                    O_APPEND))
        {
            return -EROFS;
        }
    }
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);

    fd = open (fpath, flags);

    if (res < 0)
        res = -errno;
    fi->fh = fd;
    return res;
}

    static int
vfs_read (const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    int res;
    res = pread (fi->fh, buf, size, offset);
    if (res < 0)
        res = -errno;
    return res;
}

/*
 * Set LAST_TIME_WRITTEN every time a successful write has taken place
 */

    static int
vfs_write (const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    if(read_only)
    {
        (void) path;
        (void) buf;
        (void) size;
        (void) offset;
        (void) fi;
        return -EROFS;
    }
    int res;

    res = pwrite (fi->fh, buf, size, offset);
    if (res < 0) {
        res = -errno;
    }
    set_new_written_time_to_current_time ();
    return res;
}

    void
set_new_written_time_to_current_time ()
{
    time_t current_time;
    time(&current_time);
    set_last_time_written (&current_time);
}

    static int
vfs_statfs (const char *path, struct statvfs *stbuf)
{
    int res;
    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);

    res = statvfs (fpath, stbuf);
    if (res == -1)
        return -errno;

    return res;
}

    static int
vfs_opendir(const char *path, struct fuse_file_info *fi)
{
    DIR *dp;

    char fpath[PATH_MAX];

    vfs_fullpath(fpath, path);
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    fi->fh = (intptr_t) dp;

    return 0;

}

void *vfs_init()
{
    return VFS_DATA;
}

    static int
vfs_release (const char *path, struct fuse_file_info *fi)
{
    (void) path;
    if(read_only)
    {
        (void) fi;
        return -EROFS;
    }
    int res;

    res = close(fi->fh);

    return res;
}

    static int
vfs_fsync (const char *path, int isdatasync, struct fuse_file_info *fi)
{
    (void) path;
    if(read_only)
    {
        (void) fi;
        (void) isdatasync;
        return -EROFS;
    }
    int res;

#ifdef HAVE_FDATASYNC
    if(isdatasync)
        res = fdatasync(fi->fh);
    else
#endif
        res = fsync(fi->fh);
    return res;
}

#ifdef HAVE_POSIX_FALLOCATE
    static int
vfs_fallocate (const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    if(read_only)
    {
        (void) path;
        (void) mode;
        (void) offset;
        (void) length;
        (void) fi;
        return -EROFS;
    }
    int fd;
    int res;

    (void) fi;

    if (mode)
        return -EOPNOTSUPP;

    fd = open (path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = -posix_fallocate (fd, offset, length);

    close (fd);
    return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
    static int
vfs_setxattr (const char *path, const char *name, const char *value,
        size_t size, int flags)
{
    if(read_only)
    {
        (void) path;
        (void) name;
        (void) value;
        (void) size;
        (void) flags;
        return -EROFS;
    }
    int res = 0;
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);
    lsetxattr (fpath, name, value, size, flags);
    if (res < 0)
        return -errno;
    return res;
}

    static int
vfs_getxattr (const char *path, const char *name, char *value, size_t size)
{

    int res = 0;
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);
    lgetxattr (fpath, name, value, size);
    if (res < 0)
        return -errno;
    return res;
}

    static int
vfs_listxattr (const char *path, char *list, size_t size)
{
    int res = 0;
    char fpath[PATH_MAX];
    vfs_fullpath(fpath, path);
    llistxattr (fpath, list, size);
    if (res < 0)
        return -errno;
    return res;
}

    static int
vfs_removexattr (const char *path, const char *name)
{
    if(read_only)
    {
        (void) path;
        (void) name;
        return -EROFS;
    }
    int res = lremovexattr (path, name);
    if (res == -1)
        return -errno;
    return res;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations vfs_oper = {
    .init = vfs_init,
    .getattr = vfs_getattr,
    .opendir = vfs_opendir,
    .fgetattr = vfs_fgetattr,
    .access = vfs_access,
    .readlink = vfs_readlink,
    .readdir = vfs_readdir,
    .mknod = vfs_mknod,
    .mkdir = vfs_mkdir,
    .symlink = vfs_symlink,
    .unlink = vfs_unlink,
    .rmdir = vfs_rmdir,
    .rename = vfs_rename,
    .link = vfs_link,
    .chmod = vfs_chmod,
    .chown = vfs_chown,
    .truncate = vfs_truncate,
    .utime = vfs_utime,
#ifdef HAVE_UTIMENSAT
    .utimens = vfs_utimens,
#endif
    .open = vfs_open,
    .read = vfs_read,
    .write = vfs_write,
    .statfs = vfs_statfs,
    .release = vfs_release,
    .fsync = vfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
    .fallocate = vfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
    .setxattr = vfs_setxattr,
    .getxattr = vfs_getxattr,
    .listxattr = vfs_listxattr,
    .removexattr = vfs_removexattr
#endif
};

static struct fuse_server {
	pthread_t pid;
	struct fuse *fuse;
	struct fuse_chan *ch;
	int failed;
	int running;
	char *mountpoint;
	int multithreaded;
	int foreground;
}fs;

static void *fuse_thread(){
	if(fs.multithreaded){
		printf("Using fuse_loop_mt()\n");
		if(fuse_loop_mt(fs.fuse) < 0){
			perror("Error in fuse_loop_mt");
			fs.failed = 1;
		}
		printf("Exiting fuse_loop now!");
	} else {
		printf("Using fuse_loop()\n");
		if (fuse_loop(fs.fuse) < 0){
			perror("Error in fuse_loop");
			fs.failed = 1;
		}
		printf("Exiting fuse_loop now!");
	}
	return NULL;
}

void run(int argc, char*argv[], struct vfs_state *vfs_data) //<- Keep an eye on this
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	int res;

	//Parse command line options;
	res = fuse_parse_cmdline(&args, &fs.mountpoint, &fs.multithreaded, &fs.foreground);
	if (res < 0){
		perror("Error parsing command line options");
		exit(1);
	}

	fuse_unmount(realpath(fs.mountpoint, NULL));
	fs.ch = fuse_mount(fs.mountpoint, &args);
	if(!fs.ch){
	    fuse_unmount(fs.mountpoint, fs.ch);
		perror("fuse_mount");
	}

	if(pthread_create(&fs.pid, NULL, fuse_thread, NULL) < 0){
		perror("pthread_create");
	}

	printf("Fuse server alive and operational!");
}

    int
vfs (int argc, char *argv[], struct vfs_state *vfs_data)
{
    int fuse_status;
    memset(&fs, 0, sizeof(fs));
    fs.running = 1;
    umask (0);
//  Initialize sfarro as a Read-Write system
    read_only = 0;
    fprintf (stderr, "Initialising monitor system.\n");
    fprintf (stderr, "Mounting %s to %s\n", argv[argc-1],  vfs_data->rootdir);
    fuse_status = fuse_main (argc, argv, &vfs_oper, vfs_data);
    fprintf (stderr, "vfs_rw_main returned %d\n", fuse_status);
    return fuse_status;
}
