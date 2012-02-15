/**
 *@file movie_player_Lib.h
 * @brief This file provides MP4/3GP/AVI playing and AVI/3GP recording functions
 *
 * Copyright (C) 2006 Anyka (Guangzhou) Software Technology Co., LTD
 * @author Su Dan
 * @date 2006-10-23
 * @version 1.0.0
 * @version ever green group version: x.x
 * @note
	The following is an example to use MP4 playing APIs
  @code
	T_S32 fid;
	T_MOVIE_INFO info;
	T_eMOVIE_STATUS MOVIE_status;
	T_U8 preview_buffer[FRAME_SIZE];
	T_CONFIG_TYPE config_type;

	init();	// initial file system and lcd
	fid = FS_FileOpen("/test.3gp",0,FS_MODE_READ);

	config_type.ram_type = SDRAM_TYPE;
	config_type.capability_test = 0;
	mem = Fwl_Malloc(3*1024*1024);
	if (MOVIE_Init(mem, 3*1024*1024, 0, printf, &config_type) != AK_TRUE)
	{
		printf("Init MP4 Lib error!\r\n");
	}
	if (MOVIE_Open(fid) != MOVIE_OK)
	{
		printf("Play Failure\r\n");
	}
	MOVIE_GetInfo(&info);
	set_paint();	// set paint parameter for display
	
	If (!MOVIE_Preview(preview_buffer))
	{
		printf("Preview fail!\r\n");
	}
	DisplayFrame(preview_buffer, info.ori_width, info.ori_height);

	MOVIE_Play();
	while(1)
	{
		MOVIE_status = MOVIE_Handle();
		if (MOVIE_STOP == MOVIE_status)
		{
			break;
		}
	}
	MOVIE_Stop();
	MOVIE_Close();
	MOVIE_Destroy();
	FS_FileClose(fid);
	
	@endcode

* @note
	The following is an example to use MP4 recording APIs
  @code
	T_S32 fid;
	int count;
	T_MOVIE_REC_PARAM avi_rec_param;
	T_eMOVIE_REC_STATUS rec_status;
	char *mem;
	char press_key;
	T_MOVIE_REC_INIT_PAR movie_rec_par;
	int i;
	
	fid = VD_FileOpen(filename, 1);

	if(fid <= 0)
	{
		printf("open file failed\r\n");
		return;
	}
	
	mem = VD_Malloc(3*1024*1024);

	avi_rec_param.width = 176;
	avi_rec_param.height = 144;
	avi_rec_param.fps = 10;
	avi_rec_param.bCaptureAudio = 0;
	avi_rec_param.keyframeInterval = 9;
	avi_rec_param.record_second = 100;

	avi_rec_param.bHighQuality = AK_TRUE;
	avi_rec_param.bIdxInMem = 1;//AK_TRUE;//AK_FALSE;

	movie_rec_par.pmem = mem;
	movie_rec_par.mem_size = 3*1024*1024;
	movie_rec_par.PrtFunc = printf;
	movie_rec_par.FileType = MOVIE_REC_3GP;

	//record
	MOVIE_Rec_Init(&movie_rec_par);
	MOVIE_Rec_Open(fid, &avi_rec_param);
	MOVIE_Rec_Start();

	count = 180;
	i = 0;
	while(count)
	{
		rec_status = MOVIE_Rec_Handle();
		if (MOVIE_REC_DOING != rec_status)
		{
			break;
		}
		i ++;
		printf("i = %d\r\n", i);
		press_key = getch();//_kbhit();
		switch(press_key)
		{
		case 's':
		case 'S':
			MOVIE_Rec_Stop();
			break;
		default:
			break;
		}
		tickcount += 1000/avi_rec_param.fps;
	}
	
	MOVIE_Rec_Stop();
	MOVIE_Rec_Close();
	MOVIE_Rec_Destroy();

	VD_FileClose(fid);
	VD_Free(mem);
  @endcode

 ***************************************************/


#ifndef __MOVIE_PLAYER_LIB_H__
#define __MOVIE_PLAYER_LIB_H__

#include <platform/platform.h>

