//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨GUI  *****
//        --------------------------------------
//       	            GUIͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _WIN_TYPE_H
#define _WIN_TYPE_H

#include <config.h>
#include <gui/treebase.h>

typedef DWORD	COLOR;
typedef	DWORD	PINDEX;

#if !defined(CONFIG_LCD_GRAY)
#define WIN_GUI_SAVE_UNDER
#endif

typedef struct
{
	int x, y;
}POINT;
typedef POINT *PPOINT;

typedef struct
{
	int x, y, w, h;
}RECT;
typedef RECT *PRECT;

typedef struct
{
	int x, y, w, h, x1, y1;
}WRECT;
typedef WRECT *PWRECT;


typedef struct _CHAR_BITMAP
{
    int Rows;		// The number of bitmap rows.
    int Width;		// The number of pixels in bitmap row. 
    int Pitch;		// The number of bytes taken by one bitmap row
    BYTE*  Buffer;	// Pointer to the bitmap buffer
    short  Grays;	// It gives the number of gray levels used in the bitmap
    char PixelMode;
    char PaletteMode;
    void* Palette;
}CHAR_BITMAP;
typedef CHAR_BITMAP *PCHAR_BITMAP;

typedef struct
{
	int NumEntries; 
	char Trans;  	   
	const COLOR* pPalEntries;
}LOGPALETTE; 
typedef LOGPALETTE *PLOGPALETTE;

typedef struct
{
	WORD XSize;
	WORD YSize;
	WORD BPL;			// Bytes-Per-Line
	WORD BPP;			// Bits-Per-Pixel
	const BYTE* pData;
	const LOGPALETTE* pPal;
} BITMAP;
typedef BITMAP *PBITMAP;

typedef struct
{
	BYTE Type;			// ͼƬ���ͣ�������չ��ȱʡΪ0
	BYTE BPP;			// ÿ����λ����������λΪ1����ʾΪ��ɫͼƬ
	WORD BPL;			// ͼƬÿ�������ֽ���
	WORD XSize;			// ͼƬ���
	WORD YSize;			// ͼƬ�߶�
	DWORD Trans;		// ͼƬ͸��ɫ
	const BYTE* pdata;	// ͼƬ����ָ��
}DIB;
typedef DIB *PDIB;

typedef struct
{
	DWORD MsgId;						// ��Ϣ����
	struct _WND_OBJECT *hWnd;			// ��ϢĿ�Ĵ���
	struct _WND_OBJECT *hWndSrc;		// ��ϢԴ����
	union								// ��Ϣ�������ݽṹ
	{
		void* p;
		DWORD v;
		//֧�ֶ�㴥��
#if defined(CONFIG_MULT_TOUCH) && defined(CONFIG_MAX_POINT)
		// �����δ�ţ����һ���ռ������Ƶ���Ϣ
		DWORD n[CONFIG_MAX_POINT + 1];
#endif
	}Data;
	DWORD Param;							// ���Ӳ���
}MESSAGE;
typedef MESSAGE *PMESSAGE;
#define TPMX(msg, i) ((msg)->data.n[i] & 0xffff)
#define TPMY(msg, i) ((msg)->data.n[i] >>16)

// SendMsgʹ�õ���Ϣ�ṹ�壬����Ӧ��ʱʹ�õ���Ժ�
typedef struct
{
	MESSAGE Msg;
	DWORD MatchId;
}MESSAGEEX;
typedef MESSAGEEX *PMESSAGEEX;

typedef struct _WINMSG_QUEUE
{
	LIST Link;
	MESSAGE Message;
	DWORD Stamp;						// ��Ϣʱ���
}WINMSG_QUEUE;
typedef WINMSG_QUEUE* PWINMSG_QUEUE;

typedef struct _WINMSG_QUEUEEX
{
	LIST Link;
	MESSAGEEX Message;
	DWORD Stamp;						// ��Ϣʱ���
	void *Context;
}WINMSG_QUEUEEX;
typedef WINMSG_QUEUEEX* PWINMSG_QUEUEEX;

typedef struct _WINMSG_QUEUES
{
	struct _WND_OBJECT *hCurWin;	// ��ǰ��Ϣ������
	HANDLE QEvent;					// ��Ϣ�����¼�
	HANDLE hMutex;					// ��Ϣ�ṹ�廥����
	LIST SendQ;						// ����SEND��Ϣ����
	LIST ReplyQ;					// ����REPLY��Ϣ����
	LIST PostQ;						// ����POST��Ϣ����
	LIST QuitQ;						// �����˳���Ϣ����
#if defined(CONFIG_TOUCH_ENABLE)
	LIST TouchQ;					// ���崥������Ϣ����
#endif
#if defined(CONFIG_MOUSE_ENABLE)
	LIST MouseQ;					// ���崥������Ϣ����
#endif
	LIST KeyQ;						// ���尴����Ϣ����
	LIST PaintQ;					// �����ػ���Ϣ����
	LIST TimerQ;					// ������Ϣ����
	LIST FastQ;						// ������ٴ�����Ϣ����
}WINMSG_QUEUES;
typedef WINMSG_QUEUES* PWINMSG_QUEUES;

