AC_INIT([sfarro], [1.0.0], [brendan.carroll.92@gmail.com])

AM_INIT_AUTOMAKE([foreign -Wall])
AC_CONFIG_SRCDIR([src/sfarro.c])
AC_CONFIG_HEADERS([src/config.h])

AC_PROG_CC
AC_LANG(C)


AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/statvfs.h unistd.h utime.h sys/xattr.h])

AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])

AC_FUNC_CHOWN
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([ftruncate mkdir mkfifo realpath rmdir strerror utime])

AC_CHECK_FUNCS([fdatasync])

AC_SYS_LARGEFILE
AX_PTHREAD
AX_CFLAGS_WARN_ALL
CPPFLAGS="$CPPFLAGS -DFUSE_USE_VERSION=26"

PKG_CHECK_MODULES([fuse], [fuse >= 2.6.0])
CFLAGS="$CFLAGS $fuse_CFLAGS"
LIBS="$LIBS $fuse_LIBS"

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])

AC_OUTPUT
