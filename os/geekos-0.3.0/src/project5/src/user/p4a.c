/*
 * A test program for GeekOS user mode
 */

#include "libuser.h"

void Main(void)
{
    int fd;
    int ret;
    int read;
    struct VFS_File_Stat stat;
    char buffer[47];

    /* test set effective uid */
    ret = SetEffectiveUid(33);
    if (ret != 0) {
        Print ("invalid return code for first set euid\n");
    } else {
        Print("Passed Test #1\n");
    }
    ret = SetEffectiveUid(34);
    if (ret == 0) {
        Print ("invalid return code for second set euid\n");
    } else {
        Print("Passed Test #2\n");
    }

    ret = Open("/nosuchmount", O_READ);
    if (ret != -1) {
        Print ("invalid return code open of invalid mount point\n");
    } else {
        Print("Passed Test #3\n");
    }

    fd = Open("/c/a.exe", O_READ);
    if (fd != 0) {
        Print ("invalid return code (%d) for open 0th file\n", ret);
    } else {
        Print("Passed Test #4\n");
    }

    ret = Write(fd, buffer, sizeof(buffer));
    if (ret != -1) {
        Print ("invalid return code (%d) for write to readonly file\n", ret);
    } else {
        Print("Passed Test #5\n");
    }

    ret = Read(fd, buffer, sizeof(buffer));
    if (ret != sizeof(buffer)) {
        Print ("invalid return code (%d) for read at start of file\n", ret);
    } else {
        Print("Passed Test #6\n");
    }

    read = 0;
    while (ret > 0) {
	read += ret;
	ret = Read(fd, buffer, sizeof(buffer));
    }

    ret = Stat(93, &stat);
    if (ret != -1) {
        Print ("invalid return code (%d) for stat of non-open file\n", ret);
    } else {
        Print("Passed Test #7\n");
    }

    ret = Stat(fd, &stat);
    if (ret != 0) {
        Print ("invalid return code (%d) for stat of open file\n", ret);
    } else {
        Print("Passed Test #8\n");
    }

    if (read != stat.size) {
        Print ("%d bytes read from file, stat reports %d int file\n", read, stat.size);
    } else {
        Print("Passed Test #9\n");
    }

    ret = Close(72);
    if (ret != -1) {
        Print ("invalid return code (%d) for close of non-open file\n", ret);
    } else {
        Print("Passed Test #10\n");
    }

    ret = Close(fd);
    if (ret != 0) {
        Print ("invalid return code (%d) for close of open file\n", ret);
    } else {
        Print("Passed Test #11\n");
    }
}
