Week 4
======

I got bogged down sorting out an issue with my logging system, so apologies if this week is looking sparse.
Lots of learning done though

##What's done this week
* Fixed segmentation fault issue that was introduced via the logging systems
  * Roads to hell paved with good intentions 
* Started work on implementing a time-written comparison operation to check if my any files have been written to in a while

##What's to do next week
* Finish implementing time comparison operation
* Add in program to remount filesystems (possible system call remount(2)) to read-only
* Add in logic to vfs.c, namely vfs_read(), to flip a flag if there hasn't been any writes in a while
* Verify program works by bringing in a pen-drive and mounting it in my machine
