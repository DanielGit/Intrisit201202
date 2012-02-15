//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	      ADPCM音频压缩和解压缩
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2005-10      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#include <config.h>
#include <noahos.h>

#ifndef _DEVICE_H
#define _DEVICE_H

#define DEV_OPEN_READ				(0x01)
#define DEV_OPEN_WRITE				(0x02)

#define AUDIO_STATUS_READY			(0)
#define AUDIO_STATUS_PLAY			(1)
#define AUDIO_STATUS_PAUSE			(2)
#define AUDIO_STATUS_STOP			(3)

#define USB_STATUS_DISABLE			(0) // 禁止联机
#define USB_STATUS_UNLINK			(1)	// 取消联机
#define USB_STATUS_READY			(2) // 等待联机
#define USB_STATUS_LINKED			(3) // 已经联机
#define USB_STATUS_CHARGE			(5) // 正在充电

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

#define IOCTL_POWMAN_VOLTAGE		(0x00)
#define IOCTL_POWMAN_BATCHARGE		(0x01)
#define IOCTL_POWMAN_BATFULL		(0x02)
#define IOCTL_POWMAN_AVERAGE_VOLTAGE	(0x03)
#define IOCTL_POWMAN_POWEROFF		(0x04)
#define IOCTL_POWMAN_RESET			(0x05)
#define IOCTL_POWMAN_GET_POWEROFFTIME	(0x06)//获取深度关机时间(秒)
#define IOCTL_POWMAN_SET_POWEROFFTIME	(0x07)//设置深度关机时间(秒)

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
#define IOCTL_TOUCH_TRACE_SETMODE   (0x18)//0:等待笔移动一定距离才认为是手写，1：立即进入手写
#define IOCTL_TOUCH_TRACE_STARTRECT	(0x19)
#define IOCTL_TOUCH_WINDOW_RECT		(0x1A)//触摸屏映射到窗体区域，区域为绝对坐标
#define IOCTL_TOUCH_WINDOW_RECT_GET	(0x1B)//获得触摸屏映射到窗体区域

#define IOCTL_INDICATOR_PALM_IN		(0x01)

#define IOCTL_BUZZER_ON				(0x00)
#define IOCTL_BUZZER_OFF			(0x01)
#define IOCTL_BUZZER_SN				(0x02)

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

// 设备IO控制结构体
typedef struct _DEVIO_CTRL
{
	DWORD Cmd;
	void *InBuff;
	int InSize;
	void *OutBuff;
	int OutSize;
	int *RetSize;
}DEVIO_CTRL;
typedef DEVIO_CTRL *PDEVIO_CTRL;

#endif // _DEVICE_H


