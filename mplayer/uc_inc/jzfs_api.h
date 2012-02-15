/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
*/

#ifndef __JZFS_API_H__
#define __JZFS_API_H__

#ifdef __cplusplus
extern "C"{
#endif


// macros
typedef unsigned int                jzfs_size_t;

#define JZFS_SEEK_CUR               1
#define JZFS_SEEK_END               2
#define JZFS_SEEK_SET               0

// structs
typedef struct _jzfs_file
{
#ifndef NOAH_OS
	unsigned int    dwFlags;
	unsigned char  *pBase;
	unsigned char  *pTail;
	unsigned char   sData[128];
    unsigned int  (*fnRead) (void *pData,
                             unsigned int dwSize,
                             unsigned int dwCount,
                             struct _jzfs_file *pstFile);
    unsigned int  (*fnWrite)(const void *pData,
                             unsigned int dwSize,
                             unsigned int dwCount,
                             struct _jzfs_file *pstFile);
	unsigned int    dwIdLo;
	unsigned int    dwIdHi;
	unsigned int    dwIdEx;
	int             nEOFClust;
	unsigned int    dwCurClust;
	int             nFilePos;
	int             nSize;
	int             nFreeSpace;
	int             nDeviceIndex;
	short           error;
	unsigned char   inuse;
	unsigned char   mode_r;
	unsigned char   mode_w;
	unsigned char   mode_a;
	unsigned char   mode_c;
	unsigned char   mode_b;
	unsigned short  wCreatTime;
	unsigned short  wCreatDate;
#else
   short _flg;
   unsigned char _buf;
   int _fd;
#endif	
} JZFS_FILE;


// functions
JZFS_FILE          *jzfs_Open            (const char       *sFileName,
                                          const char       *sMode);

void                jzfs_Close           (JZFS_FILE        *pstFile);

unsigned int         jzfs_Read            (void             *pData,
                                          unsigned int       dwSize,
                                          unsigned int       dwCount,
                                          JZFS_FILE        *pstFile);

unsigned int         jzfs_Write           (const void       *pData,
                                          unsigned int       dwSize,
                                          unsigned int       dwCount,
                                          JZFS_FILE        *pstFile);

int                 jzfs_Seek            (JZFS_FILE        *pstFile,
                                          int               nOffset,
                                          int               nWhence);

int                 jzfs_Tell            (JZFS_FILE        *pstFile);

short               jzfs_Error           (JZFS_FILE        *pstFile);

void                jzfs_ClearError      (JZFS_FILE        *pstFile);

int                 jzfs_Remove          (const char       *sFileName);

int                 jzfs_IoCtl           (const char       *sDevName,
                                          int               nCmd,
                                          int               nAux,
                                          void             *pBuffer);
int                 jzfs_Init            (void);

int                 jzfs_Exit            (void);


#ifdef __cplusplus
}
#endif

#endif  //__JZFS_API_H__

