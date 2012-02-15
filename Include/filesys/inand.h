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
#ifndef _INAND_H
#define _INAND_H

#include <config.h>
#include <filesys/fsio.h>

int InandrvSectorRead(DWORD sector, DWORD secs, BYTE* dst);
int InandrvSectorWrite(DWORD sector, DWORD secs, BYTE* src);
int InandrvGetInfo(PNAND_INFO info);

int InandLbsSectorRead(FSIO_AREA area, DWORD sector, DWORD secs, BYTE *buf);
int InandLbsSectorWrite(FSIO_AREA area, DWORD sector, DWORD secs, BYTE *buf);
int InandLbsGetCapacity(FSIO_AREA area);

void InandCacheInit(void);
int InandCacheRead(DWORD sector, DWORD secs, BYTE* pdata);
int InandCacheWrite(DWORD sector, DWORD secs, BYTE* pdata);
int InandCacheFlush(int immediately);
int InandCacheReady(void);


int InandAreaInit(FSIO_AREA area);
int InandAreaInitIsFinish(FSIO_AREA area);
int InandSectorRead(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* pdata);
int InandSectorWrite(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* pdata);
int InandGetCapacity(FSIO_AREA area);
int InandStatus(FSIO_AREA area);
void InandNotifyChange(FSIO_AREA area);
void InandRespondChange(FSIO_AREA area);
void InandNotifyLocked(FSIO_AREA area);
int InandFlush(FSIO_AREA area, int immediately);
int InandRelease(FSIO_AREA area, int *percent);
int InandReadRandom(FSIO_AREA area, unsigned long addr, long size, BYTE* buf);

#endif	// _INAND_H

