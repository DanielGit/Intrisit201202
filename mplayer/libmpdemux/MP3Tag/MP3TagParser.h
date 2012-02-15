#ifndef __MP3_TAG_PARSER_H__
#define __MP3_TAG_PARSER_H__

#include <winbase.h>
#include "TagParserBase.h"
 
// MP3ID3V1TAG structure
struct MP3ID3V1TAG
{
	char ident[3]; // TAG
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[28];
	BYTE reserved;
	BYTE tracknum;
	BYTE genre;
};

#pragma pack(push,1)
struct ID3_FrameDef
{
  char sFrameName[5];
};

#pragma pack(pop)

#pragma pack(push,1)
struct ID3_FrameHeader_V22
{
	char FrameID[3];
	int	Size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct ID3_FrameHeader_V23
{
	char FrameID[4];
	int  Size;
	char Flags[2];
};

#pragma pack(pop)

class MP3TagParser : public TagParserBase
{
 public:
   MP3TagParser();
   ~MP3TagParser();

    bool OpenFile(const TCHAR* file_nm);
    void CloseFile();

    TCHAR* getTitle();
    TCHAR* getArtist();
    int   getRating();
    TCHAR* getAlbum();
	TCHAR* getGenre();
    bool  getPicInf(TAG_PIC_TYPE& type, int& rawsiz);
    //caller alloc buffer
    bool  getPicDat(BYTE* pBuf);
	
   
private:
	void ClearResult();
	void ChangeEndian(void* pBuffer, int nBufSize);
	bool IsFrameV23(char* pch);
	bool IsFrameV22(char* pch);
	bool GetFrameV23(HANDLE hFile, ID3_FrameHeader_V23* pFH);
	bool GetFrameV22(HANDLE hFile, ID3_FrameHeader_V22* pFH);
    bool GetAvail(char *ID3Text,TCHAR m[MAX_TEXT_LEN+1],int len,int m_len);

	TCHAR  m_Title[MAX_TEXT_LEN+1];
	bool  m_bTitleAvail;

	TCHAR  m_Album[MAX_TEXT_LEN+1];
	bool  m_bAlbumAvail;

	TCHAR  m_Artist[MAX_TEXT_LEN+1];
	bool  m_bArtistAvail;

	TCHAR  m_Genre[MAX_TEXT_LEN+1];
	bool  m_bGenreAvail;

	int m_Rating;
	int m_PicDatOffset;
	TAG_PIC_TYPE m_PicType;
	int m_PicSiz;

	HANDLE m_hFile;

};

#endif //__MP3_TAG_PARSER_H__ 