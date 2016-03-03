bin_PROGRAMS = sfarro
noinst_HEADERS = constants.h monitor.h params.h timer.h vfs.h
sfarro_SOURCES = sfarro.c timer.c monitor.c vfs.c 

AM_CPPFLAGS = ${my_CPPFLAGS} ${fuse_CFLAGS}
AM_CFLAGS = ${my_CFLAGS}
sfarro_LDADD = $(fuse_LIBS) ${my_LDFLAGS}

