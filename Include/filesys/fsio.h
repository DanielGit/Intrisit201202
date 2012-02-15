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
#ifndef _FSIOIO_H
#define _FSIOIO_H

#include <config.h>

typedef enum
{
	FSIO_ALL,
	FSIO_LOADER,
	FSIO_BIOS,
	FSIO_SYSTEM,
	FSIO_FSISO,
	FSIO_FSROM,
	FSIO_SYSCFG,
	FSIO_RAMBK,
	FSIO_FSFAT1,
	FSIO_FSFAT0,
	FSIO_TFCARD,
	FSIO_AREA_MAX
}FSIO_AREA;

#define FSIO_STATUS_REMOVED		(1)		// 存储设备已经取出
#define FSIO_STATUS_INSERT		(2)		// 存储设备插入过程
#define FSIO_STATUS_READY		(3)		// 存储设备已经插入，并且被检查到为有效存储设备
#define FSIO_STATUS_INITED		(4)		// 存储设备已经初始化
#define FSIO_STATUS_UNKNOWN		(5)		// 存储设备无法识别
#define FSIO_STATUS_CHANGED		(6)		// 存储设备已经改变
#define FSIO_STATUS_LOCKED		(7)		// 存储设备被锁住(USB占用)

int FsioInit(FSIO_AREA area);
int FsioInitIsFinish(FSIO_AREA area);
int FsioSectorRead(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* pdata);
int FsioSectorWrite(FSIO_AREA area, DWORD sector, DWORD secs, BYTE* pdata);
int FsioReadRandom(FSIO_AREA area, long addr, long size, BYTE* buf);
int FsioGetCapacity(FSIO_AREA area);
int FsioStatus(FSIO_AREA area);
void FsioNotifyChange(FSIO_AREA area);
void FsioRespondChange(FSIO_AREA area);
void FsioNotifyLocked(FSIO_AREA area);
int FsioFlush(FSIO_AREA area, int immediately);
int FsioRelease(FSIO_AREA area, int *percent);

#endif	// FSIOIO_H

