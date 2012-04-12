//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典模拟平台  *****
//        --------------------------------------
//    	           
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <noahos.h>
#include <libc/stdlib.h>
#include "lzw.h"

#define WND_XSIZE	CONFIG_MAINLCD_XSIZE
#define WND_YSIZE	CONFIG_MAINLCD_YSIZE

typedef struct _TASK_LOGO
{
	int Wait;
	int Mode;
	int sd_test_flag;
	LZW_OBJ LzwObj;
	HANDLE hTimer;
}TASK_LOGO;
typedef TASK_LOGO *PTASK_LOGO;


static const BYTE SmartLogo[] = 
{
#if defined(CONFIG_MAC_NE350) || defined(CONFIG_MAC_NE360)
#include "logo_ne350.dat"
#elif defined(CONFIG_MAC_ET580)
#include "logo_et580.dat"
#elif defined(CONFIG_MAC_FV580)
#include "logo_fv580.dat"
#elif defined(CONFIG_MAC_FD750)
#include "logo_fd750.dat"
#elif defined(CONFIG_MAC_NP7000)
#include "logo_np7000.dat"
#elif defined(CONFIG_MAC_JZ4750)
#include "logo_jz4750.dat"
#else
#include "logo_default.dat"
#endif
};

extern int TaskDesktop(int base, int entry, int argc, char **argv);
extern int TaskBar(int base, int entry, int argc, char **argv);
extern int TaskWindows(int base, int entry, int argc, char **argv);
extern int TaskMediaPlay(int base, int entry, int argc, char **argv);


