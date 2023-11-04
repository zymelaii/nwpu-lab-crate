# Makefile for GeekOS kernel, userspace, and tools
# Copyright (c) 2004,2005 David H. Hovemeyer <daveho@cs.umd.edu>
# $Revision: 1.45 $

# This is free software.  You are permitted to use,
# redistribute, and modify it as specified in the file "COPYING".

# Required software to build GeekOS:
# - GNU Make (http://www.gnu.org/software/make)
# - gcc 2.95.2 generating code for target (i386/ELF) and host platforms
# - nasm (http://nasm.sourceforge.net)
# - Perl5, AWK (any version), egrep
#
# Cygwin (http://cygwin.com) may be used to build GeekOS.
# Make sure that gcc, binutils, nasm, and perl are installed.

# NOTES:
# - This makefile has been written carefully to work correctly
#   with the -j (parallel make) option.  I regularly use "make -j 2"
#   to speed the build process on 2 processor systems.

PROJECT_ROOT := ..
VPATH := $(PROJECT_ROOT)/src

# Figure out if we're compiling with cygwin, http://cygwin.com
SYSTEM_NAME := $(shell uname -s)
ifeq ($(findstring CYGWIN,$(SYSTEM_NAME)),CYGWIN)
SYM_PFX            := _
EXTRA_C_OPTS       := -DNEED_UNDERSCORE -DGNU_WIN32
EXTRA_NASM_OPTS    := -DNEED_UNDERSCORE
NON_ELF_SYSTEM     := yes
EXTRA_CC_USER_OPTS := -Dmain=geekos_main
endif

# ----------------------------------------------------------------------
# Configuration -
#   Various options specifying how GeekOS should be built,
#   what source files to build, which user programs to build,
#   etc.  This is generally the only section of the makefile
#   that will need to be modified.
# ----------------------------------------------------------------------

# List of targets to build by default.
# These targets encompass everything needed to boot
# and run GeekOS.
ALL_TARGETS := fd.img diskc.img


# Kernel source files
KERNEL_C_SRCS := idt.c int.c trap.c irq.c io.c \
	keyboard.c screen.c timer.c \
	mem.c crc32.c \
	gdt.c tss.c segment.c \
	bget.c malloc.c \
	synch.c kthread.c \
	dma.c floppy.c \
	elf.c blockdev.c ide.c \
	vfs.c pfat.c bitset.c \
	lprog.c \
	main.c

# Kernel object files built from C source files
KERNEL_C_OBJS := $(KERNEL_C_SRCS:%.c=geekos/%.o)

# Kernel assembly files
KERNEL_ASM_SRCS := lowlevel.asm lspawn.asm


# Kernel object files build from assembler source files
KERNEL_ASM_OBJS := \
  $(KERNEL_ASM_SRCS:%.asm=geekos/%.o)


# All kernel object files
KERNEL_OBJS := $(KERNEL_C_OBJS) \
  $(KERNEL_ASM_OBJS)

# Common library source files.
# This library is linked into both the kernel and user programs.
# It provides string functions and generic printf()-style
# formatted output.
COMMON_C_SRCS := fmtout.c string.c memmove.c

# Common library object files.
COMMON_C_OBJS := $(COMMON_C_SRCS:%.c=common/%.o)

# User libc source files.
LIBC_C_SRCS := \
	conio.c

# User libc object files.
LIBC_C_OBJS := $(LIBC_C_SRCS:%.c=libc/%.o)

# Generated libc source files
GENERATED_LIBC_SRCS := libc/errno.c


# User program source files.
USER_C_SRCS := \
	a.c
# User executables
USER_PROGS := $(USER_C_SRCS:%.c=user/%.exe)

# Base address of kernel
KERNEL_BASE_ADDR := 0x00010000

# Kernel entry point function
KERNEL_ENTRY = $(SYM_PFX)Main

# Base address of user programs
USER_BASE_ADDR := 0x1000

# User program (libc) entry point function
USER_ENTRY = $(SYM_PFX)_Entry


# ----------------------------------------------------------------------
# Tools -
#   This section defines programs that are used to build GeekOS.
# ----------------------------------------------------------------------

# Uncomment if cross compiling
TARGET_CC_PREFIX := i386-elf-

# Target C compiler.  gcc 2.95.2 or later should work.
TARGET_CC := $(TARGET_CC_PREFIX)gcc

# Host C compiler.  This is used to compile programs to execute on
# the host platform, not the target (x86) platform.  On x86/ELF
# systems, such as Linux and FreeBSD, it can generally be the same
# as the target C compiler.
HOST_CC := gcc

# Target linker.  GNU ld is probably to only one that will work.
TARGET_LD := $(TARGET_CC_PREFIX)ld

