//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    ���������ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _INC_ELF
#define _INC_ELF

#if defined(__arm__)
#include "./arm/elf.h"
#elif defined (__i386__)
#include "./i386/elf.h"
#elif defined (__ppc__)
#include "./ppc/elf.h"
#elif defined (__mips__)
#include "./mips/elf.h"
#elif defined (__sh4__)
#include "./sh4/elf.h"
#elif defined (WIN32)
#include "./win32/elf.h"
#elif defined (__c33pe)
#include "./c33l27/elf.h"
#else
#error architecture not supported
#endif

#endif // _INC_ELF
