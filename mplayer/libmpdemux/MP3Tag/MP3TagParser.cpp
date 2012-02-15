#include "stdafx.h"
#include "MP3TagParser.h"
#include "GenreLst.h"
#define	FRMAN_COUNT_V22 70
#define	FRMAN_COUNT_V23 77

static  ID3_FrameDef ID3_FrameDefs_V22[FRMAN_COUNT_V22] =
{
  "CRA\0",
  "PIC\0",
  "COM\0",
  "EQU\0",
  "ETC\0",
  "GEO\0",
  "IPL\0",
  "LNK\0",
  "MCI\0",
  "MLL\0",
  "CNT\0", 
  "POP\0", 
  "BUF\0", 
  "RVA\0", 
  "REV\0", 
  "SLT\0", 
  "STC\0", 
  "TAL\0", 
  "TBP\0", 
  "TCM\0", 
  "TCO\0", 
  "TCR\0", 
  "TDA\0", 
  "TDY\0", 
  "TEN\0", 
  "TXT\0", 
  "TFT\0", 
  "TIM\0", 
  "TT1\0", 
  "TT2\0", 
  "TT3\0", 
  "TKE\0", 
  "TLA\0", 
  "TLE\0", 
  "TMT\0", 
  "TOT\0", 
  "TOF\0", 
  "TOL\0", 
  "TOA\0", 
  "TOR\0", 
  "TP1\0", 
  "TP2\0", 
  "TP3\0", 
  "TP4\0", 
  "TPA\0", 
  "TPB\0", 
  "TRK\0", 
  "TRD\0", 
  "TRN\0", 
  "TRO\0", 
  "TSI\0", 
  "TRC\0", 
  "TSS\0", 
  "TXX\0", 
  "TYE\0", 
  "UFI\0", 
  "ULT\0", 
  "WCM\0", 
  "WCP\0", 
  "WAF\0", 
  "WAR\0", 
  "WAS\0", 
  "WRA\0", 
  "WPY\0", 
  "WPB\0", 
  "WXX\0", 
  "CRM\0", 
  "CDM\0",
  "PRI\0",    //zhou
  "CM1\0"   //Yang
};


static  ID3_FrameDef ID3_FrameDefs_V23[FRMAN_COUNT_V23] =
{
	"AENC",
	"APIC",
	"COMM",
	"COMR",
	"ENCR",
	"EQUA",
	"ETCO",
	"GEOB",
	"GRID",
	"IPLS",
	"LINK",
	"MCDI",
	"MLLT",
	"OWNE",
	"PRIV",
	"PCNT",
	"POPM",
	"POSS",
	"RBUF",
	"RVAD",
	"RVRB",
	"SYLT",
	"SYTC",
	"TALB",
	"TBPM",
	"TCOM",
	"TCON",
	"TCOP",
	"TDAT",
	"TDLY",
	"TENC",
	"TEXT",
	"TFLT",
	"TIME",
	"TIT1",
	"TIT2",
	"TIT3",
	"TKEY",
	"TLAN",
	"TLEN",
	"TMED",
	"TOAL",
	"TOFN",
	"TOLY", 
	"TOPE", 
	"TORY", 
	"TOWN", 
	"TPE1", 
	"TPE2", 
	"TPE3", 
	"TPE4", 
	"TPOS", 
	"TPUB", 
	"TRCK", 
	"TRDA", 
	"TRSN", 
	"TRSO", 
	"TSIZ", 
	"TSRC", 
	"TSSE", 
	"TYER",
	"TDRC", 
	"TXXX", 
	"UFID", 
	"USER", 
	"USLT", 
	"WCOM",
	"WCOP",
	"WOAF",
	"WOAR",
	"WOAS",
	"WORS", 
	"WPAY", 
	"WPUB", 
	"WXXX",
	"NCON",    //Yang
	"PRI""\x020"    //zhou
};

MP3TagParser::MP3TagParser()
{
	m_hFile = INVALID_HANDLE_VALUE;
}

MP3TagParser::~MP3TagParser()
{
   CloseFile();
}

