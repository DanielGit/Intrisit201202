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

#ifndef _INC_STDARG
#define _INC_STDARG

#if defined(__arm__)
#include "./arm/stdarg.h"
#elif defined (__i386__)
#include "./i386/stdarg.h"
#elif defined (__ppc__)
#include "./ppc/stdarg.h"
#elif defined (__mips__)
#include "./mips/stdarg.h"
#elif defined (__sh4__)
#include "./sh4/stdarg.h"
#elif defined (WIN32)
#include "./win32/stdarg.h"
#elif defined (__c33pe)
#include "./C33L27/stdarg.h"
#else
#error architecture not supported
#endif

#endif // _INC_STDARG
