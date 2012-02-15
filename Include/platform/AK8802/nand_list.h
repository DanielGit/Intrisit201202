/**
 * @file nand_list.h
 * @brief this is the information from nandflash
 *
 * This file provides nandflash basic information,so we can
 * search this information in order software fit hardware quickly
 *
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author Dengjian
 * @MODIFY  zhaojiahuan
 * @DATE    2006-7-17
 * @version 1.0
 * @
 */

//*******************************************************************
//*******************************************************************
#ifndef _CHIP_NFC_AK78xx_
    #define _CHIP_NFC_AK78xx_
    
// DMA
    #define NF_REG_00_DMA_CTRL      (NF_REG_BASE_ADDR + 0xA000)
    #define NF_REG_04_DMA_EXT_MEM       (NF_REG_BASE_ADDR + 0xA004)
    #define NF_REG_08_DMA_FSI1      (NF_REG_BASE_ADDR + 0xA008)
    #define NF_REG_0C_DMA_FSI2      (NF_REG_BASE_ADDR + 0xA00C)
    #define NF_REG_10_W_DAT         (NF_REG_BASE_ADDR + 0xA010)
    #define NF_REG_14_R_DAT         (NF_REG_BASE_ADDR + 0xA014)

// ECC
	#define FLASH_ECC_POSITION_REG0		(0xb002B004)
	#define FLASH_ECC_POSITION_REG1		(0xb002B004+4)
	#define FLASH_ECC_POSITION_REG2		(0xb002B004+8)
	#define FLASH_ECC_POSITION_REG3		(0xb002B004+12)
	#define FLASH_ECC_POSITION_REG4		(0xb002B004+16)
	#define FLASH_ECC_POSITION_REG5		(0xb002B004+20)
	#define FLASH_ECC_POSITION_REG6		(0xb002B004+24)
	#define FLASH_ECC_POSITION_REG7		(0xb002B004+28)
	#define FLASH_ECC_POSITION_REG8		(0xb002B004+32)

    #define NF_REG_80_ECC_CTRL1     (NF_REG_BASE_ADDR + 0xA080) // encode mudule start address
    #define NF_REG_90_ECC_CTRL2     (NF_REG_BASE_ADDR + 0xA090) // decode module start address
    #define NF_REG_94_ECC_REI1      (NF_REG_BASE_ADDR + 0xA094) // decode module start address
    #define NF_REG_98_ECC_REI2      (NF_REG_BASE_ADDR + 0xA098) // decode module start address
    #define NF_REG_9C_ECC_BEI       (NF_REG_BASE_ADDR + 0xA09C) // decode module start address

// NF controller
    #define NF_REG_00_CMD           (NF_REG_BASE_ADDR + 0xA100)
    #define NF_REG_50_RB1           (NF_REG_BASE_ADDR + 0xA150)
    #define NF_REG_54_RB2           (NF_REG_BASE_ADDR + 0xA154)
    #define NF_REG_58_CTRL_STA      (NF_REG_BASE_ADDR + 0xA158)
    #define NF_REG_5C_CMD_LEN       (NF_REG_BASE_ADDR + 0xA15C)
    #define NF_REG_60_DAT_LEN       (NF_REG_BASE_ADDR + 0xA160)
//*******************************************************************
//NF_REG_58_CTRL_STA    0x80000000
    #define BIT_DMA_CMD_DONE        0x80000000
//*******************************************************************
//basic save information
    #define NFC_FIFO_SIZE           512
    #define NFC_LOG_PAGE_SIZE       512
    #define NFC_LOG_SPARE_SIZE      16
    #define NFC_FS_INFO_OFFSET      512//must be a value %4 = 0
    #define NFC_FS_INFO_SIZE        4


/// @cond NANDFLASH_DRV


//******************************************************************************************//
//通过CTRL reg0进行命令压栈的说明如下。（共7个confiuration，1个flag）
//*************reg0~reg19命令的类型如下***********
//********××××××
/*
1×nandflash本身数据总线上传输数据的类型
××××××××××××××××××××××××××××××××××××
a,COMMAND_CYCLES_CONF:表示数据总线上写入的是Nandflash命令；
b，ADDRESS_CYCLES_CONF：表示数据总线上写入的是Nandflash地址信息；
c,WRITE_DATA_CONF:表示数据总线上写入的FIFO中的数据信息，
由于读出数据可能放在不同位置，所以这里有两种读数据命令
d，READ_DATA_CONF：表示数据中总线读数据，读出数据放在FIFO中
e，READ_CMD_CONF这命名好像不太好，我把它改为READ_INFO_CONF
  表示数据总线上读的是状态或是ID信息指令,(也可用于读取小于
  8 byte的数据两）读出数据放在reg20

  另外，如果带上ECC的读，芯片会自动把data信息和文件系统信息放到相
  应的FIFO和寄存器。

2×与nandflash无关，为了引入延时和确定读写预处理等待完成时引入的命令
××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××
f,DELAY_CNT_CONF 由于nandflash不会马上相应命令，大约有3us到50us的延时，
  数据才会准备好，所以引入该命令。延时时间在bit11~bit22中写入

g.WAIT_JUMP_CONF 在Nandflash操作中，除了可以读status 软件查询状态外，
还有一个R/B引脚用来辅助快速确定读写操作是否完成。该命令等待上跳延触发。

上面命令每次任选且只能选一条，不断通过reg0地址压到命令栈中，即可实现对
nandflash的所有操作
如果是最后一条指令，必须打上LAST_CMD_FLAG标志
*/