typedef struct _LCD_DC
{
	PINDEX BgColor;
	PINDEX FgColor;
	int LcdWidth;
	int LcdHeight;
	int VLcdWidth;
	int VLcdHeight;
	int DrawMode;
	WRECT ClipRect;
}LCD_DC;
typedef LCD_DC *PLCD_DC;

typedef struct _PAINT_STRUCT
{
	void *p;
}PAINTSTRUCT;

typedef int (*WIN_CALLBACK)(MESSAGE *pmsg);
typedef struct _WND_OBJECT
{
	KERNEL_OBJECT Header;			// �ں˶���ͷ��Ϣ
	HANDLE pTask;					// �������ڵĽ���
	HANDLE pThread;					// �������ڵ��߳�
	
	TREE Herit;						// �̳й�ϵ��
	TREE Focus;						// �۽���ϵ��
	TREE Layer;						// λ�ù�ϵ��
	
	RECT AbsRect;  					// ���ھ���(�����Ļ����ľ�������)
	RECT HoldRect;					// ���������ȴ�����к�ľ���
	RECT InvalidRect;				// ������Ч����(�����Ļ����)
	RECT ClipRect;					// ���ڼ��м���ľ���

	RECT UsrRect;					// �����û�����(��ת�����Ļ����)
	int Angle;						// ������ת�Ƕ�

	int Refresh;					// ��ǰ����ˢ�º����LCDˢ�²���

	DWORD Id;						// ����ID��
	DWORD Status;					// ����״̬��־
#ifdef CONFIG_MAINLCD_ICON
	DWORD Icon;						// ����ICON��ʶ
#endif
	DWORD hIme;						// ���ڶ�Ӧ���뷨����
	
	// ���ڹҽ���Ϣ����
	PWINMSG_QUEUES pQueues;
	WIN_CALLBACK cb;				// ���ڻص�����
	
	// ���ڶ�Ӧ��ͼ���豸
	LCD_DC SysDc;
	PVOID UsrDc;					// �û�̬�µ�DCָ��
	
	// MXU״̬
#if defined(CONFIG_ARCH_XBURST) && !defined(WIN32) && (CONFIG_MAINLCD_BPP == 32)
	int MxuCR;
#endif	
    
    DWORD Time;                       // ���崴����ʱ��OS TICKS
    BYTE Painted;                   //�Ѿ�paint���ı�־
	// �����û����ݿ�ָ��
	PVOID pUsr[4];					
}WND_OBJECT;
typedef WND_OBJECT *PWND_OBJECT;
typedef PWND_OBJECT HWND;


typedef struct _WND_TIMER
{
	KERNEL_OBJECT Header;
	LIST Link;
	HWND hWnd;
	int Id;
	DWORD Period;
	DWORD T0;
}WND_TIMER;
typedef WND_TIMER *PWND_TIMER;

typedef struct _TOUCH_KEY
{
	RECT Rect;
	int Key;
}TOUCH_KEY;
typedef TOUCH_KEY *PTOUCH_KEY;

typedef struct _TOUCH_SMART_POINT
{
	BYTE *pPointBuf;		//�켣��������
	int nPointPos;						//�켣����ĵ�ǰλ��
	struct _TOUCH_SMART_POINT* pNext;	//��һ������
}TOUCH_SMART_POINT;
typedef TOUCH_SMART_POINT *PTOUCH_SMART_POINT;

typedef struct _TOUCH_SMART
{
	PTOUCH_SMART_POINT	pPointHead;	//�켣��������
	DWORD nPointIdx;				//��ǰ����켣��
	int	nTouchMsg;					//���������ʶ��

	int nStartX,nStartY;			//��ʼ������
	int nEndX,nEndY;				//��ֹ������

	DWORD nCurTime;					//��ǰ���ʱ��Ƭ
	DWORD nForTime;					//��һ��ʱ����ʱ��Ƭ
	DWORD nPressTime;				//����ʱ��
	DWORD nForClickTime;			//��һ��CLICK��ʱ��

	BYTE nForMsg;					//��һ������ID
	int  nForX,nForY;				//��һ��������λ��

	BYTE SmartCountBuf[9];			//����BUF,������������״̬����λ������
	BYTE SmartCountSat[9];			//����BUF,������������״̬����λ�ٷֱȣ�

	DWORD nClickCount;				//click�ĸ���
	DWORD nPressCount;				//push�ĸ���
}TOUCH_SMART;
typedef TOUCH_SMART *PTOUCH_SMART;

