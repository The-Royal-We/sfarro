#!/bin/bash

set -o errexit
set -o xtrace

## TODO: Do I need to validate user as root?
#REQUIRE_ROOT=require-root.sh

export TEST_MOUNT_POINT=$(mktemp --directory sfarro_test_mount-XXXXXX) 
export TEST_DEST_POINT=$(mktemp --directory sfarro_test_dest-XXXXXX) 

SFARRO=../src/sfarro

source test-utils.sh

function exit_handler {
	if [ -n "${SFARRO_PID}" ]
	then
		kill $SFARRO_PID
	fi
	
	fusermount -u $TEST_MOUNT_POINT
	rm -rf $TEST_MOUNT_POINT $TEST_DEST_POINT
	rm_test_dir
}

function test_append_file {
	log "Testing append to file..."

	#Write test file
	for x in `seq 1 $TEST_TEXT_FILE_LENGTH`
	do
		echo "echo ${TEST_TEXT} to ${TEST_TEXT_FILE}"
	done > ${TEST_TEXT_FILE}

	#Verify file
	echo "Verifying length of test file"
	FILE_LENGTH=`wc -l $TEST_TEXT_FILE | awk '{print $1}'`
	if [ "$FILE_LENGTH" -ne "$TEST_TEXT_FILE_LENGTH" ]
	then
		echo "error: expected $TEST_TEXT_FILE_LENGTH, got $FILE_LENGTH"
		return 1
	fi

	rm_test_file
}

function test_truncate_file {
	log "Testing truncate file..."

	echo "${TEST_TEXT}" > ${TEST_TEXT_FILE}

	: > ${TEST_TEXT_FILE}

	if [ -s ${TEST_TEXT_FILE} ]
	then
		echo "error: expected ${TEST_TEXT_FILE} to be zero length"
		return 1
	fi
	rm_test_file
}

function test_mv_file {
	log "Testing mv file function"

	# if renamed file exists, remove it
	if [ -e $ALT_TEST_TEXT_FILE ]
	then
		rm $ALT_TEST_TEXT_FILE
	fi
	if [ -e $ALT_TEST_TEXT_FILE ]
	then
		echo "Could not delete file ${ALT_TEST_TEXT_FILE}, file still exists"
		return 1
	fi

	# Create test file again
	mk_test_file

	# Rename test file
	mv $TEST_TEXT_FILE $ALT_TEST_TEXT_FILE
	if [ ! -e $ALT_TEST_TEXT_FILE ]
	then
		echo "Could not move file"
		return 1
	fi

	# Check contents of file
	ALT_TEXT_LENGTH=`echo $TEST_TEXT | wc -c | awk '{print $1}'`
    ALT_FILE_LENGTH=`wc -c $ALT_TEST_TEXT_FILE | awk '{print $1}'`

	if [ "$ALT_FILE_LENGTH" -ne $ALT_TEXT_LENGTH ] 
	then
		echo "moved file length is not as expected length: $ALT_TEXT_LENGTH got: $ALT_FILE_LENGTH"
		return 1
	fi

	rm_test_file $ALT_TEST_TEXT_FILE
}

function test_mv_directory {
	log "Testing mv directory function..."
	if [ -e $TEST_DIR ]; then
		echo "Unexpected, this file/directory exists: ${TEST_DIR}"
		return 1
	fi

	mk_test_dir

	mv ${TEST_DIR} ${TEST_DIR}_rename
	if [ ! -d "${TEST_DIR}_rename" ]; then
		echo "Directory ${TEST_DIR} was not renamed"
		return 1
	fi

	rmdir ${TEST_DIR}_rename
	if [ -e "${TEST_DIR}_rename" ]; then
		echo "Could not remove the test directory, it still exists: ${TEST_DIR}_rename"
		return 1
	fi
}

function test_mkdir_rmdir {
	log "Testing creation/removal of directory"

	if [ -e $TEST_DIR ]; then
		echo "Unexpected , this file/directory exists: ${TEST_DIR}"
		return 1
	fi

	mk_test_dir
	rm_test_dir
}

function test_chmod {
	log "Testing chmod file function..."
	mk_test_file
	ORIGINAL_PERMISSIONS=$(stat --format=%a $TEST_TEXT_FILE)
	chmod 777 $TEST_TEXT_FILE;
	if [ $(stat --format=%a $TEST_TEXT_FILE) == $ORIGINAL_PERMISSIONS ]
	then
		echo "Could not modify $TEST_TEXT_FILE permissions"
		return 1
	fi

	# clean up
	rm_test_file

}

function test_list {
	log "Testing list"
	mk_test_dir
	mk_test_file
	echo `ls -l`
	file_cnt=$(ls -1 | wc -l)
	if [ $file_cnt != 2 ]; then
		echo "Expected 2 file but got $file_cnt"
		return 1
	fi

	rm_test_file
	rm_test_dir
}

function test_remove_nonempty_directory {
	log "Testing removing a non-empty directory"
	mk_test_dir
	touch "${TEST_DIR}/file"
	rmdir "${TEST_DIR}" 2>&1 | grep -q "Directory not empty"
	rm "${TEST_DIR}/file"
	rm_test_dir
}

function add_all_tests {
	add_tests test_list
	add_tests test_append_file 
	add_tests test_truncate_file 
	add_tests test_mv_file
	add_tests test_mv_directory
	add_tests test_mkdir_rmdir
	add_tests test_chmod
	add_tests test_remove_nonempty_directory
}

trap exit_handler EXIT

# Mount the directory into sfarro
$SFARRO $TEST_DEST_POINT $TEST_MOUNT_POINT

init_suite
add_all_tests
run_suite
