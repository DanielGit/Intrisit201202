/*
 * Copyright (c) 2005-2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Boot information
 *
 * The boot information is stored by an OS loader, and
 * it is refered by kernel later in boot time.
 */

#ifndef _PREX_BOOTINFO_H
#define _PREX_BOOTINFO_H

#include <sys/types.h>

/*
 * Module information for kernel, driver, and boot tasks.
 * An OS loader will build this structure regardless of its file format.
 */
typedef struct _KERNEL_MODULE
{
	char	Name[16];		/* name of image */
	DWORD	Phys;			/* physical address */
	DWORD	Size;			/* size of image */
	DWORD	Entry;			/* entry address */
	DWORD	Text;			/* text address */
	DWORD	Data;			/* data address */
	DWORD	TextSize;		/* text size */
	DWORD	DataSize;		/* data size */
	DWORD	BssSize;		/* bss size */
}KERNEL_MODULE;
typedef KERNEL_MODULE *PKERNEL_MODULE;

/*
 * Memory map
 */
typedef struct _MEM_MAP
{
	DWORD	Start;		/* start address */
	DWORD	Size;		/* size in bytes */
}MEM_MAP;
typedef MEM_MAP *PMEM_MAP;


#define NRESMEM		4	/* number of reserved memory */

/*
 * Boot information
 */
typedef struct _BOOT_INFO
{
	MEM_MAP	MainMem;			/* main memory */
	MEM_MAP	Reserved[NRESMEM];	/* system reserved memory */
	MEM_MAP	Modules;			/* boot modules (driver, boot tasks) */
	KERNEL_MODULE	Driver;		/* driver image */
	KERNEL_MODULE	BootTask;	/* boot tasks image */
}BOOT_INFO;
typedef BOOT_INFO *PBOOT_INFO;

#endif /* !_PREX_BOOTINFO_H */
