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

#ifndef _INC_SYSTRAP
#define _INC_SYSTRAP

#if defined(__arm__)
#include "./arm/systrap.h"
#elif defined (__i386__)
#include "./i386/systrap.h"
#elif defined (__ppc__)
#include "./ppc/systrap.h"
#elif defined (__mips__)
#include "./mips/systrap.h"
#elif defined (__sh4__)
#include "./sh4/systrap.h"
#elif defined (WIN32)
#include "./win32/systrap.h"
#elif defined (__c33pe)
#include "./C33L27/systrap.h"
#else
#error architecture not supported
#endif

#endif // _INC_SYSTRAP