typedef struct _MOUSE_MESSAGE_INFO
{
	int		type;				// ��ǰ��Ϣ����
	int		button;				// ��ǰ��갴������
	POINT	point;				// ��ǰ���λ��(�����õ���ǰ�������)
	POINT	Press_point;		// ��һ�ΰ��µ�λ��(�������㣬����clickʱ����һ�ΰ��µ�����)

	DWORD	click_time;			// click��Ϣ�е�clickʱ��(���µ������ʱ��)
	DWORD	click_count;		// clickһ�������Ĵ���

	DWORD	double_click_time;	// 2��click֮���ʱ��
	POINT	double_click_size;	// 2��click֮��ľ���(��������ֵ)
	
	int		WheelOffset;		// �м���ֵ�ת������
	HWND	hwnd;				// ��Ҫ���͵Ĵ���
}MOUSE_MESSAGE_INFO;
typedef MOUSE_MESSAGE_INFO *PMOUSE_MESSAGE_INFO;

typedef struct _MOUSE_SHAPE
{
	int		nMouseShapeCount;		//�����״�ĸ���
	int		nMouseShapeWidth;		//���Ŀ��
	int		nMouseShapeHeight;		//���ĸ߶�
	int		nMouseShapeTime;		//���仯�ļ��
	int		nMouseInitShape;		//����ʼ���ľ�̬ͼƬID
	void*	pMouseShapeBuf;			//�����״����
}MOUSE_SHAPE;
typedef MOUSE_SHAPE *PMOUSE_SHAPE;

// *******************************************************************
//   ���ⰴ����������
// *******************************************************************
#define VK_LBUTTON      0x01
#define VK_RBUTTON      0x02
#define VK_CANCEL       0x03
#define VK_MBUTTON      0x04

#define VK_BACK         0x08
#define VK_TAB          0x09

#define VK_CLEAR        0x0c
#define VK_RETURN       0x0d

#define VK_SHIFT        0x10
#define VK_CONTROL      0x11
#define VK_MENU         0x12
#define VK_PAUSE        0x13
#define VK_CAPITAL      0x14

#define VK_KANA         0x15
#define VK_HANGEUL      0x15
#define VK_HANGUL       0x15
#define VK_JUNJA        0x17
#define VK_FINAL        0x18
#define VK_HANJA        0x19
#define VK_KANJI        0x19

#define VK_ESCAPE       0x1b

#define VK_CONVERT      0x1c
#define VK_NONCONVERT   0x1d
#define VK_ACCEPT       0x1e
#define VK_MODECHANGE   0x1f

#define VK_ALT			0x90
#define VK_IME			0x91

#define VK_HOLD			0xa0
#define VK_PRIOR        0xa1
#define VK_NEXT         0xa2
#define VK_END          0xa3
#define VK_HOME         0xa4
#define VK_LEFT         0xa5
#define VK_UP           0xa6
#define VK_RIGHT        0xa7
#define VK_DOWN         0xa8
#define VK_INSERT       0xad
#define VK_DELETE       0xae
#define VK_HELP         0xaf

#define VK_B1			0xb0
#define VK_B2			0xb1
#define VK_B3			0xb2
#define VK_B4			0xb3
#define VK_B5			0xb4
#define VK_B6			0xb5
#define VK_B7			0xb6
#define VK_B8			0xb7
#define VK_B9			0xb8
#define VK_B10			0xb9
#define VK_B11			0xba
#define VK_B12			0xbb
#define VK_B13			0xbc
#define VK_B14			0xbd
#define VK_B15			0xbe
#define VK_B16			0xbf

#define VK_C1			0xc0
#define VK_C2			0xc1
#define VK_C3			0xc2
#define VK_C4			0xc3
#define VK_C5			0xc4
#define VK_C6			0xc5
#define VK_C7			0xc6
#define VK_C8			0xc7
#define VK_C9			0xc8
#define VK_C10			0xc9
#define VK_C11			0xca
#define VK_C12			0xcb
#define VK_C13			0xcc
#define VK_C14			0xcd
#define VK_C15			0xce
#define VK_C16			0xcf

#define VK_D1			0xd0
#define VK_D2			0xd1
#define VK_D3			0xd2
#define VK_D4			0xd3
#define VK_D5			0xd4
#define VK_D6			0xd5
#define VK_D7			0xd6
#define VK_D8			0xd7
#define VK_D9			0xd8
#define VK_D10			0xd9
#define VK_D11			0xda
#define VK_D12			0xdb
#define VK_D13			0xdc
#define VK_D14			0xdd
#define VK_D15			0xde
#define VK_D16			0xdf

#define VK_M1			0xe0
#define VK_M2			0xe1
#define VK_M3			0xe2
#define VK_M4			0xe3
#define VK_M5			0xe4
#define VK_M6			0xe5
#define VK_M7			0xe6
#define VK_M8			0xe7
#define VK_M9			0xe8
#define VK_M10			0xe9
#define VK_M11			0xea
#define VK_M12			0xeb
#define VK_M13			0xec
#define VK_M14			0xed
#define VK_M15			0xee
#define VK_M16			0xef

