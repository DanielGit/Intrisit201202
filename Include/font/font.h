//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨GUI  *****
//        --------------------------------------
//       	        ��������˽��ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2008-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _FONT_H
#define _FONT_H

#include <gui/wintype.h>
#include <gui/wingr.h>

#define MAX_FONT_NAME		64
#define MAX_FONT_CODENAME	48

#define CACHEBLOCKSIZE 14400
#define CACHEBLOCKNUMS    16
#define MAXBLOCKNUMS 2000               //FONTFILESIZE/16K
#define CACHESTEP    5
typedef struct _FONTCACHELINk
{
	LIST plnk;
	int blockno;
	BYTE FontCache[CACHEBLOCKSIZE];   //16��cache
}FONTCACHELINK;

// �����ļ�ͷ�ṹ
typedef struct _FONT_HEADER
{
	char Signal[8];						// �����ʶ���汾
	char FontName[MAX_FONT_NAME];		// ��������
	char CodeName[MAX_FONT_CODENAME];	// ��������׼
	int Sections;						// �����ֶ���
	int Reserve;
}FONT_HEADER;
typedef FONT_HEADER *PFONT_HEADER;


// �������Ϣ�ṹ
typedef struct _FONT_SECTION
{
	int CodeStart;		// �������뿪ʼֵ
	int CodeEnd;		// �����������ֵ
	int Height;			// ���������ʾ�߶�(����)
	int FixWidth;		// ������̶����ֵ(Ϊ0��ʾΪ�ǵȿ�����)
	int WDataOffset;	// ���������������ļ��е�ƫ��
	int WDataSize;		// �������������ֽ���
	int DDataBpl;		// ÿ�ַ������������ֽ���
	int DDataBytes;		// ÿ�ַ����������ֽ���
	int DDataOffset;	// ����������������ļ��е�ƫ��
	int DDataSize;		// ��������������ֽ���
}FONT_SECTION;
typedef FONT_SECTION *PFONT_SECTION;


// ��������ṹ
typedef struct _FONT_SECTION_LINK
{
	LIST Link;
	FONT_SECTION Section;
}FONT_SECTION_LINK;
typedef FONT_SECTION_LINK *PFONT_SECTION_LINK;


typedef struct _ROM_FONT
{
	FONT_HEADER Header;
	const FONT_SECTION *Sections[4];	// �ڴ������ļ���������ĸ���
}ROM_FONT;
typedef ROM_FONT *PROM_FONT;

typedef struct _FONT_DC
{
	LIST Link;
	struct _FONT *Font;
	HWND hWnd;
}FONT_DC;
typedef FONT_DC *PFONT_DC;

// ���������ṹ
typedef struct _FONT
{
	int TrueType;			// ��������
	char FontName[MAX_FONT_NAME];		// ��������
	char CodeName[MAX_FONT_CODENAME];	// ��������׼
	
	HANDLE hTask;			// �����������
	
	HANDLE FatFile;			// �ļ�������
	int FileOffset;			// �������ļ��е�ƫ��
	
	LIST Link;				// ����������
	LIST SectionLink;		// �������Ϣ��

	int CacheFlg;            //�������Ƿ���Ҫ����cache
	LIST CacheLink;         //����cache����
	int FontCountTable[MAXBLOCKNUMS];
	
	// ������������
	int (*GetCharBitmap)(struct _FONT_DC*, BYTE*, CHAR_BITMAP *, int);
	int (*GetCharInfo)(struct _FONT_DC*, BYTE*, int*, int);
	int (*CharOut)    (struct _FONT_DC*, BYTE*, POINT*, int);
	int (*GetWordInfo)(struct _FONT_DC*, BYTE*, int*, int);
	int (*WordOut)    (struct _FONT_DC*, BYTE*, POINT*, int);
	int (*GetStrInfo) (struct _FONT_DC*, BYTE*, int*, int);
	int (*StrOut)     (struct _FONT_DC*, BYTE*, POINT*, int);
	int (*IsInFont)   (struct _FONT_DC*, BYTE*);
	
	int Height;
	BYTE *DotBuf;			// �������ݻ�����
	
	// TTF���Ӳ���
#if defined(CONFIG_TTF_ENABLE) || defined(CONFIG_HF_TTF_ENABLE)
	int TtfSize;
	int TtfItaly;
	int TtfBold;
	HANDLE TtfHandle;
	CHAR_BITMAP TtfBmp;
#endif
}FONT;
typedef FONT *PFONT;
typedef PFONT HFONT;


typedef struct _FONT_INFO
{
	HANDLE hFont;
	int bTrueType;
	char *FontName;
	char *CodeName;
}FONT_INFO;
typedef FONT_INFO *PFONT_INFO;

void FontInit(void);

int FontFileInit(void);
HANDLE FontFileLoadRom(PROM_FONT fnt);
HANDLE FontFileLoadFile(const char *name, int fontoff);
HANDLE FontFileLoadTtf(const char *name);
int FontFileUnload(HANDLE hfnt);
int FontFileInfoGet(PFONT_INFO pinfo, int max);
HANDLE FontFileOpen(HWND hwnd, const char *name);
int FontFileGetName(HANDLE hfont, char *name, int max);
void FontFileClose(HANDLE hfont);
int FontFileCheckDc(HANDLE hfont);
void FontFileClearWnd(HWND hwnd);

#if defined(CONFIG_TTF_ENABLE) || defined(CONFIG_HF_TTF_ENABLE)
int TftInit(void);
void *TtfOpen(const char *name);
void TtfClose(void *ttf);
int TtfProperty(void *ttf, int size, int italy, int bold);
int TtfGetCharWidth(void *ttf, int ch);
void *TtfGetCharBitmap(void *ttf, int ch, int *base, int *width);
int TtfFontName(void *ttf, char *name, int max);
#endif

