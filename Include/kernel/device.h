//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    �ں����ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _DEVICE_H
#define _DEVICE_H

#define DEV_OPEN_READ				(0x01)
#define DEV_OPEN_WRITE				(0x02)
#define DEV_EVENT_SLEEP_ENTER		(0x00)
#define DEV_EVENT_SLEEP_EXIT		(0x01)

#define AUDIO_STATUS_READY			(0)
#define AUDIO_STATUS_PLAY			(1)
#define AUDIO_STATUS_PAUSE			(2)
#define AUDIO_STATUS_STOP			(3)

#define USB_STATUS_DISABLE			(0) // ��ֹ����
#define USB_STATUS_UNLINK			(1)	// ȡ������
#define USB_STATUS_READY			(2) // ����ȷ��
#define USB_STATUS_LINKED			(3) // �Ѿ�����
#define USB_STATUS_CHARGE			(5) // ���ڳ��

#define IOCTL_AUDIO_SAMPRATE_SET	(0x00)
#define IOCTL_AUDIO_SAMPRATE_GET	(0x01)
#define IOCTL_AUDIO_CHANNEL_SET		(0x02)
#define IOCTL_AUDIO_CHANNEL_GET		(0x03)
#define IOCTL_AUDIO_VOLUME_SET		(0x04)
#define IOCTL_AUDIO_VOLUME_GET		(0x05)
#define IOCTL_AUDIO_BLOCKS_SET		(0x06)
#define IOCTL_AUDIO_BLOCKS_GET		(0x07)
#define IOCTL_AUDIO_BLKSIZE_SET		(0x08)
#define IOCTL_AUDIO_BLKSIZE_GET		(0x09)

#define IOCTL_AUDIO_STATUS_GET		(0x10)
#define IOCTL_AUDIO_PLAY			(0x11)
#define IOCTL_AUDIO_PAUSE			(0x12)
#define IOCTL_AUDIO_STOP			(0x13)
#define IOCTL_AUDIO_CHECK_EMPTY		(0x14)
#define IOCTL_AUDIO_CHECK_FULL		(0x15)

#define IOCTL_USB_GET_DISK_INFO		(0x00)
#define IOCTL_USB_STATUS			(0x01)
#define IOCTL_USB_LINK_DISABLE		(0x02)
#define IOCTL_USB_LINK_ENABLE		(0x03)
#define IOCTL_USB_LINK_PERMIT		(0x04)
#define IOCTL_USB_LINK_DENY			(0x05)
#define IOCTL_USB_LINK_AUTO_SET		(0x06)
#define IOCTL_USB_LINK_AUTO_GET		(0x07)
#define IOCTL_USB_LINK_SPEED		(0x08)
#define IOCTL_USB_CABLE_PLUG		(0x09)

#define IOCTL_RTC_GET_DATE			(0x00)
#define IOCTL_RTC_SET_DATE			(0x01)
#define IOCTL_RTC_GET_TIME			(0x02)
#define IOCTL_RTC_SET_TIME			(0x03)
#define IOCTL_RTC_GET_ALARM			(0x04)
#define IOCTL_RTC_SET_ALARM			(0x05)
#define IOCTL_RTC_ALARM_ENABLE		(0x06)
#define IOCTL_RTC_ALARM_DISABLE		(0x07)
#define IOCTL_RTC_GET_VALUE			(0x08)
#define IOCTL_RTC_IS_COLDBOOT       (0x09)
#define IOCTL_RTC_DRVALARM_ENABLE	(0x0A)
#define IOCTL_RTC_DRVALARM_DISABLE	(0x0B)
#define IOCTL_RTC_GET_BOOTTYPE      (0x0C)

#define IOCTL_KEYBOARD_POLL_KEY		(0x00)
#define IOCTL_KEYTONE_SET			(0x01)
#define IOCTL_KEYTONE_GET			(0x02)
#define IOCTL_KEYLIGHT_SET			(0x03)
#define IOCTL_KEYLIGHT_GET			(0x04)