#define VK_F1           0xf0
#define VK_F2           0xf1
#define VK_F3           0xf2
#define VK_F4           0xf3
#define VK_F5           0xf4
#define VK_F6           0xf5
#define VK_F7           0xf6
#define VK_F8           0xf7
#define VK_F9           0xf8
#define VK_F10          0xf9
#define VK_F11          0xfa
#define VK_F12          0xfb
#define VK_F13          0xfc
#define VK_F14          0xfd
#define VK_F15          0xfe
#define VK_F16          0xff

#define VK_PAGEUP		VK_PRIOR
#define VK_PAGEDOWN		VK_NEXT
#define VK_ENTER		VK_RETURN
#define VK_ESC			VK_ESCAPE

#define VK_POWER		VK_F8
#define VK_ACCENT		VK_IME
#define VK_LAN			VK_IME

// *******************************************************************
//   ��Ϣ��������
// *******************************************************************
#define WM_PAINT				1	// �����ػ���Ϣ
#define WM_CREATE				3	// ���ڴ�����Ϣ(���ڴ���ʱ����)
#define WM_DELETE				4	// ����ɾ����Ϣ(�����˳�ǰ����)
#define WM_SIZE					6	// ���ڴ�С�ı�ʱ����
#define WM_MOVE					7	// �����ƶ�ʱ����
#define WM_SHOW					8	// �򴰿ڷ�����ʾ����Ϣ(�����û�����)
#define WM_HIDE					9	// �򴰿ڷ������ص���Ϣ(�����û�����)
#define WM_NOTIFY				10	// �ؼ��򸸴��ڷ�����ʾ��Ϣ����Ϣ
#define	WM_TIMER				11	// ��ʱ����Ϣ����ʱ����ʱ����
#define WM_TERMINATE_AP			12	// ����ǿ�ƽ�����Ϣ
#define WM_LOAD					13	// ���ڴ�����ĵ�һ����Ϣ
#define WM_REPAINT				14	// Ϊ���ٶ�ʹ�õ�һ�������PAINT�¼�
#define WM_ROTATE				15	// ���ڱ�Ҫ����ת��Ϣ
#define WM_QUIT					16	// ���屻�����˳�
#define WM_QUIT_OK				17  // ����ɹ��˳�
#define WM_QUIT_WAIT			18	// ���������ӳ�
#define WM_QUIT_CANCEL			19	// ���������˳�����
#define WM_CHANGE_TV			20	// �����л���TV-OUTģʽ
#define WM_CHANGE_LCD			21	// �����л���LCD-OUTģʽ
#define WM_GRID_IN				22	// ���ӵ�GRID
#define WM_GRID_OUT				23	// �Ͽ�GRID
#define WM_HEADPHONE_IN			24	// ��������
#define WM_HEADPHONE_OUT		25	// �����γ�
#define WM_TOUCH_SMART			26	// ����ʶ�����

#define WM_TOUCH_DOWN			40	// ���ڴ�������Ϣ(�ʰ�����Ϣ)
#define WM_TOUCH_UP				41	// ���ڴ�������Ϣ(�ʵ�����Ϣ)
#define WM_TOUCH_PRESS			42	// ���ڴ�������Ϣ(�ʰ�ס������Ϣ����ʱ����)
#define WM_TOUCH				WM_TOUCH_DOWN
#define WM_TPDOWN				WM_TOUCH_DOWN
#define WM_TPUP					WM_TOUCH_UP
#define WM_TPPRESS				WM_TOUCH_PRESS
#define WM_TPMOVE				43
#define WM_TPGESTURE			44	//����ʶ��
#define WM_TPCLICK				45	//���ڴ�������Ϣ(���������)

#define WM_KEYDOWN				50	// ���ڰ�����Ϣ(��������ʱ���ͣ�ֻ�о۽����ڲŽ��մ���Ϣ)
#define WM_KEYUP				51	// ���ڰ�����Ϣ(��������ʱ���ͣ�ֻ�о۽����ڲŽ��մ���Ϣ)
#define WM_KEYPRESS				52	// ���ڰ�����Ϣ(������ס����ʱ���ͣ���ʱ���͡�ֻ�о۽����ڲŽ��մ���Ϣ)
#define WM_KEY					WM_KEYDOWN
#define	WM_CHAR					53	// �����ַ���Ϣ(���뷨ʹ��ʱ����)
#define WM_TRACE_START			54	// ��д���뿪ʼ�¼�
#define WM_TRACE_END			55	// ��д��������¼�

