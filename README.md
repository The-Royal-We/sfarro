# sfarro
Safe Filesystem via Automatic Remount Read-Only

To compile without ```cmake```,
````shell
cc -D_FILE_OFFSET_BITS=64 -Wall -Wextra -lfuse -lpthread -Isfarro -o sfarro/sfarro *.c sfarro/*.c
````
