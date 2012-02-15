/**
 * @file	SoundDevice.h
 * @brief	Anyka Sound Device Module interfaces header file.
 *
 * This file declare Anyka Sound Device Module interfaces.\n
 * Copyright (C) 2007 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author	Deng Zhou
 * @date	2007-08-14
 * @version V0.0.1
 * @ref
 */

#ifndef __SOUND_DEVICE_CODEC_H__
#define __SOUND_DEVICE_CODEC_H__

#include "medialib_global.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup AUDIOLIB Audio library
 * @ingroup ENG
 */
/*@{*/

/* @{@name Define audio version*/
/**	Use this to define version string */	
#define AUDIOCODEC_VERSION_STRING		(T_U8 *)"AudioCodec Version V0.5.10-svn2142"
/** @} */

#ifdef _WIN32
//#define _SD_MODULE_MIDI_SUPPORT
#define _SD_MODULE_MP3_SUPPORT
#define _SD_MODULE_ENC_MP3_SUPPORT
#define _SD_MODULE_WMA_SUPPORT
#define _SD_MODULE_APE_SUPPORT
#define _SD_MODULE_FLAC_SUPPORT
#define _SD_MODULE_PCM_SUPPORT
#define _SD_MODULE_ADPCM_SUPPORT
#define _SD_MODULE_ENC_ADPCM_SUPPORT
#define _SD_MODULE_AAC_SUPPORT
#define _SD_MODULE_OGG_VORBIS_SUPPORT
#define _SD_MODULE_AMR_SUPPORT
//#define _SD_MODULE_AMR_ENC_SUPPORT
#define _SD_MODULE_ENC_AAC_SUPPORT
#define _SD_MODULE_RA8LBR_SUPPORT
#endif

typedef enum
{
	_SD_MEDIA_TYPE_UNKNOWN ,
	_SD_MEDIA_TYPE_MIDI ,
	_SD_MEDIA_TYPE_MP3 ,
	_SD_MEDIA_TYPE_AMR ,
	_SD_MEDIA_TYPE_AAC ,
	_SD_MEDIA_TYPE_WMA ,
	_SD_MEDIA_TYPE_PCM ,
	_SD_MEDIA_TYPE_ADPCM_IMA ,
	_SD_MEDIA_TYPE_ADPCM_MS ,
	_SD_MEDIA_TYPE_ADPCM_FLASH ,
	_SD_MEDIA_TYPE_APE ,
	_SD_MEDIA_TYPE_FLAC ,
	_SD_MEDIA_TYPE_OGG_FLAC ,
	_SD_MEDIA_TYPE_RA8LBR ,
	_SD_MEDIA_TYPE_DRA,
	_SD_MEDIA_TYPE_OGG_VORBIS
}T_AUDIO_TYPE;

typedef enum
{
	_SD_BUFFER_FULL ,
	_SD_BUFFER_WRITABLE ,
	_SD_BUFFER_WRITABLE_TWICE ,
	_SD_BUFFER_ERROR
}T_AUDIO_BUF_STATE;

typedef struct
{
	MEDIALIB_CALLBACK_FUN_MALLOC			Malloc;
	MEDIALIB_CALLBACK_FUN_FREE				Free;
	MEDIALIB_CALLBACK_FUN_PRINTF			printf;
	MEDIALIB_CALLBACK_FUN_RTC_DELAY			delay;
	MEDIALIB_CALLBACK_FUN_CMMBSYNCTIME		cmmbsynctime;
	MEDIALIB_CALLBACK_FUN_CMMBAUDIORECDATA  cmmbaudiorecdata;
#ifdef FOR_SPOTLIGHT
	MEDIALIB_CALLBACK_FUN_READ				read;
	MEDIALIB_CALLBACK_FUN_SEEK				seek; 
	MEDIALIB_CALLBACK_FUN_TELL				tell;
#endif
}T_AUDIO_CB_FUNS;

