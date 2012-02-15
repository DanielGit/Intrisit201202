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


#ifndef	_I386_SYSTRAP_H
#define	_I386_SYSTRAP_H

#define SYSCALL0(name) \
	.global name; .align 2; \
name##: movl	$(SYS_##name), %eax; \
	jmp	__systrap

#define SYSCALL1(name) SYSCALL0(name)
#define SYSCALL2(name) SYSCALL0(name)
#define SYSCALL3(name) SYSCALL0(name)
#define SYSCALL4(name) SYSCALL0(name)

#endif	/* _I386_SYSTRAP_H */
