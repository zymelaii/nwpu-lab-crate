#include <conio.h>
#include <fileio.h>
#include <process.h>
#include <string.h>

static void Print_Error(const char *msg, int fd)
{
    Print("%s: %s\n", msg, Get_Error_String(fd));
    Exit(1);
}

#define MESSAGE "Hello, world\n"

int main(int argc, char **argv)
{
    int fd, rc;
    char *filename;

    if (argc != 2) {
	Print("Usage: %s <filename>\n", argv[0]);
	Exit(1);
    }
    filename = argv[1];

    fd = Open(filename, O_CREATE|O_WRITE);
    if (fd < 0)
	Print_Error("Could not open file", fd);

    rc = Write(fd, MESSAGE, strlen(MESSAGE));
    if (rc < 0)
	Print_Error("Could not write to file", rc);

    rc = Close(fd);
    if (rc < 0)
	Print_Error("Could not close file", rc);

    return 0;
}

