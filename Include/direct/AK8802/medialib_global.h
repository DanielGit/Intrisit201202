/**
 * @file medialib_global.h
 * @brief Define the global public types for media lib
 *
 * @Copyright (C) 2008 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author Su_Dan
 * @date 2007-08-20
 * @update date 2008-6-13
 * @version 2.0
 */

#ifndef _MEDIA_LIB_GLOBAL_H_
#define _MEDIA_LIB_GLOBAL_H_

#include "anyka_types.h"

typedef struct
{
	T_U16	ResourceID;
	T_U8	*Buff;
	T_U32	Resource_len;
}T_AUDIO_LOADRESOURCE_CB_PARA;

typedef void (*MEDIALIB_CALLBACK_FUN_PRINTF)(T_pCSTR format, ...);

#if 0
typedef T_S32	SEM_ID;
typedef SEM_ID (*MEDIALIB_CALLBACK_FUN_SEM_CREATE)(T_U32 nSemType, T_U32 nMaxLockCount);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_TAKE)(SEM_ID semID, T_S32 nTimeOut);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_GIVE)(SEM_ID semID);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_FLUSH)(SEM_ID semID);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_DELETE)(SEM_ID semID);
#endif

//typedef T_S32 (*MEDIALIB_CALLBACK_FUN_OPEN)(T_pVOID lpData);
typedef T_S32 (*MEDIALIB_CALLBACK_FUN_READ)(T_S32 hFile, T_pVOID buf, T_S32 size);
typedef T_S32 (*MEDIALIB_CALLBACK_FUN_WRITE)(T_S32 hFile, T_pVOID buf, T_S32 size);
typedef T_S32 (*MEDIALIB_CALLBACK_FUN_SEEK)(T_S32 hFile, T_S32 offset, T_S32 whence); 
typedef T_S32 (*MEDIALIB_CALLBACK_FUN_TELL)(T_S32 hFile);
//typedef void (*MEDIALIB_CALLBACK_FUN_CLOSE)(T_S32 hFile);

typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_MALLOC)(T_U32 size); 
typedef void (*MEDIALIB_CALLBACK_FUN_FREE)(T_pVOID mem);

typedef void (*MEDIALIB_CALLBACK_FUN_LOADRESOURCE)(T_AUDIO_LOADRESOURCE_CB_PARA *pPara);	//载入资源
typedef void (*MEDIALIB_CALLBACK_FUN_RELEASERESOURCE)(T_U8 *Buff);	//释放载入的资源

typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_RTC_DELAY) (T_U32 ulTicks);

//typedef void	(*MEDIALIB_CALLBACK_FUN_SHOW_FRAME)(T_pDATA srcImg, T_U16 src_width, T_U16 src_height);

typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_DMA_MEMCPY)(T_pVOID dest, T_pCVOID src, T_U32 size);
typedef void (*MEDIALIB_CALLBACK_FUN_MMU_INVALIDATEDCACHE)(void);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_CHECK_DEC_BUF)(T_pDATA pBuf);

//typedef void (*MEDIALIB_CALLBACK_FUN_AUDIO_ENDCALLBACK)(T_U8 end_type);

//typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_DMA_MALLOC)(T_U32 size); 
//typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_DMA_FREE)(T_pVOID mem);

typedef T_S32 (*MEDIALIB_CALLBACK_FUN_CMMBSYNCTIME)(void *pHandle, T_U32 timestamp);
typedef void (*MEDIALIB_CALLBACK_FUN_CMMBAUDIORECDATA)(void *pHandle, T_U8 *buf, T_S32 len);

typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_FILESYS_ISBUSY)(void);

typedef enum
{
	MEDIALIB_ROTATE_0,
	MEDIALIB_ROTATE_90,
	MEDIALIB_ROTATE_180,
	MEDIALIB_ROTATE_270
}T_eMEDIALIB_ROTATE;

