/**
 * @file video_stream_lib.h
 * @brief This file provides H263/MPEG/FLV263 decoding and H263 encoding functions
 *
 * Copyright (C) 2008 Anyka (Guangzhou) Software Technology Co., LTD
 * @author Su Dan
 * @date 2008-2-4
 * @updata date 2008-5-20
 * @version 0.3.2
 * @version ever green group version: x.x
 * @note
	The following is an example to use H263/MPEG/FLV263 decoding APIs
   @code

	解码部分1：使用fifo进行同步解码(open_input.m_ulBufSize不能为0)
	T_pVOID g_hVS;
	T_VIDEOLIB_OPEN_INPUT open_input;
	T_S32 audio_timestamp;
	T_S32 video_timestamp;
	T_VIDEO_DECODE_OUT VideoDecOut;

	open_input.m_VideoType = VIDEO_DRV_MPEG;
	open_input.m_ulBufSize = 200*1024;
	open_input.m_bNeedSYN = AK_FALSE;

	open_input.m_CBFunc.m_FunPrintf = printf;
	open_input.m_CBFunc.m_FunMalloc = malloc;
	open_input.m_CBFunc.m_FunFree = free;
	open_input.m_CBFunc.m_FunDownBuf = AK_NULL;
	open_input.m_CBFunc.m_FunUpBuf = AK_NULL;

	g_hVS = VideoStream_Open(&open_input);
	if (AK_NULL == g_hVS)
	{
		return;
	}

	while(1)
	{
		if (pause_flag) //暂停后需要重新缓冲数据
		{
			if (VideoStream_Reset(g_hVS) == AK_FALSE) 
			{
				break;
			}
		}
		audio_timestamp = get_audio_timestap();	//由音频库实现
		video_timestamp = VideoStream_SYNDecode(g_hVS, &VideoDecOut, audio_timestamp);
		if (VideoDecOut.m_pBuffer != AK_NULL)
		{
			Display(VideoDecOut.m_pBuffer, 
			VideoDecOut.m_uDispWidth, 
			VideoDecOut.m_uDispHeight);
		}
		if (stop_flag)//检测到停止标志后退出while循环
		{
			break;
		}
	}
	VideoStream_Close(g_hVS);


	数据处理部分（即之前提到的回调函数）：
	T_BOOL video_call_back(DWORD timestamp_ms, void *data_pointer, DWORD length)
	{
		T_pDATA video_buf = AK_NULL;
		video_buf = VideoStream_GetAddr(g_hVS, length + 8);	//预留前8字节，因此要多加8
		if (AK_NULL == video_buf)
		{
			return AK_FALSE;
		}

		memcpy(video_buf + 4, &timestamp_ms, 4);//将timestamp放入5-8字节，1-4字节保留
		memcpy(video_buf + 8, data_pointer, length);//将视频数据拷入buffer

		return VideoStream_UpdateAddr(g_hVS, video_buf, length + 8);//更新视频数据
	}

*************************************************************

	//解码部分2：不使用fifo，输入一帧解码一帧(open_input.m_ulBufSize必须为0，对于linux版本无此限制)
	T_pVOID g_hVS;
	T_VIDEOLIB_OPEN_INPUT open_input;
	T_VIDEO_DECODE_OUT VideoDecOut;
	T_pVOID pVlcBuf;
	T_U32 vlc_len;
	T_S32 retval;

	open_input.m_VideoType = VIDEO_DRV_MPEG;
	open_input.m_ulBufSize = 0;
	open_input.m_bNeedSYN = AK_FALSE;

	open_input.m_CBFunc.m_FunPrintf = printf;
	open_input.m_CBFunc.m_FunMalloc = malloc;
	open_input.m_CBFunc.m_FunFree = free;
	open_input.m_CBFunc.m_FunMMUInvalidateDCache = MMU_InvalidateDCache;
	open_input.m_CBFunc.m_FunCleanInvalidateDcache = MMU_CleanInvalidateDcache;
	open_input.m_CBFunc.m_FunDownBuf = AK_NULL;
	open_input.m_CBFunc.m_FunUpBuf = AK_NULL;

	g_hVS = VideoStream_Open(&open_input);
	if (AK_NULL == g_hVS)
	{
		return;
	}

	while(1)
	{
		pVlcBuf = VideoStream_GetVlcPtr(g_hVS);
		vlc_len = get_vlc_data();	//获取码流数据
		retval = VideoStream_Decode(g_hVS, pVlcBuf, vlc_len, &VideoDecOut);
		if (VideoDecOut.m_pBuffer != AK_NULL)
		{
			Display(VideoDecOut.m_pBuffer, 
		VideoDecOut.m_uDispWidth, 
		VideoDecOut.m_uDispHeight);
		}
		if (stop_flag)//检测到停止标志后退出while循环
		{
			break;
		}
	}
	VideoStream_Close(g_ hVS);


	@endcode

	************************************************************

 * @note
	The following is an example to use H263 encoding APIs
   @code

	编码部分：
	T_pVOID g_hVS;
	T_VIDEOLIB_ENC_OPEN_INPUT open_input;
	T_VIDEOLIB_ENC_IO_PAR video_enc_io_param;
	
	T_U8 curr_buf[PIC_SIZE];
	T_U8 vlc_buf[STREAM_MAX_SIZE];
	T_U8 *out_pic = AK_NULL;
	
	open_input.m_VideoType = VIDEO_DRV_MPEG;
	open_input.m_ulWidth = 352;
	open_input.m_ulHeight = 288;
	
	open_input.m_CBFunc.m_FunPrintf = printf;
	open_input.m_CBFunc.m_FunMalloc = malloc;
	open_input.m_CBFunc.m_FunFree = free;
	open_input.m_CBFunc.m_FunMMUInvalidateDCache = MMU_InvalidateDCache;
	open_input.m_CBFunc.m_FunCleanInvalidateDcache = MMU_CleanInvalidateDcache;
	open_input.m_CBFunc.m_FunDownBuf = AK_NULL;
	open_input.m_CBFunc.m_FunUpBuf = AK_NULL;
	
	g_hVS = VideoStream_Enc_Open(&open_input);
	if (AK_NULL == g_hVS)
	{
		return;
	}
	
	video_enc_io_param.p_curr_data = curr_buf;
	video_enc_io_param.p_vlc_data = vlc_buf;
	video_enc_io_param.QP = 10;
	video_enc_io_param.mode = 0;
	video_enc_io_param.bInsertP = AK_FALSE;
	
	while (1)
	{
		video_timestamp = VideoStream_Enc_Encode(g_hVS, &video_enc_io_param);
		out_pic = VideoStream_Enc_GetDispData(g_hVS);
		if (out_pic != AK_NULL)
		{
			Display(out_pic, open_input.m_ulWidth, open_input.m_ulHeight);
		}
		video_enc_io_param.mode = 1;	//根据需要改变mode、QP、bInsertP等
		
		if (stop_flag)//检测到停止标志后退出while循环
		{
			break;
		}
	}

	VideoStream_Enc_Close(g_hVS);

	@endcode

***************************************************/


