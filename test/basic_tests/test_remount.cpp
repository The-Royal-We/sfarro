#include "test_remount.h"

TEST(test_remount, test_remount_function) {
    char *source_path = get_test_path();
    char *dest_path = get_test_path();
    int res = setup_fuse(source_path, dest_path);
    EXPECT_EQ(0, res);
    EXPECT_EQ(0, remount(dest_path));
    printf("%d\n", errno);
    EXPECT_EQ(0, access(dest_path, R_OK));
    EXPECT_EQ(1, access(dest_path, W_OK));
}

char *get_test_path() {
    char path[] = "/tmp/sfarro_dir_XXXXXX";
    std::string testpath(mkdtemp(path));
    return get_c_str(testpath);
}


int setup_fuse(char *source_path, char *dest_path) {
    char *flags = get_c_str("-d");


    char *argv[] = {flags, source_path, dest_path};
    int argc = sizeof(argv) / sizeof(*argv);

    return vfs(argc, argv);
}


char *get_c_str(std::string in) {
    char *c_str = new char[in.length() + 1];
    std::strcpy(c_str, in.c_str());
    return c_str;
}