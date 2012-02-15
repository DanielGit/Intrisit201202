//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	         
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]
 
#include <Kernel/Kernel.h>
#include <peripheral/usb.h>

#ifndef __UDCBUS_H__
#define __UDCBUS_H__

#define USB_JUDGE					0x150
#define USB_REMOVE					0x151
#define USB_CHARGE					0x152
#define USB_RESET					2
#define USB_SUSPEND					3
#define USB_SETUP_PKG_FINISH		4
#define USB_PROTAL_RECEIVE_FINISH	5
#define USB_PROTAL_SEND_FINISH		6

#define USB_HIGHSPEED	0
#define USB_FULLSPEED	1

//#define USB_HW_CONNECT		2
//#define USB_JUDGE		3
//#define USB_JUDGE		4

typedef struct _USB_LUN_INFO
{
	int Hiddens;
	int Heads;
	int Sectors;
	int PartSize;
	int SectorSize;
}USB_LUN_INFO;
typedef USB_LUN_INFO *PUSB_LUN_INFO;

typedef int (*USB_BUS_NOTIFY)(HANDLE, DWORD, BYTE*, DWORD);
typedef struct _USB_BUS
{
	DWORD DeviceName;
	USB_BUS_NOTIFY BusNotify;
	void (*StartTransfer)(HANDLE, BYTE, BYTE *, DWORD);
	int (*EnableDevice)(HANDLE);
	void (*DisableDevice)(HANDLE);
	void (*GetEndpointInfo)(HANDLE, BYTE, BYTE*, WORD*);
	void (*SetAddress)(HANDLE, WORD);
	void (*StartDetect)(HANDLE, int);
}USB_BUS;
typedef USB_BUS *PUSB_BUS;

typedef struct _USB_LUN
{
	DWORD DevName;
	DWORD (*CheckDevState)(HANDLE);
	DWORD (*FlushDev)(HANDLE);
	DWORD (*ReadDevSector)(HANDLE, BYTE*, DWORD , DWORD);
	DWORD (*WriteDevSector)(HANDLE, BYTE*, DWORD , DWORD);
	DWORD (*GetDevInfo)(HANDLE, PUSB_LUN_INFO);
	DWORD (*DeinitDev)(HANDLE);
	DWORD (*InitDev)(HANDLE);
}USB_LUN;
typedef USB_LUN *PUSB_LUN;

PUSB_BUS UsbBusAddDevice(USB_BUS_NOTIFY notify);
void UsbBusRemoveDevice(PUSB_BUS pbus);
PUSB_BUS UsbBusSwichDevice(USB_BUS_NOTIFY notify);
DWORD BusDeviceNotify(HANDLE handle, DWORD stat, BYTE *bufaddr, DWORD len);

#endif //__UDCBUS_H__