#define WM_MOUSE_ATTACH			60	// ������йص��¼�
#define WM_MOUSE_DEATTACH		61
#define WM_MOUSE_DOWN			62
#define WM_MOUSE_UP				63
#define WM_MOUSE_CLICK			64
#define WM_MOUSE_DCLICK			65
#define WM_MOUSE_MOVE			66
#define WM_MOUSE_ENTER			67
#define WM_MOUSE_LEAVE			68
#define WM_MOUSE_HOVER			69
#define WM_MOUSE_WHEEL			70
#define WM_MOUSE_REMOVE			71

#define WM_GRID_PEN				90	// ���������ʰ���
#define WM_GRID_PENDOWN			90	// ���������ʰ���
#define WM_GRID_PENPRESS		91	// ���������ʰ�ס
#define WM_GRID_PENUP			92	// ���������ʵ���

#define WM_GET_CLIENT_RECT		100	// ��ȡ�ͻ������������ֵ
#define WM_GET_CLIENT_RECT_ABS	101	// ��ȡ�ͻ����ľ�������
#define WM_GET_INSIDE_RECT		102	// ��ȡ�����ڲ�����
#define WM_GETORG				104	// ��ȡ�����󶥵�����ֵ
#define WM_GET_ID				105	// ��ȡ����ID��
#define WM_GET_CLIENT_WINDOW	106	// ��ȡ�ͻ������ھ����ȱʡֵΪ������
#define WM_SET_ENABLE			107	// ��ֹ����ʹ�ܴ���ʱ���͵���Ϣ
#define WM_SET_FOCUS			110	// ���ô��ڽ�����Ϣ�����ڻ�û���ʧȥ����ʱ����
#define WM_GET_HAS_FOCUS		113	// ��鵱ǰ�����Ƿ��ȡ����
#define WM_GET_BKCOLOR			114	// ��ȡ���ڱ�����ɫ��Ϣ
#define WM_GET_FOCUSSED_CHILD	115	// ��ȡ�۽��Ӵ��ھ��(�Ի���ʹ��)
#define WM_INIT_DIALOG			116	// �Ի����ʼ����Ϣ(�Ի���ʹ��)
#define WM_GET_SBAR				117 // ��ȡӦ�ó��򴰿�״̬�������Ϣ
#define WM_LOST_FOCUS			118	// ĳ����ʧȥ����

#define WM_USB_LINK				200	// USB������ʼ
#define WM_USB_UNLINK			201	// USB��������
#define WM_POWER_OFF			220
#define WM_POWER_ON				221
#define WM_SD_CHANGE			240
#define WM_SD_INSERT			241
#define WM_SD_REMOVE			242
#define WM_SD_UNKNOWN			243
#define WM_LOW_BAT				260	// �͵�ѹ�¼�
#define WM_CHARGE_DONE			261	// ��س���,������
#define WM_CHARGE_UNDO			262	// �����°γ�������˳�
#define WM_CHARGE_RUN			263 // ��������
#define WM_CHARGE_STOP			264	// ���ֹͣ
#define WM_CHARGE_DO			265	// ��ʼ���
#define WM_ALARM				270	// �����¼�
#define WM_AP_CREATE			280	// Ӧ�ó����
#define WM_AP_DESTROY			281	// Ӧ�ó���ر�
#define WM_AP_POWOFF			282	// �ػ���ʾӦ�ó�����¼�
#define WM_AP_POWON				283	// ������ʾӦ�ó�����¼�
#define WM_AP_AUTOFF			284	// �Զ��ػ���ʾӦ�ó�����¼�

#define WM_FILE_MODIFY			300	// �ļ����޸�

#define USBM_WRITE_SECTOR		400	// USB����д������Ϣ(USB����רҵ)
#define USBM_READ_SECTOR		401	// USB������������Ϣ(USB����רҵ)
#define USBM_DOWNBIOS			402	// USB������������BIOS��������
#define USBM_DOWNSYS			403	// USB������������ϵͳ������������
#define USBM_DOWNROM			404	// USB�����������ؿ�ӳ���ļ���������
#define USBM_DOWNFAT			405	// USB������������FAT�ļ�ϵͳӳ����������
#define USBM_DOWNEND			406	// USB���������������
#define USBM_KILL				407	// USB��������ȡ����������
#define USBM_ERASEBIOS			410	// USB�����������BIOS�¼�
#define USBM_ERASESYS			411	// USB�����������ϵͳ�¼�
#define USBM_ERASEROM			412	// USB��������������ļ��¼�
#define USBM_ERASEFAT			413	// USB�����������FAT�ļ�ϵͳ
#define USBM_CRCBIOS			414	// USB��������У��BIOS�¼�
#define USBM_CRCSYS				415	// USB��������У��ϵͳ�¼�
#define USBM_CRCROM				416	// USB��������У����ļ��¼�

#define WM_LOW_BAT_POFF				500	// �͵�ѹ�¼�, ��Ҫ���Ϲػ�

