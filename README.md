# sfarro
Safe Filesystem via Automatic Remount Read-Only


## Compiling Binaries
Run the following: 

	./autogen.sh 
	./configure && make 

## Running test suite
	
	make check

Ensure to check main-test.sh.log to get a fine detailed look at the test implementation

## Steps to run sfarro on own machine

1.  Create a directory called "test" at project root.
2.	Copy src/sfarro binary to project root
3.  Ensure that current user is root.
3.1	If not, execute `sudo su -`, enter admin password and navigate back to project directory if necessary
4.	Insert USB device into machine and make note of the device's filepath notation
4.1	For example: /media/<USER>/TEST_DRIVE
5. 	Now run the following: "./sfarro -d /path/to/usb/device/ /path/to/test/directory"
6.  SFARRO is now intialized. It is currently binding the USB devices filesystem to the "test" folder
7.	Create another terminal session, sudo su - to administrator user and navigate to project directory.
8.	Proceed to write some data into the "test" folder, example: `echo "Hello, World!" > test/hello`
9. 	Notice the increase in activity on the other terminal session
10.	After a period of about 15 seconds, you should notice that SFARRO remounts the USB device to read only!
11. Verify this by running the "mount" command and finding the device name in the list. You should see after the devices name, "ro" indicating that the device is now read only.
12. Repeat step 8 of the command and check the mounted device mode, it should be remounted to "rw"
12.1 If a "bad file descriptor" error should occur, just again to the file system. This is caused by the remount function taking too long.
