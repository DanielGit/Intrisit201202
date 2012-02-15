//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	           Nand Flash驱动部分
//        --------------------------------------
//                 版权: 新诺亚舟科技头文件
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef NANDRV_H
#define NANDRV_H

#include <config.h>
#include <kernel/kernel.h>
#include <filesys/Nand.h>

#define	 READ_MODE1_CMD		0x00
#define  READ_ADV_CMD     	0x30
#define	 READ_MODE2_CMD		0x01
#define  READ_SPARE_CMD		0x50
#define	 PRE_READ_RANDOM	0x05
#define  READ_RANDOM		0xe0
#define  AUTO_WRITE_CMD		0x80
#define  PAGE_WRITE_CMD		0x10
#define  READ_STATUS_CMD	0x70
#define  AUTO_ERASE_CMD		0x60
#define  ERASE_CMD			0xD0
#define	 RESET_CMD			0xff
#define  READ_ID      		0x90
#define  READ_ID_ADDR	 	0x00
#define  PRE_COPY_BACK		0x35
#define  COPY_BACK_CMD		0x85

////////////////////////////////////////////
// NAND-FLASH SPARE区相关参数定义
////////////////////////////////////////////
#define SPARE_SIZE				0x10

// 坏块信息常量定义
#define FSPARE_BI_GOOD			0xff	// 0xFF --> 可使用的块
#define FSPARE_BI_BAD			0x00	// 其他值-> 该块为坏块，不可用

// SPARE页中各字节内容
#define SPARE_BAD_INFO			0x00	// 0x01
#define SPARE_LOGIC_PAGE		0x01	// 0x01~0x05
#define SPARE_DATA_ECC			0x06	// 0x06~0x29(4*9BYTES)
#define SPARE_LOGIC_PAGE1		0x36	// 0x36~0x3a
#define SPARE_LOGIC_PAGE2		0x3b	// 0x3b~0x3f

#define NandrvBadInfo(p)		(*(BYTE*)((BYTE*)p + SPARE_BAD_INFO))

////////////////////////////////////////////
// NAND-FLASH 驱动函数定义
////////////////////////////////////////////
int NandrvInit(void);
int NandrvIdRead(BYTE *pid);
int NandrvGetInfo(PNAND_INFO info);

int NandrvPageInfoRead(DWORD page, DWORD *lpage);
int NandrvPageRead(DWORD page, BYTE *pbuf, DWORD *lpage);
int NandrvPageWrite(DWORD page, DWORD *pbuf, DWORD *lpage);

int NandrvBlockErase(DWORD block);
int NandrvBlockCheckBad(DWORD block);
void NandrvBlockMarkBad(DWORD block);

int NandrvLoaderPageWrite(DWORD page, DWORD *pbuf);
int NandrvLoaderPageRead(DWORD page, BYTE *pbuf);

#endif //NANDRV_H
