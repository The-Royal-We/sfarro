Week 4
======

##Whats done this week
* Finished off time comparison operation
* Added logic to vfs.c, namely vfs_write(), to flip a flag if there hasn't been any writes in a while
  * Had to fix a linkage error to make the flag visible to other functions across sfarro  


##What's to do next week
* Investigate mounting a test filesystem and calling standard methods to it (cp, mv, touch etc.)
* Add in program to remount filesystems (possible system call remount(2)) to read-only
* Add in unit tests to my program
* Verify program works by bringing in a pen-drive and mounting it in my machine
