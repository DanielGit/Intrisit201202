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
#ifndef _FTL_API_H
#define _FTL_API_H

#include <config.h>
#include <filesys/nand.h>
#include <filesys/fsio.h>

extern WORD wFtlNandPBlocks;
extern WORD wFtlNandLBlocks;
extern WORD wFtlNandPPB;			// Page Per Block
extern WORD wFtlNandSPP;			// Sector Per Page
extern WORD wFtlNandSPB;			// Sector Per Page
extern WORD wFtlNandPSize;
extern DWORD dwFtlNandPPages;
extern DWORD dwFtlNandLPages;
extern DWORD dwFtlNandPSecs;
extern DWORD dwFtlNandLSecs;
extern DWORD dwFtlNandBSize;

void FtlRespondChange(FSIO_AREA area);
void FtlNotifyChange(FSIO_AREA area);
void FtlNotifyLocked(FSIO_AREA area);
int FtlAreaInit(FSIO_AREA area);
int FtlAreaInitIsFinish(FSIO_AREA area);
int FtlFlush(FSIO_AREA area, int immediately);
int FtlStatus(FSIO_AREA area);
int FtlSectorRead(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* buf);
int FtlSectorWrite(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* buf);
int FtlGetCapacity(FSIO_AREA area);
int FtlRelease(FSIO_AREA area, int *percent);
int FtlReadRandom(FSIO_AREA area, long addr, long size, BYTE* buf);

int FtlIoInit(void);
int FtlIoGetNandInfo(PNAND_INFO info);
int FtlIoBlockErase(DWORD block);
int FtlIoCheckBad(DWORD block);
int FtlIoMarkBadBlock(DWORD block);
int FtlIoReadID(BYTE *id);
int FtlIoPageRead(DWORD page, BYTE *dst, DWORD *lpage);
int FtlIoPageWrite(DWORD page, DWORD *src, DWORD *lpage);
int FtlIoPageInfoRead(DWORD page, DWORD *lpage);

#endif // _FTL_API_H

