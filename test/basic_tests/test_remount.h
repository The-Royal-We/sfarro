#ifndef TEST_TEST_REMOUNT_H
#define TEST_TEST_REMOUNT_H

#include <iostream>
#include <cstring>
#include <errno.h>
#include "gtest/gtest.h"

extern "C" {
#include "remount.h"
#include "vfs.h"
#include <fuse.h>
}

char *get_test_path();

char *get_c_str(std::string in);

int setup_fuse(char *source_path, char *dest_path);
#endif