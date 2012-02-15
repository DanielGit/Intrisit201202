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

#ifndef _INC_ANSI
#define _INC_ANSI

#if defined(__arm__)
#include "./arm/ansi.h"
#elif defined (__i386__)
#include "./i386/ansi.h"
#elif defined (__ppc__)
#include "./ppc/ansi.h"
#elif defined (__mips__)
#include "./mips/ansi.h"
#elif defined (__sh4__)
#include "./sh4/ansi.h"
#elif defined (WIN32)
#include "./win32/ansi.h"
#elif defined (__c33pe)
#include "./C33L27/ansi.h"
#else
#error architecture not supported
#endif

#endif // _INC_ANSI
