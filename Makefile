Make:
	gcc -Wall src/vfs.c `pkg-config fuse --cflags --libs` -lulockmgr -o vfs 
	# Taken partially from fuse-2.9.4/example/fusexmp_fh.c
