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

#define FSIO_STATUS_REMOVED		(1)		// �洢�豸�Ѿ�ȡ��
#define FSIO_STATUS_INSERT		(2)		// �洢�豸�������
#define FSIO_STATUS_READY		(3)		// �洢�豸�Ѿ����룬���ұ���鵽Ϊ��Ч�洢�豸
#define FSIO_STATUS_INITED		(4)		// �洢�豸�Ѿ���ʼ��
#define FSIO_STATUS_UNKNOWN		(5)		// �洢�豸�޷�ʶ��
#define FSIO_STATUS_CHANGED		(6)		// �洢�豸�Ѿ��ı�
#define FSIO_STATUS_LOCKED		(7)		// �洢�豸����ס(USBռ��)

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