#ifndef _VIDEO_STREAM_CODEC_H_
#define _VIDEO_STREAM_CODEC_H_

#include "medialib_global.h"

#define VIDEO_LIB_VERSION		"Video Lib V0.6.1"

typedef enum
{
	VIDEO_DRV_UNKNOWN = 0,
	VIDEO_DRV_H263,
	VIDEO_DRV_MPEG,
	VIDEO_DRV_FLV263,
	VIDEO_DRV_H264
}T_eVIDEO_DRV_TYPE;

typedef T_S32 (*MEDIALIB_CALLBACK_FUN_SEMAPHORE)(T_VOID);

typedef struct
{
	MEDIALIB_CALLBACK_FUN_PRINTF				m_FunPrintf;

	MEDIALIB_CALLBACK_FUN_MALLOC				m_FunMalloc;
	MEDIALIB_CALLBACK_FUN_FREE					m_FunFree;

	MEDIALIB_CALLBACK_FUN_MMU_INVALIDATEDCACHE	m_FunMMUInvalidateDCache;
	MEDIALIB_CALLBACK_FUN_MMU_INVALIDATEDCACHE	m_FunCleanInvalidateDcache;

	MEDIALIB_CALLBACK_FUN_SEMAPHORE				m_FunDownBuf;
	MEDIALIB_CALLBACK_FUN_SEMAPHORE				m_FunUpBuf;
}T_VIDEOLIB_CB;

