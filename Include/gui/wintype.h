//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	            GUI头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
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
	BYTE Type;			// 图片类型，用于扩展，缺省为0
	BYTE BPP;			// 每象素位数。如果最高位为1，表示为彩色图片
	WORD BPL;			// 图片每行数据字节数
	WORD XSize;			// 图片宽度
	WORD YSize;			// 图片高度
	DWORD Trans;		// 图片透明色
	const BYTE* pdata;	// 图片数据指针
}DIB;
typedef DIB *PDIB;

typedef struct
{
	DWORD MsgId;						// 消息类型
	struct _WND_OBJECT *hWnd;			// 消息目的窗口
	struct _WND_OBJECT *hWndSrc;		// 消息源窗口
	union								// 消息参数数据结构
	{
		void* p;
		DWORD v;
		//支持多点触摸
#if defined(CONFIG_MULT_TOUCH) && defined(CONFIG_MAX_POINT)
		// 点依次存放，最后一个空间存放手势等信息
		DWORD n[CONFIG_MAX_POINT + 1];
#endif
	}Data;
	DWORD Param;							// 附加参数
}MESSAGE;
typedef MESSAGE *PMESSAGE;
#define TPMX(msg, i) ((msg)->data.n[i] & 0xffff)
#define TPMY(msg, i) ((msg)->data.n[i] >>16)

// SendMsg使用的消息结构体，增加应答时使用的配对号
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
	DWORD Stamp;						// 消息时间戳
}WINMSG_QUEUE;
typedef WINMSG_QUEUE* PWINMSG_QUEUE;

typedef struct _WINMSG_QUEUEEX
{
	LIST Link;
	MESSAGEEX Message;
	DWORD Stamp;						// 消息时间戳
	void *Context;
}WINMSG_QUEUEEX;
typedef WINMSG_QUEUEEX* PWINMSG_QUEUEEX;

typedef struct _WINMSG_QUEUES
{
	struct _WND_OBJECT *hCurWin;	// 当前消息处理窗体
	HANDLE QEvent;					// 消息触发事件
	HANDLE hMutex;					// 消息结构体互斥量
	LIST SendQ;						// 窗体SEND消息队列
	LIST ReplyQ;					// 窗体REPLY消息队列
	LIST PostQ;						// 窗体POST消息队列
	LIST QuitQ;						// 窗体退出消息队列
#if defined(CONFIG_TOUCH_ENABLE)
	LIST TouchQ;					// 窗体触摸屏消息队列
#endif
#if defined(CONFIG_MOUSE_ENABLE)
	LIST MouseQ;					// 窗体触摸屏消息队列
#endif
	LIST KeyQ;						// 窗体按键消息队列
	LIST PaintQ;					// 窗体重绘消息队列
	LIST TimerQ;					// 窗体消息队列
	LIST FastQ;						// 窗体快速处理消息队列
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
	KERNEL_OBJECT Header;			// 内核对象头信息
	HANDLE pTask;					// 窗体所在的进程
	HANDLE pThread;					// 窗体所在的线程
	
	TREE Herit;						// 继承关系树
	TREE Focus;						// 聚焦关系树
	TREE Layer;						// 位置关系树
	
	RECT AbsRect;  					// 窗口矩形(相对屏幕坐标的绝对坐标)
	RECT HoldRect;					// 和所有祖先窗体剪切后的矩形
	RECT InvalidRect;				// 窗口无效矩形(相对屏幕坐标)
	RECT ClipRect;					// 用于剪切计算的矩形

	RECT UsrRect;					// 窗口用户矩形(旋转后的屏幕坐标)
	int Angle;						// 窗口旋转角度

	int Refresh;					// 当前窗体刷新后进行LCD刷新操作

	DWORD Id;						// 窗口ID号
	DWORD Status;					// 窗口状态标志
#ifdef CONFIG_MAINLCD_ICON
	DWORD Icon;						// 窗口ICON标识
#endif
	DWORD hIme;						// 窗口对应输入法窗体
	
	// 窗口挂接消息队列
	PWINMSG_QUEUES pQueues;
	WIN_CALLBACK cb;				// 窗口回调函数
	
	// 窗口对应的图形设备
	LCD_DC SysDc;
	PVOID UsrDc;					// 用户态下的DC指针
	
	// MXU状态
#if defined(CONFIG_ARCH_XBURST) && !defined(WIN32) && (CONFIG_MAINLCD_BPP == 32)
	int MxuCR;
#endif	
    
    DWORD Time;                       // 窗体创建的时间OS TICKS
    BYTE Painted;                   //已经paint过的标志
	// 窗口用户数据块指针
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
	BYTE *pPointBuf;		//轨迹保存数据
	int nPointPos;						//轨迹保存的当前位置
	struct _TOUCH_SMART_POINT* pNext;	//下一个链表
}TOUCH_SMART_POINT;
typedef TOUCH_SMART_POINT *PTOUCH_SMART_POINT;

