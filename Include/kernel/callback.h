//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    �ص�����ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _CALLBACK_H
#define _CALLBACK_H

typedef void (*CALLBACK_FUNC)(void *);

typedef struct _CALLBACK_OBJECT
{
	KERNEL_OBJECT Header;
	LIST Link;
	HANDLE WaitSema;
	HANDLE FinishSema;	
	void *Param;
	int ParamSize;
}CALLBACK_OBJECT;
typedef CALLBACK_OBJECT *PCALLBACK_OBJECT;

void KernCallbackInit(void);
int KernCallbackFunc(HANDLE hcb, void *param);

#if defined(STC_EXP)
int sKernCallbackWait(HANDLE hcb, void *param);
int sKernCallbackFinish(HANDLE hcb, void *param);
int sKernCallbackDestroy(HANDLE hcb);
HANDLE sKernCallbackCreate(int paramsize, int trig);
#else
int KernCallbackWait(HANDLE hcb, void *param);
int KernCallbackFinish(HANDLE hcb, void *param);
int KernCallbackDestroy(HANDLE hcb);
HANDLE KernCallbackCreate(int paramsize, int trig);
#endif

#endif // !_CALLBACK_H