/** @defgroup MOVIELIB Movie Library 
*/
/*@{*/
/*@}*/


/** @defgroup MOVIELIB API for playing MPEG4/H263 file
    @ingroup MOVIELIB
*/
/*@{*/

typedef enum
{
	MOVIE_END,
	MOVIE_PLAY,
	MOVIE_FF,
	MOVIE_FR,
	MOVIE_PAUSE,
	MOVIE_STOP,
	MOVIE_REW,
	MOVIE_SLOW,
	MOVIE_ERR,
	MOVIE_CLOSE
}T_eMOVIE_STATUS;

typedef enum
{
	MOVIE_OK,
	MOVIE_FORMAT_ERR,
	MOVIE_PARAM_ERR,
	MOVIE_SYSTEM_ERR,
	MOVIE_STATUS_ERR
}T_eMOVIE_ERR;

typedef enum
{
	MOVIE_VIDEO_UNKNOWN,
	MOVIE_VIDEO_MPEG4,
	MOVIE_VIDEO_H263,
	MOVIE_VIDEO_WMV1,
	MOVIE_VIDEO_WMV2,
	MOVIE_VIDEO_WMV3,
	MOVIE_VIDEO_FLV263,	
	MOVIE_VIDEO_H264,
	MOVIE_VIDEO_MJPEG	
}T_eMOVIE_VIDEO_CODE;

typedef enum
{
	MOVIE_AUDIO_UNKNOWN,
	MOVIE_AUDIO_AMR,
	MOVIE_AUDIO_MP3,
	MOVIE_AUDIO_AAC,
	MOVIE_AUDIO_PCM,
	MOVIE_AUDIO_WMA,
	MOVIE_AUDIO_ADPCM
}T_eMOVIE_AUDIO_CODE;

typedef enum
{
	MOVIE_FILE_UNKNOWN,
	MOVIE_FILE_AKV,
	MOVIE_FILE_AVI,
	MOVIE_FILE_WAV,
	MOVIE_FILE_MP4,
	MOVIE_FILE_3GP,
	MOVIE_FILE_M4A,
	MOVIE_FILE_MOV,
	MOVIE_FILE_ASF,
	MOVIE_FILE_REAL,
	MOVIE_FILE_MP3,
	MOVIE_FILE_NEW_AKV,
	MOVIE_FILE_AAC,
	MOVIE_FILE_AMR,
	MOVIE_FILE_MIDI,
	MOVIE_FILE_FLV
}T_eMOVIE_FILE_TYPE;

typedef enum
{
	SRAM_TYPE,
	SDRAM_TYPE
}T_eMOVIE_RAM_TYPE;

typedef struct
{
	T_eMOVIE_RAM_TYPE ram_type;
	T_U8 capability_test;		//capability_test 0 = disable  1 = enable
}T_CONFIG_TYPE;

typedef struct
{
	T_U32  SampleRate;		//sample rate, sample per second
	T_U16  Channels;		//channel number
	T_U16  BitsPerSample;	//bits per sample
	T_U32  AudioType;		//PCM flag.
}T_AUDIO_PARAM;

typedef struct
{
	T_eMOVIE_VIDEO_CODE video_code;
	T_eMOVIE_AUDIO_CODE audio_code;
	T_U16 ori_width;
	T_U16 ori_height;
	T_U16 fps;

	T_U32 cur_time_ms;
	T_U32 total_time_ms;
	
	T_eMOVIE_STATUS		movie_status;
	T_eMOVIE_FILE_TYPE	movie_file_type;
	T_BOOL				hasAudio;
	T_U32				video_bitrate;
	T_U32				audio_bitrate;
}T_MOVIE_INFO;


/**
 *************************************************************************
 *	Functions for playing MPEG4/H263 MOVIE file								   *
 **************************************************************************
*/