typedef struct
{
	T_eVIDEO_DRV_TYPE	m_VideoType;
	T_U32				m_ulBufSize;	//if 0 == m_ulBufSize, use VideoStream_Decode to decode;
	T_BOOL				m_bNeedSYN;
	T_VIDEOLIB_CB		m_CBFunc;
	T_eMEDIALIB_ROTATE	m_Rotate;
}T_VIDEOLIB_OPEN_INPUT;

typedef enum
{
	VIDEO_STREAM_OK,
	VIDEO_STREAM_SYS_ERR,	//such as malloc failed
	VIDEO_STREAM_IDR_ERR,	//idr frame not found
	VIDEO_STREAM_SYN_FAST,	//video time is too fast
	VIDEO_STREAM_CODEC_ERR,	//maybe data error or codec error
	VIDEO_STREAM_SYN_ERR	//audio and video pts is out of range, should be error
}T_eVIDEOLIB_ERROR;


/**
 * @brief Get video codec library version
 *
 * @author Su_Dan
 * @return const T_CHR *
 * @retval	version string
 */
const T_CHR *VideoLib_GetVersion(T_VOID);


/**
 * @brief Initial Videostream and allocate resource
 *
 * @author Su_Dan
 * @param	open_input		[in]	pointer of T_VIDEOLIB_OPEN_INPUT
 * @return T_pVOID
 * @retval 	handle of videostream
 */
T_pVOID VideoStream_Open(T_VIDEOLIB_OPEN_INPUT *open_input);


/**
 * @brief Close Videostream
 *
 * @author Su_Dan
 * @param	hVS		[in]	pointer returned by VideoStream_Open
 * @return T_BOOL
 * @retval	AK_TRUE		Close ok
 * @retval	AK_FALSE	Close fail
 */
T_BOOL VideoStream_Close(T_pVOID hVS);


/**
 * @brief Get Videostream error
 *
 * @author Su_Dan
 * @param	hVS		[in]	pointer returned by VideoStream_Open
 * @return T_eVIDEOLIB_ERROR
 * @retval	see T_eVIDEOLIB_ERROR struct
 */
T_eVIDEOLIB_ERROR VideoStream_GetLastError(T_pVOID hVS);

#ifndef LINUX_CODEC_SUPPORT
/**
 * @brief Reset Videostream
 *
 * @author Su_Dan
 * @param	hVS		[in]	pointer returned by VideoStream_Open
 * @return T_BOOL
 * @retval	AK_TRUE		Reset ok
 * @retval	AK_FALSE	Reset fail
 */
T_BOOL VideoStream_Reset(T_pVOID hVS);


/**
 * @brief Get address to fill video data
 *
 * @author Su_Dan
 * @param	hVS		[in]	pointer returned by VideoStream_Open
 * @param	size	[in]	video data size
 * @return T_pVOID
 * @retval	AK_NULL		Get fail
 * @retval	other		valid pointer
 */
T_pVOID VideoStream_GetAddr(T_pVOID hVS, T_U32 size);


/**
 * @brief Update address
 *
 * @author Su_Dan
 * @param	hVS		[in]	pointer returned by VideoStream_Open
 * @param	pInaddr	[in]	pointer returned by VideoStream_GetAddr
 * @param	size	[in]	video data size, must equal to input of VideoStream_GetAddr
 * @return T_BOOL
 * @retval	AK_TRUE		Update ok
 * @retval	AK_FALSE	Update fail
 */
T_BOOL VideoStream_UpdateAddr(T_pVOID hVS, T_pVOID pInaddr, T_U32 size);


