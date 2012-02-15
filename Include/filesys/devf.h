//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	         系统配置文件头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
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
