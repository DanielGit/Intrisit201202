//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	              内核句柄管理
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2008-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _HANDLE_H
#define _HANDLE_H

#include <kernel/kernel.h>

// 内核对象魔数
#define PORT_MAGIC		0x4f626a3f	// 'Obj?'
#define TASK_MAGIC		0x54736b3f	// 'Tsk?'
#define THREAD_MAGIC	0x5468723f	// 'Thr?'
#define DEVICE_MAGIC	0x4465763f	// 'Dev?'
#define MUTEX_MAGIC		0x4d75783f	// 'Mux?'
#define COND_MAGIC		0x436f6e3f	// 'Con?'
#define SEM_MAGIC		0x53656d3f	// 'Sem?'
#define KMUTEX_MAGIC	0x4b6d783f	// 'Kmx?'
#define KCOND_MAGIC		0x4b636e3f	// 'Kcn?'
#define KSEM_MAGIC		0x4b736d3f	// 'Ksm?'
#define IRQ_MAGIC		0x5972713f	// 'Irq?'
#define FILE_MAGIC		0x4661743f	// 'Fat?'
#define DIR_MAGIC		0x4469723f	// 'Dir?'
#define KFILE_MAGIC		0x4b66743f	// 'Kft?'
#define KDIR_MAGIC		0x4b64723f	// 'Kdr?'
#define WINDOW_MAGIC	0x57696e3f	// 'Win?'
#define WINTIMER_MAGIC	0x57746d3f	// 'Wtm?'
#define CALLBACK_MAGIC	0x43626b3f	// 'Cbk?'
#define ALARM_MAGIC		0x416c6d3f	// 'Alm?"
#define AUDIO_MAGIC		0x41646f3f	// 'Ado?''
#define VEDIO_MAGIC		0x56646f3f	// 'Vdo?'
#define VIDEO_MAGIC		VEDIO_MAGIC	// 'Vdo?'
#define MEDIA_MAGIC		0x4d64613f	// 'Mda?'
#define WAVOUT_MAGIC	0x57766f3f	// 'Wvo?'
#define DEVF_MAGIC		0x4476663f	// 'Dvf?'
#define CFGF_MAGIC		0x4366663f	// 'Cff?'
#define RAMF_MAGIC		0x526d673f	// 'Rmf?'
#define ISOF_MAGIC		0x59736f3f	// 'Iso?'
#define RFSF_MAGIC		0x5267733f	// 'Rfs?'
#define BIMG_MAGIC		0x426d673f	// 'Bmg?'
#define SIMG_MAGIC		0x546d673f	// 'Smg?'
#define BURN_MAGIC		0x42726e3f	// 'Brn?'

typedef struct _KERNEL_OBJECT
{
	DWORD Magic;
	HANDLE Handle;
	int (*Destroy)(HANDLE);
	LIST TaskLink;					// 内核对象任务链
}KERNEL_OBJECT;
typedef KERNEL_OBJECT *PKERNEL_OBJECT;

void HandleInit(void);
HANDLE HandleSet(void *object, DWORD magic, int (*destroy)(HANDLE));
void *HandleGet(HANDLE handle, DWORD magic);
int HandleValid(HANDLE handle);
int HandleValidEx(HANDLE handle, DWORD magic);
int HandleDestroy(HANDLE handle, DWORD magic);

#if defined(STC_EXP)
int sHandleClose(HANDLE handle);
#else
int HandleClose(HANDLE handle);
#endif

#endif // _HANDLE_H