/**
 * @brief Initial MOVIE library and allocate resource
 *
 * @author Su_Dan
 * @param	pmem		[in]	Memory buffer for index data
 * @param	mem_size	[in]	Memory buffer size
 * @param	play_flag	[in]	play flag: 0 for play; 1 for preview; 
 * 								2 for play and without checking chip error; 3 for preview and without checking chip error
 * 								(2 and 3 are not recommended)
 * @param	PrtFunc		[in]	output function pointer
 * @param	config_type	[in]	the pointer for config of system
 * @return 	T_BOOL
 * @retval	AK_TRUE		Initial ok
 * @retval	AK_FALSE	Initial false
 */
T_BOOL MOVIE_Init(void *pmem, T_U32 mem_size, T_U8 play_flag, void *PrtFunc, T_CONFIG_TYPE *config_type);


/**
 * @brief Destroy MOVIE library and free resource
 *
 * @author Su_Dan
 * @return void
 */
void MOVIE_Destroy(void);
/**
 * @brief Open a file to play
 *
 * @author Su_Dan
 * @param  hFile MOVIE 	[in]	file handle
 * @return T_eMOVIE_ERR
 * @retval	MOVIE_OK		open ok
 * @retval	MOVIE_FORMAT_ERR	format error
 * @retval	MOVIE_PARAM_ERR		parameter error
 * @retval	MOVIE_SYSTEM_ERR	other error
 */
T_eMOVIE_ERR MOVIE_Open(T_S32 hMovieFile);


/**
 * @brief Close and release resource for playing a file
 * @author Su_Dan
 * @return T_BOOL
 * @retval AK_TRUE	Close ok
 * @retval AK_FALSE	Close fail
 */
T_BOOL MOVIE_Close(void);


/**
 * @brief Decode a frame and return image YUV data
 *
 * @author Su_Dan
 * @param	pImgYUV		[out]	Image buffer of YUV data
 * @return T_BOOL
 * @retval	AK_TRUE		preview ok
 * @retval	AK_FALSE	preview fail
 */
T_BOOL MOVIE_Preview(void *pImgYUV);


/**
 * @brief play movie from current position
 *
 * @author Su_Dan
 * @return T_eMOVIE_STATUS
 * @retval MOVIE_END		movie end
 * @retval MOVIE_PLAY		movie start to play
 */
T_eMOVIE_STATUS MOVIE_Play(void);

/**
 * @brief Stop playing the movie
 *
 * @author Su_Dan
 * @return T_eMOVIE_STATUS
 * @retval MOVIE_STOP		movie has come to an end
 */
T_eMOVIE_STATUS MOVIE_Stop(void);


/**
 * @brief Pause audio, then video will pause
 *
 * @author Su_Dan
 * @return T_eMOVIE_STATUS
 * @retval	MOVIE_PAUSE		pause ok
 * @retval	other			pause fail
 */
T_eMOVIE_STATUS MOVIE_Pause(void);


/**
 * @brief Set fast speed
 *
 * @author Su_Dan
 * @param	nMultiple	[in]	speed multiple
 * @return void
 */
void MOVIE_SetFastSpeed(T_U8 nMultiple);


/**
 * @brief Switch movie to fast forward status
 *
 * @author Su_Dan
 * @return T_BOOL
 * @retval	AK_FALSE	switch fail
 * @retval	AK_TRUE		switch ok
 */
T_BOOL MOVIE_FastForward(void);


/**
 * @brief Switch movie to fast reward status
 *
 * @author Su_Dan
 * @return T_BOOL
 * @retval	AK_FALSE	switch fail
 * @retval	AK_TRUE		switch ok
 */
T_BOOL MOVIE_FastReward(void);


/**
 * @brief Handle playing MOVIE
 *
 * @author Su_Dan
 * @return T_eMOVIE_STATUS
 * @retval	MOVIE_END		movie end
 * @retval	MOVIE_PLAY		movie start to play
 * @retval	MOVIE_STOP		movie stop
 * @retval	MOVIE_PAUSE		movie pause
 * @retval	MOVIE_FF		movie fast forward
 * @retval	MOVIE_ERR		play error
 */
T_eMOVIE_STATUS MOVIE_Handle(void);


/**
 * @brief Get current MOVIE information
 *
 * @author Su_Dan
 * @param pInfo	[out]	Information structure pointer
 * @return T_BOOL
 * @retval AK_FALSE	get information fail
 * @retval	AK_TRUE		get information ok
 */
