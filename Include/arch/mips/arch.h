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


#ifndef _MIPS_ARCH_H
#define _MIPS_ARCH_H

#include <config.h>

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
	DWORD res0;		// 00-00
	DWORD res1;		// 01-04
	DWORD res2; 	// 02-08
	DWORD res3;		// 03-12
	DWORD res4;		// 04-16
	DWORD Flag;		// 状态寄存器
#if defined(CONFIG_ARCH_XBURST)
	DWORD XR1; 				// 06-24
	DWORD XR2; 				// 07-28
	DWORD XR3; 				// 08-32
	DWORD XR4; 				// 09-36
	DWORD XR5; 				// 10-40
	DWORD XR6; 				// 11-44
	DWORD XR7; 				// 12-48
	DWORD XR8; 				// 13-52
	DWORD XR9; 				// 14-56
	DWORD XR10;				// 15-60
	DWORD XR11;				// 16-64
	DWORD XR12;				// 17-68
	DWORD XR13;				// 18-72
	DWORD XR14;				// 19-76
	DWORD XR15;				// 20-80
	DWORD XR16;				// 21-84
#endif	
	DWORD LO;	// 06-24	// 22-88  
	DWORD HI;	// 07-28	// 23-92  
	DWORD RAT;	// 08-32	// 24-96 
	DWORD V0;	// 09-36	// 25-100 
	DWORD V1;	// 10-40	// 26-104 
	DWORD A0;	// 11-44	// 27-108 
	DWORD A1;	// 12-48	// 28-112 
	DWORD A2;	// 13-52	// 29-116 
	DWORD A3;	// 14-56	// 30-120 
	DWORD T0;	// 15-60	// 31-124 
	DWORD T1;	// 16-64	// 32-128 
	DWORD T2;	// 17-68	// 33-132 
	DWORD T3;	// 18-72	// 34-136
	DWORD T4;	// 19-76	// 35-140 
	DWORD T5;	// 20-80	// 36-144 
	DWORD T6;	// 21-84	// 37-148 
	DWORD T7;	// 22-88	// 38-152 
	DWORD S0;	// 23-92	// 39-156 
	DWORD S1;	// 24-96	// 40-160 
	DWORD S2;	// 25-100   // 41-164
	DWORD S3;	// 26-104   // 42-168
	DWORD S4;	// 27-108   // 43-172
	DWORD S5;	// 28-112   // 44-176
	DWORD S6;	// 29-116   // 45-180
	DWORD S7;	// 30-120   // 46-184
	DWORD T8;	// 31-124   // 47-188
	DWORD T9;	// 32-128   // 48-192
	DWORD GP;	// 33-132   // 49-196
	DWORD FP;	// 34-136	// 50-200	
	DWORD RA;	// 35-140   // 51-204
	DWORD SP;	// 36-144   // 52-208
}CONTEXT;         
typedef CONTEXT *PCONTEXT;

// Kernel mode context for context switching.
typedef struct _KCONTEXT
{
	DWORD res0;		// 00-00
	DWORD res1;		// 01-04
	DWORD res2; 	// 02-08
	DWORD res3;		// 03-12
	DWORD res4;		// 04-16
#if defined(CONFIG_ARCH_XBURST)
	DWORD XR1; 				// 05-20
	DWORD XR2; 				// 06-24
	DWORD XR3; 				// 07-28
	DWORD XR4; 				// 08-32
	DWORD XR5; 				// 09-36
	DWORD XR6; 				// 10-40
	DWORD XR7; 				// 11-44
	DWORD XR8; 				// 12-48
	DWORD XR9; 				// 13-52
	DWORD XR10;				// 14-56
	DWORD XR11;				// 15-60
	DWORD XR12;				// 16-64
	DWORD XR13;				// 17-68
	DWORD XR14;				// 18-72
	DWORD XR15;				// 19-76
	DWORD XR16;				// 20-80
#endif	                    
	DWORD LO;	// 05-20	// 21-84 
	DWORD HI;	// 06-24	// 22-88 
	DWORD S0;	// 07-28	// 23-92 
	DWORD S1;	// 08-32	// 24-96 
	DWORD S2;	// 09-36	// 25-100
	DWORD S3;	// 10-40	// 26-104
	DWORD S4;	// 11-44	// 27-108
	DWORD S5;	// 12-48	// 28-112
	DWORD S6;	// 13-52	// 29-116
	DWORD S7;	// 14-56	// 30-120
	DWORD GP;	// 15-60	// 31-124
	DWORD SP;	// 16-64	// 32-128
	DWORD FP;	// 17-68	// 33-132		
	DWORD RA;	// 18-72	// 34-136
	DWORD Flag;	// 19-76	// 35-140
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

#endif /* !_MIPS_ARCH_H */
