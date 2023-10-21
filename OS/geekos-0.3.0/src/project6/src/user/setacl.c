/*
 * A acl test program for GeekOS user mode
 */

#include "libuser.h"

void Main(int argc, char *argv[])
{
    int ret;
    int perm;

    if (argc != 4) {
        Print("Usage: setacl <file> <uid> [r | w | rw | clear\n");
	Exit();
    }

    if (!strcmp(argv[3], "r")) {
        perm = O_READ;
    } else if (!strcmp(argv[3], "rw")) {
        perm = O_READ|O_WRITE;
    } else if (!strcmp(argv[3], "w")) {
        perm = O_WRITE;
    } else if (!strcmp(argv[3], "clear")) {
        perm = 0;
    } else {
        Print("Usage: setacl <file> <uid> [r | w | rw | clear\n");
	Exit();
    }

    ret = SetAcl(argv[1], atoi(argv[2]), perm);
    if (ret) {
        Print("SetAcl returned %d\n", ret);
    }
}