#define IOCTL_LCDC_ON				(0x00)
#define IOCTL_LCDC_OFF				(0x01)
#define IOCTL_LCDC_GET_PROPERTY		(0x02)
#define IOCTL_LCDC_GET_VRAM_ADDR	(0x03)
#define IOCTL_LCDC_CHANGE_MODE		(0x04)
#define IOCTL_LCDC_CACHE_FLUSH		(0x05)
#define IOCTL_LCDC_BACKLIGHT_SET	(0x06)
#define IOCTL_LCDC_BACKLIGHT_GET	(0x07)
#define IOCTL_LCDC_BACKLIGHT_FADE	(0x08)
#define IOCTL_LCDC_TVOUT_SET		(0x09)
#define IOCTL_LCDC_TVOUT_GET		(0x0A)
#define IOCTL_LCDC_TVOUT_CABLE		(0x0B)

#define IOCTL_SD_INIT				(0x00)
#define IOCTL_SD_REINIT				(0x01)
#define IOCTL_SD_GET_CAPACITY		(0x02)
#define IOCTL_SD_GET_STATUS			(0x03)
#define IOCTL_SD_CHECK_VALID		(0x04)
#define IOCTL_SD_OPEN_CARD			(0x05)
#define IOCTL_SD_OPEN_CARD_GET_STATUS	(0x06)
#define IOCTL_SD_OPEN_CARD_POWER_OFF	(0x07)

//For nand test//
#define IOCTL_NAND_BLOCK_ERASE			(0x00)
#define IOCTL_NAND_BLOCK_CAHECK_BAD			(0x01)
#define IOCTL_NAND_BLOCK_MARK_BAD	(0x02)
#define IOCTL_NAND_GET_INFO			(0x03)
#define IOCTL_NAND_GET_PARMA		(0x04)

//��Դ�����豸
#define IOCTL_POWMAN_VOLTAGE		(0x00)
#define IOCTL_POWMAN_BATCHARGE		(0x01)
#define IOCTL_POWMAN_BATFULL		(0x02)
#define IOCTL_POWMAN_AVERAGE_VOLTAGE	(0x03)
#define IOCTL_POWMAN_POWEROFF		(0x04)
#define IOCTL_POWMAN_RESET			(0x05)
#define IOCTL_POWMAN_GET_POWEROFFTIME	(0x06)//��ȡ��ȹػ�ʱ��(��)
#define IOCTL_POWMAN_SET_POWEROFFTIME	(0x07)//������ȹػ�ʱ��(��)

#define IOCTL_TP_CALIBRATION		(0x01)
#define IOCTL_TP_SET_CALIBRATION	(0x02)
#define IOCTL_TP_GET_CALIBRATION	(0x03)
#define IOCTL_TP_DONE_CALIBRATION	(0x04)
#define IOCTL_TP_CLR_CALIBRATION	(0x05)
#define IOCTL_TP_SET_HANDWRITE		(0x06)
#define IOCTL_TP_CLR_HANDWRITE		(0x07)
#define IOCTL_TP_SET_SAMPLE_INTERVAL	(0x08)

