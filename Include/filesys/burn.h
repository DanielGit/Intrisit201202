//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//    	         ϵͳ�����ļ�ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2010-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _BURN_H
#define _BURN_H

typedef struct _BURN_AREA
{
	char	Description[256];		// ��ǰ��������
	DWORD	Valid;					// ��ǰ�����Ƿ���Ч
	DWORD	PStart;					// ��ǰ������Nand����ʼ������
	DWORD	PEnd;					// ��ǰ������Nand�н���������
	DWORD	LStart;					// ��ǰ������ʼ�߼����
	DWORD	LEnd;					// ��ǰ��������߼����
	DWORD	ValidRws;				// ��ǰ������Ч����ҳ��
	DWORD	AreaID;					// ��ǰ����ID��Ϣ
	DWORD	Index;					// ָ����������λ��
	DWORD	Reserved[8];			// ����λ��
	char	Dummy[256-64];			// �գ���֤BURN_AREA��СΪ512�ֽ�
}BURN_AREA;
typedef BURN_AREA *PBURN_AREA;

typedef struct _BURN_HEADER
{
	char	Description[128];		// ӳ��������Ϣ
	char    CpuDesc[32];			// ֧��CPU����
	char	Machine[32];			// ӳ��֧�ֻ�������
	DWORD	HeaderSize;				// ����ͷ��Ϣ��С(��������������Ϣ)
	DWORD	Version;				// �汾��Ϣ
	DWORD	FileType;				// ӳ���ļ�����(0: NAND  1: BURN)
	DWORD	ImgAreas;				// ӳ���ļ����ֳɵ���������
	DWORD	ImgRwUnit;				// ��д��λ��С(��λ:�ֽ�)
	DWORD	ImgEUnit;				// ����λ��С(��λ:�ֽ�)
	DWORD	Capacity;				// �洢�豸����
	DWORD	DataHash;				// �����ļ�32λHashֵ
	DWORD	IndexHash;				// �����ļ�32λHashֵ
	DWORD	ValidRws;				// ��ȡ��λ����
	DWORD	Reserve[5];				// ������
	DWORD	DataFiles;				// ��¼�����ļ�����
	DWORD	DataFsize[64];			// ��¼�������ļ���С
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
	DWORD	Code;					// ��������
	DWORD	DataSize;				// ���ݴ�С
	DWORD	Reserve0;				// ���ش������
	DWORD	Reserve1;				// ����
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