T_BOOL MOVIE_GetInfo(T_MOVIE_INFO *pInfo);


/**
 * @brief Get current MOVIE playing status
 *
 * @author Su_Dan
 * @return T_eMOVIE_STATUS
 * @retval	MOVIE_END		movie end
 * @retval	MOVIE_PLAY		movie start to play
 * @retval	MOVIE_STOP		movie stop
 * @retval	MOVIE_PAUSE		movie pause
 * @retval	MOVIE_FF		movie fast forward
 * @retval	MOVIE_ERR		play error
 */
T_eMOVIE_STATUS MOVIE_Status(void);


/**
 * @brief Set movies position
 *
 * @author zhanghao
 * @param msTime	[in]	the movies millisecond
 * @param bDisp   	[in]	display or not
 * @return 	T_BOOL
 * @retval	AK_TRUE		set ok
 * @retval	AK_FALSE	set false
 */
T_BOOL MOVIE_SetPosition(T_S32 msTime, T_BOOL bDisp);


/**
 * @brief Check file type
 *
 * @author Su_Dan
 * @param	hMovieFile MOVIE	[in]	file handle
 * @return T_eMOVIE_FILE_TYPE
 * @retval	MOVIE_FILE_UNKNOWN	unknown file type
 * @retval	MOVIE_FILE_AKV		AKV file
 * @retval	MOVIE_FILE_AVI		AVI file
 * @retval	MOVIE_FILE_MP4		MP4 file
 ... ...
 */
T_eMOVIE_FILE_TYPE MOVIE_CheckFileType(T_S32 hMovieFile);

/*@}*/


/**
 ************************************************************************
 *	Functions for recording H263/PCM avi file									   *
 *************************************************************************
*/

/** @defgroup MOVIELIB API for recording H263/PCM avi file or H263/AMR 3gp file
    @ingroup MOVIELIB
*/
/*@{*/

typedef enum _MOVIE_Rec_Status
{
	MOVIE_REC_OPEN,
	MOVIE_REC_STOP,
	MOVIE_REC_CLOSE,
	MOVIE_REC_DOING,
	MOVIE_REC_SYSERR,
	MOVIE_REC_MEMFULL
}T_eMOVIE_REC_STATUS;

typedef enum
{
	MOVIE_REC_AVI_NORMAL,
	MOVIE_REC_AVI_CYC,
	MOVIE_REC_3GP,
	MOVIE_REC_AVI_SEGMENT
}T_eMOVIE_REC_TYPE;

typedef enum
{
	MOVIE_REC_H263,
	MOVIE_REC_MJPG,
	MOVIE_REC_MPEG4
}T_eMOVIE_ENCODE_TYPE;

typedef struct
{
	void	*pmem;
	T_U32	mem_size;
	void	*PrtFunc;
	
	T_eMOVIE_REC_TYPE	FileType;	//flag indicating file type	
}T_MOVIE_REC_INIT_PAR;

typedef struct
{
	T_U16	width;
	T_U16	height;
	T_U16	fps;
	T_U16	keyframeInterval;
	T_U16	record_second;		//add for time limit record
	
	T_BOOL	bCaptureAudio;
	T_BOOL	bIdxInMem;			//flag indicating Index is saved in memory
	T_BOOL	bHighQuality;		//for 3gp recording
	T_BOOL	bGrayImage;

	T_U32	index_mem_size;		//index size set by system
	T_U32	v_bps;
	T_eMOVIE_ENCODE_TYPE	videoType;
	T_U32	sector_size;		//add the variable for know a sector size,for example ,4k,8k.

	T_S32	m_hTmpFile;	//add for time limit record
	T_S32	m_Reserved;
}T_MOVIE_REC_PARAM;

typedef struct
{
	T_U16	ori_width;
	T_U16	ori_height;
	T_U16	fps;
	T_U16	keyframeInterval;
	T_BOOL	bCaptureAudio;

	T_U32	total_frames;
	T_U32	total_video_frames;

	T_eMOVIE_REC_STATUS record_status;
	
	T_U16	record_second;			//add for time limit record

	T_U32	info_bytes;

}T_MOVIE_REC_INFO;

