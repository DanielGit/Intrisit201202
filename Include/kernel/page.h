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


#ifndef _PAGE_H
#define _PAGE_H

void	*PageAlloc(size_t);
void	 PageFree(void *, size_t);
int	 PageReserve(void *, size_t);
void	 PageDump(char *buf, char *obj);
void	 PageInit(void);

#endif // !_PAGE_H
