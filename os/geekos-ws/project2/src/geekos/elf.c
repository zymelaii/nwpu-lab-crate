/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
    elfHeader *head = (elfHeader*)exeFileData;
    programHeader *pheader = (programHeader*)(exeFileData + head->phoff);
    exeFormat->entryAddr = head->entry;
    exeFormat->numSegments = head->phnum;
    for (int i = 0; i < head->phnum; ++i) {
        exeFormat->segmentList[i].offsetInFile = pheader->offset;
        exeFormat->segmentList[i].lengthInFile = pheader->fileSize;
        exeFormat->segmentList[i].startAddress = pheader->vaddr;
        exeFormat->segmentList[i].sizeInMemory = pheader->memSize;
        ++pheader;
    }
    return 0;
}
