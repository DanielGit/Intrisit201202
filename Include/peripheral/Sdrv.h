//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	           Nand Flash驱动部分
//        --------------------------------------
//                 版权: 新诺亚舟科技头文件
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef NANDRV_H
#define NANDRV_H

#include <config.h>
#include <kernel/kernel.h>

int SdrvSectorRead(DWORD sector, DWORD secs, BYTE* pdata);
int SdrvSectorWrite(DWORD sector, DWORD secs, BYTE* pdata);
int SdrvGetCapacity(void);
int SdrvIsValid(void);
int SdrvStatus(void);
void SdrvDetect(void);
void SdrvRespondChange(void);
int SdrvReinit(void);

extern int SdSectorReadEx(DWORD sector, DWORD secs, BYTE* pdata);
extern int SdSectorWriteEx(DWORD sector, DWORD secs, BYTE* pdata);
unsigned long SdGetCapacity(void);
int SdStatus(void);
int SdIsValid(void);
int SdDetect(void);

#endif //NANDRV_H
