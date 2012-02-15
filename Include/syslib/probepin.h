//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//       	        ʱ�����ڴ�����
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _PROBE_PIN_H
#define _PROBE_PIN_H

#if defined(STC_EXP)
void sProbePinOpen(void);
void sProbePinClose(void);
int sProbePinFlp(void);
int sProbePinOut(int lvl);
int sProbePinGet(void);
#else
void ProbePinOpen(void);
void ProbePinClose(void);
int ProbePinFlp(void);
int ProbePinOut(int lvl);
int ProbePinGet(void);
#endif

#endif // _PROBE_PIN_H