typedef struct _TOUCH_SMART
{
	PTOUCH_SMART_POINT	pPointHead;	//轨迹保存链表
	DWORD nPointIdx;				//当前保存轨迹点
	int	nTouchMsg;					//计算出手势识别

	int nStartX,nStartY;			//起始点坐标
	int nEndX,nEndY;				//终止点坐标

	DWORD nCurTime;					//当前点的时间片
	DWORD nForTime;					//上一个时间点的时间片
	DWORD nPressTime;				//动作时间
	DWORD nForClickTime;			//上一个CLICK的时间

	BYTE nForMsg;					//上一个动作ID
	int  nForX,nForY;				//上一个动作的位置

	BYTE SmartCountBuf[9];			//属性BUF,用来计算手势状态（单位个数）
	BYTE SmartCountSat[9];			//属性BUF,用来计算手势状态（单位百分比）

	DWORD nClickCount;				//click的个数
	DWORD nPressCount;				//push的个数
}TOUCH_SMART;
typedef TOUCH_SMART *PTOUCH_SMART;

typedef struct _MOUSE_MESSAGE_INFO
{
	int		type;				// 当前消息类型
	int		button;				// 当前鼠标按键属性
	POINT	point;				// 当前鼠标位置(用来得到当前鼠标坐标)
	POINT	Press_point;		// 第一次按下的位置(用来计算，比如click时，第一次按下的坐标)

	DWORD	click_time;			// click消息中的click时间(按下到弹起的时间)
	DWORD	click_count;		// click一共持续的次数

	DWORD	double_click_time;	// 2次click之间的时间
	POINT	double_click_size;	// 2次click之间的距离(有正，负值)
	
	int		WheelOffset;		// 中间滚轮的转动距离
	HWND	hwnd;				// 需要发送的窗口
}MOUSE_MESSAGE_INFO;
typedef MOUSE_MESSAGE_INFO *PMOUSE_MESSAGE_INFO;

typedef struct _MOUSE_SHAPE
{
	int		nMouseShapeCount;		//鼠标形状的个数
	int		nMouseShapeWidth;		//鼠标的宽度
	int		nMouseShapeHeight;		//鼠标的高度
	int		nMouseShapeTime;		//鼠标变化的间隔
	int		nMouseInitShape;		//鼠标初始化的静态图片ID
	void*	pMouseShapeBuf;			//鼠标形状缓冲
}MOUSE_SHAPE;
typedef MOUSE_SHAPE *PMOUSE_SHAPE;

// *******************************************************************
//   虚拟按键常量定义
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
//   消息常量定义
// *******************************************************************
#define WM_PAINT				1	// 窗口重绘消息
#define WM_CREATE				3	// 窗口创建消息(窗口创建时发送)
#define WM_DELETE				4	// 窗口删除消息(窗口退出前发送)
#define WM_SIZE					6	// 窗口大小改变时发送
#define WM_MOVE					7	// 窗口移动时发送
#define WM_SHOW					8	// 向窗口发送显示的消息(无需用户处理)
#define WM_HIDE					9	// 向窗口发送隐藏的消息(无需用户处理)
#define WM_NOTIFY				10	// 控件向父窗口发送提示信息的消息
#define	WM_TIMER				11	// 定时器消息，定时器打开时发送
#define WM_TERMINATE_AP			12	// 任务被强制结束消息
#define WM_LOAD					13	// 窗口创建后的第一个消息
#define WM_REPAINT				14	// 为加速而使用的一个特殊的PAINT事件
#define WM_ROTATE				15	// 窗口被要求旋转消息
#define WM_QUIT					16	// 窗体被请求退出
#define WM_QUIT_OK				17  // 窗体成功退出
#define WM_QUIT_WAIT			18	// 窗体请求将延迟
#define WM_QUIT_CANCEL			19	// 窗体请求退出被否定
#define WM_CHANGE_TV			20	// 窗口切换到TV-OUT模式
#define WM_CHANGE_LCD			21	// 窗口切换到LCD-OUT模式
#define WM_GRID_IN				22	// 连接到GRID
#define WM_GRID_OUT				23	// 断开GRID
#define WM_HEADPHONE_IN			24	// 耳机插入
#define WM_HEADPHONE_OUT		25	// 耳机拔出
#define WM_TOUCH_SMART			26	// 手势识别接收

