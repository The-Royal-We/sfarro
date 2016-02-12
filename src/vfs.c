#include "vfs.h"

// TODO: Add in user defined remount period, else default to 30 s

/*
 * All paths that we pull out will be relative to the root
 * of the mounted filesystem. This method should pull out the
 * mountpoint, so we can construct all paths with the mount in
 * mind
 */

/* Settings */

static struct Settings {
	char *mntsrc;
	char *mntdest;
	char* working_dir;
	int mntsrc_fd;
	int mntdest_len;
} settings;

static void vfs_fullpath(char fpath[PATH_MAX], const char *path) {
	strcpy(fpath, VFS_DATA->rootdir);
	strncat(fpath, path, PATH_MAX);
}

static void *vfs_init()
{
	assert(settings.mntsrc_fd > 0);

	if(fchdir(settings.mntsrc_fd) != 0){
		fprintf(
				stderr,
				"Could not change working directory to '%s': %s\n",
				settings.mntsrc,
				strerror(errno)
				);
		fuse_exit(fuse_get_context()->fuse);
	}
	return NULL;
}

static char *process_path(const char *path)
{
	if(path == NULL){
		errno = EINVAL;
		return NULL;
	}

	while(*path == '/')
		++path;

	if(*path == '\0')
		path = ".";

	char* result = realpath(path, NULL);
	if (result == NULL){
		return strdup(path);
	} else if (strncmp(result, settings.mntdest, settings.mntdest_len) == 0){
		/* Recursive call. Cannot handle this without deadlocking */
		DPRINTF("Denying recursive access to mountpoint `%s'", result);
		free(result);
		errno = EPERM;
		return NULL;
	}
		return result;

}

/*
 * Get object attributes
 */
static int vfs_getattr(const char *path, struct stat *stbuf) {
	int res;
	char *real_path;

	real_path = process_path(path);
	if(real_path == NULL)
		return -errno;

	res = lstat(path, stbuf);

	if (res != 0) {
		res = -errno;
	}
	return res;
}


/*
 *Get file attributes
 */

static int vfs_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

	int res;
	(void) path;

	res = fstat(fi->fh, stbuf);

	if (res != -1)
		return -errno;

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

	vfs_fullpath(fpath, path);

	res = readlink(path, buf, size - 1);
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
		res = -errno;

	return res;
}

static int vfs_mkdir(const char *path, mode_t mode) {
	int res;
	res = mkdir(path, mode);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_unlink(const char *path) {
	int res;

	res = unlink(path);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_rmdir(const char *path) {
	int res;

	res = rmdir(path);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_symlink(const char *from, const char *to) {
	int res;

	res = symlink(from, to);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_rename(const char *from, const char *to) {
	int res;

	res = rename(from, to);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_link(const char *from, const char *to) {
	int res;

	res = link(from, to);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_chmod(const char *path, mode_t mode) {
	int res;

	res = chmod(path, mode);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_chown(const char *path, uid_t uid, gid_t gid) {
	int res;

	res = lchown(path, uid, gid);
	if (res < 0)
		return -errno;

	return res;
}

static int vfs_truncate(const char *path, off_t size) {
	int res;

	res = truncate(path, size);
	if (res < 0)
		return -errno;

	return res;
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
/*
 * Set LAST_TIME_WRITTEN every time a successful write has taken place
 */

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

	set_new_written_time_to_current_time();

	return res;
}

void set_new_written_time_to_current_time() {
	set_last_time_written(get_current_time());
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
/*
   static void setup_signal_handling()
   {
   struct sigaction sa;
   sa.sa_handler = signal_handler;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;

   sigaction(SIGUSR1, &sa, NULL);
   }

   static void signal_handler(int sig)
   {
   invalidate_user_cache();
   }*/

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
	.init     = vfs_init,
	.getattr  = vfs_getattr,
	.fgetattr = vfs_fgetattr,
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

static char *get_working_dir()
{
	size_t buf_size = 4096;
	char* buf = mallow(buf_size);
	while(!getcwd(buf, buf_size)){
		buf_size *= 2;
		buf = realloc(buf, buf_size);
	}
	return buf;
}

static void at_exitfunc()
{
	free(settings.mntsrc);
	free(settings.mntdest);
	free(settings.working_dir);
}

static int process_option(void *data, const char *arg,
		struct fuse_args *outargs)
{
	if(!settings.mntsrc) {
		settings.mntsrc = realpath(arg, NULL);
		if(settings.mntsrc == NULL){
			fprintf(stderr, "Failed to resolve source directory %s:", arg);
			return -1;
		}
	}
	if(!settings.mntdest) {
		if(settings.mntdest == NULL) {
			fprintf(stderr, "Failed to resolve mount directory %s:", arg);
			return -1;
		}
	}
	if(!settings.working_dir) {
		if(settings.mntdest == NULL) {
			fprintf(stderr, "Failed to resolve mount directory %s:", arg);
			return -1;
		}
	}
}

int vfs(int argc, char *argv[]) {
	int fuse_status;
	struct vfs_state *vfs_data;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	struct OptionData {
		char *mount_path;
		char *mount_dest;
		int multithreaded;
	} od;

	/* Initalising settings */
	memset(&od, 0 , sizeof(od));
	settings.mntsrc = NULL;
	settings.mntdest = NULL;
	settings.working_dir = NULL;
	atexit(&at_exitfunc());

	if(fuse_opt_parse(&args, &od, NULL, &process_option)){
		return 1;
	}

	if ((getuid() == 0) || (geteuid() == 0))
	{
		fprintf(stderr, "Running sfarro as root opens security holes\n");
		      return 1;
	}
	vfs_data = malloc(sizeof(struct vfs_state));

	if (vfs_data == NULL) {
		perror("main calloc");
		abort();
	}


	/*
	 * Pull the root directory from the argument list and save it in my internal data
	 */


	vfs_data->rootdir = realpath(argv[2], NULL);
	argv[argc - 2] = argv[argc - 1];
	argv[argc - 1] = NULL;
	argc--;

	fprintf(stderr, "Allocated rootdir: %s\n", vfs_data->rootdir);
	fprintf(stderr, "Calling fuse_main\n");
	umask(0);
	fuse_status = fuse_main(argc, argv, &vfs_oper, vfs_data);
	fprintf(stderr, "fuse_main returned %d\n", fuse_status);
	return fuse_status;
}

