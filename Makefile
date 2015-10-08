all: vfs

# Taken partially from fuse-2.9.4/example/fusexmp_fh.c
CPPFLAGS += -Wall
CPPFLAGS += $(shell pkg-config fuse --cflags)
LDFLAGS += $(shell pkg-config fuse --libs)
LOADLIBES += -lulockmgr

vfs: src/vfs
	ln -s $< $@