#if defined(STC_EXP)
HANDLE sFontLoadRom(PROM_FONT fnt);
HANDLE sFontLoadFile(const char *name, int fontoff);
HANDLE sFontLoadTtf(const char *name);
int sFontUnload(HANDLE hfnt);
int sFontInfoGet(PFONT_INFO pinfo, int max);
int sFontGetName(HANDLE hfont, char *name, int max);
HANDLE sFontOpen(HWND hwnd, const char *name);
void sFontClose(HANDLE hfont);
int sFontCheck(HANDLE hfont);
int sFontTtfProperty(HANDLE hfont, int size, int italy, int bold);

int sFontGetCharBitmap(HANDLE hfont, BYTE* s, PCHAR_BITMAP bitmap, int bufmax);
int sFontGetCharInfo(HANDLE fdc, BYTE* s, int* width, int max);
int sFontCharOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int sFontGetWordInfo(HANDLE fdc, BYTE* s, int* width, int max);
int sFontWordOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int sFontGetStrInfo(HANDLE fdc, BYTE* s, int* width, int max);
int sFontStrOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int sFontIsInFont(HANDLE fdc, BYTE *s);
int sFontDisplayHeight(HANDLE fdc);

#else
HANDLE FontLoadRom(PROM_FONT fnt);
HANDLE FontLoadFile(const char *name, int fontoff);
HANDLE FontLoadTtf(const char *name);
int FontUnload(HANDLE hfnt);
int FontInfoGet(PFONT_INFO pinfo, int max);
int FontGetName(HANDLE hfont, char *name, int max);
HANDLE FontOpen(HWND hwnd, const char *name);
void FontClose(HANDLE hfont);
int FontCheck(HANDLE hfont);
int FontTtfProperty(HANDLE hfont, int size, int italy, int bold);

int FontGetCharBitmap(HANDLE hfont, BYTE* s, PCHAR_BITMAP bitmap, int bufmax);
int FontGetCharInfo(HANDLE fdc, BYTE* s, int* width, int max);
int FontCharOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int FontGetWordInfo(HANDLE fdc, BYTE* s, int* width, int max);
int FontWordOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int FontGetStrInfo(HANDLE fdc, BYTE* s, int* width, int max);
int FontStrOut(HANDLE fdc, BYTE *s, POINT *pos, int max);
int FontIsInFont(HANDLE fdc, BYTE *s);
int FontDisplayHeight(HANDLE fdc);

#endif

// �ַ�������������
PFONT_SECTION FontGetCharSection(PFONT fnt, int c);
int FontGetCharWidth(PFONT fnt, int c);
int FontGetCharDot(PFONT fnt, int c, int *height, int *bpl, void *dotbuf);


// ASCII�ַ��� ��������
int FntASCII_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntASCII_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntASCII_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntASCII_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntASCII_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntASCII_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntASCII_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntASCII_IsInFont(PFONT_DC dc, BYTE *s);

// GB2312�ַ��� ��������
int FntGB2312_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntGB2312_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGB2312_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGB2312_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGB2312_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGB2312_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGB2312_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGB2312_IsInFont(PFONT_DC dc, BYTE *s);

// GB18030�ַ��� ��������
int FntGBK_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntGBK_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGBK_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGBK_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGBK_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGBK_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntGBK_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntGBK_IsInFont(PFONT_DC dc, BYTE *s);

// UNICODE�ַ��� ��������
int FntUCD_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntUCD_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntUCD_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntUCD_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntUCD_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntUCD_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntUCD_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntUCD_IsInFont(PFONT_DC dc, BYTE *s);

// ISO�ַ��� ��������
int FntISO_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntISO_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntISO_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntISO_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntISO_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntISO_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntISO_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntISO_IsInFont(PFONT_DC dc, BYTE *s);

// ��5�� BIG5�ַ���
int FntBIG5_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntBIG5_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntBIG5_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntBIG5_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntBIG5_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntBIG5_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntBIG5_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntBIG5_IsInFont(PFONT_DC dc, BYTE *s);

// ������������ 
int FntKOE_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntKOE_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKOE_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKOE_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKOE_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKOE_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKOE_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKOE_IsInFont(PFONT_DC dc, BYTE *s);


// KSC9601-1992�ַ��� ������������ 
int FntKSC_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntKSC_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKSC_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKSC_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKSC_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKSC_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntKSC_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntKSC_IsInFont(PFONT_DC dc, BYTE *s);


// ShiftJIS�ַ��� ������������ 
int FntSJIS_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int FntSJIS_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntSJIS_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntSJIS_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntSJIS_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntSJIS_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int FntSJIS_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int FntSJIS_IsInFont(PFONT_DC dc, BYTE *s);

// GB18030�ַ��� ��������
int TtfGBK_GetCharBitmap(PFONT_DC dc, BYTE *c, PCHAR_BITMAP bitmap, int bufsize);
int TtfGBK_GetCharInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int TtfGBK_CharOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int TtfGBK_GetWordInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int TtfGBK_WordOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int TtfGBK_GetStrInfo(PFONT_DC dc, BYTE* s, int* width, int max);
int TtfGBK_StrOut(PFONT_DC dc, BYTE *s, POINT *pos, int max);
int TtfGBK_IsInFont(PFONT_DC dc, BYTE *s);

#endif	// _FONT_H