void MP3TagParser::ClearResult()
{
   memset(m_Title, 0, sizeof(m_Title));
   memset(m_Album, 0, sizeof(m_Album));
   memset(m_Artist, 0, sizeof(m_Artist));
   memset(m_Genre, 0, sizeof(m_Genre));
   m_bTitleAvail = m_bAlbumAvail = m_bArtistAvail = m_bGenreAvail = false;
   
   m_Rating = 0;
   m_PicDatOffset = -1;

   m_PicType = TAG_PIC_NONE;
}

void MP3TagParser::CloseFile()
{
   if(m_hFile != INVALID_HANDLE_VALUE)
      CloseHandle(m_hFile);

   	m_hFile = INVALID_HANDLE_VALUE;
}

bool MP3TagParser::OpenFile(const TCHAR* file_nm)
{ 
   ClearResult();
   
   m_hFile = CreateFile(file_nm, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
   if(m_hFile == INVALID_HANDLE_VALUE)
	  return false;

   	DWORD dwNumBytesRead;
	MP3ID3V1TAG sID3V1;
	ID3_FrameHeader_V22 fhV22;
	ID3_FrameHeader_V23 fhV23;

	char id3Tag[3];
	memset(id3Tag, 0 , sizeof(id3Tag));
	char strID3Text[MAX_TEXT_LEN+1];

	if(SetFilePointer(m_hFile,0,NULL,FILE_BEGIN) != 0xFFFFFFFF &&
		ReadFile(m_hFile,&id3Tag,3,&dwNumBytesRead,NULL) &&
		dwNumBytesRead == 3 && memcmp(id3Tag,"ID3",3) == 0) 
	{
		BYTE byMainVer = 0;
		ReadFile(m_hFile,&byMainVer,1,&dwNumBytesRead,NULL);
		SetFilePointer(m_hFile,10,NULL,FILE_BEGIN);
		if(byMainVer == 2)
		{
			while(GetFrameV22(m_hFile, &fhV22)) 
			{
	 			if(memcmp(fhV22.FrameID,"TT2",3) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					//*XuG: quick fix(edited by Yang 060301)
					if(fhV22.Size>MAX_TEXT_LEN)
					{
						ReadFile(m_hFile,&strID3Text,MAX_TEXT_LEN,&dwNumBytesRead,NULL);
						SetFilePointer(m_hFile, fhV22.Size - dwNumBytesRead,NULL,FILE_CURRENT);
					}
					else
					//*/
					ReadFile(m_hFile,&strID3Text,fhV22.Size,&dwNumBytesRead,NULL);
					m_bTitleAvail = GetAvail((char *)strID3Text,m_Title,sizeof(strID3Text),sizeof(m_Title));
				}
				else if(memcmp(fhV22.FrameID,"TP1",3) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					///*XuG: quick fix(edited by Yang 060301)
					if(fhV22.Size>MAX_TEXT_LEN)
					{
						ReadFile(m_hFile,&strID3Text,MAX_TEXT_LEN,&dwNumBytesRead,NULL);
						SetFilePointer(m_hFile, fhV22.Size - dwNumBytesRead,NULL,FILE_CURRENT);
					}
					else
					//*/
					ReadFile(m_hFile,&strID3Text,fhV22.Size,&dwNumBytesRead,NULL);
					m_bArtistAvail = GetAvail((char *)strID3Text,m_Artist,sizeof(strID3Text),sizeof(m_Artist));
				}
				else if(memcmp(fhV22.FrameID,"TAL",3) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					///*XuG: quick fix(edited by Yang 060301)
					if(fhV22.Size>MAX_TEXT_LEN)
					{
						ReadFile(m_hFile,&strID3Text,MAX_TEXT_LEN,&dwNumBytesRead,NULL);
						SetFilePointer(m_hFile, fhV22.Size - dwNumBytesRead,NULL,FILE_CURRENT);
					}
					else
					//*/
					ReadFile(m_hFile,&strID3Text,fhV22.Size,&dwNumBytesRead,NULL);
					m_bAlbumAvail = GetAvail((char *)strID3Text,m_Album,sizeof(strID3Text),sizeof(m_Album));
				}
				else if(memcmp(fhV22.FrameID,"TCO",3) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					////*XuG: quick fix(edited by Yang 060301)
					if(fhV22.Size>MAX_TEXT_LEN)
					{
						ReadFile(m_hFile,&strID3Text,MAX_TEXT_LEN,&dwNumBytesRead,NULL);
						SetFilePointer(m_hFile, fhV22.Size - dwNumBytesRead,NULL,FILE_CURRENT);
					}
					else
					//*/
					ReadFile(m_hFile,&strID3Text,fhV22.Size,&dwNumBytesRead,NULL);
                    m_bGenreAvail = GetAvail((char *)strID3Text,m_Genre,sizeof(strID3Text),sizeof(m_Genre));
					
					//zzhou(edited by Yang for Stack fault)
					int genre = _tcslen(m_Genre);
					if((genre>2) && ('('==m_Genre[0]) &&(')'==m_Genre[genre-1])
						&&(('0'<=m_Genre[1])&&('9'>=m_Genre[1])) )
					{
						genre=_ttoi(m_Genre+1);
						if(genre >=0 && genre < g_nGenreCount)
							_tcscpy(m_Genre, g_arrGenre[genre]);
					}
				}
				else if(memcmp(fhV22.FrameID,"PIC",3) == 0)
				{	
					DWORD dwCur = SetFilePointer(m_hFile, 0,NULL,FILE_CURRENT);
					char chPic = '\0';
					ReadFile(m_hFile,&chPic,1,&dwNumBytesRead,NULL);
					if(chPic == 'j' || chPic == 'J')
					{
						m_PicType = TAG_PIC_JPG;
						m_PicDatOffset = dwCur + 5;
						m_PicSiz = fhV22.Size - 5;
					}
					else if(chPic == 'p' || chPic == 'P')
					{
						m_PicType = TAG_PIC_PNG;
						m_PicDatOffset = dwCur + 5;
						m_PicSiz = fhV22.Size - 5;
					}
					else
					{
						m_PicType = TAG_PIC_NONE;
						m_PicDatOffset = -1;
						m_PicSiz = 0;
					}
					SetFilePointer(m_hFile, dwCur+fhV22.Size,NULL,FILE_BEGIN);
				}
				else
				{
					SetFilePointer(m_hFile, fhV22.Size,NULL,FILE_CURRENT);
				}
				
			}
		}
		else
		{	
			while(GetFrameV23(m_hFile, &fhV23)) 
			{
				if(memcmp(fhV23.FrameID,"TIT2",4) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					//XuG: quick fix
					if(fhV23.Size > MAX_TEXT_LEN)
					   fhV23.Size = MAX_TEXT_LEN;
					
					ReadFile(m_hFile,&strID3Text,fhV23.Size,&dwNumBytesRead,NULL);
					m_bTitleAvail = GetAvail((char *)strID3Text,m_Title,sizeof(strID3Text),sizeof(m_Title));
				}
				else if(memcmp(fhV23.FrameID,"TPE1",4) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					//XuG: quick fix
					if(fhV23.Size > MAX_TEXT_LEN)
					   fhV23.Size = MAX_TEXT_LEN;
					
					ReadFile(m_hFile,&strID3Text,fhV23.Size,&dwNumBytesRead,NULL);
                    m_bArtistAvail = GetAvail((char *)strID3Text,m_Artist,sizeof(strID3Text),sizeof(m_Artist));
				}
				else if(memcmp(fhV23.FrameID,"TALB",4) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					//XuG: quick fix
					if(fhV23.Size > MAX_TEXT_LEN)
					   fhV23.Size = MAX_TEXT_LEN;

					ReadFile(m_hFile,&strID3Text,fhV23.Size,&dwNumBytesRead,NULL);
					m_bAlbumAvail = GetAvail((char *)strID3Text,m_Album,sizeof(strID3Text),sizeof(m_Album));
				}
				else if(memcmp(fhV23.FrameID,"TCON",4) == 0)
				{
					memset(strID3Text, 0, sizeof(strID3Text));
					
					//XuG: quick fix
					if(fhV23.Size > MAX_TEXT_LEN)
					   fhV23.Size = MAX_TEXT_LEN;

					ReadFile(m_hFile,&strID3Text,fhV23.Size,&dwNumBytesRead,NULL);
                    m_bGenreAvail = GetAvail((char *)strID3Text,m_Genre,sizeof(strID3Text),sizeof(m_Genre));
					
					//zzhou(edited by Yang for Stack fault)
					int genre = _tcslen(m_Genre);
					if((genre>2) && ('('==m_Genre[0]) &&(')'==m_Genre[genre-1])
						&&(('0'<=m_Genre[1])&&('9'>=m_Genre[1])) )
					{
						genre=_ttoi(m_Genre+1);
						if(genre >=0 && genre < g_nGenreCount)
							_tcscpy(m_Genre, g_arrGenre[genre]);
					}
				}
				else if(memcmp(fhV23.FrameID,"APIC",4) == 0)
				{
					DWORD dwCur = SetFilePointer(m_hFile, 7,NULL,FILE_CURRENT);
					char chPic = '\0';
					ReadFile(m_hFile,&chPic,1,&dwNumBytesRead,NULL);
					if(chPic == 'j' || chPic == 'J')
					{
						m_PicType = TAG_PIC_JPG;
						m_PicDatOffset = dwCur + 7;
						m_PicSiz = fhV23.Size - 14;
					}
					else if(chPic == 'p' || chPic == 'P')
					{
						m_PicType = TAG_PIC_PNG;
						m_PicDatOffset = dwCur + 6;
						m_PicSiz = fhV23.Size - 13;
					}
					else
					{
						m_PicType = TAG_PIC_NONE;
						m_PicDatOffset = -1;
						m_PicSiz = 0;
					}
					SetFilePointer(m_hFile, dwCur + fhV23.Size - 7,NULL,FILE_BEGIN);
				}
				else
				{
					SetFilePointer(m_hFile, fhV23.Size,NULL,FILE_CURRENT);
				}
				
			}
		}

	}
	else
	{
		if (SetFilePointer(m_hFile,-128,NULL,FILE_END) != 0xFFFFFFFF &&// != INVALID_SET_FILE_POINTER &&
			ReadFile(m_hFile,&sID3V1,128,&dwNumBytesRead,NULL) &&
			dwNumBytesRead == 128 && memcmp(sID3V1.ident,"TAG",3) == 0)
		{
			char strTemp[31]; 
			strTemp[30] = 0;

			memcpy(strTemp,sID3V1.title,30); 
			memset(m_Title, 0, sizeof(m_Title));
			if(ANSIToUNICODE(strTemp, sizeof(strTemp), m_Title, sizeof(m_Title)))
			{
				m_bTitleAvail = true;
			}
			else
			{
				m_bTitleAvail = false;
			}

			memcpy(strTemp,sID3V1.artist,30); 
			memset(m_Artist, 0, sizeof(m_Artist));
			if(ANSIToUNICODE(strTemp, sizeof(strTemp), m_Artist, sizeof(m_Artist)))
			{
				m_bArtistAvail = true;
			}
			else
			{
				m_bArtistAvail = false;
			}

			memcpy(strTemp,sID3V1.album,30); 
			memset(m_Album, 0, sizeof(m_Album));
			if(ANSIToUNICODE(strTemp, sizeof(strTemp), m_Album, sizeof(m_Album)))
			{
				m_bAlbumAvail = true;
			}
			else
			{
				m_bAlbumAvail = false;
			}
			//*Yang 060209
			if(sID3V1.genre >=0 && sID3V1.genre < g_nGenreCount)
			{
				ASSERT(sizeof(m_Genre)>_tcslen(g_arrGenre[sID3V1.genre]));
				_tcscpy(m_Genre, g_arrGenre[sID3V1.genre]);
				m_bGenreAvail = true;
			}
			//*/end 060209
			else
			{
				m_bGenreAvail = false;
			}

		}
	}

   return true;
}

TCHAR* MP3TagParser::getTitle()
{
   if(m_bTitleAvail)
	  return m_Title;

   return NULL;
}

TCHAR* MP3TagParser::getArtist()
{
   if(m_bArtistAvail)
	  return m_Artist;

   return NULL;
}

TCHAR* MP3TagParser::getGenre()
{
   if(m_bGenreAvail)
	  return m_Genre;

   return NULL;
}

int MP3TagParser::getRating()
{
   return m_Rating;
}

TCHAR* MP3TagParser::getAlbum()
{
   if(m_bAlbumAvail)
	  return m_Album;

   return NULL;
}

bool MP3TagParser::getPicInf(TAG_PIC_TYPE& type, int& rawsiz)
{
    type = m_PicType;
	rawsiz = m_PicSiz;

	if(m_PicSiz > 0)
	   return true;
	
	return false;
}

bool MP3TagParser::getPicDat(BYTE* pBuf)
{
    ASSERT(pBuf);
	if((m_hFile == INVALID_HANDLE_VALUE) || (m_PicType == TAG_PIC_NONE) || !pBuf)
	   return false;
	
	DWORD siz;
	if(SetFilePointer(m_hFile, m_PicDatOffset, NULL, FILE_BEGIN) != (DWORD)m_PicDatOffset)
	   return false;

	if(ReadFile(m_hFile, pBuf, m_PicSiz, &siz, NULL) && ((int)siz == m_PicSiz))
	   return true;

	return false;
}	

void MP3TagParser::ChangeEndian(void* pBuffer, int nBufSize)
{
	if (!pBuffer || !nBufSize)
		return;

	char temp;
	for (int i = 0; i < nBufSize / 2; i++)
	{
		temp = ((char*)pBuffer)[i];
		((char*)pBuffer)[i] = ((char*)pBuffer)[nBufSize - i - 1];
		((char*)pBuffer)[nBufSize - i - 1] = temp;
	}
}

bool MP3TagParser::IsFrameV23(char* pch)
{
	bool bIsFram = false; 

	for(int i = 0; i < FRMAN_COUNT_V23; i++)
	{
		if(memcmp(pch, &ID3_FrameDefs_V23[i], 4) == 0)
		{
			bIsFram = true;
			break;
		}
	}
	
	return bIsFram;
}

bool MP3TagParser::IsFrameV22(char* pch)
{
	bool bIsFram = false;

	for(int i = 0; i < FRMAN_COUNT_V22; i++)
	{
		if(memcmp(pch, &ID3_FrameDefs_V22[i], 3) == 0)
		{
			bIsFram = true;
			break;
		}
	}
	
	return bIsFram;
}

bool MP3TagParser::GetFrameV23(HANDLE hFile, ID3_FrameHeader_V23* pFH)
{
	bool bReturn = true;
	DWORD dwNumBytesRead = 0;
	if(ReadFile(hFile,pFH, sizeof(ID3_FrameHeader_V23),&dwNumBytesRead,NULL)
		&& dwNumBytesRead == sizeof(ID3_FrameHeader_V23))
	{
		ChangeEndian(&pFH->Size,4);
		if(!IsFrameV23(pFH->FrameID))
		{
			bReturn = false;
		}
	}
	else
	{
		bReturn = false;
	}

	return bReturn;
}

bool MP3TagParser::GetFrameV22(HANDLE hFile, ID3_FrameHeader_V22* pFH)
{
	bool bReturn = true;
	DWORD dwNumBytesRead = 0;
	pFH->Size = 0;
	if(ReadFile(hFile,pFH, sizeof(ID3_FrameHeader_V22) - 1,&dwNumBytesRead,NULL)
		&& dwNumBytesRead == sizeof(ID3_FrameHeader_V22) - 1)
	{
		ChangeEndian(&pFH->Size,3);
		if(!IsFrameV22(pFH->FrameID))
		{
			bReturn = false;
		}
	}
	else
	{
		bReturn = false;
	}

	return bReturn;
}

bool MP3TagParser::GetAvail(char *ID3Text,TCHAR m[MAX_TEXT_LEN+1],int len,int m_len)
{
	//zzhou: for unicode
    if(memcmp(ID3Text,"\x01\xFF\xFE",3) == 0)
	{
		memcpy(m, ID3Text + 3, len - 4);
    	return true;
	}
	else if(memcmp(ID3Text,"\x01\xFE\xFF",3) == 0)
	{
        memcpy(m, ID3Text + 4, len - 5);
		return true;
	}
	else if(ANSIToUNICODE(ID3Text + 1, len -1, m, m_len))
	{
		return true;
	}
		return false;
}  



