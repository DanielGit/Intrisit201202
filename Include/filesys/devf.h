//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//    	         ϵͳ�����ļ�ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2010-3      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DEV_FILE_H
#define _DEV_FILE_H

typedef struct _DEVF_CALLBACK
{
	const char *Name;
	int (*DevFInit)(void);
	HANDLE (*DevFOpen)(const char *name, const char *mode);
	int (*DevFClose)(HANDLE hfile);
	long (*DevFRead)(void *pbuf, long size, long count, HANDLE hfile); 
	long (*DevFWrite)(void *pbuf, long size, long count, HANDLE hfile);
	int (*DevFSeek)(HANDLE hfile, long offset, int origin);
	long (*DevFTell)(HANDLE hfile);
	long (*DevFSize)(HANDLE hfile); 
}DEVF_CALLBACK;
typedef DEVF_CALLBACK *PDEVF_CALLBACK;


int DevFInit(void);

int DevFMount(PDEVF_CALLBACK callback);
int DevFUnmount(PDEVF_CALLBACK callback);

HANDLE DevFOpen(const char *name, const char *mode);
int DevFClose(HANDLE hfile);
long DevFRead(void *pbuf, long size, long count, HANDLE hfile); 
long DevFWrite(void *pbuf, long size, long count, HANDLE hfile);
int DevFSeek(HANDLE hfile, long offset, int origin);
long DevFTell(HANDLE hfile);
long DevFSize(HANDLE hfile); 


#endif // _DEV_FILE_H
