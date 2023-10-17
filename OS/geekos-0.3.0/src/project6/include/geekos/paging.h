/*
 * Paging (virtual memory) support
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.46 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_PAGING_H
#define GEEKOS_PAGING_H

#include <geekos/ktypes.h>
#include <geekos/defs.h>
#include <geekos/bootinfo.h>
#include <geekos/list.h>

struct Page;
struct User_Context;

#define NUM_PAGE_TABLE_ENTRIES	1024
#define NUM_PAGE_DIR_ENTRIES	1024

#define PAGE_DIRECTORY_INDEX(x)	(((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)	(((x) >> 12) & 0x3ff)

#define PAGE_ALLIGNED_ADDR(x)   (((unsigned int) (x)) >> 12)
#define PAGE_ADDR(x)   (PAGE_ALLIGNED_ADDR(x) << 12)

/*
 * Bits for flags field of pde_t and pte_t.
 */
#define VM_WRITE   1	 /* Memory is writable */
#define VM_USER    2	 /* Memory is accessible to user code */
#define VM_NOCACHE 8	 /* Memory should not be cached */
#define VM_READ    0	 /* Memory can be read (ignored for x86) */
#define VM_EXEC    0	 /* Memory can be executed (ignored for x86) */


/*
 * Page directory entry datatype.
 * If marked as present, it specifies the physical address
 * and permissions of a page table.
 */
typedef struct {
    uint_t present:1;
    uint_t flags:4;
    uint_t accesed:1;
    uint_t reserved:1;
    uint_t largePages:1;
    uint_t globalPage:1;
    uint_t kernelInfo:3;
    uint_t pageTableBaseAddr:20;
} pde_t;

/*
 * Page table entry datatype.
 * If marked as present, it specifies the physical address
 * and permissions of a page of memory.
 */
typedef struct {
    uint_t present:1;
    uint_t flags:4;
    uint_t accesed:1;
    uint_t dirty:1;
    uint_t pteAttribute:1;
    uint_t globalPage:1;
    uint_t kernelInfo:3;
    uint_t pageBaseAddr:20;
} pte_t;

/*
 * Datatype representing the hardware error code
 * pushed onto the stack by the processor on a page fault.
 * The error code is stored in the "errorCode" field
 * of the Interrupt_State struct.
 */
typedef struct {
    uint_t protectionViolation:1;
    uint_t writeFault:1;
    uint_t userModeFault:1;
    uint_t reservedBitFault:1;
    uint_t reserved:28;
} faultcode_t;

/*
 * Bits used in the kernelInfo field of the PTE's:
 */
#define KINFO_PAGE_ON_DISK	0x4	 /* Page not present; contents in paging file */

void Init_VM(struct Boot_Info *bootInfo);
void Init_Paging(void);

extern void Flush_TLB(void);
extern void Set_PDBR(pde_t *pageDir);
extern pde_t *Get_PDBR(void);
extern void Enable_Paging(pde_t *pageDir);

/*
 * Return the address that caused a page fault.
 */
static __inline__ ulong_t Get_Page_Fault_Address(void)
{
    ulong_t faultAddress;
    __asm__ __volatile__ (
	"mov %%cr2, %0"
	: "=r" (faultAddress)
    );
    return faultAddress;
}

int Find_Space_On_Paging_File(void);
void Free_Space_On_Paging_File(int pagefileIndex);
void Write_To_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex);
void Read_From_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex);


#endif