typedef enum
{
	MEDIALIB_CHIP_UNKNOWN,		//default 3223
	MEDIALIB_CHIP_AK3223,		//3220, 3221, 3223, 3224 first version
	MEDIALIB_CHIP_AK3224,		//3224 metal fix, 3225
	MEDIALIB_CHIP_AK3610,		//sundance(3610 and 3620)
	MEDIALIB_CHIP_AK3810,		//aspen(3810), aspen2(7801 and 7802)
	MEDIALIB_CHIP_AK3610_2,		//sundance shrink(3631 and 3631L), 322L
	MEDIALIB_CHIP_AK8801,		//aspen3(8801 and 8802)
	MEDIALIB_CHIP_AK3671		//sundance2A
}T_eMEDIALIB_CHIP_TYPE;

//chip type in detail, only for audio and video module
#define MEDIALIB_CHIP_AK3221	MEDIALIB_CHIP_AK3223
#define MEDIALIB_CHIP_AK3225	MEDIALIB_CHIP_AK3224
#define MEDIALIB_CHIP_AK3620	MEDIALIB_CHIP_AK3610
#define MEDIALIB_CHIP_AK322L	MEDIALIB_CHIP_AK3610_2
#define MEDIALIB_CHIP_AK3631	MEDIALIB_CHIP_AK3610_2
#define MEDIALIB_CHIP_AK7801	MEDIALIB_CHIP_AK3810
#define MEDIALIB_CHIP_AK7802	MEDIALIB_CHIP_AK3810
#define MEDIALIB_CHIP_AK8802	MEDIALIB_CHIP_AK8801

#if 0
typedef enum
{
	MEDIALIB_SRAM,
	MEDIALIB_SDRAM
}T_eMEDIALIB_RAM_TYPE;
#endif

typedef enum
{
	I2S_UNUSE,
	I2S_DA,
	I2S_AD,
	I2S_DA_AD 
}T_AUDIO_I2S_APP;

typedef struct
{
	T_eMEDIALIB_CHIP_TYPE	m_ChipType;
	T_AUDIO_I2S_APP			m_AudioI2S;
}T_MEDIALIB_INIT_INPUT;

typedef struct
{
	MEDIALIB_CALLBACK_FUN_PRINTF			m_FunPrintf;
	MEDIALIB_CALLBACK_FUN_LOADRESOURCE		m_FunLoadResource;
	MEDIALIB_CALLBACK_FUN_RELEASERESOURCE	m_FunReleaseResource;
#if 0
	MEDIALIB_CALLBACK_FUN_SEM_CREATE		m_FunSemCreate;
	MEDIALIB_CALLBACK_FUN_SEM_TAKE			m_FunSemTake;
	MEDIALIB_CALLBACK_FUN_SEM_GIVE			m_FunSemGive;
	MEDIALIB_CALLBACK_FUN_SEM_FLUSH			m_FunSemFlush;
	MEDIALIB_CALLBACK_FUN_SEM_DELETE		m_FunSemDelete;
#endif

}T_MEDIALIB_INIT_CB;

typedef struct
{
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 

	T_U32	m_ulSize;
	T_U32	m_ulDecDataSize;
	T_U8	*m_pBuffer;
}T_AUDIO_DECODE_OUT;

typedef struct
{
	T_U32	m_ulSize;
	T_U16	m_uDispWidth;
	T_U16	m_uDispHeight;
	T_U8	*m_pBuffer;
	T_U16	m_uOriWidth;		//原始宽，不一定是16的倍数
	T_U16	m_uOriHeight;		//原始高，不一定是16的倍数
}T_VIDEO_DECODE_OUT;

typedef struct
{
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 
}T_AUDIO_OUT_INFO;

typedef struct
{
	T_U16	m_OutWidth;			//output width
	T_U16	m_OutHeight;		//output height
}T_VIDEO_OUT_INFO;

typedef struct
{
	T_S32	real_time;

	union {
		struct{
			T_U32 nCurBitIndex;
			T_U32 nFrameIndex;
		} m_ape;
	}m_Private;
}T_AUDIO_SEEK_INFO;

#ifndef NEW_AUDIO_DRIVER
#define NEW_AUDIO_DRIVER
#endif

#endif//_MEDIA_LIB_GLOBAL_H_