//////// command sequece configuration define ////////////

/* command cycle's configure:
 *      CMD_END=X, ALE=0, CLE=1, CNT_EN=0,  (BIT[0:3])
 *      REN=0, WEN=1, CMD_EN=1, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=0, RBN_EN=0, CMD_WAIT=0,     (BIT[8:10])
 */
    #define COMMAND_CYCLES_CONF     0x64
/* address cycle's:
 *      CMD_END=X, ALE=1, CLE=0, CNT_EN=0,  (BIT[0:3])
 *      REN=0, WEN=1, CMD_EN=1, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=0, RBN_EN=0, CMD_WAIT=0,     (BIT[8:10])
 */
    #define ADDRESS_CYCLES_CONF     0x62

/*  read data cycle's:
 *      CMD_END=X, ALE=0, CLE=0, CNT_EN=1,  (BIT[0:3])
 *      REN=1, WEN=0, CMD_EN=0, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=1, RBN_EN=0, CMD_WAIT=0,     (BIT[8:10])
 */
    #define READ_DATA_CONF          0x118

/*  write data cycle's:
 *      CMD_END=X, ALE=0, CLE=0, CNT_EN=1,  (BIT[0:3])
 *      REN=0, WEN=1, CMD_EN=0, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=1, RBN_EN=0, CMD_WAIT=0,     (BIT[8:10])
 */
    #define WRITE_DATA_CONF         0x128

/* read command status's:(example: read ID(pass), status(pass), 回读数据比较少的操作?
//告诉芯片现在读取status ID信息，芯片把该信息搬到相应寄存器(8 byte)，而不是FIFO。方便查询Nandflash状态。
 *      CMD_END=X, ALE=0, CLE=0, CNT_EN=1,  (BIT[0:3])
 *      REN=1, WEN=0, CMD_EN=1, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=0, RBN_EN=0, CMD_WAIT=0,     (BIT[8:10])
 */
    #define READ_CMD_CONF           0x58
//rename this command to //@hustdj
    #define READ_INFO_CONF          0x58

//time delay by use this command
/*
 *      CMD_END=0, ALE=0, CLE=0, CNT_EN=0,  (BIT[0:3])
 *      REN=0, WEN=0, CMD_EN=1, STAFF_EN=0, (BIT[4:7])
 *      DAT_EN=0, RBN_EN=0, CMD_WAIT=1,     (BIT[8:10])
 */
    #define DELAY_CNT_CONF                  0x400   // 1<<10

    #define WAIT_JUMP_CONF                  0x200   // 1<<9

// last command's bit0 set to 1
    #define LAST_CMD_FLAG           (1 << 0)
//##################################### end reg0 command sequece configuration define ################




//#######excute comamnd,ctrl reg22 command configuration define ################
//所有的配置定义针对发送命令的方式。
//必须进行片选或片保护、操作模式、以及是否使能的省电模式flag
//片选使用宏NCHIP_SELECT(x)
//同时打该片的写保护，使用宏WRITE_PROTECT(x),
    #define NCHIP_SELECT(x)         (((0x01 << (x)) << 10) | ((1<<(x)) << 15))
    //#define NCHIP_SELECT(x)         (((0x01 << (x)) << 10) | (1 << 15))
    #define WRITE_PROTECT      (1 << 15)

//操作模式为
//正常操作
/*
bit0(save mode) = 0;
bit1-bit8(staff_cont)=0
bit9(watit save mode jump) = 0
bit10-bit13(chip select)=0
bit14(sta_clr)=0
bit15-bit18(write protect)=0
bit19-bit29 reserved =0;
bit30(start control) = 1;
bit31(check statu) = 0;
*/
    #define DEFAULT_GO          ((1 << 30)|(1<<9))

//toshiba操作??
/*
bit0(save mode) = 0;
bit1-bit8(staff_cont)=0xFF ??
bit9(watit save mode jump) = 0
bit10-bit13(chip select)=0
bit14(sta_clr)=0
bit15-bit18(write protect)=0
bit19-bit29 reserved =0;
bit30(start control) = 1;
bit31(check statu) = 0;
*/
    #define TOSHIBA_GO          ((1 << 30)|(0xff << 1))


//省电flag为
    #define SAVE_MODE_FLAG      1
//#######end ctrl reg22 command configuration define ################


