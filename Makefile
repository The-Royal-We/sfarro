Make:
	gcc -Wall src/vfs.h `pkg-config fuse --cflags --libs` -lulockmgr -o fusexmp_fh 
	# Taken partially from fuse-2.9.4/example/fusexmp_fh.c