/**
 * @brief Handle decoding
 *
 * @author Su_Dan
 * @param	hVS				[in]	pointer returned by VideoStream_Open
 * @param	pVideoDecOut	[in]	pointer of T_VIDEO_DECODE_OUT struct
 * @param	ulMilliSec		[in]	play time of system or audio
 * @return T_S32
 * @retval	< 0		decode fail
 * @retval	other	video time in millisecond
 */
T_S32 VideoStream_SYNDecode(T_pVOID hVS, T_VIDEO_DECODE_OUT *pVideoDecOut, T_S32 ulMilliSec);

#else

/**
 * @brief Get vlc buffer address
 *
 * @author Su_Dan
 * @param	hVS			[in]	pointer returned by VideoStream_Open
 * @return T_pVOID
 * @retval	AK_NULL		get fail
 * @retval	other		get ok
 */
T_pVOID VideoStream_GetVlcPtr(T_pVOID hVS);
#endif


/**
 * @brief Decode one frame
 *
 * @author Su_Dan
 * @param	hVS				[in]	pointer returned by VideoStream_Open
 * @param	pStreamBuf		[in]	stream buffer
 * @param	ulStreamLen		[in]	stream length
 * @param	pVideoDecOut	[in]	pointer of T_VIDEO_DECODE_OUT struct
 * @return T_S32
 * @retval	< 0		decode fail
 * @retval	other	decode ok
 */
T_S32 VideoStream_Decode(T_pVOID hVS, T_pVOID pStreamBuf, T_U32 ulStreamLen, T_VIDEO_DECODE_OUT *pVideoDecOut);


/***********************   encoder  **************************/
typedef struct
{
	T_eVIDEO_DRV_TYPE	m_VideoType;
	T_U32				m_ulWidth;		//宽度
	T_U32				m_ulHeight;		//高度
	T_U32				m_ulMaxVideoSize;	//存放码流的buffer空间大小，不可小于Width*Height/2，最好是512的整数倍
	T_VIDEOLIB_CB		m_CBFunc;
}T_VIDEOLIB_ENC_OPEN_INPUT;

typedef struct
{
	T_pDATA		p_curr_data;
	T_pDATA		p_vlc_data;
	T_S32			QP;
	T_U8			mode;
	T_BOOL			bInsertP;
}T_VIDEOLIB_ENC_IO_PAR;


/*
 * @brief Initial encoder to encode H263
 *
 * @author	Zhao_Xing
 * @param	open_input	[in]	pointer of T_VIDEOLIB_ENC_OPEN_INPUT
 * @return 	T_pVOID
 * @retval	AK_NULL		open failed
 * @retval	other		handle of videostream
 */
T_pVOID VideoStream_Enc_Open(const T_VIDEOLIB_ENC_OPEN_INPUT *open_input);

/*
 * @brief close video encoder
 *
 * @author 	Zhao_Xing
 * @param	hVideo		[in]	pointer returned by VideoStream_Enc_Open
 * @return 	T_VOID
 */
T_VOID VideoStream_Enc_Close(T_pVOID hVS);

/*
 * @brief Encode one H263 frame
 *
 * @author	Zhao_Xing
 * @param	hVideo				[in]		pointer returned by VideoStream_Enc_Open
 * @param	video_enc_io_param	[in/out]	pointer of T_VIDEOLIB_ENC_IO_PAR
 * @return 	T_S32
 * @retval	< 0		fail
 * @retval	other	encoded stream length
 */
T_S32 VideoStream_Enc_Encode(T_pVOID hVS, T_VIDEOLIB_ENC_IO_PAR *video_enc_io_param);


/*
 * @brief get the recon picture
 *
 * @author 	Zhao_Xing
 * @param	hVideo		[in]	pointer returned by VideoStream_Enc_Open
 * @return 	T_pDATA
 * @retval	AK_NULL		get failed
 * @retval	other		pointer to the YUV recon data encoded
 */
T_pDATA VideoStream_Enc_GetDispData(T_pVOID hVS);

#endif//_VIDEO_STREAM_CODEC_H_
