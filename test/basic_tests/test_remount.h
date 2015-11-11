#ifndef TEST_TEST_REMOUNT_H
#define TEST_TEST_REMOUNT_H

#include <iostream>
#include <cstring>
#include <errno.h>
#include "gtest/gtest.h"

extern "C" {
#include "remount.h"
}

char *get_test_path();

#endif