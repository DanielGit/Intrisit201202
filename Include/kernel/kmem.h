//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    内核相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
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