#define WM_TOUCH_DOWN			40	// 窗口触摸屏消息(笔按下消息)
#define WM_TOUCH_UP				41	// 窗口触摸屏消息(笔弹起消息)
#define WM_TOUCH_PRESS			42	// 窗口触摸屏消息(笔按住不放消息，定时发送)
#define WM_TOUCH				WM_TOUCH_DOWN
#define WM_TPDOWN				WM_TOUCH_DOWN
#define WM_TPUP					WM_TOUCH_UP
#define WM_TPPRESS				WM_TOUCH_PRESS
#define WM_TPMOVE				43
#define WM_TPGESTURE			44	//手势识别
#define WM_TPCLICK				45	//窗口触摸屏消息(触摸屏点击)

#define WM_KEYDOWN				50	// 窗口按键消息(按键按下时发送，只有聚焦窗口才接收此消息)
#define WM_KEYUP				51	// 窗口按键消息(按键弹起时发送，只有聚焦窗口才接收此消息)
#define WM_KEYPRESS				52	// 窗口按键消息(按键按住不放时发送，定时发送。只有聚焦窗口才接收此消息)
#define WM_KEY					WM_KEYDOWN
#define	WM_CHAR					53	// 输入字符消息(输入法使用时存在)
#define WM_TRACE_START			54	// 手写输入开始事件
#define WM_TRACE_END			55	// 手写输入结束事件

#define WM_MOUSE_ATTACH			60	// 与鼠标有关的事件
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

#define WM_GRID_PEN				90	// 点读机网格笔按下
#define WM_GRID_PENDOWN			90	// 点读机网格笔按下
#define WM_GRID_PENPRESS		91	// 点读机网格笔按住
#define WM_GRID_PENUP			92	// 点读机网格笔弹起

#define WM_GET_CLIENT_RECT		100	// 获取客户区的相对坐标值
#define WM_GET_CLIENT_RECT_ABS	101	// 获取客户区的绝对坐标
#define WM_GET_INSIDE_RECT		102	// 获取窗口内部矩形
#define WM_GETORG				104	// 获取窗口左顶点坐标值
#define WM_GET_ID				105	// 获取窗口ID号
#define WM_GET_CLIENT_WINDOW	106	// 获取客户区窗口句柄，缺省值为主窗口
#define WM_SET_ENABLE			107	// 禁止或者使能窗口时发送的消息
#define WM_SET_FOCUS			110	// 设置窗口焦点消息，窗口获得或者失去焦点时发送
#define WM_GET_HAS_FOCUS		113	// 检查当前窗口是否获取焦点
#define WM_GET_BKCOLOR			114	// 获取窗口背景颜色消息
#define WM_GET_FOCUSSED_CHILD	115	// 获取聚焦子窗口句柄(对话框使用)
#define WM_INIT_DIALOG			116	// 对话框初始化消息(对话框使用)
#define WM_GET_SBAR				117 // 获取应用程序窗口状态条句柄消息
#define WM_LOST_FOCUS			118	// 某窗口失去焦点

#define WM_USB_LINK				200	// USB联机开始
#define WM_USB_UNLINK			201	// USB联机结束
#define WM_POWER_OFF			220
#define WM_POWER_ON				221
#define WM_SD_CHANGE			240
#define WM_SD_INSERT			241
#define WM_SD_REMOVE			242
#define WM_SD_UNKNOWN			243
#define WM_LOW_BAT				260	// 低电压事件
#define WM_CHARGE_DONE			261	// 电池充满,充电完成
#define WM_CHARGE_UNDO			262	// 充电电缆拔出，充电退出
#define WM_CHARGE_RUN			263 // 充电进行中
#define WM_CHARGE_STOP			264	// 充电停止
#define WM_CHARGE_DO			265	// 开始充电
#define WM_ALARM				270	// 闹铃事件
#define WM_AP_CREATE			280	// 应用程序打开
#define WM_AP_DESTROY			281	// 应用程序关闭
#define WM_AP_POWOFF			282	// 关机提示应用程序打开事件
#define WM_AP_POWON				283	// 开机提示应用程序打开事件
#define WM_AP_AUTOFF			284	// 自动关机提示应用程序打开事件

