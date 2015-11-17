#include "test_remount.h"
/*

  int remount(char *remount_directory) {
    int res;
    res = mount(remount_directory, remount_directory, MS_REMOUNT | O_RDONLY, NULL, NULL);
    printf("\n **** Remount occured at %s ***** \n", remount_directory);
    return 0;
}

*/

TEST(test_remount, test_remount_function) {
    char *test_path = get_test_path();
    EXPECT_EQ(0, remount(test_path));
    EXPECT_EQ(0, access(test_path, R_OK));
    printf("%d\n", errno);
    EXPECT_EQ(1, access(test_path, W_OK));
}

char *get_test_path() {
    char path[] = "/tmp/text_sfarro_XXXXXX";
    mktemp(path);
    std::string testpath(path);
    char *ctestpath = new char[testpath.length() + 1];
    std::strcpy(ctestpath, testpath.c_str());

    return ctestpath;
}
