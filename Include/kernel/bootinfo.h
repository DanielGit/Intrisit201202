//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    处理器相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

/*
 * Boot information
 *
 * The boot information is stored by an OS loader, and
 * it is refered by kernel later in boot time.
 */

#ifndef _KERNEL_BOOTINFO_H
#define _KERNEL_BOOTINFO_H

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
	MEM_MAP	MainMem;				/* main memory */
	MEM_MAP	Reserved[NRESMEM];		/* system reserved memory */
	MEM_MAP	Modules;				/* boot modules (driver, boot tasks) */
	KERNEL_MODULE	Driver;			/* driver image */
	KERNEL_MODULE	BootTask;	/* boot tasks image */
}BOOT_INFO;
typedef BOOT_INFO *PBOOT_INFO;

#endif /* !_KERNEL_BOOTINFO_H */
