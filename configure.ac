AC_INIT([sfarro], [0.0.1], [brendan.carroll.92@gmail.com])
LT_INIT

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_LANG(C)

my_CPPFLAGS="-D_REENTRANT -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26"
my_CFLAGS="$my_CFLAGS -Wall"
my_LDFLAGS="-pthread"

AC_SUBST([my_CPPFLAGS])
AC_SUBST([my_CFLAGS])
AC_SUBST([my_LDFLAGS])

PKG_CHECK_MODULES([fuse], [fuse >= 2.6.0])
AC_CONFIG_FILES([Makefile \
                 src/Makefile])
AC_OUTPUT
