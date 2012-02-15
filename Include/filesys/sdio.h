//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//    	       Nand Flash�߼�/����ת���㲿��
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]
#ifndef _SDIO_H
#define _SDIO_H

#include <config.h>

int SdioInit(void);
int SdioSectorRead(DWORD sector, DWORD secs, BYTE* pdata);
int SdioSectorWrite(DWORD sector, DWORD secs, BYTE* pdata);
int SdioGetCapacity(void);
int SdioStatus(void);
void SdioNotifyChange(void);
void SdioRespondChange(void);
void SdioNotifyLocked(void);
int SdioFlush(int immediately);

#endif	// _SDIO_H

