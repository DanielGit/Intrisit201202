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
#ifndef _FTL_PRIVATE_H
#define _FTL_PRIVATE_H

#include <config.h>
#include <filesys/nand.h>
#include <filesys/fsio.h>
#include <filesys/ftl.h>

////////////////////////////////////////////
// 逻辑－物理转换表内容定义
////////////////////////////////////////////
#define BLANK_BLOCK			0xffff		// 定义未使用的块
#define BAD_BLOCK			0xfffe		// 定义当前块为坏块
#define BLANK_PAGE			0xffffff	// 定义未使用的页
#define FINISH_PAGE_FLAG	0x800000	// 完整块标志

#define MAX_PFTL_BLOCK_CACHE		16
#define MAX_PFTL_PAGE_CACHE			128

#if (MAX_PFTL_PAGE_CACHE < MAX_PFTL_BLOCK_CACHE)
#error "SFTL PAGE-CACHES IS LACK!"
#endif

#define FTL_MEDIA_REMOVED		(1)		// 存储设备已经取出
#define FTL_MEDIA_INSERT		(2)		// 存储设备插入过程
#define FTL_MEDIA_READY			(3)		// 存储设备已经插入，并且被检查到为有效存储设备
#define FTL_MEDIA_INITED		(4)		// 存储设备已经初始化
#define FTL_MEDIA_UNKNOWN		(5)		// 存储设备无法识别
#define FTL_MEDIA_CHANGED		(6)		// 存储设备已经改变
#define FTL_MEDIA_LOCKED		(7)		// 存储设备被锁住(USB占用)

#define FTL_AREA_RFS			0
#define FTL_AREA_FAT			1
#define FTL_AREA_RESERVE0		2
#define FTL_AREA_RESERVE1		3
#define FTL_AREA_RESERVE2		4
#define FTL_AREA_RESERVE3		5
#define FTL_AREA_ISOFS			6

typedef struct _LBS_CACHE
{
	char bInit;						//初始化标志 1:始化了，0:没有
	char bCreate;					//创建标志 1:创建了, 0:没有
	char Times;						//Sector Per Page
	int UnitIn;						//Sector size
	int UnitOut;					//Page size
	int ReadUnit;					//读到的sector数
	int WriteUnit;					//写入的sector数
	DWORD *ReadBuf;					//读buf (page size)
	DWORD *WriteBuf;				//写buf (page size)
}LBS_CACHE;
typedef LBS_CACHE *PLBS_CACHE;

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

// Loader, BIOS, SYTEM相关
int FtlLbsDrvGetUnits(FSIO_AREA area);
int FtlLbsDrvGetUnitSize(FSIO_AREA area);
int FtlLbsDrvWriteUnit(FSIO_AREA area, DWORD unit, DWORD *buf);
int FtlLbsDrvReadUnit(FSIO_AREA area, int unit, BYTE *buf);
int FtlLbsDrvAreaErase(FSIO_AREA area, int *percent);

int FtlLbsCacheInit(FSIO_AREA area);
void FtlLbsCacheFlush(FSIO_AREA area);
int FtlLbsCacheReadUnit(FSIO_AREA area, DWORD unit, BYTE *buf);
int FtlLbsCacheWriteUnit(FSIO_AREA area, DWORD unit, BYTE *buf);

int FtlLbsFlush(FSIO_AREA area, int immediately);
int FtlLbsSectorRead(FSIO_AREA area, DWORD sector, DWORD secs, BYTE *buf);
int FtlLbsSectorWrite(FSIO_AREA area, DWORD sector, DWORD secs, BYTE *buf);
void FtlLbsRespondChange(FSIO_AREA area);
void FtlLbsNotifyChange(FSIO_AREA area);
void FtlLbsNotifyLocked(FSIO_AREA area);
int FtlLbsAreaInit(FSIO_AREA area);
int FtlLbsFlush(FSIO_AREA area, int immediately);
int FtlLbsStatus(FSIO_AREA area);
int FtlLbsGetCapacity(FSIO_AREA area);
int FtlLbsRelease(FSIO_AREA area, int *percent);


// 块信息相关函数
void FtlBInfoInit(void);
void FtlBInfoSetUsed(WORD pblock);
void FtlBInfoClearUsed(WORD pblock);
void FtlBInfoMarkBad(WORD pblock);
WORD FtlBInfoGetBlank(void);
WORD FtlBInfoGetP(WORD lblock);
WORD FtlBInfoGetL(WORD pblock);
void FtlBInfoReleaseL(WORD lblock);
void FtlBInfoSetPL(WORD lblock, WORD pblock);
BYTE FtlBInfoGetCount(WORD pblock);

// 读缓冲区管理
int FtlRCacheInit(void);
int FtlRCachePageGet(DWORD lpage, DWORD **addr);
int FtlRCachePageCheck(DWORD lpage, DWORD **addr);
int FtlRCacheRead(DWORD lsec, DWORD secs, BYTE *buf);
int FtlRCacheUpdate(DWORD lpage, DWORD *buf);
int FtlRCacheRelease(WORD lblock);
int FtlRCachePageLock(DWORD lpage, int lock);

// 写缓冲区管理
void FtlWCacheInit(void);
DWORD FtlWCacheGetP(DWORD lpage);
int FtlWCacheWrite(DWORD lsec, DWORD secs, BYTE *buf);
int FtlWCacheFlushAll(int immediately);
int FtlWCacheFlushReady(void);

// 小数据写入加速处理
int FtlAccInit(void);
int FtlAccWrite(DWORD lsec, DWORD secs, BYTE *buf);
void FtlAccRelease(DWORD lpage);
int FtlAccFlushAll(void);
int FtlAccCheck(DWORD lpage);

// 异常恢复部分
int FtlFixInit(void);
int FtlFixAddConflict(WORD pblock0, WORD pblock1);
int FtlFixProcess(void);

// 其他函数
int FtlMiscReadRandom(long addr, long size, BYTE* buf);

#endif // _FTL_PRIVATE_H