#define IOCTL_TOUCH_CAL_STATUS		(0x00)
#define IOCTL_TOUCH_CAL_START		(0x01)
#define IOCTL_TOUCH_CAL_SET			(0x02)
#define IOCTL_TOUCH_CAL_GET			(0x03)
#define IOCTL_TOUCH_CAL_END			(0x04)
#define IOCTL_TOUCH_CAL_OK			(0x05)
#define IOCTL_TOUCH_CAL_RESET		(0x13)
#define IOCTL_TOUCH_SAMPLE_INTERVAL_SET	(0x14)
#define IOCTL_TOUCH_CAL_CLEAR			(0x0f)
#define IOCTL_TOUCH_SET_HANDWRITE		(0x0d)
#define IOCTL_TOUCH_CLR_HANDWRITE		(0x0e)
#define IOCTL_TOUCH_TRACE_CLEAR		(0x06)
#define IOCTL_TOUCH_TRACE_START		(0x07)
#define IOCTL_TOUCH_TRACE_DONE		(0x15)
#define IOCTL_TOUCH_TRACE_WIDTH_SET	(0x08)
#define IOCTL_TOUCH_TRACE_WIDTH_GET	(0x09)
#define IOCTL_TOUCH_TRACE_COLOR_SET	(0x0A)
#define IOCTL_TOUCH_TRACE_COLOR_GET	(0x0B)
#define IOCTL_TOUCH_TRACE_END_TIME	(0x0C)
#define IOCTL_TOUCH_TRACE_RECT		(0x10)
#define IOCTL_TOUCH_TRACE_ENABLE	(0x11)
#define IOCTL_TOUCH_TRACE_DISABLE	(0x12)
#define IOCTL_TOUCH_TRACE_ISENABLE  (0x16)
#define IOCTL_TOUCH_TRACE_ISSTART   (0x17)
#define IOCTL_TOUCH_TRACE_SETMODE   (0x18)//0:�ȴ����ƶ�һ���������Ϊ����д��1������������д
#define IOCTL_TOUCH_TRACE_STARTRECT	(0x19)
#define IOCTL_TOUCH_WINDOW_RECT		(0x1A)//������ӳ�䵽������������Ϊ��������
#define IOCTL_TOUCH_WINDOW_RECT_GET	(0x1B)//��ô�����ӳ�䵽��������


#define IOCTL_BUZZER_ON				(0x00)
#define IOCTL_BUZZER_OFF			(0x01)
#define IOCTL_BUZZER_SN				(0x02)

#define IOCTL_INDICATOR_PALM_IN		(0x01)

#define IOCTL_MEMORY_SEEK			(0)
#define IOCTL_MEMORY_READ			(1)
#define IOCTL_MEMORY_WRITE			(2)

#define IOCTL_BIOS_UPGRADE_START	(0)
#define IOCTL_BIOS_UPGRADE_DONE		(1)
#define IOCTL_SYS_UPGRADE_START		(2)
#define IOCTL_SYS_UPGRADE_DONE		(3)
#define IOCTL_BIOS_CRC_CHECK		(4)
#define IOCTL_SYS_CRC_CHECK			(5)

#define IOCTL_UART_INIT				(1)
#define IOCTL_UART_GET_CTRL_BLOCK	(2)
#define IOCTL_UART_GET_STATUS		(3)
#define IOCTL_UART_CLEAR_STATUS		(4)
#define IOCTL_UART_INQUEUE_SIZE		(5)

#define IOCTL_INAND_NAME			(1)
#define IOCTL_INAND_CHECK_VALID		(2)
#define IOCTL_INAND_REINIT				(0x03)
#define IOCTL_INAND_GET_CAPACITY		(0x04)
#define IOCTL_INAND_OPEN_CARD			(0x05)
#define IOCTL_INAND_OPEN_CARD_GET_STATUS	(0x06)
#define IOCTL_INAND_OPEN_CARD_POWER_OFF	(0x07)
#define IOCTL_INAND_CHECK_PHYSICAL_DATA	(0x08)
#define IOCTL_INAND_CHECK_PHYSICAL_DATA_ECC	(0x09)

#define IOCTL_NAND_NAME				(1)
#define IOCTL_NAND_CHECK_VALID		(2)

#define IOCTL_USBHOST_DEV_STAT		(1)

// IO����״̬����
#define MULTI_IO_NAND				(0)
#define MULTI_IO_LCD				(1)
#define MULTI_IO_SD					(2)

typedef struct
{
	int x;
	int	y;
}TOUCH_POINT;
typedef TOUCH_POINT *PTOUCH_POINT;

typedef struct _TOUCH_EVENT
{
	int	e;
	
	//֧�ֶ�㴥��
#if defined(CONFIG_MULT_TOUCH) && defined(CONFIG_MAX_POINT)
	int pn;//��Ч����
	DWORD info;//���Ƶ���Ϣ��δ���壩
	TOUCH_POINT point[CONFIG_MAX_POINT];
#else
	int x;
	int y;
#endif
    DWORD time;
}TOUCH_EVENT;
typedef TOUCH_EVENT *PTOUCH_EVENT;

