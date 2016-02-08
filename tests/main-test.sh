#!/bin/bash

set -o errexit

## TODO: Do I need to validate user as root?
#REQUIRE_ROOT=require-root.sh

source test-utils.sh

function test_append_file {
	log "Testing append to file..."

	#Write test file
	for x in `seq 1 $TEST_TEXT_FILE_LENGTH`
	do
		echo "echo ${TEST_TEXT} to ${TEST_TEXT_FILE}"
	done > ${TEST_TEXT_FILE}

	#Verify file
	echo "Verifying length of test file"
	FILE_LENGTH=`wc -l $TEST_TEXT_FILE | awk '${print $1}'`
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

	if [ -s ${TEST_TEXT_FILE}]
	then
		echo "error: expected ${TEST_TEXT_FILE} to be zero length"
		return 1
	fi
	rm_test_file
}

function test_mv_file {
	log "Testing mv file function"

}

echo "$0: tests complete."