//*************ecc encode and decode, ecc reg0 and ecc reg4 *********
//在目前硬件条件下，ecc硬件编码和ecc硬件解码的参数都比较固定
// ECC encode
//this is the demo program
//HAL_WRITE_UINT32(FLASH_ECC_REG0, 0x0019); // software ECC
//HAL_WRITE_UINT32(FLASH_ECC_REG0, 0x0018); // hardware ECC
//ecc encode config for ecc reg0
/*
bit0(add hardware ecc) = 0;
bit1(not clean state)=0
bit2(big endian) = 1
bit3(do ecc handle)=1
bit4(do ecc encode)=1
bit5-bit15(reserve)=0
bit16-bit27(save date count = 528 -1?;
bit28(pass encode) = 1;
*/

//#define ECC_ENCODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|(527<<16)|(1<<28))
//#define ECC_ENCODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|((NFC_FIFO_SIZE+NFC_LOG_SPARE_SIZE-1)<<16)|(1<<28))
//#define ECC_ENCODE_CONFIG     ((1<<3)|(1<<4)|((NFC_FIFO_SIZE+NFC_LOG_SPARE_SIZE-1)<<16)|(1<<28))
//#define ECC_ENCODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|((NFC_FIFO_SIZE-1)<<16)|(1<<28))
//#define ECC_ENCODE_CONFIG     ((1<<3)|(1<<4)|((NFC_FIFO_SIZE-1)<<16)|(1<<28))
    #define ECC_ENCODE_CONFIG       0x0018


//ecc decode config for ecc reg4
//this is the demo code
//HAL_WRITE_UINT32(FLASH_ECC_REG4, 0x0018);
/*
bit0(reserved) = 0;
bit1(not clean state)=0
bit2(big endian) = 1
bit3(do ecc handle)=1
bit4(do ecc decode)=1
bit5-bit16(ecc decode information)=0//ecc correct use this bit
bit17-bit28(save date count = 528 -1;
bit29(pass decode) = 1;
*/
//#define ECC_DECODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|(527<<17)|(1<<29))
//#define ECC_DECODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|((NFC_FIFO_SIZE+NFC_LOG_SPARE_SIZE-1)<<17)|(1<<29))
//#define ECC_DECODE_CONFIG     ((1<<3)|(1<<4)|((NFC_FIFO_SIZE+NFC_LOG_SPARE_SIZE-1)<<17)|(1<<29))
    #define ECC_DECODE_CONFIG       0x0018
//#define ECC_DECODE_CONFIG     ((1<<2)|(1<<3)|(1<<4)|((NFC_FIFO_SIZE-1)<<17)|(1<<29))
//#define ECC_DECODE_CONFIG     ((1<<3)|(1<<4)|((NFC_FIFO_SIZE-1)<<17)|(1<<29))
//#######enddefine of ecc encode and decode ################


    #define NFLASH_LOGPAGE_SIZE         512
    #define NFLASH_LOGSPARE_SIZE            16
/// @endcond
#endif //_CHIP_NFC_3223_



#ifndef _NAND_LIST_H_
    #define _NAND_LIST_H_

    #define ERROR_CHIP_ID   0//0xFFFFFFFF

/** @{@name Command and Status define
 *  Define command set and status of HY27UF1G2M nandflash
 *  and also used by other comman nandflash in the list.
 *
 */
    #define NFLASH_READ1            0x00
    #define NFLASH_READ2            0x30
#define NFLASH_RAND_READ1                0x05//this command is not good for ak3223
#define NFLASH_RAND_READ2                0xE0//this command is not good for ak3223
//#define NFLASH_CACHE_READ     0x31//this command is not good for ak3223
//#define NFLASH_EXIT_CACHE_READ        0x34//this command is not good for ak3223

    #define NFLASH_COPY_BACK_READ       0x35
    #define NFLASH_COPY_BACK_WRITE      0x85
    #define NFLASH_COPY_BACK_CONFIRM    0x10
    #define NFLASH_RESET            0xff

    #define NFLASH_FRAME_PROGRAM0       0x80
//#define NFLASH_RADOM_PROGRAM0 0x85//this command is not good for ak3223
    #define NFLASH_FRAME_PROGRAM1       0x10
//#define NFLASH_CACHE_PROGRAM1 0x15//this command is not good for ak3223

    #define NFLASH_BLOCK_ERASE0     0x60
    #define NFLASH_BLOCK_ERASE1     0xD0
    #define NFLASH_STATUS_READ      0x70
    #define NFLASH_READ_ID          0x90


//status register bit
    #define NFLASH_PROGRAM_SUCCESS      0x01 //bit 0
//#define NFLASH_CACHE_SUCCESS      0x02 //bit 1//this command is not good for ak3223
    #define NFLASH_HANDLE_READY     0x40 //bit 6
//#define NFLASH_CACHE_READY        0x20 //bit 5//this command is not good for ak3223

#endif