#define WM_AUTO_TEST_RECORD_START	550
#define WM_AUTO_TEST_PLAY_START		551
#define WM_AUTO_TEST_RECORD_END		552
#define WM_AUTO_TEST_PLAY_END		553

#define WM_TERMINATE			1001	// ����AP��Ľ��̿���(һ��APҪ������һ��AP����)
#define	WM_USER					2000

// *******************************************************************
//   (BGR)��ɫ��������
// *******************************************************************
#define COLOR_BLACK   		0x000000
#define COLOR_BLUE			0xFF0000
#define COLOR_GREEN   		0x00ff00
#define COLOR_CYAN			0xffff00
#define COLOR_RED 			0x0000FF
#define COLOR_MAGENTA 		0x8b008b
#define COLOR_BROWN   		0x2a2aa5
#define COLOR_DARKGRAY		0x404040
#define COLOR_GRAY			0x808080
#define COLOR_LIGHTGRAY     0xd3d3d3
#define COLOR_LIGHTBLUE     0xff8080
#define COLOR_LIGHTGREEN    0x80ff80
#define COLOR_LIGHTCYAN     0x80ffff
#define COLOR_LIGHTRED		0x8080ff
#define COLOR_LIGHTMAGENTA  0xff80ff
#define COLOR_YELLOW		0x00ffff
#define COLOR_WHITE			0xffffff
#define COLOR_ORANGE		0x0AC7F5
#define COLOR_PINK			0xFF80FF
#define COLOR_GREYGREEN		0x549C4B
#define COLOR_INVALID		0xfffffff

#if	(CONFIG_MAINLCD_BPP == 2)
#define COLOR_GRAY0			COLOR_WHITE
#define COLOR_GRAY1			0x818181
#define COLOR_GRAY2			0x414141
#define COLOR_GRAY3			COLOR_BLACK
#define COLOR_GRAY4			COLOR_GRAY1
#define COLOR_GRAY5			COLOR_GRAY1
#define COLOR_GRAY6			COLOR_GRAY1
#define COLOR_GRAY7			COLOR_GRAY1
#define COLOR_GRAY8			COLOR_GRAY2
#define COLOR_GRAY9			COLOR_GRAY2
#define COLOR_GRAY10		COLOR_GRAY2
#define COLOR_GRAY11		COLOR_GRAY2
#define COLOR_GRAY12		COLOR_GRAY4
#define COLOR_GRAY13		COLOR_GRAY4
#define COLOR_GRAY14		COLOR_GRAY4
#define COLOR_GRAY15		COLOR_GRAY4
#else
#define COLOR_GRAY0			COLOR_WHITE
#define COLOR_GRAY1			0xe1e1e1
#define COLOR_GRAY2			0xd1d1d1
#define COLOR_GRAY3			0xc1c1c1
#define COLOR_GRAY4			0xb1b1b1
#define COLOR_GRAY5			0xa1a1a1
#define COLOR_GRAY6			0x919191
#define COLOR_GRAY7			0x818181
#define COLOR_GRAY8			0x717171
#define COLOR_GRAY9			0x616161
#define COLOR_GRAY10		0x515151
#define COLOR_GRAY11		0x414141
#define COLOR_GRAY12		0x313131
#define COLOR_GRAY13		0x212121
#define COLOR_GRAY14		0x111111
#define COLOR_GRAY15		COLOR_BLACK
#endif


// *******************************************************************
//   GDI�йس�������
// *******************************************************************
#define DM_NORMAL		(0x00)
#define DM_XOR			(0x01)
#define DM_TRANS		(0x02)
#define DM_REV			(0x04)
#define DM_GRAY			(0x08)
#define DM_SHALLOW		(0x10)

#define TM_NORMAL   	DM_NORMAL
#define TM_XOR  		DM_XOR
#define TM_TRANS		DM_TRANS
#define TM_REV  		DM_REV
#define TM_SHALLOW		DM_SHALLOW

#define LS_SOLID		(0)
#define LS_DASH			(1)
#define LS_DOT			(2)
#define LS_DASHDOT		(3)
#define LS_DASHDOTDOT	(4)

#define PS_ROUND		(0)
#define PS_FLAT			(1)
#define PS_SQUARE		(2)

#define TA_HORIZONTAL	(3<<0)
#define TA_LEFT 		(0<<0)
#define TA_RIGHT		(1<<0)
#define TA_CENTER		(2<<0)
#define TA_HCENTER		TA_CENTER

#define TA_VERTICAL		(3<<2)
#define TA_TOP			(0<<2)
#define TA_BOTTOM		(1<<2)
#define TA_VCENTER		(3<<2)