#define WM_FILE_MODIFY			300	// 文件被修改

#define USBM_WRITE_SECTOR		400	// USB主机写扇区消息(USB任务专业)
#define USBM_READ_SECTOR		401	// USB主机读扇区消息(USB任务专业)
#define USBM_DOWNBIOS			402	// USB主机发起下载BIOS联机处理
#define USBM_DOWNSYS			403	// USB主机发起下载系统程序联机处理
#define USBM_DOWNROM			404	// USB主机发起下载库映象文件联机处理
#define USBM_DOWNFAT			405	// USB主机发起下载FAT文件系统映象联机处理
#define USBM_DOWNEND			406	// USB主机发起下载完毕
#define USBM_KILL				407	// USB主机发起取消联机处理
#define USBM_ERASEBIOS			410	// USB主机发起擦除BIOS事件
#define USBM_ERASESYS			411	// USB主机发起擦除系统事件
#define USBM_ERASEROM			412	// USB主机发起擦除库文件事件
#define USBM_ERASEFAT			413	// USB主机发起擦除FAT文件系统
#define USBM_CRCBIOS			414	// USB主机发起校验BIOS事件
#define USBM_CRCSYS				415	// USB主机发起校验系统事件
#define USBM_CRCROM				416	// USB主机发起校验库文件事件

#define WM_LOW_BAT_POFF				500	// 低电压事件, 需要马上关机

#define WM_AUTO_TEST_RECORD_START	550
#define WM_AUTO_TEST_PLAY_START		551
#define WM_AUTO_TEST_RECORD_END		552
#define WM_AUTO_TEST_PLAY_END		553

#define WM_TERMINATE			1001	// 用于AP间的进程控制(一个AP要求另外一个AP结束)
#define	WM_USER					2000

// *******************************************************************
//   (BGR)颜色常量定义
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
//   GDI有关常量定义
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
//   窗口状态有关常量
// *******************************************************************
// 窗口状态标志
#define WS_INVALID				(1<<0)	// 当前窗口是否无效(等待PAINT事件)
#define WS_TRANS				(1<<1)	// 窗口是否为透明窗口
#define WS_ENABLE				(1<<2)	// 当前窗口是否被允许
#define WS_ENABLE_MASK			(1<<3)	// 当前窗口是否被允许屏蔽位
#define WS_SHOW					(1<<4)	// 当前窗口是否可见
#define WS_SHOW_MASK			(1<<5)	// 当前窗口可见标志屏蔽位
#define WS_FOCUS				(1<<6)	// 当前窗口是否获得焦点
#define WS_FOCUS_MASK			(1<<7)	// 当前窗口是否禁止接受焦点
#define WS_GETDC				(1<<8)	// 当前窗体已经获取DC设备                        	
#define WS_GRAY					(1<<9)	// 当前窗体使用灰度显示
#define WS_TOUCH_IGNORE			(1<<10)	// 当前窗口不获得点触消息

#define WS_MOUSE_PROPERTIES		(1<<11)	// 当前窗口对应鼠标的属性
										// enable可以接受 WM_MOUSE_HOVER,WM_MOUSE_LEAVE,WM_MOUSE_ENTER 消息
										// disable不会接收到 WM_MOUSE_HOVER,WM_MOUSE_LEAVE,WM_MOUSE_ENTER 消息

// 主窗体状态相关标志，以下标志在创建时设置，以后不能改变                        	
#define WS_MAIN					(1<<12)	// 主窗体(其子功能窗口获得焦点，则主窗口获得焦点)
#define WS_MODAL				(1<<13)	// 模态窗体
#define WS_EXFOCUS				(1<<14)	// 主窗体焦点和父窗体联动(MDI窗体使用)
#define WS_EXRECT				(1<<15)	// 主窗体显示范围超出父窗口范围
#define WS_MSGQ					(1<<16)	// 主窗体是否存在独立的消息队列
#define WS_FOCUS_ALWAYS				(1<<17)	// 在窗体什么周期内永远获得焦点
#define WS_SAVE_UNDER				(1<<18)	// 保存透明窗体底层数据

