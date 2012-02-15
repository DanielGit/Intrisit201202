//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//    	          Nand Flash���Զ���
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _NFLASH_H
#define _NFLASH_H

#include <config.h>

typedef struct _NAND_INFO
{
	char ChipName[128];		//оƬ����
	int Type;				//оƬ����
	DWORD BlockSize;		//���С(�ֽ�)
	DWORD PageSize;			//ҳ��С(�ֽ�)
	DWORD PBlocks;			//�������
	DWORD LBlocks;			//�߼�����
}NAND_INFO;
typedef NAND_INFO *PNAND_INFO;

////////////////////////////////////////////
// =========== FTL������ռ���� ===========
////////////////////////////////////////////
// FTL���NAND-FLASH��������Ϊ������
// ��������鲿�ֺ��߼�����ӳ��ռ䲿��
// ��������鲿������BOOT���������δԤ�Ƶ���;
// �߼�����ӳ�䲿������ROM��FAT�ļ�ϵͳ�Ŀռ����

// +------------------------+ �����0
// |                        |
// |     ��������鲿��     |
// |    (�������������)    |
// |                        |
// +------------------------+ RESERVE_BLOCKS
// |                        |
// |    �߼�����ӳ�䲿��    |
// | (����ROM��FAT�ļ�ϵͳ) |
// |                        |
// +------------------------+ MAX_PHYSICS_BLOCK


////////////////////////////////////////////
// �ռ���䶨��, ����BLOCK����!
////////////////////////////////////////////
#define NAND_SECTOR_SIZE		0x200

#define NAND_BOOT_SIZE			(CONFIG_NAND_BOOT_SIZE*1024*1024)		// BOOT����Ĵ�С(MB)
#define NAND_ISOFS_SIZE			(CONFIG_NAND_ISOFS_SIZE*1024*1024)		// ISO�ļ�ϵͳʹ�ÿռ��С
#define NAND_ROMFS_SIZE			(CONFIG_NAND_ROMFS_SIZE*1024*1024)		// ROM�ļ�ϵͳʹ�ÿռ��С
#define NAND_SYSCFG_SIZE		(CONFIG_NAND_SYSCFG_SIZE*1024*1024)		// �����߼���0�Ŀռ��С
#define NAND_RAMBK_SIZE			(CONFIG_NAND_RAMBK_SIZE*1024*1024)		// �����߼���1�Ŀռ��С
#define NAND_FATFS1_SIZE		(CONFIG_NAND_FATFS1_SIZE*1024*1024)		// FAT�ļ�ϵͳ(������)ʹ�ÿռ��С

#define NAND_ALLOC_SIZE			(NAND_ROMFS_SIZE+NAND_ISOFS_SIZE+ \
									NAND_SYSCFG_SIZE+NAND_RAMBK_SIZE+ \
									NAND_FATFS1_SIZE)

// �ռ������Ч�Ա�����
#if (NAND_LOGIC_SIZE > NAND_FTL_SIZE)
#error FTL MEMORY ALLOC ERROR!!!
#endif

////////////////////////////////////////////
// �߼����¸���������(�߼��顢�߼�����)
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

