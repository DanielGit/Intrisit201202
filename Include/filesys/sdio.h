//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	       Nand Flash逻辑/物理转换层部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
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

