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

#ifndef _INC_SETJMP
#define _INC_SETJMP

#if defined(__arm__)
#include "./arm/setjmp.h"
#elif defined (__i386__)
#include "./i386/setjmp.h"
#elif defined (__ppc__)
#include "./ppc/setjmp.h"
#elif defined (__mips__)
#include "./mips/setjmp.h"
#elif defined (__sh4__)
#include "./sh4/setjmp.h"
#elif defined (WIN32)
#include "./win32/setjmp.h"
#elif defined (__c33pe)
#include "./C33L27/setjmp.h"
#else
#error architecture not supported
#endif

#endif // _INC_SETJMP
