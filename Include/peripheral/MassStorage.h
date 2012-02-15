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
//  V0.1    2009-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef __MASS_STRORAGE_H
#define	__MASS_STRORAGE_H

#define RECEIVE_CBW         0
#define RECEIVE_DATA        1
#define SENDING_CSW         2
#define SEND_DATA           3
#define RECEIVE_SERIES_DATA 4
#define SEND_SERIES_DATA    5
#define SENDED_CSW          6

//  USB mass storage class subclasses
enum UMASS_SUBCLASS
{
	MASS_SUBCLASS_RBC = 1, 	// Flash
	MASS_SUBCLASS_8020, 	// CD-ROM
	MASS_SUBCLASS_QIC, 		// Tape
	MASS_SUBCLASS_UFI, 		// Floppy disk device
	MASS_SUBCLASS_8070, 	// Floppy disk device
	MASS_SUBCLASS_SCSI		// Any device with a SCSI-defined command set
};

// USB mass storage class protocols
enum UMASS_PROTOCOL
{
	MASS_PROTOCOL_CBI, 			// Command/Bulk/Interrupt
	MASS_PROTOCOL_CBI_CCI, 		// Command/Bulk/Interrupt
	MASS_PROTOCOL_BULK = 0x50	// Bulk-Only transport
};


typedef struct _CONFIG_DESC
{
	USB_CONFIG_DESC    ConfigDesc;
	USB_INTERFACE_DESC InterfaceDesc;
	USB_ENDPOINT_DESC  EndpointDesc[2];
}__attribute__((packed)) CONFIG_DESC;

// Command Block Wrapper (CBW)
#define	CBWSIGNATURE	0x43425355		// "CBSU"
#define	CBWFLAGS_OUT	0x00			// HOST-to-DEVICE
#define	CBWFLAGS_IN		0x80			// DEVICE-to-HOST
#define	CBWCDBLENGTH	16


typedef struct
{
	DWORD dCBWSignature;
	DWORD dCBWTag;
	int dCBWDataXferLength;
	BYTE bmCBWFlags;
	// The bits of this field are defined as follows:
	//     Bit 7 Direction - the device shall ignore this bit if the
	//                       dCBWDataTransferLength zero, otherwise:
	//         0 = Data-Out from host to the device, 
	//         1 = Data-In from the device to the host.
	//     Bit 6 Obsolete. The host shall set this bit to zero.
	//     Bits 5..0 Reserved - the host shall set these bits to zero.
	// 
	BYTE bCBWLUN : 4, 
	   reserved0 : 4;

	BYTE bCBWCBLength : 5, 
	   reserved1    : 3;
	BYTE CBWCB[CBWCDBLENGTH];
} __attribute__((packed)) CBW;


// Command Status Wrapper (CSW)
#define	CSWSIGNATURE					0x53425355	// "SBSU"
#define	CSWSIGNATURE_IMAGINATION_DBX1	0x43425355	// "CBSU"
#define	CSWSIGNATURE_OLYMPUS_C1			0x55425355	// "UBSU"

#define	CSWSTATUS_GOOD				0x0
#define	CSWSTATUS_FAILED			0x1
#define	CSWSTATUS_PHASE_ERR			0x2

typedef struct
{
	DWORD dCSWSignature;
	DWORD dCSWTag;
	DWORD dCSWDataResidue;
	BYTE bCSWStatus;
	// 00h Command Passed ("good status")
	// 01h Command Failed
	// 02h Phase Error
	// 03h and 04h Reserved (Obsolete)
	// 05h to FFh Reserved
}__attribute__((packed)) CSW;


// Required UFI Commands
#define	UFI_FORMAT_UNIT				0x04	// output
#define	UFI_INQUIRY					0x12	// input
#define	UFI_MODE_SELECT				0x55	// output
#define	UFI_MODE_SENSE_6			0x1A	// input
#define	UFI_MODE_SENSE_10			0x5A	// input
#define	UFI_PREVENT_MEDIUM_REMOVAL	0x1E
#define	UFI_READ_10					0x28	// input
#define	UFI_READ_12					0xA8	// input
#define	UFI_READ_CAPACITY			0x25	// input
#define	UFI_READ_FORMAT_CAPACITY	0x23	// input
#define	UFI_REQUEST_SENSE			0x03	// input
#define	UFI_REZERO_UNIT				0x01
#define	UFI_SEEK_10					0x2B
#define	UFI_SEND_DIAGNOSTIC			0x1D
#define	UFI_START_UNIT				0x1B
#define	UFI_TEST_UNIT_READY			0x00
#define	UFI_VERIFY					0x2F
#define	UFI_WRITE_10				0x2A	// output
#define	UFI_WRITE_12				0xAA	// output
#define	UFI_WRITE_AND_VERIFY		0x2E	// output
#define	UFI_ALLOW_MEDIUM_REMOVAL	UFI_PREVENT_MEDIUM_REMOVAL
#define	UFI_STOP_UNIT				UFI_START_UNIT
#define UFI_READ_TOC			0x43

#define ATAPI_GET_CONFIGURATION		0x46
#define ATAPI_GET_EVENT_STATUS_NOTIFICATION 0x4A
#define ATAPI_READ_DISK_INFORMATION	0x51

#define MAC_READ_CD					0xbe
#define MAC_SET_CD_SPEED			0xbb

typedef struct _CAPACITY_DATA 
{
	DWORD Blocks;
	DWORD BlockLen;   
}CAPACITY_DATA;
typedef struct _READ_FORMAT_CAPACITY_DATA 
{
	BYTE Reserve1[3];
	BYTE CapacityListLen;
	CAPACITY_DATA CurMaxCapacity;
	CAPACITY_DATA CapacityData[30];
}READ_FORMAT_CAPACITY_DATA;


#define WAIT_RECEIVECBW_FINISH		0
#define WAIT_SENDCSW_FINISH   		1
#define WAIT_RECEIVE_FINISH   		2
#define WAIT_SEND_FINISH      		3


#define TRANSFER_CONTROL		0
#define TRANSFER_BULK_TX		1
#define TRANSFER_BULK_RX		2


#endif //__MASS_STRORAGE_H

