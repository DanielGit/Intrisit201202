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
#ifndef _FTL_PRIVATE_H
#define _FTL_PRIVATE_H

#include <config.h>
#include <filesys/nand.h>
#include <filesys/fsio.h>
#include <filesys/ftl.h>

////////////////////////////////////////////
// �߼�������ת�������ݶ���
////////////////////////////////////////////
#define BLANK_BLOCK			0xffff		// ����δʹ�õĿ�
#define BAD_BLOCK			0xfffe		// ���嵱ǰ��Ϊ����
#define BLANK_PAGE			0xffffff	// ����δʹ�õ�ҳ
#define FINISH_PAGE_FLAG	0x800000	// �������־

#define MAX_PFTL_BLOCK_CACHE		16
#define MAX_PFTL_PAGE_CACHE			128

#if (MAX_PFTL_PAGE_CACHE < MAX_PFTL_BLOCK_CACHE)
#error "SFTL PAGE-CACHES IS LACK!"
#endif

#define FTL_MEDIA_REMOVED		(1)		// �洢�豸�Ѿ�ȡ��
#define FTL_MEDIA_INSERT		(2)		// �洢�豸�������
#define FTL_MEDIA_READY			(3)		// �洢�豸�Ѿ����룬���ұ���鵽Ϊ��Ч�洢�豸
#define FTL_MEDIA_INITED		(4)		// �洢�豸�Ѿ���ʼ��
#define FTL_MEDIA_UNKNOWN		(5)		// �洢�豸�޷�ʶ��
#define FTL_MEDIA_CHANGED		(6)		// �洢�豸�Ѿ��ı�
#define FTL_MEDIA_LOCKED		(7)		// �洢�豸����ס(USBռ��)

#define FTL_AREA_RFS			0
#define FTL_AREA_FAT			1
#define FTL_AREA_RESERVE0		2
#define FTL_AREA_RESERVE1		3
#define FTL_AREA_RESERVE2		4
#define FTL_AREA_RESERVE3		5
#define FTL_AREA_ISOFS			6

typedef struct _LBS_CACHE
{
	char bInit;						//��ʼ����־ 1:ʼ���ˣ�0:û��
	char bCreate;					//������־ 1:������, 0:û��
	char Times;						//Sector Per Page
	int UnitIn;						//Sector size
	int UnitOut;					//Page size
	int ReadUnit;					//������sector��
	int WriteUnit;					//д���sector��
	DWORD *ReadBuf;					//��buf (page size)
	DWORD *WriteBuf;				//дbuf (page size)
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

// Loader, BIOS, SYTEM���
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


// ����Ϣ��غ���
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

// ������������
int FtlRCacheInit(void);
int FtlRCachePageGet(DWORD lpage, DWORD **addr);
int FtlRCachePageCheck(DWORD lpage, DWORD **addr);
int FtlRCacheRead(DWORD lsec, DWORD secs, BYTE *buf);
int FtlRCacheUpdate(DWORD lpage, DWORD *buf);
int FtlRCacheRelease(WORD lblock);
int FtlRCachePageLock(DWORD lpage, int lock);

// д����������
void FtlWCacheInit(void);
DWORD FtlWCacheGetP(DWORD lpage);
int FtlWCacheWrite(DWORD lsec, DWORD secs, BYTE *buf);
int FtlWCacheFlushAll(int immediately);
int FtlWCacheFlushReady(void);

// С����д����ٴ���
int FtlAccInit(void);
int FtlAccWrite(DWORD lsec, DWORD secs, BYTE *buf);
void FtlAccRelease(DWORD lpage);
int FtlAccFlushAll(void);
int FtlAccCheck(DWORD lpage);

// �쳣�ָ�����
int FtlFixInit(void);
int FtlFixAddConflict(WORD pblock0, WORD pblock1);
int FtlFixProcess(void);

// ��������
int FtlMiscReadRandom(long addr, long size, BYTE* buf);

#endif // _FTL_PRIVATE_H
