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
#include <geekos/screen.h> /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>

/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData 已装入内存的可执行文件所占用空间的起始地址 buffer containing the executable file
 * @param exeFileLength 可执行文件长度 length of the executable file in bytes
 * @param exeFormat 保存分析得到的elf文件信息的结构体指针 structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
                         struct Exe_Format *exeFormat)
{
    // TODO("Parse an ELF executable image");
    //利用ELF头部结构体指向可执行文件头部，便于获取相关信息
    elfHeader *ehdr = (elfHeader *)exeFileData;
    //段的个数
    exeFormat->numSegments = ehdr->phnum;
    //代码入口地址
    exeFormat->entryAddr = ehdr->entry;
    //获取头部表在文件中的位置，便于读取信息
    programHeader *phdr = (programHeader *)(exeFileData + ehdr->phoff);
    
    //填充Exe_Segment
    unsigned int i;
    struct Exe_Segment *segment;
    for (i = 0; i < exeFormat->numSegments; i++)
    {
        segment = &exeFormat->segmentList[i];
        //获取该段在文件中的偏移量*
        segment->offsetInFile = phdr->offset;
        //获取该段的数据在文件中的长度
        segment->lengthInFile = phdr->fileSize;
        //获取该段在用户内存中的起始地址
        segment->startAddress = phdr->vaddr;
        //获取该段在内存中的大小
        segment->sizeInMemory = phdr->memSize;
        //获取该段的保护标志位
        segment->protFlags = phdr->flags;
        phdr = (programHeader*)((char*)phdr + ehdr->phentsize);
    }
    return 0;
}