// *******************************************************************
//   ����״̬�йس���
// *******************************************************************
// ����״̬��־
#define WS_INVALID				(1<<0)	// ��ǰ�����Ƿ���Ч(�ȴ�PAINT�¼�)
#define WS_TRANS				(1<<1)	// �����Ƿ�Ϊ͸������
#define WS_ENABLE				(1<<2)	// ��ǰ�����Ƿ�����
#define WS_ENABLE_MASK			(1<<3)	// ��ǰ�����Ƿ���������λ
#define WS_SHOW					(1<<4)	// ��ǰ�����Ƿ�ɼ�
#define WS_SHOW_MASK			(1<<5)	// ��ǰ���ڿɼ���־����λ
#define WS_FOCUS				(1<<6)	// ��ǰ�����Ƿ��ý���
#define WS_FOCUS_MASK			(1<<7)	// ��ǰ�����Ƿ��ֹ���ܽ���
#define WS_GETDC				(1<<8)	// ��ǰ�����Ѿ���ȡDC�豸                        	
#define WS_GRAY					(1<<9)	// ��ǰ����ʹ�ûҶ���ʾ
#define WS_TOUCH_IGNORE			(1<<10)	// ��ǰ���ڲ���õ㴥��Ϣ

#define WS_MOUSE_PROPERTIES		(1<<11)	// ��ǰ���ڶ�Ӧ��������
										// enable���Խ��� WM_MOUSE_HOVER,WM_MOUSE_LEAVE,WM_MOUSE_ENTER ��Ϣ
										// disable������յ� WM_MOUSE_HOVER,WM_MOUSE_LEAVE,WM_MOUSE_ENTER ��Ϣ

// ������״̬��ر�־�����±�־�ڴ���ʱ���ã��Ժ��ܸı�                        	
#define WS_MAIN					(1<<12)	// ������(���ӹ��ܴ��ڻ�ý��㣬�������ڻ�ý���)
#define WS_MODAL				(1<<13)	// ģ̬����
#define WS_EXFOCUS				(1<<14)	// �����役��͸���������(MDI����ʹ��)
#define WS_EXRECT				(1<<15)	// ��������ʾ��Χ���������ڷ�Χ
#define WS_MSGQ					(1<<16)	// �������Ƿ���ڶ�������Ϣ����
#define WS_FOCUS_ALWAYS				(1<<17)	// �ڴ���ʲô��������Զ��ý���
#define WS_SAVE_UNDER				(1<<18)	// ����͸������ײ�����

// �û������ö����弶��
#define WS_ULAYER				(0x03<<20)
#define WS_SLAYER				(0x07<<22)
#define WS_LAYER				(0x1f<<20)	// �����ö�����

// Ӧ�ó����ڴ����μ���
#define WS_ULAYER_SHIFT			(20)
#define WS_ULAYER0				(0<<WS_ULAYER_SHIFT)
#define WS_ULAYER1				(1<<WS_ULAYER_SHIFT)
#define WS_ULAYER2				(2<<WS_ULAYER_SHIFT)
#define WS_ULAYER_IME			(3<<WS_ULAYER_SHIFT)	// ���뷨ռ��

// Ӧ�ó���䴰���μ���
#define WS_SLAYER_SHIFT			(22)
#define WS_SLAYER_APP0			(0<<WS_SLAYER_SHIFT)	// 1: ��ͨӦ�ó���
#define WS_SLAYER_APP1			(1<<WS_SLAYER_SHIFT)	// 2: ����Ӧ�ó���1
#define WS_SLAYER_SYS2			(2<<WS_SLAYER_SHIFT)	// 3: ��ʼ��
#define WS_SLAYER_APP2			(3<<WS_SLAYER_SHIFT)	// 4: ����Ӧ�ó���2
#define WS_SLAYER_SYS0			(4<<WS_SLAYER_SHIFT)	// 5: ���������
#define WS_SLAYER_SYS1			(5<<WS_SLAYER_SHIFT)	// 6: ϵͳ��Ϣ
#define WS_SLAYER_DIRECT		(6<<WS_SLAYER_SHIFT)	// 7: ֱ��д������
#define WS_SLAYER_DESKTOP		(7<<WS_SLAYER_SHIFT)	// 0: ����(��ײ�)

// ������ת���Ա�־                       	
#define WS_ROTMASK				(3<<28)	// ������ת�������λ
#define WS_ROT0					(0<<28)	// ����˳ʱ����ת0��
#define WS_ROT90				(1<<28)	// ����˳ʱ����ת90��
#define WS_ROT180				(2<<28)	// ����˳ʱ����ת180��
#define WS_ROT270				(3<<28)	// ����˳ʱ����ת270��


#define COUNTOF(a)				(sizeof(a) / sizeof(a[0]))
#define HWND_NULL				(0)

// *******************************************************************
//  ͼ�궨��
// *******************************************************************
#define ICON_LEFT		0x00
#define ICON_RIGHT		0x01
#define ICON_UP			0x02
#define ICON_DOWN		0x03
#define ICON_PAGEUP		0x04
#define ICON_PAGEDOWN	0x05
#define ICON_SPEAK		0x06
#define ICON_NUM		0x07

