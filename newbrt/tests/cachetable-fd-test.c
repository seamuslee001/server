#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "test.h"
#include "cachetable.h"

static void
cachetable_fd_test (void) {
    const int test_limit = 1;
    int r;
    CACHETABLE ct;
    r = toku_create_cachetable(&ct, test_limit, ZERO_LSN, NULL_LOGGER); assert(r == 0);
    char fname1[] = __FILE__ "test1.dat";
    unlink(fname1);
    CACHEFILE cf;
    r = toku_cachetable_openf(&cf, ct, fname1, O_RDWR|O_CREAT, 0777); assert(r == 0);

    int fd1 = toku_cachefile_fd(cf); assert(fd1 >= 0);

    // test set to good fd succeeds
    char fname2[] = __FILE__ "test2.data";
    unlink(fname2);
    int fd2 = open(fname2, O_RDWR + O_CREAT); assert(fd2 >= 0 && fd1 != fd2);
    r = toku_cachefile_set_fd(cf, fd2, fname2); assert(r == 0);
    assert(toku_cachefile_fd(cf) == fd2);

    // test set to bogus fd fails
    int fd3 = open("/dev/null", O_RDWR); assert(fd3 >= 0);
    r = close(fd3); assert(r == 0);
    r = toku_cachefile_set_fd(cf, fd3, "/dev/null"); assert(r != 0);
    assert(toku_cachefile_fd(cf) == fd2);

    // test the filenum functions
    FILENUM fn = toku_cachefile_filenum(cf);
    CACHEFILE newcf = 0;
    r = toku_cachefile_of_filenum(ct, fn, &newcf);
    assert(r == 0 && cf == newcf);

    // test a bogus filenum
    fn.fileid++;
    r = toku_cachefile_of_filenum(ct, fn, &newcf);
    assert(r == ENOENT);

    r = toku_cachefile_close(&cf, NULL_LOGGER); assert(r == 0 && cf == 0);
    r = toku_cachetable_close(&ct); assert(r == 0 && ct == 0);
}

int main(int argc, const char *argv[]) {
    default_parse_args(argc, argv);
    cachetable_fd_test();
    return 0;
}
