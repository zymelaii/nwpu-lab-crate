#include <conio.h>
#include <fileio.h>
#include <process.h>

static void Print_Error(const char *msg, int fd)
{
    Print("%s: %s\n", msg, Get_Error_String(fd));
    Exit(1);
}

#define BUFSIZE 256

int main(int argc, char **argv)
{
    int fd, rc;
    char *filename;
    char buf[BUFSIZE];

    if (argc != 2) {
	Print("Usage: %s <filename>\n", argv[0]);
	Exit(1);
    }
    filename = argv[1];

    fd = Open(filename, O_READ);
    if (fd < 0)
	Print_Error("Could not open file", fd);

    for (;;) {
	int len = Read(fd, buf, BUFSIZE);
	if (len == 0)
	    break; /* EOF */
	else if (len < 0)
	    Print_Error("Read error", len);
	else {
	    int i;
	    for (i = 0; i < len; ++i)
		Put_Char(buf[i]);
	}
    }

    rc = Close(fd);
    if (rc < 0)
	Print_Error("Could not close file", rc);

    return 0;
}