////////////////////////////////////////////////////
// 功能:
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int cbTaskLogo(PMESSAGE msg)
{
	HWND hwnd = msg->hWnd;
	HDC hdc;
	PTASK_LOGO logo;
	HANDLE hdesktop;
	int w, h, key, temp_count;
	
	logo = (PTASK_LOGO)GuiGetUsrPoint(hwnd, 0);
	switch(msg->MsgId)
	{
	case WM_CREATE:
		logo = malloc(sizeof(TASK_LOGO));
		GuiSetUsrPoint(hwnd, logo, 0);
		logo->Mode = 0;
		logo->Wait = 0;
		logo->sd_test_flag = 0;
		logo->hTimer = NewTimer(hwnd, 0, 500);
		logo->LzwObj.in.base = (void*)&SmartLogo[8];
		logo->LzwObj.in.offset = 0;
		logo->LzwObj.in.size = SmartLogo[0];
		logo->LzwObj.in.size +=  (SmartLogo[1] << 8);
		logo->LzwObj.in.size +=  (SmartLogo[2] << 16);
		logo->LzwObj.in.size +=  (SmartLogo[3] << 24);
		logo->LzwObj.out.size = SmartLogo[4];
		logo->LzwObj.out.size +=  (SmartLogo[5] << 8);
		logo->LzwObj.out.size +=  (SmartLogo[6] << 16);
		logo->LzwObj.out.size +=  (SmartLogo[7] << 24);
		logo->LzwObj.out.base = malloc(logo->LzwObj.out.size);
		logo->LzwObj.out.offset = 0;
		LzwDecode(&logo->LzwObj);
		break;
		
	case WM_PAINT:
		hdc = BeginPaint(hwnd, NULL);

		// 清除屏幕
		SetColor(hdc, COLOR_BLACK);
		FillRect(hdc, 0, 0, WND_XSIZE, 30);
		FillRect(hdc, 0, WND_YSIZE-30, WND_XSIZE, 30);
		
		// 显示加载画面
		DrawBmpData(hdc, 0, 0, logo->LzwObj.out.base);
		
		// 显示等待图标
		EndPaint(hwnd, NULL);
		break;
		
	case WM_TIMER:
		temp_count = 2;
		if(logo->sd_test_flag)
			temp_count = 20;

		if(fsinited() && (logo->Wait >= temp_count))
		{
			// 退出logo画面
			DeleteTimer(logo->hTimer);
			logo->hTimer = NULL;
			DeleteWindow(hwnd);
		}
		else
		{
			logo->Wait++;
			if(++logo->Mode == 4)
				logo->Mode = 0;
			InvalidateWindow(hwnd);
		}
		break;
	case WM_KEYDOWN:
		key = msg->Data.v;
		if( key == VK_DOWN )
			logo->sd_test_flag = 1;
		break;

	case WM_DELETE:
		hdesktop = NULL;
#if defined(WIN32)
		hdesktop = CreateSimDebugProcess("Desktop", "", TaskDesktop);
		CreateSimDebugProcess("TaskBar", "", TaskBar);
		CreateSimDebugProcess("Windows", "", TaskWindows);
#elif defined(CONFIG_MAC_NE350) || defined(CONFIG_MAC_NE360) || defined(CONFIG_MAC_NP7000) || defined(CONFIG_MAC_NP5800) || defined(CONFIG_MAC_ND800)
		if(logo->sd_test_flag)
			hdesktop = CreateProcess("E:\\Programs\\sdTestMain.elf", "");

		if(hdesktop == NULL)
			hdesktop = CreateProcess("F:\\Programs\\jDesktop.elf", "");

		if(hdesktop == NULL)
			hdesktop = CreateProcess("D:\\Programs\\jDesktop.elf", "");

		if(hdesktop == NULL)
			hdesktop = CreateProcess("C:\\Programs\\jDesktop.elf", "");
#else
		if(logo->sd_test_flag)
			hdesktop = CreateProcess("E:\\Programs\\sdTestMain.elf", "");

		if(hdesktop == NULL)
			hdesktop = CreateProcess("D:\\Programs\\jDesktop.elf", "");

		if(hdesktop == NULL)
			hdesktop = CreateProcess("C:\\Programs\\jDesktop.elf", "");
#endif		

#ifndef WIN32
		if(hdesktop == NULL)
			hdesktop = CreateProcess("C:\\jDesktop.elf", "");
#endif

		if(hdesktop == NULL)
		{
			DEVIO_CTRL ctl;
			HANDLE husb;
			int val;
			
			// 打开自动联机
			husb = DeviceOpen("USB-SLAVE", DEV_OPEN_READ|DEV_OPEN_WRITE);
			if(husb == NULL)
				break;
			
			val = 1;
			ctl.Cmd = IOCTL_USB_LINK_AUTO_SET;
			ctl.InBuff = NULL;
			ctl.InSize = 0;
			ctl.OutBuff = &val;
			ctl.OutSize = sizeof(int);
			ctl.RetSize = NULL;
			DeviceIoctl(husb, &ctl);
			
			DeviceClose(husb);
		}
		else
		{
			// 允许USB联机
			DEVIO_CTRL ctl;
			HANDLE husb;
			int val;
			
			// 打开自动联机
			husb = DeviceOpen("USB-SLAVE", DEV_OPEN_READ|DEV_OPEN_WRITE);
			if(husb == NULL)
				break;
			
			val = 1;
			ctl.Cmd = IOCTL_USB_LINK_ENABLE;
			ctl.InBuff = NULL;
			ctl.InSize = 0;
			ctl.OutBuff = NULL;
			ctl.OutSize = 0;
			ctl.RetSize = NULL;
			DeviceIoctl(husb, &ctl);
			
			DeviceClose(husb);
		}	
		PostQuitMsg(hwnd);
		break;
	}
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	HWND hwnd;
	MESSAGE msg;
	hwnd = RegisterWindow(0, 0, WND_XSIZE, WND_YSIZE, NULL, WS_SHOW|WS_SLAYER_DESKTOP, cbTaskLogo, 0);
	while(MsgGet(&msg))
	{
		MsgDispatch(&msg);
	}
	for(;;)
	{
		TaskTerminate(TaskSelf());
	}
}

