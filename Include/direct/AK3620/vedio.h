//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	          ֱ����Ƶ���ſ��ƺ�������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_VEDIO_H
#define _DIRECT_VEDIO_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(CONFIG_MCU_AK3620) && !defined(WIN32)
#include "movie_player_lib.h"
#include "VedioPortShow.h"
#include "VedioPortCam.h"
#include "VedioPortFs.h"
#include "VedioPortMem.h"
#include "VedioPortShow.h"
#endif

typedef struct _VEDIO_FILE
{
	int PlayOffset;			// ��������ƫ��
	int PlayLength;			// �������ݳ���
	int vOffset;			// �����ļ�/������ƫ��
	
	FILE *File;				// ý�������ļ����
	
	BYTE *uBuf;				// �û�������ָ��
	int uSize;				// �û���������С
	int uValid;				// �û�����������Ч����
	int uOffset;			// �û���������������ƫ��
	HANDLE uCb;				// �ص��������

#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// ���ڶ�ȡ�û��ռ�ʹ��
#endif	
}VEDIO_FILE;
typedef VEDIO_FILE *PVEDIO_FILE;

typedef struct _VEDIO_TASK
{
	HWND hWnd;				// ��Ƶ��Ϣ���ʹ���
	int Record;				// ��Ƶ���� 0-���� 1-¼��
	int MediaType;			// ��Ƶý������
	
	char *FileName;			// ��Ƶ�ļ�����
	BYTE *DataBuf;			// ��Ƶ���ݻ�����
	int BufSize;			// ��Ƶ��������С
	DWORD PlayOffset;		// ��Ƶ������ʼƫ��
	DWORD PlayLength;		// ��Ƶ�������ݳ���
	HANDLE hCallback;		// �ص��������
	
	RECT ShowRect;			// ��Ƶ��ʾ����
	int Volume;				// ��Ƶ¼������
	int Eq;					// ��Ƶ����EQģʽ

	DWORD ABitrate;			// ��Ƶ¼�Ʋ�����
	DWORD ASamplerate;		// ��Ƶ¼�Ʋ�����
	DWORD VBitrate;			// ��Ƶ¼�Ʋ�����
	DWORD Device;			// ¼���豸���壺MIC/LINE-IN
	
	void *Extend;			// ��������
}VEDIO_TASK;
typedef VEDIO_TASK *PVEDIO_TASK;

typedef struct _VEDIO_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}VEDIO_CTRL;
typedef VEDIO_CTRL *PVEDIO_CTRL;

typedef struct _VEDIO_INFO
{
	int Type;
	int MovieWidth;
	int MovieHeight;
	int FPS;
	DWORD TotalTime;
	DWORD VedioBitrate;
	DWORD AudioBitrate;
}VEDIO_INFO;
typedef VEDIO_INFO *PVEDIO_INFO;

typedef struct _VEDIO_OBJECT
{
	KERNEL_OBJECT Header;	// �ں˶�����Ϣ
	LIST Link;
	int bGetInfo;
#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// ���ڶ�ȡ�û��ռ�ʹ��
#endif	
	VEDIO_TASK Task;
	VEDIO_CTRL Ctrl;		// ������Ϣ
	VEDIO_FILE File;		// ���ݻ���������ṹ��
	VEDIO_INFO Info;		// ������Ϣ�ṹ��
}VEDIO_OBJECT;
typedef VEDIO_OBJECT *PVEDIO_OBJECT;

#define VEDIO_LOCK()		//	MediaSrvLock()
#define VEDIO_UNLOCK()		//  MediaSrvUnlock()

// VedioCtl.c
int VedioCtlInit(void);
int VedioCtlVolumeSet(PVEDIO_OBJECT obj, int volume);
int VedioCtlPause(PVEDIO_OBJECT obj);
int VedioCtlResume(PVEDIO_OBJECT obj);
int VedioCtlSeek(PVEDIO_OBJECT obj, DWORD milliSecond);
int VedioCtlGetDuration(PVEDIO_OBJECT obj, DWORD *duration);
int VedioCtlIsPlaying(PVEDIO_OBJECT obj);
int VedioCtlPhonePlus(void);
void VedioCtlPipo(int ena);
int VedioCtlForward(PVEDIO_OBJECT obj);
int VedioCtlBackward(PVEDIO_OBJECT obj);

// VedioThread.c
int VedioThreadInit(void);
void VedioThreadClose(PVEDIO_OBJECT obj, int code);
void VedioThreadOpen(PVEDIO_OBJECT obj);
int VedioThreadRun(void);

// VedioSrv.c
void VedioSrvInit(void);
int VedioSrvDestroyNotify(PVEDIO_TASK task);
int VedioSrvPowerOff(int mode);
int VedioSrvMode(void);
void VedioSrvRefreshFrame(BYTE *srcImg, short src_width, short src_height);
void VedioSrvProcMsg(PVEDIO_OBJECT obj);
int VedioSrvInfo(void *media, void *info);

#endif	// _DIRECT_VEDIO_H

