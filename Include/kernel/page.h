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


#ifndef _PAGE_H
#define _PAGE_H

void	*PageAlloc(size_t);
void	 PageFree(void *, size_t);
int	 PageReserve(void *, size_t);
void	 PageDump(char *buf, char *obj);
void	 PageInit(void);

#endif // !_PAGE_H
