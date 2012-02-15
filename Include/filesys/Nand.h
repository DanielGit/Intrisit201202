//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	          Nand Flash特性定义
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _NFLASH_H
#define _NFLASH_H

#include <config.h>

typedef struct _NAND_INFO
{
	char ChipName[128];		//芯片名称
	int Type;				//芯片类型
	DWORD BlockSize;		//块大小(字节)
	DWORD PageSize;			//页大小(字节)
	DWORD PBlocks;			//物理块数
	DWORD LBlocks;			//逻辑块数
}NAND_INFO;
typedef NAND_INFO *PNAND_INFO;

////////////////////////////////////////////
// =========== FTL层物理空间分配 ===========
////////////////////////////////////////////
// FTL层把NAND-FLASH按物理块分为两部分
// 保留物理块部分和逻辑物理映射空间部分
// 保留物理块部分用于BOOT代码或其他未预计的用途
// 逻辑物理映射部分用于ROM和FAT文件系统的空间分配

// +------------------------+ 物理块0
// |                        |
// |     保留物理块部分     |
// |    (用于引导代码等)    |
// |                        |
// +------------------------+ RESERVE_BLOCKS
// |                        |
// |    逻辑物理映射部分    |
// | (用于ROM和FAT文件系统) |
// |                        |
// +------------------------+ MAX_PHYSICS_BLOCK


////////////////////////////////////////////
// 空间分配定义, 必须BLOCK对齐!
////////////////////////////////////////////
#define NAND_SECTOR_SIZE		0x200

#define NAND_BOOT_SIZE			(CONFIG_NAND_BOOT_SIZE*1024*1024)		// BOOT区域的大小(MB)
#define NAND_ISOFS_SIZE			(CONFIG_NAND_ISOFS_SIZE*1024*1024)		// ISO文件系统使用空间大小
#define NAND_ROMFS_SIZE			(CONFIG_NAND_ROMFS_SIZE*1024*1024)		// ROM文件系统使用空间大小
#define NAND_SYSCFG_SIZE		(CONFIG_NAND_SYSCFG_SIZE*1024*1024)		// 保留逻辑区0的空间大小
#define NAND_RAMBK_SIZE			(CONFIG_NAND_RAMBK_SIZE*1024*1024)		// 保留逻辑区1的空间大小
#define NAND_FATFS1_SIZE		(CONFIG_NAND_FATFS1_SIZE*1024*1024)		// FAT文件系统(隐藏盘)使用空间大小

#define NAND_ALLOC_SIZE			(NAND_ROMFS_SIZE+NAND_ISOFS_SIZE+ \
									NAND_SYSCFG_SIZE+NAND_RAMBK_SIZE+ \
									NAND_FATFS1_SIZE)

// 空间分配有效性编译检查
#if (NAND_LOGIC_SIZE > NAND_FTL_SIZE)
#error FTL MEMORY ALLOC ERROR!!!
#endif

////////////////////////////////////////////
// 逻辑区下各分区定义(逻辑块、逻辑扇区)
////////////////////////////////////////////
#define ISOFS_START_LSECTOR		0
#define ISOFS_LSECTORS			((DWORD)(NAND_ISOFS_SIZE/NAND_SECTOR_SIZE))
#define ISOFS_END_LSECTOR		(ISOFS_START_LSECTOR+ISOFS_LSECTORS)

#define ROMFS_START_LSECTOR		ISOFS_END_LSECTOR
#define ROMFS_LSECTORS			((DWORD)(NAND_ROMFS_SIZE/NAND_SECTOR_SIZE))
#define ROMFS_END_LSECTOR		(ROMFS_START_LSECTOR+ROMFS_LSECTORS)

#define SYSCFG_START_LSECTOR	ROMFS_END_LSECTOR
#define SYSCFG_LSECTORS			((DWORD)(NAND_SYSCFG_SIZE/NAND_SECTOR_SIZE))
#define SYSCFG_END_LSECTOR		(SYSCFG_START_LSECTOR+SYSCFG_LSECTORS)

#define RAMBK_START_LSECTOR		SYSCFG_END_LSECTOR
#define RAMBK_LSECTORS			((DWORD)(NAND_RAMBK_SIZE/NAND_SECTOR_SIZE))
#define RAMBK_END_LSECTOR		(RAMBK_START_LSECTOR+RAMBK_LSECTORS)

#define FATFS1_START_LSECTOR	RAMBK_END_LSECTOR
#define FATFS1_LSECTORS			((DWORD)(NAND_FATFS1_SIZE/NAND_SECTOR_SIZE))
#define FATFS1_END_LSECTOR		(FATFS1_START_LSECTOR+FATFS1_LSECTORS)

#define FATFS0_START_LSECTOR	FATFS1_END_LSECTOR

#endif //_NFLASH_H

