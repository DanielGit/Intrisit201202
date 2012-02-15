//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//    	         系统配置文件头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2010-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _BURN_H
#define _BURN_H

typedef struct _BURN_AREA
{
	char	Description[256];		// 当前区域描述
	DWORD	Valid;					// 当前区域是否有效
	DWORD	PStart;					// 当前区域在Nand中起始物理块号
	DWORD	PEnd;					// 当前区域在Nand中结束物理块号
	DWORD	LStart;					// 当前区域起始逻辑块号
	DWORD	LEnd;					// 当前区域结束逻辑块号
	DWORD	ValidRws;				// 当前区域有效数据页数
	DWORD	AreaID;					// 当前区域ID信息
	DWORD	Index;					// 指向索引数据位置
	DWORD	Reserved[8];			// 保留位置
	char	Dummy[256-64];			// 空，保证BURN_AREA大小为512字节
}BURN_AREA;
typedef BURN_AREA *PBURN_AREA;

typedef struct _BURN_HEADER
{
	char	Description[128];		// 映象描述信息
	char    CpuDesc[32];			// 支持CPU描述
	char	Machine[32];			// 映象支持机型描述
	DWORD	HeaderSize;				// 索引头信息大小(包含自身及区域信息)
	DWORD	Version;				// 版本信息
	DWORD	FileType;				// 映像文件类型(0: NAND  1: BURN)
	DWORD	ImgAreas;				// 映像文件被分成的区域数量
	DWORD	ImgRwUnit;				// 读写单位大小(单位:字节)
	DWORD	ImgEUnit;				// 擦单位大小(单位:字节)
	DWORD	Capacity;				// 存储设备容量
	DWORD	DataHash;				// 数据文件32位Hash值
	DWORD	IndexHash;				// 索引文件32位Hash值
	DWORD	ValidRws;				// 读取单位数量
	DWORD	Reserve[5];				// 保留字
	DWORD	DataFiles;				// 记录数据文件个数
	DWORD	DataFsize[64];			// 记录各数据文件大小
}BURN_HEADER;
typedef BURN_HEADER *PBURN_HEADER;

typedef struct _BURN_CELL
{
	DWORD	Area;
	DWORD	DataSum;
	DWORD	Logic;
	DWORD	CellSum;
}BURN_CELL;
typedef BURN_CELL *PBURN_CELL;

typedef struct _BURN_CODE
{
	DWORD	Code;					// 操作编码
	DWORD	DataSize;				// 数据大小
	DWORD	Reserve0;				// 返回错误编码
	DWORD	Reserve1;				// 保留
}BURN_CODE;
typedef BURN_CODE *PBURN_CODE;

typedef struct _BURN_DRIVE
{
	HANDLE (*BurnCreate)(BYTE *, int);
	void (*BurnDestroy)(HANDLE);
	int (*BurnGetLastError)(HANDLE);
	int (*BurnFormateError)(HANDLE, int , char*, int);
	int (*BurnWriteArea)(HANDLE, BYTE*, int);
	int (*BurnWriteCell)(HANDLE, BYTE*, int);
	int (*BurnWriteBlock)(HANDLE, BYTE*, int);
	void (*BurnFlush)(HANDLE);
}BURN_DRIVE;
typedef BURN_DRIVE *PBURN_DRIVE;


#define BCODE_LAST_ERROR		0x0000
#define BCODE_FORMAT_ERROR		0x0001
#define BCODE_SET_HEADER		0x0002
#define BCODE_SET_AREAS			0x0003
#define BCODE_WRITE_CELL		0x0004
#define BCODE_WRITE_BLOCK		0x0005
#define BCODE_CHECK_CELL		0x0006
#define BCODE_CHECK_BLOCK		0x0007
#define BCODE_FLUSH				0x0008

#endif // _BURN_H