typedef enum _MOVIE_Rec_Event
{
	MOVIE_REC_EV_NORMAL,
	MOVIE_REC_EV_EMERGENCY,
	MOVIE_REC_EV_NOTHING
}T_eMOVIE_REC_EVENT;


/**
 * @brief Initial MOVIE rec library and allocate resource
 *
 * @author Su_Dan
 * @param	rec_init_param	[in]	Initial parameter for movie recording
 * @return 	T_BOOL
 * @retval	AK_TRUE		Initial ok
 * @retval	AK_FALSE	Initial false
 */
T_BOOL MOVIE_Rec_Init(T_MOVIE_REC_INIT_PAR *rec_init_param);

/**
 * @brief Destroy MOVIE rec library and free resource
 *
 * @author Su_Dan
 * @return void
 */
void MOVIE_Rec_Destroy(void);

/**
 * @brief initialize recording driver
 *
 * @author Su_Dan
 * @param	hFile MOVIE	[in]	file handle
 * @param 	avi_rec_param	[in]	encode paramenter pointer
 * @return 	T_eMOVIE_ERR
 * @retval	MOVIE_OK		open ok
 * @retval	MOVIE_STATUS_ERR	status error
 * @retval	MOVIE_PARAM_ERR		parameter error
 * @retval	MOVIE_SYSTEM_ERR	other error
 */
T_eMOVIE_ERR MOVIE_Rec_Open(T_S32 hMovieFile, T_MOVIE_REC_PARAM *media_rec_param);

/**
 * @brief start recording
 * @author Su_Dan
 * @return T_eMOVIE_REC_STATUS
 * @retval MOVIE_REC_OPEN	init ok but record failed
 * @retval MOVIE_REC_CLOSE	the recording driver has not been opened 
 * @retval MOVIE_REC_DOING		recording
 */
T_eMOVIE_REC_STATUS MOVIE_Rec_Start(void);

/**
 * @brief Handle recording
 * @author Su_Dan
 * @return T_eMOVIE_REC_STATUS
 * @retval MOVIE_REC_OPEN	init ok
 * @retval MOVIE_REC_CLOSE	the recording driver has not been opened
 * @retval MOVIE_REC_STOP	already stopped recording
 * @retval MOVIE_REC_DOING		recording
 */
T_eMOVIE_REC_STATUS MOVIE_Rec_Handle(void);

/**
 * @brief stop recording
 *
 * @author Su_Dan
 * @return T_eMOVIE_REC_STATUS
 * @retval MOVIE_REC_OPEN	init ok
 * @retval MOVIE_REC_CLOSE	the recording driver has not been opened
 * @retval MOVIE_REC_STOP	already stopped recording
 */
T_eMOVIE_REC_STATUS MOVIE_Rec_Stop(void);


/**
 * @brief close recording driver
 *
 * @author Su_Dan
 * @return T_BOOL
 * @retval AK_TRUE	Close ok
 * @retval AK_FALSE	Close fail
 */
T_BOOL MOVIE_Rec_Close(void);


/**
 * @brief close recording info
 *
 * @author Su_Dan
 * @param  pRecInfo	[out]	Record Information pointer
 * @return T_BOOL
 * @retval AK_TRUE	get info ok
 * @retval AK_FALSE	get info fail
 */
T_BOOL MOVIE_Rec_GetInfo(T_MOVIE_REC_INFO *pRecInfo);


/**
 * @brief Restart recording
 *
 * @author Su_Dan
 * @param	hFile				[in]	file handle
 * @param	rec_event			[in]	see T_eMOVIE_REC_EVENT
 * @return T_eMOVIE_REC_STATUS
 * @retval	MOVIE_REC_OPEN		init ok but record failed
 * @retval	MOVIE_REC_CLOSE		the recording driver has not been opened
 * @retval	MOVIE_REC_DOING		recording
 * @retval	MOVIE_REC_SYSERR	start error
 */
T_eMOVIE_REC_STATUS MOVIE_Rec_Restart(T_S32 hFile, T_eMOVIE_REC_EVENT rec_event);