typedef struct
{
#ifdef FOR_SPOTLIGHT
	T_S32	haudio;
#endif
	T_U32	m_Type;				//media type
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 

	union {
		struct{
			T_U32 nBlocksPerFrame;							// the samples in a frame (frames are used internally)
			T_U32 nFinalFrameBlocks;						// the number of samples in the final frame
			T_U32 nCompressionLevel;						// the compression level
			T_U32 nTotalFrames;								// the total number frames (frames are used internally)
			T_U32 nVersion;									// file version number * 1000 (3.93 = 3930)
			T_U32 m_nBlockAlign;
		} m_ape;
		struct {
			T_U32	nAvgBytesPerSec;
			T_U32	nBlockAlign;
			T_U32	nSamplesPerBlock;
			T_U16	nVersion;
			T_U16	nEncodeOpt;
#ifdef FOR_SPOTLIGHT
			T_U32	cbPacketSize;
			T_U32	cPackets; 
			T_U16	wAudioStreamId;
#endif
		} m_wma;
		struct {
			T_U32	nSamplesPerPacket;
			T_U32	streamflag;
		} m_adpcm;
		struct
		{
			T_U16	min_blocksize;
			T_U16	max_blocksize;
			T_U32	min_framesize;
			T_U32	max_framesize;
			T_U32	total_samples;
		}m_flac;
		struct
		{
			T_U32	nSamples;
			T_U32	nRegions;
			T_U32	nFrameBits;
			T_U32	cplStart;
			T_U32	cplQbits;
		}m_ra8lbr;
		struct
		{
			T_U32	cmmb_adts_flag;
		}m_aac;
		struct  
		{
			T_U32	nFileSize;
		} m_midi;
		struct  
		{
			char *	buff;
			T_U32	buff_len;
		}m_vorbis;	
	}m_Private;
}T_AUDIO_IN_INFO;

typedef struct
{
	T_AUDIO_CB_FUNS		cb_fun;
	T_AUDIO_IN_INFO		m_info;
}T_AUDIO_DECODE_INPUT;

typedef struct
{
	T_U8	*pwrite;	//pointer of write pos
	T_U32	free_len;	//buffer free length
	T_U8	*pstart;	//buffer start address
	T_U32	start_len;	//start free length
}T_AUDIO_BUFFER_CONTROL;


typedef enum{ AMR_ENC_MR475 = 0,
			AMR_ENC_MR515,
			AMR_ENC_MR59,
			AMR_ENC_MR67,
			AMR_ENC_MR74,
			AMR_ENC_MR795,
			AMR_ENC_MR102,
			AMR_ENC_MR122,

			AMR_ENC_MRDTX,

			AMR_ENC_N_MODES	/* number of (SPC) modes */

			} T_AUDIO_AMR_ENCODE_MODE ;


typedef struct
{
	T_U32	m_Type;			//media type
	T_U16	m_nChannel;		//������(2)��������(1)
	T_U16	m_BitsPerSample;//16 bit�̶�(16)
	T_U32	m_nSampleRate;	//������(8000)
	union{
		struct{
			T_AUDIO_AMR_ENCODE_MODE mode;
		}m_amr_enc;
		struct{
			T_U32 enc_bits;
		}m_adpcm;
		struct{
			T_U32 bitrate;
			T_BOOL mono_from_stereo;
		}m_mp3;
	}m_private;
}T_AUDIO_ENC_IN_INFO;

typedef struct
{
	T_U16	wFormatTag;
	T_U16	nChannels;
	T_U32	nSamplesPerSec;

	union {
		struct {
			T_U32	nAvgBytesPerSec;
			T_U16	nBlockAlign;
			T_U16	wBitsPerSample;
			T_U16	nSamplesPerPacket;
		} m_adpcm;
	}m_Private;
	
}T_AUDIO_ENC_OUT_INFO;

typedef struct
{
	void *buf_in;
	void *buf_out;
	T_U32 len_in;
	T_U32 len_out;
}T_AUDIO_ENC_BUF_STRC;

typedef struct
{
	T_AUDIO_CB_FUNS		cb_fun;
	T_AUDIO_ENC_IN_INFO		enc_in_info;
}T_AUDIO_REC_INPUT;

typedef enum 
{
	_SD_BM_NORMAL = 0,
	_SD_BM_ENDING = 1,
    _SD_BM_LIVE = 1,
} T_AUDIO_BUFFER_MODE;


/**
 * @brief	��ȡ������汾��Ϣ.
 * @author	Deng Zhou
 * @date	2008-04-21
 * @param	[in] void
 * @return	T_S8 *
 * @retval	���ؿ�汾��
 */
T_S8 *_SD_GetAudioCodecVersionInfo(void);

/**
 * @brief	���ý����������䴫�����������Ա��ڵ��ûص�ʹ��
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_input:
 * ������Ϣ������ṹ
 * @param	[in] void *pHandle:
 * ����ľ��
 * @return	void *
 * @retval	������Ƶ���ڲ�����ṹ��ָ�룬�ձ�ʾʧ��
 */
void _SD_SetHandle(void *audio_decode, void *pHandle);
/**
 * @brief	����Ƶ�����豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_input:
 * ������Ϣ������ṹ
 * @param	[in] audio_output:
 * Ҫ��pcm������ṹ
 * @return	void *
 * @retval	������Ƶ���ڲ�����ṹ��ָ�룬�ձ�ʾʧ��
 */
void *_SD_Decode_Open(T_AUDIO_DECODE_INPUT *audio_input, T_AUDIO_DECODE_OUT *audio_output);

