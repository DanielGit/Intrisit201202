/**
 * @file	SoundFilter.h
 * @brief	Anyka Sound Device Module interfaces header file.
 *
 * This file declare Anyka Sound Device Module interfaces.\n
 * Copyright (C) 2008 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @version V0.0.1
 * @ref
 */

#ifndef __SOUND_FILTER_H__
#define __SOUND_FILTER_H__

#include "medialib_global.h"

/** @defgroup Audio Filter library
 * @ingroup ENG
 */
/*@{*/

/* @{@name Define audio version*/
/**	Use this to define version string */	
#define AUDIO_FILTER_VERSION_STRING		(T_U8 *)"AudioFilter Version V0.0.9"
/** @} */

#ifdef _WIN32
#define _SD_FILTER_EQ_SUPPORT    
#define _SD_FILTER_WSOLA_SUPPORT   
#define _SD_FILTER_3DSOUND_SUPPORT
#define _SD_FILTER_RESAMPLE_SUPPORT
#endif

typedef enum
{
	_SD_FILTER_UNKNOWN ,
	_SD_FILTER_EQ ,
	_SD_FILTER_WSOLA ,
	_SD_FILTER_RESAMPLE,
	_SD_FILTER_3DSOUND,
	_SD_FILTER_DENOICE
}T_AUDIO_FILTER_TYPE;

typedef enum
{
	_SD_EQ_MODE_NORMAL,
	_SD_EQ_MODE_CLASSIC,
	_SD_EQ_MODE_JAZZ,
    _SD_EQ_MODE_POP,
    _SD_EQ_MODE_ROCK,
    _SD_EQ_MODE_EXBASS,
    _SD_EQ_MODE_SOFT,
    _SD_EQ_USER_DEFINE,
} T_EQ_MODE;

#define _SD_EQ_MAX_BANDS 10

typedef enum
{
	_SD_WSOLA_0_5 ,
	_SD_WSOLA_0_6 ,
	_SD_WSOLA_0_7 ,
	_SD_WSOLA_0_8 ,
	_SD_WSOLA_0_9 ,
	_SD_WSOLA_1_0 ,
	_SD_WSOLA_1_1 ,
	_SD_WSOLA_1_2 ,
	_SD_WSOLA_1_3 ,
	_SD_WSOLA_1_4 ,
	_SD_WSOLA_1_5 ,
	_SD_WSOLA_1_6 ,
	_SD_WSOLA_1_7 ,
	_SD_WSOLA_1_8 ,
	_SD_WSOLA_1_9 ,
	_SD_WSOLA_2_0 
}T_WSOLA_TEMPO;

typedef enum
{
	_SD_OUTSR_48KHZ = 1,
	_SD_OUTSR_44KHZ,
	_SD_OUTSR_32KHZ,
	_SD_OUTSR_22KHZ,
	_SD_OUTSR_16KHZ,
	_SD_OUTSR_11KHZ,
	_SD_OUTSR_8KHZ
}T_RES_OUTSR;

typedef enum
{
	_SD_WSOLA_ARITHMATIC_0 , // 0:WSOLA, fast but tone bab
	_SD_WSOLA_ARITHMATIC_1   // 1:PJWSOLA, slow but tone well
}T_WSOLA_ARITHMATIC;

typedef struct
{
	MEDIALIB_CALLBACK_FUN_MALLOC			Malloc;
	MEDIALIB_CALLBACK_FUN_FREE				Free;
	MEDIALIB_CALLBACK_FUN_PRINTF			printf;
	MEDIALIB_CALLBACK_FUN_RTC_DELAY			delay;
}T_AUDIO_FILTER_CB_FUNS;

typedef struct
{
	T_U32	m_Type;				//media type
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 

	union {
		struct {
			T_EQ_MODE eqmode;	
			// Reserved for Old Interface
			T_U8     m_FilterNum ;    //Filter Number
	        double   m_g;		//gain by DB
			// New Interface
			T_U32 rectification;	// 0 ~ 1024, pre-amplified volume
			// For User Presets
			T_U32 bands;      //1~10
			T_U32 bandfreqs[_SD_EQ_MAX_BANDS];
			T_U32 bandgains[_SD_EQ_MAX_BANDS];
		} m_eq;
		struct {
			T_WSOLA_TEMPO tempo;
            T_WSOLA_ARITHMATIC arithmeticChoice;
		} m_wsola;
		struct{
			T_U8 is3DSurround;
		}m_3dsound;
		struct {
			//目标采样率 1:48k 2:44k 3:32k 4:22K 5:16K 6:11K 7:8K
			T_RES_OUTSR  outSrindex;

			//设置最大输入长度(bytes)，open时需要用作动态分配的依据。
			//后面具体调用重采样时，输入长度不能超过这个值
			T_U32 maxinputlen; 
		}m_resample;
	}m_Private;

}T_AUDIO_FILTER_IN_INFO;

typedef struct
{
	T_AUDIO_FILTER_CB_FUNS	cb_fun;
	T_AUDIO_FILTER_IN_INFO	m_info;
}T_AUDIO_FILTER_INPUT;

typedef struct
{
	void *buf_in;
	T_U32 len_in;
	void *buf_out;
	T_U32 len_out;
}T_AUDIO_FILTER_BUF_STRC;

//////////////////////////////////////////////////////////////////////////

/**
 * @brief	获取音效处理库版本信息.
 * @author	Deng Zhou
 * @date	2009-04-21
 * @param	[in] void
 * @return	T_S8 *
 * @retval	返回音效处理库版本号
 */
T_S8 *_SD_GetAudioFilterVersionInfo(void);


/**
 * @brief	打开音效处理设备.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] filter_input:
 * 音效处理的输入结构
 * @return	void *
 * @retval	返回音频库内部解码结构的指针，空表示失败
 */
void *_SD_Filter_Open(T_AUDIO_FILTER_INPUT *filter_input);

/**
 * @brief	音效处理.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] audio_filter:
 * 音效处理内部解码保存结构
 * @param	[in] audio_filter_buf:
 * 输入输出buffer结构
 * @return	T_S32
 * @retval	返回音频库解码出的音频数据大小，以byte为单位
 */
T_S32 _SD_Filter_Control(void *audio_filter, T_AUDIO_FILTER_BUF_STRC *audio_filter_buf);

/**
 * @brief	关闭音效处理设备.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] audio_decode:
 * 音效处理内部解码保存结构
 * @return	T_S32
 * @retval	AK_TRUE :  关闭成功
 * @retval	AK_FLASE :  关闭异常
 */
T_S32 _SD_Filter_Close(void *audio_filter);

/**
* @brief	设置音效参数:播放速度,EQ模式.
如果m_SampleRate,m_BitsPerSample,m_Channels三个有1个为0,则不改变任何音效,返回AK_TRUE
* @author	Wang Bo
* @date	2008-10-07
* @param	[in] audio_filter:
* 音效处理内部解码保存结构
* @param	[in] info:
* 音效信息保存结构
* @return	T_S32
* @retval	AK_TRUE :  设置成功
* @retval	AK_FLASE :  设置异常
*/
T_S32 _SD_Filter_SetParam(void *audio_filter, T_AUDIO_FILTER_IN_INFO *info);

#endif
/* end of soundfilter.h */
/*@}*/