typedef struct _MOUSE_EVENT
{
	BYTE lbdown:1;
	BYTE rbdown:1;
	BYTE mbdown:1;
	BYTE reserve:1;
	BYTE xsign:1;
	BYTE ysign:1;
	BYTE xof:1;
	BYTE yof:1;

	WORD  x;
	WORD  y;
	short w;

	DWORD ts;
}MOUSE_EVENT;
typedef MOUSE_EVENT *PMOUSE_EVENT;

// �豸IO���ƽṹ��
typedef struct _DEVIO_CTRL
{
	DWORD Cmd;		//�豸IO��������
	void *InBuff;	//��ȡ��Ϣ�Ļ���
	int InSize;		//��ȡ��Ϣ�Ļ���Ĵ�С
	void *OutBuff;	//������Ϣ�Ļ���
	int OutSize;	//������Ϣ�Ļ���Ĵ�С
	int *RetSize;	//ʵ�ʻ�ȡ��Ϣ�Ĵ�С
}DEVIO_CTRL;
typedef DEVIO_CTRL *PDEVIO_CTRL;

// �豸���������ṹ��
typedef struct _KERNEL_DEVICE_DRV
{
	int (*Init) (void);
	HANDLE (*Open)(int);
	int	(*Close)(HANDLE);
	int	(*Read)	(HANDLE, void *, int *, int);
	int	(*Write)(HANDLE, void *, int *, int);
	int	(*Ioctl)(HANDLE, PDEVIO_CTRL ctl);
	int (*Event)(int);
}KERNEL_DEVICE_DRV;
typedef KERNEL_DEVICE_DRV *PKERNEL_DEVICE_DRV;

// �豸����ṹ��
typedef struct _KERNEL_DEVICE
{
	KERNEL_OBJECT Header;
	char	Name[CONFIG_MAX_DEVNAME];		// �豸����
	LIST	Link;							// �豸��
	int		RefCount;						// �򿪼���
	int		MaxOpens;						// �������򿪴���
	int		Mode;							// �豸�����/дģʽ
	HANDLE	hIoMutex;						// �豸IO���ʻ�����
	PKERNEL_DEVICE_DRV	Devio;				// �豸������
	LIST	OpenList;
}KERNEL_DEVICE;
typedef KERNEL_DEVICE *PKERNEL_DEVICE;

typedef struct _DEVICE_HEADER
{
	KERNEL_OBJECT Header;
	LIST OpenList;
	PKERNEL_DEVICE pKernDev;
	HANDLE	hIoMutex;						// �豸IO���ʻ�����
	HANDLE  hRwMutex;						// �豸��д���ʻ�����
	int Mode;
}DEVICE_HEADER;
typedef DEVICE_HEADER *PDEVICE_HEADER;

void	 DeviceInit(void);
int DeviceBroadcast(int, int);

#if defined(STC_EXP)
HANDLE sDeviceOpen(const char *name, int mode);
int	sDeviceClose(HANDLE hdev);
int	sDeviceRead(HANDLE hdev, void *buf, int *count, int blkno);
int	sDeviceWrite(HANDLE hdev, void *buf, int *count, int blkno);
int	sDeviceIoctl(HANDLE hdev, PDEVIO_CTRL ctl);
#else
HANDLE DeviceOpen(const char *name, int mode);
int	DeviceClose(HANDLE hdev);
int	DeviceRead(HANDLE hdev, void *buf, int *count, int blkno);
int	DeviceWrite(HANDLE hdev, void *buf, int *count, int blkno);
int	DeviceIoctl(HANDLE hdev, PDEVIO_CTRL ctl);
#endif

// ����IO�˿ڻ�ȡ����
int MultiIoInit(void);
int MultiIoLock(int type);
int MultiIoUnlock(int type);
#endif // !_DEVICE_H
