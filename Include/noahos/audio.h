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


#ifndef _AUDIO_H
#define _AUDIO_H

// ���ò����ļ�����
#define AUDIO_PLAY_TYPE_UNKNOWN			0
#define AUDIO_PLAY_TYPE_MIDI			1
#define AUDIO_PLAY_TYPE_MP3				2
#define AUDIO_PLAY_TYPE_AMR				3
#define AUDIO_PLAY_TYPE_WAVE			4
#define AUDIO_PLAY_TYPE_WMA				5
#define AUDIO_PLAY_TYPE_PCM				6
#define AUDIO_PLAY_TYPE_MELODY			7
#define AUDIO_PLAY_TYPE_MP3_I			8
#define AUDIO_PLAY_TYPE_MP3_II			9
#define AUDIO_PLAY_TYPE_AAC				10
#define AUDIO_PLAY_TYPE_NES				11
#define AUDIO_PLAY_TYPE_BT				12
#define AUDIO_PLAY_TYPE_SNES			13
#define AUDIO_PLAY_TYPE_LPC_WMA			14
#define AUDIO_PLAY_TYPE_FLAC_NATIVE		15
#define AUDIO_PLAY_TYPE_FLAC_OGG		16

// ����¼���ļ�����
#define AUDIO_REC_TYPE_WAVE				0
#define AUDIO_REC_TYPE_PCM    			101
#define AUDIO_REC_TYPE_AMR				102
#define AUDIO_REC_TYPE_MP3				103
#define AUDIO_REC_TYPE_UNKOWN			255

typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

typedef struct _AUDIO_TASK
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
	
	int Volume;				// ��Ƶ¼������
	int Tempo;				// ��Ƶ�����ٶ�
	int Eq;					// ��Ƶ����EQģʽ
	
	DWORD Bitrate;			// ��Ƶ¼�Ʋ�����
	DWORD Samplerate;		// ��Ƶ¼�Ʋ�����
	DWORD Device;			// ¼���豸���壺MIC/LINE-IN
	
	void *Extend;			// ��������
}AUDIO_TASK;
typedef AUDIO_TASK *PAUDIO_TASK;

#endif //_AUDIO_H

