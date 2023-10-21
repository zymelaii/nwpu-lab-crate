/*
 * A user mode program which uses deep recursion
 * to test stack growth.
 */

#include <conio.h>
#include <string.h>

void Recurse(int x)
{
    int stuff[512];

    if (x == 0) return;

    stuff[0] = x;
    Print("calling Recurse %d\n", x);
    Recurse(x-1);
}

int main(int argc, char **argv)
{
    /* change recurse to 5-10 to see stack faults without page outs */
    int depth = 512;

    if (argc > 1) {
	depth = atoi(argv[1]);
	Print("Depth is %d\n", depth);
    }

    Recurse(depth);

    return 0;
}

