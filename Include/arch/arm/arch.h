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


#ifndef _ARM_ARCH_H
#define _ARM_ARCH_H

//  Common register frame for trap/interrupt.
//  These cpu state are saved into top of the kernel stack in
//  trap/interrupt entries. Since the arguments of system calls are
//  passed via registers, the system call library is completely
//  dependent on this register format.
//
//  The value of ss & esp are not valid for kernel mode trap
//  because these are set only when privilege level is changed.
typedef struct _CONTEXT
{
	DWORD R0;	// 00-00
	DWORD R1;	// 01-04
	DWORD R2; 	// 02-08
	DWORD R3;	// 03-12
	DWORD R4;	// 04-16
	DWORD R5;	// 05-20
	DWORD R6;	// 06-24
	DWORD R7;	// 07-28
	DWORD R8;	// 08-32
	DWORD R9;	// 09-36
	DWORD R10;	// 10-40
	DWORD R11;	// 11-44
	DWORD R12;	// 12-48
	DWORD CPSR;	// 13-52
	DWORD LR;	// 14-56
	DWORD SP;	// 15-60
	DWORD PC;	// 16-64
}CONTEXT;         
typedef CONTEXT *PCONTEXT;


// Kernel mode context for context switching.
typedef struct _KCONTEXT
{
	DWORD R4;	// 00-00
	DWORD R5;	// 01-04
	DWORD R6;	// 02-08
	DWORD R7;	// 03-12
	DWORD R8;	// 04-16
	DWORD R9;	// 05-20
	DWORD R10;	// 06-24
	DWORD R11;	// 07-28
	DWORD R12;	// 08-32
	DWORD CPSR;	// 11-44
	DWORD LR;	// 09-36
	DWORD SP;	// 10-40
}KCONTEXT;           
typedef KCONTEXT *PKCONTEXT;

// types for Contextset
#define CTX_KENTRY	0x00		// set kernel mode entry address
#define CTX_KARG0	0x01		// set kernel mode argument
#define CTX_KARG1	0x02		// set kernel mode argument
#define CTX_KARG2	0x03		// set kernel mode argument
#define CTX_KARG3	0x04		// set kernel mode argument

#define CTX_UENTRY	0x08		// set user mode entry addres
#define CTX_USTACK	0x09		// set user mode stack address
#define CTX_UARG0	0x0a		// set user thread input param0
#define CTX_UARG1	0x0b		// set user thread input param1
#define CTX_UARG2	0x0c		// set user thread input param2
#define CTX_UARG3	0x0d		// set user thread input param3

#endif /* !_ARM_ARCH_H */
