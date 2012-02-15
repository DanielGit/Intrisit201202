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


#ifndef _KMEM_H
#define _KMEM_H

void	*kmalloc(size_t);
void	 kfree(void *);

void	*KmMap(void *, size_t);
void	 KmInit(void);

#if defined(STC_EXP)
int sKmInfo(DWORD *blks, DWORD *alcsize);
#else
int KmInfo(DWORD *blks, DWORD *alcsize);
#endif

#endif // !_KMEM_H
