/**
 * @file media_recorder_lib.h
 * @brief This file provides 3GP/AVI recording functions
 *
 * Copyright (C) 2009 Anyka (Guangzhou) Software Technology Co., LTD
 * @author Su Dan, Zeng Yong
 * @date 2008-4-9
 * @update date 2009-2-12
 * @version 0.1.1
 * @version ever green group version: x.x
 * @note
	The following is an example to use playing APIs
   @code
void record_media(char* filaname);
void main(int argc, char* argv[])
{
	T_MEDIALIB_INIT_CB init_cb;
	T_MEDIALIB_INIT_INPUT init_input;

	init();	// initial file system, memory, camera, lcd and etc.

	init_cb_func_init(&init_cb);	//initial callback function pointer

	init_input.m_ChipType = MEDIALIB_CHIP_AK8801;
	init_input.m_AudioI2S = I2S_UNUSE;

	if (MediaLib_Init(&init_input, &init_cb) == AK_FALSE)
	{
		return;
	}
	//above only call one time when system start

	//play film or music
	record_media(argv[1]);

	//below only call one time when system close
	MediaLib_Destroy();
	return;
}

void record_media(char *filename)
{
	T_S32 fid;

	T_MEDIALIB_REC_OPEN_INPUT rec_open_input;
	T_MEDIALIB_REC_OPEN_OUTPUT rec_open_output;

	T_eMEDIALIB_REC_STATUS rec_status;
	char press_key;
	T_U8 tmp_buf[1600];
	T_MEDIALIB_REC_INFO RecInfo;
	void *hMedia;
//	T_pDATA pVideo;
	T_S32 video_time;
	T_U8 *pYUV1;
	T_U8 *pYUV2;
	T_U32 audio_tytes = 0;

	fid = FileOpen(filename);
	if(fid <= 0)
	{
		printf("open file failed\r\n");
		return;
	}

	memset(&rec_open_input, 0, sizeof(T_MEDIALIB_REC_OPEN_INPUT));

	rec_open_input.m_hMediaDest = fid;
	rec_open_input.m_bCaptureAudio = 1;
	rec_open_input.m_bHighQuality = 1;
	rec_open_input.m_bIdxInMem = 1;
	rec_open_input.m_IndexMemSize = 0;
	rec_open_input.m_RecordSecond = 0;
	rec_open_input.m_MediaRecType = MEDIALIB_REC_AVI_NORMAL;//or MEDIALIB_REC_3GP;
// set video open info
	rec_open_input.m_VideoRecInfo.m_nFPS = 10;
	rec_open_input.m_VideoRecInfo.m_nWidth = 352;
	rec_open_input.m_VideoRecInfo.m_nHeight = 288;
	rec_open_input.m_VideoRecInfo.m_nKeyframeInterval = 19;
	rec_open_input.m_VideoRecInfo.m_nvbps = 600*1024;
	rec_open_input.m_VideoRecInfo.m_eVideoType = MEDIALIB_V_ENC_MJPG;//or MEDIALIB_V_ENC_H263;
	rec_open_input.m_SectorSize = 2048;
	rec_open_input.m_ExFunEnc = VD_EXfunc_YUV2JPEG;//set mjpeg encode function
	
// set audio open info
	rec_open_input.m_AudioRecInfo.m_BitsPerSample = 16;
	rec_open_input.m_AudioRecInfo.m_nChannel = 1;
	rec_open_input.m_AudioRecInfo.m_nSampleRate = 8000;
	rec_open_input.m_AudioRecInfo.m_ulDuration = 1000;
	rec_open_input.m_AudioRecInfo.m_Type = _SD_MEDIA_TYPE_PCM;
	
	open_cb_func_init(&(rec_open_input.m_CBFunc));	//initial callback function pointer;
	
	hMedia = MediaLib_Rec_Open(&rec_open_input,&rec_open_output);
	if (AK_NULL == hMedia)
	{
		printf("##MOVIE: MediaLib_Rec_Open Return NULL\r\n");
		FileClose(fid);
		return;
	}

	if (MediaLib_Rec_GetInfo(hMedia, &RecInfo) == AK_FALSE)
	{
		MediaLib_Rec_Close(hMedia);
		FileClose(fid);
		return;
	}

	if (AK_FALSE == MediaLib_Rec_Start(hMedia))
	{
		MediaLib_Rec_Close(hMedia);
		FileClose(fid);
		return;
	}

	while (AK_NULL == pYUV1)
	{
		pYUV1 = get_yuv_data();
	}

	while(1)
	{
		if (rec_open_input.m_bCaptureAudio)
		{
			audio_tytes = get_audio_data(tmp_buf);//get audio data from audio encoder
			if (audio_tytes ! = 0)
			{
				if(MediaLib_Rec_ProcessAudio(hMedia, (T_U8 *)tmp_buf, audio_tytes) == AK_FALSE)
				{
					printf("MediaLib_Rec_ProcessAudio error\r\n");
					break;
				}
			}
		}
		tickcount = get_system_time_ms();//get current time in ms from starting recording
		pYUV2 = get_yuv_data();
		if (pYUV2 != AK_NULL)
		{
			pYUV1 = pYUV2;
		}
		video_time = MediaLib_Rec_ProcessVideo(hMedia, pYUV1, tickcount);
		if (video_time < 0)
		{
			printf("MediaLib_Rec_ProcessVideo error\r\n");
			break;
		}

		press_key = is_stop_button();//check whether stop
		if (press_key)
		{
			break;
		}
		rec_status = MediaLib_Rec_GetStatus(hMedia);
		if (MEDIALIB_REC_DOING != rec_status)
		{
			break;
		}
	}

	MediaLib_Rec_Stop(hMedia);
	MediaLib_Rec_Close(hMedia);

	FileClose(fid);

	return;
}
	@endcode

 ***************************************************/