// 用户窗体置顶窗体级别
#define WS_ULAYER				(0x03<<20)
#define WS_SLAYER				(0x07<<22)
#define WS_LAYER				(0x1f<<20)	// 窗体置顶级别

// 应用程序内窗体层次级别
#define WS_ULAYER_SHIFT			(20)
#define WS_ULAYER0				(0<<WS_ULAYER_SHIFT)
#define WS_ULAYER1				(1<<WS_ULAYER_SHIFT)
#define WS_ULAYER2				(2<<WS_ULAYER_SHIFT)
#define WS_ULAYER_IME			(3<<WS_ULAYER_SHIFT)	// 输入法占用

// 应用程序间窗体层次级别
#define WS_SLAYER_SHIFT			(22)
#define WS_SLAYER_APP0			(0<<WS_SLAYER_SHIFT)	// 1: 普通应用程序
#define WS_SLAYER_APP1			(1<<WS_SLAYER_SHIFT)	// 2: 特殊应用程序1
#define WS_SLAYER_SYS2			(2<<WS_SLAYER_SHIFT)	// 3: 开始栏
#define WS_SLAYER_APP2			(3<<WS_SLAYER_SHIFT)	// 4: 特殊应用程序2
#define WS_SLAYER_SYS0			(4<<WS_SLAYER_SHIFT)	// 5: 任务管理器
#define WS_SLAYER_SYS1			(5<<WS_SLAYER_SHIFT)	// 6: 系统消息
#define WS_SLAYER_DIRECT		(6<<WS_SLAYER_SHIFT)	// 7: 直接写屏程序
#define WS_SLAYER_DESKTOP		(7<<WS_SLAYER_SHIFT)	// 0: 桌面(最底层)

// 窗体旋转特性标志                       	
#define WS_ROTMASK				(3<<28)	// 窗口旋转标记屏蔽位
#define WS_ROT0					(0<<28)	// 窗口顺时针旋转0度
#define WS_ROT90				(1<<28)	// 窗口顺时针旋转90度
#define WS_ROT180				(2<<28)	// 窗口顺时针旋转180度
#define WS_ROT270				(3<<28)	// 窗口顺时针旋转270度


#define COUNTOF(a)				(sizeof(a) / sizeof(a[0]))
#define HWND_NULL				(0)

// *******************************************************************
//  图标定义
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

// 指示ICON
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

// 星号ICON
#define ICON_STAR0		ICON_IND17
#define ICON_STAR1		ICON_IND18
#define ICON_STAR2		ICON_IND19

// *******************************************************************
//  手势识别常量
// *******************************************************************
#define TOUCH_SMART_CLICK		1
#define TOUCH_SMART_PUSH		2
#define TOUCH_SMART_SWIPE		3
#define TOUCH_SMART_PRESS		4
#define TOUCH_SMART_DRAG		5
#define TOUCH_SMART_ROTATE		6

#define	SMART_CLICK				1		//手势识别，单击
#define SMART_DOUBLE_CLICK		2		//手势识别，双击
#define SMART_PUSH				3		//手势识别，按下
#define SMART_DRAG				4		//手势识别，拖动
#define SMART_PRESS				5		//手势识别，press
#define SMART_MOVE_LEFT			6		//手势识别，左移
#define SMART_MOVE_RIGHT		7		//手势识别，右移
#define SMART_MOVE_UP			8		//手势识别，上移
#define SMART_MOVE_DOWN			9		//手势识别，下移
#define SMART_MOVE_RIGHT_UP		10		//手势识别，右上
#define SMART_MOVE_LEFT_UP		11		//手势识别，左上
#define SMART_MOVE_RIGHT_DOWN	12		//手势识别，右下
#define SMART_MOVE_LEFT_DOWN	13		//手势识别，左下
#define SMART_MOVE_ROTE0		14		//手势识别，顺时针旋转
#define SMART_MOVE_ROTE1		15		//手势识别，逆时针旋转

// *******************************************************************
//  USB 鼠标标识常量
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