/**
 * @brief	��Ƶ����.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] audio_output:
 * Ҫ��pcm������ṹ
 * @return	T_S32
 * @retval	������Ƶ����������Ƶ���ݴ�С����byteΪ��λ
 */
T_S32 _SD_Decode(void *audio_decode, T_AUDIO_DECODE_OUT *audio_output);

/**
 * @brief	�ر���Ƶ�����豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  �رճɹ�
 * @retval	AK_FLASE :  �ر��쳣
 */
T_S32 _SD_Decode_Close(void *audio_decode);

/**
 * @brief	��Ƶ����seek.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  seek�ɹ�
 * @retval	AK_FLASE :  seek�쳣
 */
T_S32 _SD_Decode_Seek(void *audio_decode, T_AUDIO_SEEK_INFO *seek_info);

/**
 * @brief	���ý��뻺�幤��ģʽ.
 * @author	Deng Zhou
 * @date	2009-8-7
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] bufmode:
 * ���幤��ģʽ
 * @return	
 */

T_S32 _SD_SetBufferMode(void *audio_decode, T_AUDIO_BUFFER_MODE buf_mode);

/**
 * @brief	��ȡwma���������ͣ�LPC/Mid/High rate����
 * @author	Li Jun
 * @date	2010-1-14
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return  ���ر��������ͣ�0/1/2�ֱ��ӦLPC/Mid/High rate	
 */
T_S32 _SD_GetWMABitrateType(void *audio_decode);
/**
 * @brief	�����Ƶ�����ڲ�������free�ռ��С.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] buffer_control:
 * ��Ƶ�����ڲ�������״̬�ṹ
 * @return	T_AUDIO_BUF_STATE
 * @retval	������״̬
 */
T_AUDIO_BUF_STATE _SD_Buffer_Check(void *audio_decode, T_AUDIO_BUFFER_CONTROL *buffer_control);

/**
 * @brief	������Ƶ�����ڲ�������дָ��.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] len:
 * ����Ƶ�����ڲ�������д�볤��
 * @return	T_S32
 * @retval	AK_TRUE : ���³ɹ�
 * @retval	AK_FLASE : ����ʧ��
 */
T_S32 _SD_Buffer_Update(void *audio_decode, T_U32 len);

/**
 * @brief	�����Ƶ�����ڲ�������.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE : ����ɹ�
 * @retval	AK_FLASE : ���ʧ��
 */
T_S32 _SD_Buffer_Clear(void *audio_decode);

/**
 * @brief	��¼���豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] enc_input:
 * ¼��������Ϣ�ṹ
 * @param	[in] enc_output:
 * ¼�������Ϣ�ṹ
 * @return	void *
 * @retval	¼����Ƶ�ڲ��ṹָ�룬Ϊ�ձ�ʾ��ʧ��
 */
void *_SD_Encode_Open(T_AUDIO_REC_INPUT *enc_input, T_AUDIO_ENC_OUT_INFO *enc_output);

/**
 * @brief	��¼�õ�pcm���ݽ��б���.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_record:
 * ¼����Ƶ���ڲ��ṹ
 * @param	[in] enc_buf_strc:
 * �������bufferָ�볤�Ƚṹ
 * @return	T_S32 
 * @retval	���볤��
 */
T_S32 _SD_Encode(void *audio_encode, T_AUDIO_ENC_BUF_STRC *enc_buf_strc);

/**
 * @brief	�ر�¼���豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_encode:
 * ¼����Ƶ���ڲ��ṹ
 * @return	T_S32 
 * @retval	AK_TRUE : �رճɹ�
 * @retval	AK_FALSE : �ر�ʧ��
 */
T_S32 _SD_Encode_Close(void *audio_encode);

/**
 * @brief	��ȡ�����ʱ��.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_codec:
 * �������ڲ��ṹ
 * @param	[in] codec_flag:
 * ������־ 0������ 1������
 * @return	T_S32 
 * @retval	��ȡ��ʱ��
 */
T_S32 _SD_GetCodecTime(void *audio_codec, T_U8 codec_flag);


/**
 * @brief	��ȡ��ǰ���뻺���е�����������.
 * @author	Li Jun
 * @date	2007-10-15
 * @param	[in] audio_codec:
 * �������ڲ��ṹ
 * @param	[in] T_U8 *pBuf:
 * �洢�����Ļ���
 * @param	[in] T_U32 *len:
 * �洢�����Ļ����������ĳ���
 * @return	void 
 */
void _SD_LogBufferSave(T_U8 *pBuf, T_U32 *len,void *audio_codec);

#ifdef __cplusplus
}
#endif

#endif
/* end of sounddevice.h */
/*@}*/
