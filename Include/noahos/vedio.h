//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//       	        �������������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2006-3      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <config.h>
#include <noahos.h>
#include <libc/stdlib.h>
#include <gui/wintype.h>


#ifndef _VEDIO_H
#define _VEDIO_H

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


#endif //_VEDIO_H

