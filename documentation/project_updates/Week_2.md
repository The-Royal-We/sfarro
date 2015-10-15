Week 2
======

##What's done this week
* Switched to Clion/CMake development environment instead of a Vim/Makefile environment
* Project hierarchy fleshed out
* Project broken down into components
* Pulled in a script from https://github.com/Pronghorn/pronghorn/blob/master/FindFUSE.cmake 
to load in the FUSE library to CMake
* Added in a rudimentary logging system for Sfarro to help 'lift the lid' on what FUSE is doing 

##What's to do next week
* Add in program to remount filesystems (possible system call remount(2)) to read-only
* Add in logic to vfs.c, namely vfs_read(), to flip a flag if there hasn't been any writes in a while
* Verify program works by bringing in a pen-drive and mounting it in my machine