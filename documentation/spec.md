Sfarro Spec
==========

Sfarro (Safe Filesystem via Automatic Remount Read-Only) can be implemented in the following manner: 

*Have a daemon observing any compatible mounts as they enter /proc/mounts
*Once a compatible mount is found, then intercept this and send any mounting information over to sfarro 
*Sfarro creates a VFS (Virtual File System) from the newly mounted device and mounts it as Read-Write
*Sfarro then sleeps while the VFS is being written to
*After a set time in which no writes to the VFS have been written, we then re-mount that VFS as Read-Only
*If a write comes in from a desired input, we then re-mount the VFS as Read-Write
*Once we are finished with the file system, we then unmount the VFS from the system completely 

