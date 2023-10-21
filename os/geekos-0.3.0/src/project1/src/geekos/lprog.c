/*
 * Code for loading and running an ELF executable in kernel mode 
 * 
 * Copyright (c) 2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2004 Iulian Neamtiu <neamtiu@cs.umd.edu> 
 * $Revision: 1.1 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */
#include <geekos/pfat.h>
#include <geekos/screen.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/kthread.h>
#include <geekos/segment.h>
#include <geekos/gdt.h>
#include <geekos/mem.h>
#include <geekos/int.h>
#include <geekos/idt.h>
#include <geekos/elf.h>
#include <geekos/vfs.h>

/* housekeeping routines, not much relevant */
extern void Trampoline(unsigned short CodeSelector, unsigned short DataSelector, unsigned long entry);
static void Printrap_Handler(struct Interrupt_State* state);

static void * virtSpace;

static int lprogdebug = 0;

/* 
 * Spawn_Program() sets up the memory space, and kickstarts the program
 */

static int Spawn_Program(char *exeFileData, struct Exe_Format *exeFormat)
{
  struct Segment_Descriptor* desc;
  unsigned long virtSize;
  unsigned short codeSelector, dataSelector;

  int i;
  ulong_t maxva = 0;

  /* Find maximum virtual address */
  for (i = 0; i < exeFormat->numSegments; ++i) {
    struct Exe_Segment *segment = &exeFormat->segmentList[i];
    ulong_t topva = segment->startAddress + segment->sizeInMemory; 
    
    if (topva > maxva)
      maxva = topva;
  }

  /* setup some memory space for the program */

  virtSize = Round_Up_To_Page(maxva) + 4096; /* leave some slack for stack */
  virtSpace = Malloc(virtSize);
  memset((char *) virtSpace, '\0', virtSize);

  /* Load segment data into memory */
  for (i = 0; i < exeFormat->numSegments; ++i) {
    struct Exe_Segment *segment = &exeFormat->segmentList[i];

    memcpy(virtSpace + segment->startAddress,
	   exeFileData + segment->offsetInFile,
	   segment->lengthInFile);
  }

  /* allocate and init descriptors and selectors for code and data */

  // Kernel code segment.
  desc = Allocate_Segment_Descriptor();
  Init_Code_Segment_Descriptor(
			       desc,
			       (unsigned long)virtSpace, // base address
			       (virtSize/PAGE_SIZE)+10,	 // num pages
			       0		         // privilege level (0 == kernel)
			       );
  codeSelector = Selector( 0, true, Get_Descriptor_Index( desc ) );
  // Kernel data segment.
  desc = Allocate_Segment_Descriptor();
  Init_Data_Segment_Descriptor(
			       desc,
			       (unsigned long)virtSpace, // base address
			       (virtSize/PAGE_SIZE)+10,	 // num pages
			       0		         // privilege level (0 == kernel)
			       );
  dataSelector = Selector( 0, true, Get_Descriptor_Index( desc ) );

  Install_Interrupt_Handler( 0x90, &Printrap_Handler );

  if (lprogdebug)
    {
      Print("Spawn_Program(): all structures are set up\n");
      Print(" virtSpace    = %x\n", (unsigned int) virtSpace);
      Print(" virtSize     = %x\n", (unsigned int) virtSize);
      Print(" codeSelector = %x\n", codeSelector);
      Print(" dataSelector = %x\n", dataSelector);

      Print("Now calling Trampoline()... \n");
    }

  Trampoline(codeSelector, dataSelector, exeFormat->entryAddr); 
  return 0;
}


/* Spawner is a a thread that accomodates the program to be loaded & executed
 * it is started from main.c
 */

void Spawner( unsigned long arg )
{
  const char *program = "/c/a.exe";
  char *exeFileData = 0;
  ulong_t exeFileLength;
  struct Exe_Format exeFormat;
 

  /*
   * Load the executable file data, parse ELF headers,
   * and load code and data segments into user memory.
   */

  if (lprogdebug)
    {
      Print("Reading %s...\n", program);
    }

  if (Read_Fully(program, (void**) &exeFileData, &exeFileLength) != 0)
    {
      Print("Read_Fully failed to read %s from disk\n", program);
      goto fail;
    }

  if (lprogdebug)
    {  
      Print("Read_Fully OK\n");
    }

  if (Parse_ELF_Executable(exeFileData, exeFileLength, &exeFormat) != 0)
    {
      Print("Parse_ELF_Executable failed\n");
      goto fail;
    }

  if (lprogdebug)
    { 
      Print("Parse_ELF_Executable OK\n");
    }

  if (Spawn_Program(exeFileData, &exeFormat) != 0)
    {
      Print("Spawn_Program failed\n");
      goto fail;
    }

    /*
     * User program has been loaded, so we can free the
     * executable file data now.
     */
    Free(exeFileData);
    exeFileData = 0;

  /* If we arrived here, everything was fine and the program exited */

  Print("Hi ! This is the third (and last) string\n");
  Print("If you see this you're happy\n");

  Exit(0);

fail:
    /* We failed; release any allocated memory */
    Disable_Interrupts();

    Free(virtSpace);

    Enable_Interrupts();
}


static void Printrap_Handler( struct Interrupt_State* state )
{
  char * msg = (char *)virtSpace + state->eax;

  Print(msg);

  g_needReschedule = true;
  return;
}