/**
 * @brief Set stop flag, only useful while MOVIE_REC_AVI_SEGMENT
 *
 * @author Su_Dan
 * @param	stop_flag	[in]	0: save current; 1: save all
 * @return T_BOOL
 * @retval	AK_TRUE		set ok
 * @retval	AK_FALSE	set fail
 */
T_BOOL MOVIE_Rec_SetStopFlag(T_U16 stop_flag);


/*@}*/


/**
 * @brief Callback function when audio playing ends. audio_EndCallback() must be called in this function
 *
 * @author Su_Dan
 * @param user_stop	[in]	flag for audio stop
 * @return void
 */
void audio_EndCallback(T_U8 user_stop);


/**
 * @brief Callback function when recording audio. audio_RecordCallback() must be called in this function
 *
 * @author Su_Dan
 * @param buff		[in]	encoded data buffer
 * @param bufflen	[in]	buffer length
 * @return void
 */
void audio_RecordCallback(T_U8 *buff, T_U32 bufflen);

/*@}*/


/**
 ************************************************************************
 *	Functions for addtional function: 4x scale, rotate, chip adaptive					   *
 *************************************************************************
*/

/** @defgroup addtional API for addtional function
    @ingroup MOVIELIB
*/
/*@{*/
typedef enum
{
	MOVIE_SCALE_1X,
	MOVIE_SCALE_4X
}T_eMOVIE_SCALE;

typedef enum
{
	MOVIE_ROTATE_0,
	MOVIE_ROTATE_90,
	MOVIE_ROTATE_180,
	MOVIE_ROTATE_270
}T_eMOVIE_ROTATE;

typedef enum
{
	VD_CHIP_UNKNOWN,	//default 3223
	VD_CHIP_AK322XM,	//3220, 3221, 3223
	VD_CHIP_AK3224M,	//3224
	VD_CHIP_SUNDANCE,	//3610 and 3620
	VD_CHIP_AK3631L,	//3631
	VD_CHIP_ASPEN,		//78xx
	VD_CHIP_SUNDANCE2A	//sundance2A
}T_eCHIP_TYPE;

//chip type in detail, only for video module
#define VD_CHIP_AK3221M	VD_CHIP_AK322XM
#define VD_CHIP_AK3223M	VD_CHIP_AK322XM
#define VD_CHIP_AK3225M	VD_CHIP_AK3224M
#define VD_CHIP_AK322L	VD_CHIP_AK3631L
#define VD_CHIP_AK3225L	VD_CHIP_AK3631L
#define VD_CHIP_AK3610	VD_CHIP_SUNDANCE
#define VD_CHIP_AK3620	VD_CHIP_SUNDANCE

/**
 * @brief Set scale parameter
 *
 * @author Su_Dan
 * @param	scale		[in]	param for scale, see T_eMOVIE_SCALE
 * @return T_BOOL
 * @retval 	AK_TRUE		set ok
 * @retval	AK_FALSE	set fail
 */
T_BOOL MOVIE_SetDispScale(T_eMOVIE_SCALE scale);


/**
 * @brief Set rotate mode for movie play
 *
 * @author Su_Dan
 * @param	rotate		[in]	rotate mode, see T_eMOVIE_ROTATE
 * @return T_BOOL
 * @retval 	AK_TRUE		set ok
 * @retval	AK_FALSE	set fail
 */
T_BOOL MOVIE_SetDispRotate(T_eMOVIE_ROTATE rotate);


/**
 * @brief Set chip type
 *
 * @author Su_Dan
 * @param	chip_type	[in]	chip type, see T_eCHIP_TYPE
 * @return T_BOOL
 * @retval 	AK_TRUE		set ok
 * @retval	AK_FALSE	set fail
 */
T_BOOL MOVIE_SetChipType(T_eCHIP_TYPE chip_type);


/**
 * @brief Get lib version
 *
 * @author Su_Dan
 * @return const T_U8 *
 */
const T_U8 *MOVIE_GetLibVersion(void);

/*@}*/

#endif

/**
 * end of movie_player_lib.h
 */

