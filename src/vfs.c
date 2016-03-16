#include "vfs.h"

// TODO: Add in user defined remount period, else default to 30 s

/*
 * All paths that we pull out will be relative to the root
 * of the mounted filesystem. This method should pull out the
 * mountpoint, so we can construct all paths with the mount in
 * mind
 */

static void vfs_fullpath(char fpath[PATH_MAX], const char *path) {
	strcpy(fpath, VFS_DATA->rootdir);
	strncat(fpath, path, PATH_MAX);
}

/*
 * Get object attributes
 */
static int vfs_getattr(const char *path, struct stat *stbuf) {
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

static int vfs_fgetattr(const char *path, struct stat *stbuf,
		struct fuse_file_info *fi) {
	int res;

	if (!strcmp(path, "/"))
		return vfs_getattr(path, stbuf);
	res = fstat(fi->fh, stbuf);

	if (res < 0)
		return -ENOENT;

	return res;
}

static int vfs_access(const char *path, int mask) {
	int res = 0;
	char fpath[PATH_MAX];
	vfs_fullpath(fpath, path);

	res = access(fpath, mask);

	if (res < 0)
		return -errno;

	return res;
}

static int vfs_readlink(const char *path, char *buf, size_t size) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = readlink(fpath, buf, size - 1);
	if (res < 0) {
		res = -errno;
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

	dp = (DIR *) (uintptr_t) fi->fh;

	de = readdir(dp);

	if (de == NULL)
		return -errno;

	do {
		if (filler(buf, de->d_name, NULL, 0) != 0) {
			return -ENOMEM;
		}

	} while ((de = readdir(dp)) != NULL);

	closedir(dp);
	return 0;
}

static int vfs_mknod(const char *path, mode_t mode, dev_t rdev) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res < 0)
		res = -errno;

	return res;
}

static int vfs_mkdir(const char *path, mode_t mode) {

	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = mkdir(fpath, mode);
	if (res < 0)
		return -errno;

	return res;
}

int vfs_releasedir(const char *path, struct fuse_file_info *fi) {
	int res = 0;
	closedir((DIR *) (uintptr_t) fi->fh);

	return res;
}

static int vfs_unlink(const char *path) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = unlink(fpath);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_rmdir(const char *path) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = rmdir(fpath);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_symlink(const char *path, const char *link) {
	int res;
	char flink[PATH_MAX];

	vfs_fullpath(flink, link);

	res = symlink(path, flink);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_rename(const char *path, const char *newpath) {
	int res;
	char fpath[PATH_MAX];
	char fnewpath[PATH_MAX];

	vfs_fullpath(fpath, path);
	vfs_fullpath(fnewpath, newpath);
	res = rename(fpath, fnewpath);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_link(const char *path, const char *newpath) {
	int res;
	char fpath[PATH_MAX];
	char fnewpath[PATH_MAX];

	vfs_fullpath(fpath, path);
	vfs_fullpath(fnewpath, newpath);

	res = link(fpath, fnewpath);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_chmod(const char *path, mode_t mode) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);
	res = chmod(fpath, mode);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_chown(const char *path, uid_t uid, gid_t gid) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = lchown(fpath, uid, gid);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_truncate(const char *path, off_t size) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = truncate(fpath, size);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_utime(const char *path, struct utimbuf *ubuf) {
	int res = 0;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);
	res = utime(fpath, ubuf);
	if (res < 0)
		return -errno;
	return res;

}

#ifdef HAVE_UTIMENSAT
static int
vfs_utimens (const char *path, const struct timespec ts[2])
{
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

static int vfs_open(const char *path, struct fuse_file_info *fi) {
	int res = 0;
	int fd;
	int flags = fi->flags;
	char fpath[PATH_MAX];
	vfs_fullpath(fpath, path);

	if(get_device_is_readonly() == 1){
		remount_device(VFS_DATA->rootdir, "rw");
		set_device_is_readonly(0);
	}


	fd = open(fpath, flags);

	if (res < 0)
		res = -errno;
	fi->fh = fd;
	return res;
}

static int vfs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	int res;
	res = pread(fi->fh, buf, size, offset);
	if (res < 0)
		res = -errno;
	return res;
}

/*
 * Set LAST_TIME_WRITTEN every time a successful write has taken place
 */

static int vfs_write(const char *path, const char *buf, size_t size,
		off_t offset, struct fuse_file_info *fi) {
	int res;

	res = pwrite(fi->fh, buf, size, offset);
	if (res < 0) {
		res = -errno;
	}
	set_new_written_time_to_current_time();
	return res;
}

void set_new_written_time_to_current_time() {
	time_t current_time;
	time(&current_time);
	set_last_time_written(&current_time);
}

static int vfs_statfs(const char *path, struct statvfs *stbuf) {
	int res;
	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);

	res = statvfs(fpath, stbuf);
	if (res == -1)
		return -errno;

	return res;
}

static int vfs_opendir(const char *path, struct fuse_file_info *fi) {
	DIR *dp;

	char fpath[PATH_MAX];

	vfs_fullpath(fpath, path);
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;
	fi->fh = (intptr_t) dp;

	return 0;

}

void *vfs_init() {
	return VFS_DATA;
}

static int vfs_release(const char *path, struct fuse_file_info *fi) {
	(void) path;
	int res;

	res = close(fi->fh);

	return res;
}

static int vfs_fsync(const char *path, int isdatasync,
		struct fuse_file_info *fi) {
	(void) path;
	int res;

#ifdef HAVE_FDATASYNC
	if (isdatasync)
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
	int res = lremovexattr (path, name);

	if (res == -1)
	return -errno;
	return res;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations vfs_oper = { .init = vfs_init, .getattr =
		vfs_getattr, .opendir = vfs_opendir, .fgetattr = vfs_fgetattr, .access =
		vfs_access, .readlink = vfs_readlink, .readdir = vfs_readdir, .mknod =
		vfs_mknod, .mkdir = vfs_mkdir, .symlink = vfs_symlink, .unlink =
		vfs_unlink, .rmdir = vfs_rmdir, .rename = vfs_rename, .link = vfs_link,
		.chmod = vfs_chmod, .chown = vfs_chown, .truncate = vfs_truncate,
		.utime = vfs_utime,
#ifdef HAVE_UTIMENSAT
		.utimens = vfs_utimens,
#endif
		.open = vfs_open, .read = vfs_read, .write = vfs_write, .statfs =
				vfs_statfs, .release = vfs_release, .fsync = vfs_fsync,
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

extern int initialize_vfs(int argc, char *argv[], struct vfs_state *vfs_data) {
	int res;
	umask(0);
	res = fuse_main(argc, argv, &vfs_oper, vfs_data);
	return res;

}
