//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          直接音频播放控制函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_SURFACE_H
#define _DIRECT_SURFACE_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

typedef struct _SURFACE_INFO
{
	int BPP;
	int Width;
	int Height;
	void *Addr;	
}SURFACE_INFO;
typedef SURFACE_INFO *PSURFACE_INFO;

int SurfaceInit(void);

#if defined(STC_EXP)
HANDLE sSurfaceCreate(char *name, RECT *r, DWORD *addr, int cache);
int sSurfaceInfo(HANDLE hsf, PSURFACE_INFO info);
int sSurfaceDestroy(HANDLE hsf);
int sSurfaceUpdate(HANDLE hsf);
#else
HANDLE SurfaceCreate(char *name, RECT *r, DWORD *addr, int cache);
int SurfaceInfo(HANDLE hsf, PSURFACE_INFO info);
int SurfaceDestroy(HANDLE hsf);
int SurfaceUpdate(HANDLE hsf);
#endif

#endif	// _DIRECT_SURFACE_H

