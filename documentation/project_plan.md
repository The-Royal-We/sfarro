Project Plan for Sfarro ((Safe Filesystem via Automatic Remount Read-Only))
==========================================================================

##Description
The aim for Sfarro is to create a system that would ensure the safety of filesystems as they are mounted during runtime.

##Deliverable
An application that wouild remount file systems as Read-Only after a period of time has elapsed in which no writes have been committed to it.

##Start/End Dates 

07/10/15 - ?????

##Key Risks
Theres an inherent risk wrt file integrity when re-mounting during writes. Also, there might be a risk in interrupting mounts to the system and sending to you own application.
