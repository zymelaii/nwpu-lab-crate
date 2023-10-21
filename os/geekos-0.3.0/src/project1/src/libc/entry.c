/*
 * User program entry point function
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.10 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/argblock.h>

int main(int argc, char **argv);
void Exit(int exitCode);

/*
 * Entry point.  Calls user program's main() routine, then exits.
 */
void _Entry(void)
{

    /* Call main(); arguments won't be needed */
    main(0, 0);

    /* make the inter-selector jump back */
  __asm__ __volatile__ ("leave");
  __asm__ __volatile__ ("lret");

}

