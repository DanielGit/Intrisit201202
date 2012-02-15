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

#ifndef _SYSPROG_H
#define _SYSPROG_H

#include <kernel/kernel.h>

typedef struct 
{
	DWORD crc;
	DWORD size;
	BYTE  dt[17];
	BYTE  sver[17];
	BYTE  aver[17];
}SYSPACK_INFO;

typedef struct 
{
	DWORD crc;
	DWORD size;
	BYTE  dt[17];
	BYTE  hwver[17];
	BYTE  swver[17];
	BYTE  sn[17];
}BIOSPACK_INFO;

int SysPackageLogoLoader(DWORD base);
int SysPackageLogoSize(void);
int BiosPackageLogoLoader(DWORD base);
int BiosPackageLogoSize(void);

#if defined(STC_EXP)
int sSysPackageInfo(SYSPACK_INFO *info);
int sSysPackageCrcCheck(void);
int sBiosPackageInfo(BIOSPACK_INFO *info);
int sBiosPackageCrcCheck(void);
#else
int SysPackageInfo(SYSPACK_INFO *info);
int SysPackageCrcCheck(void);
int BiosPackageInfo(BIOSPACK_INFO *info);
int BiosPackageCrcCheck(void);
#endif

int IdtPackCheck(BYTE *buf);
void IdtPackSec(BYTE *buf);
void IdtUnpackSec(BYTE *buf);
void IdtPackCrc(BYTE *buf);

int BiosIdtRead(BYTE *buf);
int SysIdtRead(BYTE *buf);

void Crc32Ex(const BYTE*s, DWORD len, DWORD *crc32);

#endif //_SYSPROG_H
