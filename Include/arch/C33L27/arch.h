#ifndef _C33PE_ARCH_H
#define _C33PE_ARCH_H

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
	DWORD ALR;	// 00
	DWORD AHR;	// 01
	DWORD R0;	// 02
	DWORD R1;	// 03
	DWORD R2; 	// 04
	DWORD R3;	// 05
	DWORD R4;	// 06
	DWORD R5;	// 07
	DWORD R6;	// 08
	DWORD R7;	// 09
	DWORD R8;	// 10
	DWORD R9;	// 11
	DWORD R10;	// 12
	DWORD R11;	// 13
	DWORD R12;	// 14
	DWORD R13;	// 15
	DWORD R14;	// 16
	DWORD DP;	// 17
	DWORD SP;	// 18
	DWORD PSR;	// 19
	DWORD RET;	// 20
}CONTEXT;         
typedef CONTEXT *PCONTEXT;


// Kernel mode context for context switching.
typedef struct _KCONTEXT
{
	DWORD ALR;	// 00
	DWORD AHR;	// 01
	DWORD R0;	// 02
	DWORD R1;	// 03
	DWORD R2;	// 04
	DWORD R3;	// 05
	DWORD R4;	// 06
	DWORD R5;	// 07
	DWORD R6;	// 08
	DWORD R7;	// 09
	DWORD R8;	// 10
	DWORD R9;	// 11
	DWORD R10;	// 12
	DWORD R11;	// 13
	DWORD R12;	// 14
	DWORD R13;	// 15
	DWORD R14;	// 16
	DWORD DP;	// 17
	DWORD SP;	// 18
	DWORD PSR;	// 19
	DWORD RET;	// 20
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



#endif
