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

#ifndef __USB_H
#define	__USB_H


typedef enum _USB_ENDPOINT_TYPE
{
	ENDPOINT_TYPE_CONTROL,
	/* Typically used to configure a device when attached to the host.
	 * It may also be used for other device specific purposes, including
	 * control of other pipes on the device.
	 */
	ENDPOINT_TYPE_ISOCHRONOUS,
	/* Typically used for applications which need guaranteed speed.
	 * Isochronous transfer is fast but with possible data loss. A typical
	 * use is audio data which requires a constant data rate.
	 */
	ENDPOINT_TYPE_BULK,
	/* Typically used by devices that generate or consume data in relatively
	 * large and bursty quantities. Bulk transfer has wide dynamic latitude
	 * in transmission constraints. It can use all remaining available bandwidth,
	 * but with no guarantees on bandwidth or latency. Since the USB bus is
	 * normally not very busy, there is typically 90% or more of the bandwidth
	 * available for USB transfers.
	 */
	ENDPOINT_TYPE_INTERRUPT
	/* Typically used by devices that need guaranteed quick responses
	 * (bounded latency).
	 */
}USB_ENDPOINT_TYPE;


typedef enum _USB_STANDARD_REQUEST_CODE
{
	GET_STATUS,
	CLEAR_FEATURE,
	SET_FEATURE = 3,
	SET_ADDRESS = 5,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	GET_CONFIGURATION,
	SET_CONFIGURATION,
	GET_INTERFACE,
	SET_INTERFACE,
	SYNCH_FRAME
}USB_STANDARD_REQUEST_CODE;


typedef enum _USB_DESCRIPTOR_TYPE
{
	DEVICE_DESCRIPTOR = 1,		//设备描述符
	CONFIGURATION_DESCRIPTOR,	//配置描述符
	STRING_DESCRIPTOR,			//字符串描述符
	INTERFACE_DESCRIPTOR,		//接口描述符
	ENDPOINT_DESCRIPTOR,		//端点描述符
	DEVICE_QUALIFIER_DESCRIPTOR,	//设备限定描述符
	OTHER_SPEED_CONFIGURATION_DESCRIPTOR,	//其它速率配置描述符
	INTERFACE_POWER1_DESCRIPTOR		//接口电源描述符
}USB_DESCRIPTOR_TYPE;


typedef enum _USB_FEATURE_SELECTOR
{
	ENDPOINT_HALT,
	DEVICE_REMOTE_WAKEUP,
	TEST_MODE
}USB_FEATURE_SELECTOR;


typedef enum _USB_CLASS_CODE
{
	CLASS_DEVICE,
	CLASS_AUDIO,
	CLASS_COMM_AND_CDC_CONTROL,
	CLASS_HID,
	CLASS_PHYSICAL = 0x05,
	CLASS_STILL_IMAGING,
	CLASS_PRINTER,
	CLASS_MASS_STORAGE,
	CLASS_HUB,
	CLASS_CDC_DATA,
	CLASS_SMART_CARD,
	CLASS_CONTENT_SECURITY = 0x0d,
	CLASS_VIDEO,
	CLASS_DIAGNOSTIC_DEVICE = 0xdc,
	CLASS_WIRELESS_CONTROLLER = 0xe0,
	CLASS_MISCELLANEOUS = 0xef,
	CLASS_APP_SPECIFIC = 0xfe,
	CLASS_VENDOR_SPECIFIC = 0xff
}USB_CLASS_CODE;


typedef struct 
{
	BYTE bmRequestType;
	BYTE bRequest;
	WORD wValue;
	WORD wIndex;
	WORD wLength;
} __attribute__ ((packed)) USB_DEVICE_REQUEST;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdUSB;
	BYTE bDeviceClass;
	BYTE bDeviceSubClass;
	BYTE bDeviceProtocol;
	BYTE bMaxPacketSize0;
	WORD idVendor;
	WORD idProduct;
	WORD bcdDevice;
	BYTE iManufacturer;
	BYTE iProduct;
	BYTE iSerialNumber;
	BYTE bNumConfigurations;
} __attribute__ ((packed)) USB_DEVICE_DESC;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdUSB;
	BYTE bDeviceClass;
	BYTE bDeviceSubClass;
	BYTE bDeviceProtocol;
	BYTE bMaxPacketSize0;
	BYTE bNumConfigurations;
	BYTE bReserved;
} __attribute__ ((packed)) USB_DEVICE_QUALIFIER_DESC;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD wTotalLength;
	BYTE bNumInterfaces;
	BYTE bConfigurationValue;
	BYTE iConfiguration;
	BYTE bmAttributes;
	BYTE MaxPower;
} __attribute__ ((packed)) USB_CONFIG_DESC;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD wTotalLength;
	BYTE bNumInterfaces;
	BYTE bConfigurationValue;
	BYTE iConfiguration;
	BYTE bmAttributes;
	BYTE bMaxPower;
} __attribute__ ((packed)) USB_OTHER_SPEED_CONFIG_DESC;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	BYTE bInterfaceNumber;
	BYTE bAlternateSetting;
	BYTE bNumEndpoints;
	BYTE bInterfaceClass;
	BYTE bInterfaceSubClass;
	BYTE bInterfaceProtocol;
	BYTE iInterface;
} __attribute__ ((packed)) USB_INTERFACE_DESC;


typedef struct 
{
	BYTE bLegth;
	BYTE bDescriptorType;
	BYTE bEndpointAddress;
	BYTE bmAttributes;
	WORD wMaxPacketSize;
	BYTE bInterval;
} __attribute__ ((packed)) USB_ENDPOINT_DESC;


typedef struct 
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD SomeDesriptor[1];
} __attribute__ ((packed)) USB_STRING_DESC;

#endif // !defined(__USB_H)

