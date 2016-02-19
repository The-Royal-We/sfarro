#!/bin/bash

set -o errexit

#Sample Variables
TEST_TEXT="HELLO_WORLD"
TEST_TEXT_FILE=test-sfarro.txt
TEST_DIR=testdir
ALT_TEST_TEXT_FILE=test-sfarro-ALT.txt
TEST_TEXT_FILE_LENGTH=15

export RUN_DIR

function mk_test_file {
	if [ $# == 0 ]; then
		TEXT=$TEST_TEXT
	else
		TEXT=$1
	fi
	echo $TEXT > $TEST_TEXT_FILE
	
	if [ ! -e $TEST_TEXT_FILE ]; then
		echo "Could not create file ${TEST_TEXT_FILE}, it does not exist"
		exit 1
	fi
}

function rm_test_file {
	if [ $# == 0 ]; then
		FILE=$TEST_TEXT_FILE
	else
		FILE=$1
	fi
	rm -f $FILE

	if [ -e $FILE ]; then
		##How@?
		echo "Could not cleanup file ${TEST_TEST_FILE}"
		exit 1
	fi
}

function mk_test_dir {
	mkdir ${TEST_DIR}

	if [ ! -d ${TEST_DIR} ]; then
		echo "Directory ${TEST_DIR} was not created"
		exit 1
	fi
}

function rm_test_dir {
	rmdir ${TEST_DIR}

	if [ -e ${TEST_DIR} ]; then
		echo "Could not remove test directory, it still exists: ${TEST_DIR}"
		exit 1
	fi
}

function cd_run_dir {
	if [ "$TEST_DEST_POINT" == "" ]; then
		echo "$TEST_DEST_POINT variable not set"
		exit 1
	fi
    cd $TEST_DEST_POINT 
}

function clean_run_dir {
	if []; then
		rm -rf ${RUN_DIR} || echo "Error removing ${RUN_DIR}"
	fi
}

function init_suite {
	TEST_LIST=()
	TEST_FAILED_LIST=()
	TEST_PASSED_LIST=()
}

function report_pass {
	echo "$1 passed"
	TEST_PASSED_LIST+=($1)
}

function report_fail {
	TEST_FAILED_LIST+=($1)
}

function add_tests {
	TEST_LIST+=("$@")
}

function log {
	echo ${FUNCNAME[1]}: "$@"
}

function run_suite {
	orig_dir=$PWD
	cd_run_dir
	for t in "${TEST_LIST[@]}"; do
		set +o errexit
		(set -o errexit; $t)
		if [[ $? == 0 ]]; then
			report_pass $t
		else
			report_fail $t
		fi
		set -o errexit
	done
	cd ${orig_dir}
	clean_run_dir

	for t in "${TEST_PASSED_LIST[@]}"; do
		echo "PASS: $t"
	done
	for t in "${TEST_FAILED_LIST[@]}"; do
		echo "FAIL: $t"
	done

	passed=${#TEST_PASSED_LIST[@]}
	failed=${#TEST_FAILED_LIST[@]}
	echo "SUMMARY for $0: $passed tests passed. $failed tests failed"
	if [[ $failed != 0 ]]; then
		return 1
	else
		return 0
	fi
}