#define ICON_LOCK		(0x80+0x18)
#define ICON_SHIFT		(0x80+0x19)
#define ICON_CAPS		(0x80+0x1a)
#define ICON_ALARM		(0x80+0x1b)

#define ICON_BAT0		(0x80+0x1c)
#define ICON_BAT1		(0x80+0x1d)
#define ICON_BAT2		(0x80+0x1e)
#define ICON_BAT3		(0x80+0x1f)

// ָʾICON
#define ICON_IND0		0x08
#define ICON_IND1		0x09
#define ICON_IND2		0x0a
#define ICON_IND3		0x0b
#define ICON_IND4		0x0c
#define ICON_IND5		0x0d
#define ICON_IND6		0x0e
#define ICON_IND7		0x0f
#define ICON_IND8		0x10
#define ICON_IND9		0x11
#define ICON_IND10		0x12
#define ICON_IND11		0x13
#define ICON_IND12		0x14
#define ICON_IND13		0x15
#define ICON_IND14		0x16
#define ICON_IND15		0x17
#define ICON_IND16		0x1c
#define ICON_IND17		0x1d
#define ICON_IND18		0x1e
#define ICON_IND19		0x1f

// �Ǻ�ICON
#define ICON_STAR0		ICON_IND17
#define ICON_STAR1		ICON_IND18
#define ICON_STAR2		ICON_IND19

// *******************************************************************
//  ����ʶ����
// *******************************************************************
#define TOUCH_SMART_CLICK		1
#define TOUCH_SMART_PUSH		2
#define TOUCH_SMART_SWIPE		3
#define TOUCH_SMART_PRESS		4
#define TOUCH_SMART_DRAG		5
#define TOUCH_SMART_ROTATE		6

#define	SMART_CLICK				1		//����ʶ�𣬵���
#define SMART_DOUBLE_CLICK		2		//����ʶ��˫��
#define SMART_PUSH				3		//����ʶ�𣬰���
#define SMART_DRAG				4		//����ʶ���϶�
#define SMART_PRESS				5		//����ʶ��press
#define SMART_MOVE_LEFT			6		//����ʶ������
#define SMART_MOVE_RIGHT		7		//����ʶ������
#define SMART_MOVE_UP			8		//����ʶ������
#define SMART_MOVE_DOWN			9		//����ʶ������
#define SMART_MOVE_RIGHT_UP		10		//����ʶ������
#define SMART_MOVE_LEFT_UP		11		//����ʶ������
#define SMART_MOVE_RIGHT_DOWN	12		//����ʶ������
#define SMART_MOVE_LEFT_DOWN	13		//����ʶ������
#define SMART_MOVE_ROTE0		14		//����ʶ��˳ʱ����ת
#define SMART_MOVE_ROTE1		15		//����ʶ����ʱ����ת

// *******************************************************************
//  USB ����ʶ����
// *******************************************************************
#define LGET_MOUSE_POSX(n) ( n & 0xffff )
#define LGET_MOUSE_POSY(n) ( ( n >> 16 ) & 0xffff )

#define LGET_MOUSE_BUTTON(n)		( n & 0x03 )
#define LGET_MOUSE_LEFT_BUTTON(n)	( n & 0x01 )
#define LGET_MOUSE_RIGHT_BUTTON(n)	( (n >> 1) & 0x01 )
#define LGET_MOUSE_MIDDLE_BUTTON(n) ( (n >> 2) & 0x01 )
#define LGET_MOUSE_OR_TOUCH(n)		( (n >> 3) & 0x01 )
#define LGET_MOUSE_TYPE(n)			( (n >> 4) & 0x0f )
#define LGET_MOUSE_WHEEL_OFFSET(n)	( n>>16 )

#define USB_MOUSE_NULL					0
#define USB_MOUSE_LEFT_UP				1
#define USB_MOUSE_LEFT_DOWN				2
#define USB_MOUSE_RIGHT_UP				3
#define USB_MOUSE_RIGHT_DOWN			4
#define USB_MOUSE_MIDDLE_UP				5
#define USB_MOUSE_MIDDLE_DOWN			6
#define USB_MOUSE_LEFT_CLICK			7
#define USB_MOUSE_RIGHT_CLICK			8
#define USB_MOUSE_MIDDLE_CLICK			9
#define USB_MOUSE_LEFT_DOUBLE_CLICK		10
#define USB_MOUSE_RIGHT_DOUBLE_CLICK	11
#define USB_MOUSE_MIDDLE_DOUBLE_CLICK	12
#define USB_MOUSE_ENTRY					13
#define USB_MOUSE_LEAVE					14
#define USB_MOUSE_HOVER					15
#define USB_MOUSE_MOVE					16
#define USB_MOUSE_WHEEL					17

#endif // _WIN_TYPE_H
