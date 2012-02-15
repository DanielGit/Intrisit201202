//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//               文件系统相关定义头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2006-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _FS_H
#define _FS_H

#include <config.h>
#include <sys/types.h>

#define FS_ATTR_READ_ONLY		0x01
#define FS_ATTR_HIDDEN			0x02
#define FS_ATTR_SYSTEM			0x04
#define FS_ATTR_VOLUME_ID		0x08
#define FS_ATTR_ARCHIVE			0x20
#define FS_ATTR_DIRECTORY		0x10

#define FS_SEEK_CUR				1
#define FS_SEEK_END				2
#define FS_SEEK_SET				0

#define FS_START_DISK			('C')
#ifdef CONFIG_ISO9660_SUPPORT
#define FS_END_DISK				('G')
#else
#define FS_END_DISK				('F')
#endif
#define FS_MAX_DEV				(FS_END_DISK-FS_START_DISK+1)

//目录登记项结构体定义
typedef struct
{
	BYTE	name[13];	//短文件名称
	WORD	ctime;		//创建的时间(bit0~4 second 2秒为1单位:bit5~10 minute 0~59:bit11~15 hour 0~23)
	WORD	cdate;		//创建的日期(bit0~4 day:bit5~8 month:bit9~15 year 是基于1980年的)
	BYTE	attr;		//属性
	WORD	mtime;		//修改的时间
	WORD	mdate;		//修改的日期
	DWORD	size;		//文件大小
	BYTE	longname[0x200];//长文件名称
}DIR_ENTRY;

typedef DIR_ENTRY FILE_ENTRY;

//文件系统信息结构体定义
typedef struct 
{
	DWORD	TotalClust;	//总簇数
	DWORD	FreeClust;	//自由簇数
	WORD	SecperClust;//每个簇的扇区数
	WORD	BytesperSec;//每个扇区的字节数
}FS_INFO;


// 磁盘信息值
//只读文件系统信息结构体定义
typedef struct
{
	BYTE	Volume[12];	//卷标名称
	DWORD	Hash;		//结构体成员内容的哈希值
	DWORD	ID;			//磁盘ID
	DWORD	CRC32;		//系统内容CRC值
	DWORD	DiskSize;	//磁盘的大小
	WORD	Date;		//日期
	WORD	Time;		//时间
}RFS_INFO;

//光盘文件系统信息结构体定义
typedef struct
{
	BYTE	Volume[32];	//卷标名称
	DWORD	CRC32;		//系统内容CRC值
	DWORD	DiskSize;	//磁盘的大小
	WORD	Date;		//日期
	WORD	Time;		//时间
}ISOFS_INFO;

typedef struct
{
	HANDLE (*_fsfopen)(const char *pname, const char *pmode);
	int (*_fsfclose)(HANDLE hfile);
	long (*_fsfread)(void *pbuf, long size, long count, HANDLE hfile);
	long (*_fsfwrite)(const void *pbuf, long size, long count, HANDLE hfile);
	long (*_fsftell)(HANDLE hfile);
	int (*_fsfeof)(HANDLE hfile);
	int (*_fsfseek)(HANDLE hfile, long offset, int origin);
	int (*_fsfremove)(const char *pname);
	int (*_fsfrename)(const char *oldname, const char *newname);
	int (*_fsfmove)(const char *oldname, const char *newname);
	int (*_fsfproperty)(const char *pname, FILE_ENTRY *pentry);
	int (*_fsftrim)(const char *pname, long size);
	
	HANDLE (*_fsdopen)(const char *pname);
	int (*_fsdclose)(HANDLE hfile);
	int (*_fsdread)(HANDLE hdir, DIR_ENTRY *pentry);
	int (*_fsdmake)(const char *pname);
	int (*_fsdremove)(const char *pname);
	int (*_fsdrename)(const char *oldname, const char *newname);
	int (*_fsdmove)(const char *oldname, const char *newname);

	int (*_fserror)(HANDLE hfile); 

	int (*_fsformat)(const char *pname);
	int (*_fsinfo)(const char*pname, FS_INFO *pinfo);
	int (*_fsinit)(const char *pname); 
	int (*_fsuninit)(const char *pname); 
}FS_DEV_API;