# Target archiver
TARGET_AR := $(TARGET_CC_PREFIX)ar

# Target ranlib
TARGET_RANLIB := $(TARGET_CC_PREFIX)ranlib

# Target nm
TARGET_NM := $(TARGET_CC_PREFIX)nm

# Target objcopy
TARGET_OBJCOPY := $(TARGET_CC_PREFIX)objcopy

# Nasm (http://nasm.sourceforge.net)
NASM := nasm

# Tool to build PFAT filesystem images.
BUILDFAT := tools/builtFat.exe

# Perl5 or later
PERL := perl

# Pad a file so its size is a multiple of some unit (i.e., sector size)
PAD := $(PERL) $(PROJECT_ROOT)/scripts/pad

# Create a file filled with zeroes.
ZEROFILE := $(PERL) $(PROJECT_ROOT)/scripts/zerofile

# Calculate size of file in sectors
NUMSECS := $(PERL) $(PROJECT_ROOT)/scripts/numsecs


# Generate C file containing table of error strings from <geekos/errno.h>
GENERRS := $(PERL) $(PROJECT_ROOT)/scripts/generrs

# ----------------------------------------------------------------------
# Definitions -
#   Options passed to the tools.
# ----------------------------------------------------------------------

# Flags used for all C source files
GENERAL_OPTS := -O -w $(EXTRA_C_OPTS)
CC_GENERAL_OPTS := $(GENERAL_OPTS) -O0 -fno-stack-protector

# Flags used for kernel C source files
CC_KERNEL_OPTS := -g -DGEEKOS -I$(PROJECT_ROOT)/include

# Flags user for kernel assembly files
NASM_KERNEL_OPTS := -I$(PROJECT_ROOT)/src/geekos/ -f elf $(EXTRA_NASM_OPTS)

# Flags used for common library and libc source files
CC_USER_OPTS := -I$(PROJECT_ROOT)/include -I$(PROJECT_ROOT)/include/libc \
	$(EXTRA_CC_USER_OPTS)

# Flags passed to objcopy program (strip unnecessary sections from kernel.exe)
OBJCOPY_FLAGS := -R .dynamic -R .note -R .comment

# ----------------------------------------------------------------------
# Rules -
#   Describes how to compile the source files.
# ----------------------------------------------------------------------

# Compilation of kernel C source files

geekos/%.o : geekos/%.c
	$(TARGET_CC) -c $(CC_GENERAL_OPTS) $(CC_KERNEL_OPTS) $< -o geekos/$*.o


# Compilation of kernel assembly source files
geekos/%.o : geekos/%.asm
	$(NASM) $(NASM_KERNEL_OPTS) $< -o geekos/$*.o

geekos/%.o : geekos/%.S
	$(TARGET_CC) -c $(CC_GENERAL_OPTS) $(CC_KERNEL_OPTS) $< -o geekos/$*.o

# Compilation of common library C source files
common/%.o : common/%.c
	$(TARGET_CC) -c $(CC_GENERAL_OPTS) $(CC_USER_OPTS) $< -o common/$*.o

# Compilation of libc source files
libc/%.o : libc/%.c
	$(TARGET_CC) -c $(CC_GENERAL_OPTS) $(CC_USER_OPTS) $< -o libc/$*.o

# Compilation of user programs
user/%.exe : user/%.c libc/libc.a libc/entry.o
	$(TARGET_CC) -c $(CC_GENERAL_OPTS) $(CC_USER_OPTS) $< -o user/$*.o
	$(TARGET_LD) -o $@ -Ttext $(USER_BASE_ADDR) -e $(USER_ENTRY) \
		libc/entry.o user/$*.o libc/libc.a
ifeq ($(NON_ELF_SYSTEM),yes)
	$(TARGET_OBJCOPY) -O elf32-i386 $@ $@
endif

# ----------------------------------------------------------------------
# Targets -
#   Specifies files to be built
# ----------------------------------------------------------------------

# Default target - see definition of ALL_TARGETS in Configuration section
all : $(ALL_TARGETS)

# Standard floppy image - just boots the kernel
fd.img : geekos/fd_boot.bin geekos/setup.bin geekos/kernel.bin
	cat geekos/fd_boot.bin geekos/setup.bin geekos/kernel.bin > $@
	truncate -s 1474560 $@

# Augmented floppy image - contains kernel and user executables on PFAT filesystem
fd_aug.img : geekos/fd_boot.bin geekos/setup.bin geekos/kernel.bin $(USER_PROGS) $(BUILDFAT)
	$(ZEROFILE) $@ 2880
	$(BUILDFAT) -b geekos/fd_boot.bin $@ geekos/setup.bin geekos/kernel.bin $(USER_PROGS) $(BUILDFAT)

