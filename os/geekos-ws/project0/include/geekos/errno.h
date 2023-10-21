/*
 * GeekOS error codes
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.16 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_ERRNO_H
#define GEEKOS_ERRNO_H

/*
 * Error codes returned by kernel functions and
 * system calls.  These are meant to be returned to user
 * code to describe system call failures.
 */
#define EUNSPECIFIED		-1	 /* Unspecified error */
#define ENOTFOUND		-2	 /* No such file or directory */
#define EUNSUPPORTED		-3	 /* Operation not supported */
#define ENODEV			-4	 /* No such device */
#define EIO			-5	 /* Input/output error */
#define EBUSY			-6	 /* Resource in use */
#define ENOMEM			-7	 /* Out of memory */
#define ENOFILESYS		-8	 /* No such filesystem */
#define ENAMETOOLONG		-9	 /* Name too long */
#define EINVALIDFS		-10	 /* Invalid format for filesystem */
#define EACCESS			-11	 /* Permission denied */
#define EINVALID		-12	 /* Invalid argument */
#define EMFILE			-13	 /* File descriptor table full */
#define ENOTDIR			-14	 /* Not a directory */
#define EEXIST			-15	 /* File or directory already exists */
#define ENOSPACE		-16	 /* Out of space on device */
#define EPIPE			-17	 /* Pipe has no reader */
#define ENOEXEC			-18	 /* Invalid executable format */

#endif  /* GEEKOS_ERRNO_H */