//////////////////////////////////////////////////////////////////
//  文件系统函数声明
//////////////////////////////////////////////////////////////////
#if defined(STC_EXP)
HANDLE sfsfopen(const char *pname, const char *pmode); 
int sfsfclose(HANDLE hfile); 
int sfsfremove(const char *pname); 
int sfsfseek(HANDLE hfile, long offset, int origin); 
int sfsfeof(HANDLE hfile); 
long sfsftell(HANDLE hfile); 
long sfsfread(void *pbuf, long size, long count, HANDLE hfile); 
long sfsfwrite(const void *pbuf, long size, long count, HANDLE hfile);
int sfsfrename(const char *oldname, const char *newname);
int sfsfmove(const char *oldname, const char *newname);
int sfsfproperty(const char *pname, FILE_ENTRY *pentry);
long sfsfsize(HANDLE fh);
int sfsftrim(const char *pname, long size);

HANDLE sfsdopen(const char *pname); 
int sfsdclose(HANDLE hdir); 
int sfsdread(HANDLE hdir, DIR_ENTRY *pentry); 
int sfsdmake(const char *pname); 
int sfsdremove(const char *pname); 
int sfsdrename(const char *oldname, const char *newname);
int sfsdmove(const char *oldname, const char *newname);

int sfsferror(HANDLE hfile); 
int sfsuninit(const char *pname); 
int sfsformat(const char *pname);
int sfsinfo(const char *pname, FS_INFO *pinfo); 
int sfsinited(void);

int sFsFormated(const char *pname); 
int srfs_info(const char *pname, RFS_INFO *pinfo); 

int sBiosPackageCrcCheck(void);
int sSysPackageCrcCheck(void);

int sFsLowFormat(int *percent);
int sNandReadId(BYTE *id);
int sRomReadRandom(long addr, long size, BYTE* buf);
int sLibPackageCrcCheck(int *per, int *stat);
int sIsoPackageCrcCheck(int *per, int *stat);
int sIsSysFile(const char *name, DWORD offset);
int sIsBiosFile(const char *name, DWORD offset);
#else
HANDLE fsfopen(const char *pname, const char *pmode); 
int fsfclose(HANDLE hfile); 
int fsfremove(const char *pname); 
int fsfseek(HANDLE hfile, long offset, int origin); 
int fsfeof(HANDLE hfile); 
long fsftell(HANDLE hfile); 
long fsfread(void *pbuf, long size, long count, HANDLE hfile); 
long fsfwrite(const void *pbuf, long size, long count, HANDLE hfile);
int fsfrename(const char *oldname, const char *newname);
int fsfmove(const char *oldname, const char *newname);
int fsfproperty(const char *pname, FILE_ENTRY *pentry);
long fsfsize(HANDLE fh);
int fsftrim(const char *pname, long size);

HANDLE fsdopen(const char *pname); 
int fsdclose(HANDLE hdir); 
int fsdread(HANDLE hdir, DIR_ENTRY *pentry); 
int fsdmake(const char *pname); 
int fsdremove(const char *pname); 
int fsdrename(const char *oldname, const char *newname);
int fsdmove(const char *oldname, const char *newname);

int fsferror(HANDLE hfile); 
int fsuninit(const char *pname); 
int fsformat(const char *pname);
int fsinfo(const char *pname, FS_INFO *pinfo); 
int fsinited(void);

int FsFormated(const char *pname); 
int rfs_info(const char *pname, RFS_INFO *pinfo); 

int BiosPackageCrcCheck(void);
int SysPackageCrcCheck(void);
int FsLowFormat(int *percent);
int NandReadId(BYTE *id);
int RomReadRandom(long addr, long size, BYTE* buf);
int LibPackageCrcCheck(int *per, int *stat);
int IsoPackageCrcCheck(int *per, int *stat);
int IsSysFile(const char *name, DWORD offset);
int IsBiosFile(const char *name, DWORD offset);
#endif		// STC_EXP

#endif // _FS_H