# First hard drive image (10 MB).
# This contains a PFAT filesystem with the user programs on it.
diskc.img : $(USER_PROGS) $(BUILDFAT)
	$(ZEROFILE) $@ 20480
	$(BUILDFAT) $@ $(USER_PROGS)

# Tool to build PFAT filesystem images
$(BUILDFAT) : $(PROJECT_ROOT)/src/tools/buildFat.c $(PROJECT_ROOT)/include/geekos/pfat.h
	$(HOST_CC) $(CC_GENERAL_OPTS) -I$(PROJECT_ROOT)/include $(PROJECT_ROOT)/src/tools/buildFat.c -o $@

# Floppy boot sector (first stage boot loader).
geekos/fd_boot.bin : geekos/setup.bin geekos/kernel.bin $(PROJECT_ROOT)/src/geekos/fd_boot.asm
	$(NASM) -f bin \
		-I$(PROJECT_ROOT)/src/geekos/ \
		-DNUM_SETUP_SECTORS=`$(NUMSECS) geekos/setup.bin` \
		-DNUM_KERN_SECTORS=`$(NUMSECS) geekos/kernel.bin` \
		$(PROJECT_ROOT)/src/geekos/fd_boot.asm \
		-o $@

# Setup program (second stage boot loader).
geekos/setup.bin : geekos/kernel.exe $(PROJECT_ROOT)/src/geekos/setup.asm
	$(NASM) -f bin \
		-I$(PROJECT_ROOT)/src/geekos/ \
		-DENTRY_POINT=0x`egrep 'Main$$' geekos/kernel.syms |awk '{print $$1}'` \
		$(PROJECT_ROOT)/src/geekos/setup.asm \
		-o $@
	$(PAD) $@ 512

# Loadable (flat) kernel image.
geekos/kernel.bin : geekos/kernel.exe
	$(TARGET_OBJCOPY) $(OBJCOPY_FLAGS) -S -O binary geekos/kernel.exe geekos/kernel.bin
	$(PAD) $@ 512

# The kernel executable and symbol map.
geekos/kernel.exe : $(KERNEL_OBJS) $(COMMON_C_OBJS)
	$(TARGET_LD) -o geekos/kernel.exe -Ttext $(KERNEL_BASE_ADDR) -e $(KERNEL_ENTRY) \
		$(KERNEL_OBJS) $(COMMON_C_OBJS)
	$(TARGET_NM) geekos/kernel.exe > geekos/kernel.syms

# C library for user mode programs
libc/libc.a : $(LIBC_C_OBJS) libc/errno.o $(COMMON_C_OBJS)
	$(TARGET_AR) ruv $@ $(LIBC_C_OBJS) libc/errno.o $(COMMON_C_OBJS)
	$(TARGET_RANLIB) $@

# Source file containing the table of error strings for each error code.
# This is derived automatically from the comments in <geekos/errno.h>.
libc/errno.c : $(PROJECT_ROOT)/include/geekos/errno.h $(PROJECT_ROOT)/scripts/generrs
	$(GENERRS) $(PROJECT_ROOT)/include/geekos/errno.h > $@

# Clean build directories of generated files
clean :
	for d in geekos common libc user tools; do \
		(cd $$d && rm -f *); \
	done

# Build header file dependencies, so source files are recompiled when
# header files they depend on are modified.
depend : $(GENERATED_LIBC_SRCS)
	$(TARGET_CC) -M $(CC_GENERAL_OPTS) $(CC_KERNEL_OPTS) \
		$(KERNEL_C_SRCS:%.c=$(PROJECT_ROOT)/src/geekos/%.c) \
		| $(PERL) -n -e 's,^(\S),geekos/$$1,;print' \
		> depend.mak
	$(TARGET_CC) -M $(CC_GENERAL_OPTS) $(CC_USER_OPTS) \
		$(LIBC_C_SRCS:%.c=$(PROJECT_ROOT)/src/libc/%.c) libc/errno.c \
		| $(PERL) -n -e 's,^(\S),libc/$$1,;print' \
		>> depend.mak
	$(TARGET_CC) -M $(CC_GENERAL_OPTS) $(CC_USER_OPTS) \
		$(COMMON_C_SRCS:%.c=$(PROJECT_ROOT)/src/common/%.c) \
		| $(PERL) -n -e 's,^(\S),common/$$1,;print' \
		>> depend.mak

# By default, there are no header file dependencies.
depend.mak :
	touch $@

run: fd.img
	@qemu-system-i386 -boot a -drive format=raw,index=0,if=floppy,file=$< -display curses -m 10M -s -S

include depend.mak
