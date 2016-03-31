/** @file
* This file defines our safe filesystem that mounts from the 
* mount directory from the user input to the root directory
*/

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
#include <pthread.h>

#include <fuse/fuse_lowlevel.h>
#include "monitor.h"
#include "params.h"
#include "timer.h"

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

/**
* Creates our remount monitor thread and initalizes our
* safe filsystem.
* 
* @param fpath Character array to contain new filepath resolved against rootpath
* @param *path Current filepath
*/

void vfs_fullpath(char fpath[PATH_MAX], const char *path);

/**
* Gets attributes from object
* 
* @param path Path to object
* @param stbuf Empty stat buffer
* @return ENOENT if lstat (2) fails, else 0
*/
int vfs_getattr (const char *path, struct stat *stbuf);

/**
* Gets attributes from file
* 
* @param *path Path to file
* @param *stbuf Empty stat buffer
* @param *fi FUSE file information
* @return ENOENT if fstat (2) fails, else 0
*/
int vfs_fgetattr (const char *path, struct stat *stbuf, struct fuse_file_info *fi);

/**
* Access file
* 
* @param *path Path to file
* @param mask File access mask
* @return errno if access (2) failed, else result of previous
*/
int vfs_access (const char *path, int mask);

/**
* Read symlink
* 
* @param *path Path to symlink
* @param *buf Empty character buffer
* @param size Size of returned path
* @return errno if readlink (2) failed, else result of previous
*/
int vfs_readlink (const char *path, char *buf, size_t size);

/**
* Read directory contents
* 
* @param *path Path to directory
* @param *buf Empty character buffer
* @param filler FUSE filler
* @param offset Buffer offset
* @param *fi FUSE file information
* @return 0
*/
int vfs_readdir (const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi);

/**
* Create filesystem node
* 
* @param *path Desired creation path
* @param mode File mode
* @param rdev Dev attributes
* @return errno if mknod (2) failed, else result of previous
*/
int vfs_mknod (const char *path, mode_t mode, dev_t rdev);

/**
* Create directory
* 
* @param *path Desired creation path
* @param mode Object mode
* @return errno if mkdir (2) failed, else result of previous
*/
int vfs_mkdir (const char *path, mode_t mode);

/**
* Releases directory
* 
* @param *path File path
* @param *fi Fuse file information
* @return 0
*/
int vfs_releasedir(const char *path, struct fuse_file_info *fi);

/**
* Unlinks symlinks
* 
* @param *path Symlink path
* @return errno if unlink (2) failed, else result of previous
*/
int vfs_unlink (const char *path);

/**
* Removes directory
* 
* @param *path Directory path
* @return errno if rmdir (2) failed, else result of previous
*/
int vfs_rmdir (const char *path);

/**
* Creates symlink
* 
* @param *path Object path
* @param *link Destination link
* @return errno if symlink (2) failed, else result of previous
*/
int vfs_symlink (const char *path, const char *link);

/**
* Renames object
* 
* @param *path Object path
* @param *newpath Desired new path
* @return errno if symlink (2) failed, else result of previous
*/
int vfs_rename (const char *path, const char *newpath);

/**
* Links two paths
* 
* @param *path Object path
* @param *newpath Desired new path
* @return errno if link (2) failed, else result of previous
*/
int vfs_link (const char *path, const char *newpath);

/**
* Change object priviledges
* 
* @param *path Object path
* @param mode Desired priviledges
* @return errno if chmod (2) failed, else result of previous
*/
int vfs_chmod (const char *path, mode_t mode);

/**
* Change object owner
* 
* @param Object path
* @param Desired user id
* @param Desired global id
* @return errno if symlink (2) failed, else result of previous
*/
int vfs_chown (const char *path, uid_t uid, gid_t gid);

/**
* Truncates file
* 
* @param *path File path
* @param size Desired size of truncation
* @return errno if truncate (2) failed, else result of previous
*/
int vfs_truncate (const char *path, off_t size);

/**
* Gets utime from object
* 
* @param *path Object path
* @param ubuf utime buffer to store retrieved time
* @return errno if utime (2) failed, else result of previous
*/
int vfs_utime(const char *path, struct utimbuf *ubuf);

/**
* Gets utimens from object
* 
* @param *path Object path
* @param ts timespec struct to store retrieved time
* @return errno if utimensat (2) failed, else result of previous
*/
int vfs_utimens (const char *path, const struct timespec ts[2]);

/**
* Opens file
* 
* @param *path File path
* @param *fi FUSE file information
* @return errno if open (2) failed, else result of previous
*/
int vfs_open(const char *path, struct fuse_file_info *fi);

/**
* Reads file
* 
* @param *path File path
* @param *buf Char buffer to store information
* @param size Size of expected input to buffer
* @param offset Offset of buffer
* @param *fi FUSE file information
* @return errno if pread (2) failed, else result of previous
*/
int vfs_read (const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi);

/**
* Writes to file. Sets LAST_TIME_WRITTEN flag for monitor usage
* 
* @param *path File path
* @param *buf Char buffer to store information
* @param size Size of expected input to buffer
* @param offset Offset of buffer
* @param *fi FUSE file information
* @return errno if pwrite (2) failed, else result of previous
*/
int vfs_write (const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi);

/**
* Gets file system statistics
* 
* @param *path File path
* @param *stbuf statvfs buffer to store statistics
* @return errno if statvfs (2) failed, else result of previous
*/
int vfs_statfs (const char *path, struct statvfs *stbuf);

/**
* Opens directory
* 
* @param *path File path
* @param *fi FUSE file information
* @return errno if opendir (2) failed, else result of previous
*/
int vfs_opendir(const char *path, struct fuse_file_info *fi);

/**
* Releases object
* 
* @param *path File path
* @param *fi FUSE file information
* @return errno if release (2) failed, else result of previous
*/
int vfs_release (const char *path, struct fuse_file_info *fi);

/**
* Synchronises file
* 
* @param *path File path
* @param isdatasync Is data synchronisation enabled
* @param *fi FUSE file information
* @return errno if fdatasync/fsync (2) failed, else result of previous
*/
int vfs_fsync (const char *path, int isdatasync, struct fuse_file_info *fi);

/**
* Allocates file to memory
* 
* @param *path File path
* @param mode Desired file mode
* @param offset File offset
* @param length Desired allocation length
* @param *fi FUSE file information
* @return errno if open (2) failed or posix_fallocate (2), else result of previous
*/
int vfs_fallocate (const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi);


/**
* Sets the LAST_TIME_WRITTEN attribute to current system time 
*/
void set_new_written_time_to_current_time();

/**
* Intilializes the safe file system
* 
* @param argc Command line argument count
* @param *argv[] Command line arguments
* @param *vfs_data Struct containing the mount and root directories
* @return Return status of fuse_main
*/
extern int initialize_vfs(int argc, char *argv[], struct vfs_state *vfs_data);
