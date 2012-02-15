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


#ifndef _WIN32_ARCH_H
#define _WIN32_ARCH_H

//  Common register frame for trap/interrupt.
//  These cpu state are saved into top of the kernel stack in
//  trap/interrupt entries. Since the arguments of system calls are
//  passed via registers, the system call library is completely
//  dependent on this register format.
//
//  The value of ss & esp are not valid for kernel mode trap
//  because these are set only when privilege level is changed.
// FPU register for fsave/frstor
typedef struct _FCONTEXT
{
	DWORD	CtrlWord;
	DWORD	StatWord;
	DWORD	TagWord;
	DWORD	IpOffset;
	DWORD	CsSel;
	DWORD	OpOffset;
	DWORD	OpSel;
	DWORD	St[20];
}FCONTEXT;
typedef FCONTEXT *PFCONTEXT;


typedef struct _CONTEXT
{
	DWORD	Eax;		// 00
	DWORD	Ebx;		// 04
	DWORD	Ecx;		// 08
	DWORD	Edx;		// 12
	DWORD	Esi;		// 16
	DWORD	Edi;		// 20
	DWORD	Ebp;		// 24
	DWORD	Efl;		// 28
	DWORD	Ret;		// 32
	DWORD	Eip;		// 36
	DWORD	Esp;		// 40
}CONTEXT;
typedef CONTEXT *PCONTEXT;


typedef struct _INT_CONTEXT
{
	FCONTEXT Fpu;		// 44
	DWORD	Edi;		// 20
	DWORD	Esi;		// 16
	DWORD	Ebp;		// 24
	DWORD	Esp;		// 40
	DWORD	Ebx;		// 04
	DWORD	Edx;		// 12
	DWORD	Ecx;		// 08
	DWORD	Eax;		// 00
	DWORD	Efl;		// 
}INT_CONTEXT;
typedef INT_CONTEXT *PINT_CONTEXT;


// Kernel mode context for context switching.
typedef struct _KCONTEXT
{
	DWORD   Ebx;		//  +0 (00)
	DWORD	Eip;		//  +4 (04)
	DWORD	Edi;		//  +8 (08)
	DWORD	Esi;		// +12 (0C)
	DWORD	Ebp;		// +16 (10)
	DWORD	Esp;		// +20 (14)
	FCONTEXT Fpu;
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

#endif /* !_WIN32_ARCH_H */
