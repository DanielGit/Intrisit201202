//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    �ں����ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _SYSTEM_H
#define _SYSTEM_H

#if defined(STC_EXP)
int sKernelInfo(int, void *);
int sKernelTicks(void);
void * sKernMemcpy(void *dst, const void *src, size_t count);
#else
int KernelInfo(int, void *);
int KernelTicks(void);
void * KernMemcpy(void *dst, const void *src, size_t count)
#endif

#endif // !_SYSTEM_H
