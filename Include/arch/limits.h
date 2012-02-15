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

#ifndef _INC_LIMITS
#define _INC_LIMITS

#if defined(__arm__)
#include "./arm/limits.h"
#elif defined (__i386__)
#include "./i386/limits.h"
#elif defined (__ppc__)
#include "./ppc/limits.h"
#elif defined (__mips__)
#include "./mips/limits.h"
#elif defined (__sh4__)
#include "./sh4/limits.h"
#elif defined (WIN32)
#include "./win32/limits.h"
#elif defined (__c33pe)
#include "./C33L27/limits.h"
#else
#error architecture not supported
#endif

#endif // _INC_LIMITS