#ifndef _MEDIA_RECORDER_LIB_H_
#define _MEDIA_RECORDER_LIB_H_

#include "medialib_struct.h"

/**
 * @brief Open a resource
 *
 * @author Su_Dan
 * @param	rec_open_input		[in]	pointer of T_MEDIALIB_REC_OPEN_INPUT struct
 * @param	rec_open_output		[out]	pointer of T_MEDIALIB_REC_OPEN_OUTPUT struct
 * @return T_MEDIALIB_STRUCT
 * @retval	AK_NULL		open failed
 * @retval	other		open ok
 */
T_MEDIALIB_STRUCT MediaLib_Rec_Open(const T_MEDIALIB_REC_OPEN_INPUT *rec_open_input, T_MEDIALIB_REC_OPEN_OUTPUT *rec_open_output);


/**
 * @brief Close recorder
 *
 * @author Su_Dan
 * @param	hMedia		[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @return T_BOOL
 * @retval	AK_TRUE		Close ok
 * @retval	AK_FALSE	Close fail
 */
T_BOOL MediaLib_Rec_Close(T_MEDIALIB_STRUCT hMedia);


/**
 * @brief Start recording
 *
 * @author Su_Dan
 * @param	hMedia		[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @return T_BOOL
 * @retval	AK_TRUE		Start ok
 * @retval	AK_FALSE	Start fail
 */
T_BOOL MediaLib_Rec_Start(T_MEDIALIB_STRUCT hMedia);


/**
 * @brief Stop recording
 *
 * @author Su_Dan
 * @param	hMedia		[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @return T_BOOL
 * @retval	AK_TRUE		Stop ok
 * @retval	AK_FALSE	Stop fail
 */
T_BOOL MediaLib_Rec_Stop(T_MEDIALIB_STRUCT hMedia);


/**
 * @brief process audio data
 *
 * @author Su_Dan
 * @param	hMedia		[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @return T_BOOL
 * @retval	AK_TRUE		process ok
 * @retval	AK_FALSE	process fail
 */
T_BOOL MediaLib_Rec_ProcessAudio(T_MEDIALIB_STRUCT hMedia, T_U8 *pAudioData, T_U32 ulAudioSize);


/**
 * @brief process video data
 *
 * @author Su_Dan
 * @param	 hMedia			[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @param	 pVideoData		[in]	pointer of T_VIDEO_DECODE_OUT struct
 * @param	 ulMilliSec		[in]	audio or system time in millisecond
 * @return T_S32
 * @retval	< 0		encode video fail
 * @retval	other	video time in millisecond
 */
T_S32 MediaLib_Rec_ProcessVideo(T_MEDIALIB_STRUCT hMedia, T_U8 *pVideoData, T_S32 ulMilliSec);


/**
 * @brief Get current recording status
 *
 * @author Su_Dan
 * @param	hMedia		[in]		pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @return T_eMEDIALIB_REC_STATUS
 * @retval MEDIALIB_REC_OPEN		the recording driver has not been opened
 * @retval MEDIALIB_REC_STOP		the recording driver has not been stopped
 * @retval MEDIALIB_REC_DOING		recording
 * @retval MEDIALIB_REC_SYSERR		system error while recording
 * @retval MEDIALIB_REC_MEMFULL		memory full error while recording
 * @retval MEDIALIB_REC_SYNERR		audio and video syn error while recording
 */
T_eMEDIALIB_REC_STATUS MediaLib_Rec_GetStatus(T_MEDIALIB_STRUCT hMedia);


/**
 * @brief Get information
 *
 * @author Su_Dan
 * @param	hMedia		[in]	pointer of T_MEDIALIB_STRUCT struct which is returned by MediaLib_Rec_Open function
 * @param	pInfo		[out]	pointer of T_MEDIALIB_REC_INFO struct
 * @return T_BOOL
 * @retval	AK_TRUE		get info ok
 * @retval	AK_FALSE	get info fail
 */
T_BOOL MediaLib_Rec_GetInfo(T_MEDIALIB_STRUCT hMedia, T_MEDIALIB_REC_INFO *pInfo);

#endif
