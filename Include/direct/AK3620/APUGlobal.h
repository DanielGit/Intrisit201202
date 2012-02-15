/**
 * @file SdGlobal.h
 *
 * @brief Anyka Sound Device data structure and macro definition header file.
 *
 * This file declare Anyka Sound Device data structure and macro. 
 * Copyright (C) 2005 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author 
 * @date 2005-01-24
 * @version 1.0	
 * @ref
 */

#ifndef __SOUND_DEVICE_GLOBAL_H__
#define	__SOUND_DEVICE_GLOBAL_H__

#include "APUConfig.h"

/** @{@name Set host working clock */
/**	Use this flag to set host working in 60 MHz */
#define _SD_WORKING_IN_60M			0	
/**	Use this flag to set host working in 84 MHz */
#define _SD_WORKING_IN_84M			1	
/**	Use this flag to set host working in 72 MHz */
#define _SD_WORKING_IN_72M			2	
/**	Use this flag to set host working in 90 MHz */
#define _SD_WORKING_IN_90M          3
/**	Use this flag to set host working in 92 MHz */
#define _SD_WORKING_IN_92M          4
/** @} */

/** @{@name define Boolean constant and NULL pointer constant */
/**	Use this flag to define Boolean TRUE */
#define _SD_FALSE					0
/**	Use this flag to define Boolean FALSE */
#define _SD_TRUE					1	
/**	Use this flag to define NULL value */
#define _SD_NULL					((void*)0)
/** @} */
#define _SD_ERROR                   2

/** @{@name Define Anyka sound device error coce, type: _SD_T_U16 */
/**	Use this flag to define Operation is successful */
#define _SD_OK						0x0000	
/**	Use this flag to define DSP is not run, it means a hardware error occurred */
#define _SD_DSP_NOT_RUN				0x000f
/**	Use this flag to define Operation is failed */
#define _SD_FAIL					0x00ff	
/** @} */

/** @{@name Define MIDI error code */
/**	Use this flag to identicate wrong MIDI format,only support MIDI type0 and type1 */
#define _SD_ERR_MID_FORMAT			0x0101	 
/**	Use this flag to identicate MIDI header is not "MThd" */
#define _SD_ERR_MID_HEADER			0x0102	
/**	Use this flag to identicate MIDI track header is not "MTrk" */
#define _SD_ERR_MID_TRACK_HEAD		0x0103  
/**	Use this flag to identicate Format is 0, but track number is more than 1 */
#define _SD_ERR_MID_FMT_TRACK		0x0104	 
/**	Use this flag to identicate The message type is not supported */
#define _SD_ERR_MID_MESSAGE			0x0105	  
/**	Use this flag to identicate Empty track */
#define _SD_ERR_MID_TRACK_EMPTY		0x0106	  
/**	Use this flag to identicate Track come to end */
#define _SD_ERR_MID_TRACK_ENDED		0x0107	 
/** @} */

/** @{@name Define MP3 error code */
/**	Use this flag to identicate the MP3 header is missing. the file is not a validated MP3 file */
#define _SD_ERR_MP3_FORMAT			0x0201	 
/**	Use this flag to identicate end of the MP3 file */
#define _SD_ERR_MP3_ENDED			0x0202	 
/**	Use this flag to identicate can't find the tag flag in MP3 file */
#define _SD_ERR_MP3_NO_TAG_FLAG		0x0203	
/**	Use this flag to identicate unsupport mp3 layer 1, layer 2 or MPEG2.5 */
#define _SD_ERR_MP3_NOT_SUPPORT		0x0204
/**	Use this flag to identicate can't find the MP3 header */ 
#define _SD_ERR_MP3_NO_HEADER		0x0205  
/**	Use this flag to identicate unsupported MPEG2.5 file */
#define _SD_ERR_MP3_IS_MPEG25		0x0206	 
/**	Use this flag to identicate not layer 3, unsupported */
#define _SD_ERR_MP3_NOT_LAYER3		0x0207	 
/**	Use this flag to identicate is free bit-rate, unsupported */
#define _SD_ERR_MP3_FREE_BITRATE	0x0208	 
/**	Use this flag to identicate find next frame failed */
#define _SD_ERR_MP3_NEXT_FRAME		0x0209	  
/** @} */

/** @{@name Define AMR error code */
/**	Use this flag to identicate error AMR format */
#define _SD_ERR_AMR_FORMAT			0x0301	 
/**	Use this flag to identicate not support AMR mode */
#define _SD_ERR_AMR_UNKOWN_MODE     0x0302  
/**	Use this flag to identicate not encoder,but try to send PCM */
#define _SD_ERR_AMR_NOT_ENCODER     0x0303  
/**	Use this flag to identicate encoder is too slow */
#define _SD_ERR_AMR_ENCODER_SLOW    0x0304
/** Use this flag to identicate anyka seek file */  
#define _SD_ERR_AMR_SEEK_FILE       0x0305
/** @} */

/** @{@name Define WAVE error code */
/**	Use this flag to identicate error WAVE format */
#define _SD_ERR_WAV_FORMAT			0x0401	 
/**	Use this flag to identicate error in request data */
#define _SD_ERR_WAV_REQUEST			0x0402  
/**	Use this flag to identicate wav encoder or decoder is finished */
#define _SD_ERR_WAV_ENDED			0x0403	 
/** @} */

/** @{@name Define Other error code */
/**	Use this flag to identicate Sound device has not been initialized */
#define _SD_ERR_DEVICE_NOT_INIT		0x1001	 
/**	Use this flag to identicate Sound device is busy */ 
#define _SD_ERR_DEVICE_BUSY			0x1002	
/**	Use this flag to identicate Unknown media type */
#define _SD_ERR_MEDIA_TYPE			0x1003	 
/**	Use this flag to identicate Wrong user parameter  */
#define _SD_ERR_PARAMETER			0x1004	
/**	Use this flag to identicate No opened media  */
#define _SD_ERR_NO_OPENED_MEDIA		0x1005	 
/**	Use this flag to identicate Do nothing */
#define _SD_ERR_DO_NOTHING			0x1006	 
/**	Use this flag to identicate File read failed */
#define _SD_ERR_FILE_READ			0x1007	 
/**	Use this flag to identicate File seek failed */
#define _SD_ERR_FILE_SEEK			0x1008	 
/**	Use this flag to identicate File tell failed */
#define _SD_ERR_FILE_TELL			0x1009	  
/** @} */

/** @{@name Define the media type */
/**	Use this flag to identicate Unknown media type */
#define _SD_MEDIA_TYPE_UNKNOWN		0		 
/**	Use this flag to identicate MIDI */
#define _SD_MEDIA_TYPE_MIDI			1	 
/**	Use this flag to identicate MP3 layer III */
#define _SD_MEDIA_TYPE_MP3			2	
/**	Use this flag to identicate AMR */
#define _SD_MEDIA_TYPE_AMR			3	
/**	Use this flag to identicate WAVE */
#define _SD_MEDIA_TYPE_WAVE			4	
/**	Use this flag to identicate WMA */
#define _SD_MEDIA_TYPE_WMA			5		 
/**	Use this flag to identicate PCM */
#define _SD_MEDIA_TYPE_PCM			6	 
/**	Use this flag to identicate Melody */
#define _SD_MEDIA_TYPE_MELODY		7	
/**	Use this flag to identicate MP3 layer I */
#define _SD_MEDIA_TYPE_MP3_I		8	 
/**	Use this flag to identicate MP3 layer II */
#define _SD_MEDIA_TYPE_MP3_II		9	
/**	Use this flag to identicate AAC */
#define _SD_MEDIA_TYPE_AAC			10
/** Use this flag to identicate NES */
#define _SD_MEDIA_TYPE_NES			11		
/** Use this flag to identicate Blue Tooth  */
#define _SD_MEDIA_TYPE_BT			12
/** Use this flag to identicate SNES  */
#define _SD_MEDIA_TYPE_SNES         13
//Use this flag to identicate low rate wma(with LPC)
#define _SD_MEDIA_TYPE_LPC_WMA		14
/** Use this flag to identicate FLAC  */
#define _SD_MEDIA_TYPE_FLAC_NATIVE  15
#define _SD_MEDIA_TYPE_FLAC_OGG     16
/** @} */

/** @{@name Define the AMR mode */
/**	Use this flag to identicate 4.75kbps mode */
#define _SD_AMR_MODE_MR475		    0       
/**	Use this flag to identicate 5.15kbps mode */
#define _SD_AMR_MODE_MR515		    1     
/**	Use this flag to identicate 5.9kbps mode */
#define _SD_AMR_MODE_MR59		    2   
/**	Use this flag to identicate 6.7kbps mode */
#define _SD_AMR_MODE_MR67		    3      
/**	Use this flag to identicate 7.4kbps mode */
#define _SD_AMR_MODE_MR74		    4      
/**	Use this flag to identicate 7.95kbps mode */
#define _SD_AMR_MODE_MR795		    5       
/**	Use this flag to identicate 10.2kbps mode */
#define _SD_AMR_MODE_MR102		    6       
/**	Use this flag to identicate 12.2kbps mode */
#define _SD_AMR_MODE_MR122		    7      
/** @} */

/** @{@name Define AMR codec */
/**	Use this flag to identicate only decode AMR data */
#define _SD_AMR_ONLY_DECODE         0     
/**	Use this flag to identicate only encode PCM data using AMR */
#define _SD_AMR_ONLY_ENCODE         1  
/**	Use this flag to identicate decode and encode at the same time */
#define _SD_AMR_ONLY_CODEC          2      
/** @} */

/**	Use this flag to identicate fast slow play tempo 0.5 */
#define _SD_TEMPO_00		0		
/**	Use this flag to identicate fast slow play tempo 0.6 */
#define _SD_TEMPO_01		1	
/**	Use this flag to identicate fast slow play tempo 0.7 */
#define _SD_TEMPO_02		2	
/**	Use this flag to identicate fast slow play tempo 0.8 */
#define _SD_TEMPO_03		3	
/**	Use this flag to identicate fast slow play tempo 0.9 */
#define _SD_TEMPO_04		4	
/**	Use this flag to identicate fast slow play tempo 1.0 */
#define _SD_TEMPO_05		5	
/**	Use this flag to identicate fast slow play tempo 1.1 */
#define _SD_TEMPO_06		6	
/**	Use this flag to identicate fast slow play tempo 1.2 */
#define _SD_TEMPO_07		7	
/**	Use this flag to identicate fast slow play tempo 1.3 */
#define _SD_TEMPO_08		8	
/**	Use this flag to identicate fast slow play tempo 1.4 */
#define _SD_TEMPO_09		9	
/**	Use this flag to identicate fast slow play tempo 1.5 */
#define _SD_TEMPO_10		10	

/** @{@name Define the volume macro */
/**	Use this flag to identicate minimum volume value */
#define _SD_VOLUME_MIN				0		
/**	Use this flag to identicate maximum volume value */
#define _SD_VOLUME_MAX				0x7fff	
/**	Use this flag to identicate default volume value e */
#define _SD_VOLUME_DEF				0x100
/** @} */

/** @{@name Define the EQ-mode macros */
/**	Use this flag to identicate normal EQ mode */
#define _SD_EQ_MODE_NORMAL			1		
/**	Use this flag to identicate classic EQ mode */
#define _SD_EQ_MODE_CLASSIC			2		
/**	Use this flag to identicate jazz EQ mode */
#define _SD_EQ_MODE_JAZZ			3	
/**	Use this flag to identicate pop EQ mode */
#define _SD_EQ_MODE_POP				4		
/**	Use this flag to identicate rock EQ mode */
#define _SD_EQ_MODE_ROCK			5		 
/**	Use this flag to identicate Filter EQ mode: mul 2 */
#define _SD_EQ_MODE_BRIGHT2			6		
/**	Use this flag to identicate Filter EQ mode: mul 4 + ASLC */
#define _SD_EQ_MODE_BRIGHT4			7	
#define _SD_EQ_SPEAKER_MODE_NORMAL			8		
#define _SD_EQ_SPEAKER_MODE_CLASSIC			9		
#define _SD_EQ_SPEAKER_MODE_JAZZ			10	
#define _SD_EQ_SPEAKER_MODE_POP				11		
#define _SD_EQ_SPEAKER_MODE_ROCK			12	

/** @} */

/** @{@name Define the MP3 MPEG Version macros */
/**	Use this flag to identicate unknown mpeg */
#define  _SD_MPEGVersion_Unknown	0
/**	Use this flag to identicate mpeg 1*/
#define  _SD_MPEGVersion_1			1
/**	Use this flag to identicate mpeg 2 */
#define  _SD_MPEGVersion_2          2
/**	Use this flag to identicate mpeg 3 */
#define  _SD_MPEGVersion_2_5		3
/** @} */

/** @{@name Define the channel macros */
/**	Use this flag to identicate left channel */
#define _SD_CHANNEL_LEFT			0		  
/**	Use this flag to identicate right channel */
#define _SD_CHANNEL_RIGHT			1		
/**	Use this flag to identicate stereo */
#define _SD_CHANNEL_STEREO			2	
/** @} */

/** @{@name Define the PLL macros */
/**	Use this flag to identicate PLL circuit is power down */
#define _SD_PLL_OFF					0	
/**	Use this flag to identicate PLL circuit is bypassed */
#define _SD_PLL_BYPASS				1	
/**	Use this flag to identicate PLL circuit is  not bypassed*/
#define _SD_PLL_NOT_BYPASS			2	
/** @} */

/** @{@name Define the amplifier macros */
/**	Use this flag to identicate Both class AB and D power amplifiers are power down */
#define _SD_AMPLIFIER_OFF			0		
/**	Use this flag to identicate The class AB power amplifier 
* is power on while the class D amplifier is power down */
#define _SD_AMPLIFIER_CLASS_AB		1		 
/**	Use this flag to identicate The class D power amplifier
* is power on while the class AB amplifier is power down */
#define _SD_AMPLIFIER_CLASS_D		2		
/** @} */

/** @{@name Define the headphone macros */
/**	Use this flag to identicate The headphone circuit is power down */
#define _SD_SPEAKER_ON				0	
/**	Use this flag to identicate The headphone circuit is power on */	
#define _SD_HEADPHONE_ON			1	
/**	Use this flag to identicate The headphone and speaker are all power on */	
#define _SD_ALL_ON					2
/**	Use this flag to identicate The speaker circuit is power off */
#define _SD_SPEAKER_OFF             3
/**	Use this flag to identicate The headphone circuit is power off */	
#define _SD_HEADPHONE_OFF           4
/**	Use this flag to identicate The speaker and headphone circuit is all power off */
#define _SD_ALL_OFF                 5

/** Use this flag to identicate enable headphone by pre_charge mode */
#define _SD_PRE_CHARGE_MODE         0
/** Use this flag to identicate enable headphone by linear mode */
#define _SD_LINEAR_MODE             1
/** @} */

/** Use this flag to indicate input from line in port */
#define _SD_LINEIN_ON	0
/** Use this flag to indicate input from mic in port */
#define _SD_MIC_ON	1

/** @{@name Define meta data length */
/**	Use this flag to identicate Maximum string length of the "song name" field */
#define _SD_MAX_SONG_NAME_LEN		30		
/**	Use this flag to identicate Maximum string length of the "artist name" field */
#define _SD_MAX_ART_NAME_LEN		30		
/**	Use this flag to identicate Maximum string length of the "album" field */
#define _SD_MAX_ALBUM_LEN			30		
/**	Use this flag to identicate Maximum string length of the "comment" field */
#define _SD_MAX_COMMENT_LEN			30	
/**	Use this flag to identicate Maximum string length of the "year" field */
#define _SD_MAX_YEAR_LEN			4		
/** @} */

/** @{@name Define the buffer status */
/**	Use this flag to identicate The buffer is full */
#define _SD_BUFFER_FULL				0	
/**	Use this flag to identicate The buffer is writable */	
#define _SD_BUFFER_WRITABLE			1	
/**	Use this flag to identicate The buffer can be writable twice */
#define _SD_BUFFER_WRITABLE_TWICE	2
/** @} */

#define DIFFERENTIAL_INPUT_MODE		1
#define SINGLE_ENDED_INPUT_MODE		2


/** Define sound device parameter data structure */ 
typedef struct
{
	_SD_T_U8 	PLL;				/**< PLL: _SD_PLL_OFF, _SD_PLL_BYPASS, or _SD_PLL_NOT_BYPASS(Default) */
	_SD_T_U8 	amplifier;			/**< amplifier class: _SD_AMPLIFIER_OFF, _SD_AMPLIFIER_CLASS_AB or _SD_AMPLIFIER_CLASS_D(Default) */
	_SD_T_U8 	headphone;			/**< headphone switch: _SD_HEADPHONE_OFF(Default) or _SD_HEADPHONE_ON */
} _SD_T_PARAM;


/** Define meta data structure */
typedef struct
{
	_SD_T_U8 	song_name[_SD_MAX_SONG_NAME_LEN+1];		/**< song name */
	_SD_T_U8 	artist_name[_SD_MAX_ART_NAME_LEN+1];	/**< artist name */
	_SD_T_U8 	album[_SD_MAX_ALBUM_LEN+1];				/**< album */
	_SD_T_U8 	comment[_SD_MAX_COMMENT_LEN+1];			/**< comment */
	_SD_T_U8 	year[_SD_MAX_YEAR_LEN+1];				/**< year(string mode) */
} _SD_T_META_INFO;

typedef struct {
    _SD_T_S32	offset;			// <0: continue, >=0: reset the offset
    _SD_T_U32	send_len;
    _SD_T_U32	write_len;		// out, how many bytes has been read.
    _SD_T_U8    *BufAddr;
    _SD_T_BOOL	syn_read_flag;	// _SD_TRUE: syn(read at once), _SD_FALSE: asy
    _SD_T_BOOL	end_flag;		// whether the file is end.
} _SD_T_DATA_SEND_CB_PARA;

typedef struct
{
#ifdef _SD_SUPPORT_FILE_SYSTEM
	_SD_T_FILE	pFile;		// to remember audio file handle
	_SD_T_U32	fOriginalOffset; // file absolute original offset 
#endif
	const _SD_T_U8	*pBuff;		// to remember audio data address
	_SD_T_U32	BuffLen;	// audio data length 
	_SD_T_U32	CurOffset;	// current absolute offset of the file 
	_SD_T_BOOL	bDataFromFile;	// audio data is form file or not  
}T_SD_REC_DATA;
 
typedef struct
{
	const _SD_T_U8	*pBuff;		// to remember audio data address
	_SD_T_U32	BuffLen;	// audio data length 
	_SD_T_U32	CurOffset;	// current absolute offset of the file 
} T_SD_DATA;

/************Resource*********************************************/
typedef struct {
    _SD_T_U16	ResourceID;
	_SD_T_U8	*Buff;          
	_SD_T_U32   Resource_len;  
}_SD_T_LOADRESOURCE_CB_PARA;

typedef enum
{
	_SD_CHIP_UNKOWN = 0,
    _SD_CHIP_AK3223,//for 3223
	_SD_CHIP_AK3224,  //also for 3225
	_SD_CHIP_AK322L, //also 3225L
	_SD_CHIP_AK3610, //also for 3620
	_SD_CHIP_AK3631L,
	_SD_CHIP_AK3671
}_SD_CHIP_ID;

extern _SD_CHIP_ID _SD_gChipID;

typedef enum
{
	_SD_REC_TYPE_WAVE = 0,
    _SD_REC_TYPE_TV_PCM = 101,
		_SD_REC_TYPE_AMR ,
		_SD_REC_TYPE_UNKOWMN,
    _SD_REC_TYPE_WAVE_LINE_IN,
    _SD_REC_TYPE_AMR_LINE_IN
}_SD_REC_MEDIA_TYPE;

typedef enum
{
	_AMR_REC_MODE_4750BPS = 0 ,
		_AMR_REC_MODE_5150BPS ,
		_AMR_REC_MODE_5900BPS ,
		_AMR_REC_MODE_6700BPS ,
		_AMR_REC_MODE_7400BPS ,
		_AMR_REC_MODE_7950BPS ,
		_AMR_REC_MODE_10200BPS ,
		_AMR_REC_MODE_12200BPS ,
		_AMR_REC_MODE_UNKOWMN ,
		_WAVE_REC_MODE_8000BPS,
		_WAVE_REC_MODE_11025BPS,
		_WAVE_REC_MODE_12000BPS,
		_WAVE_REC_MODE_16000BPS,
		_WAVE_REC_MODE_22050BPS,
		_WAVE_REC_MODE_24000BPS,
		_WAVE_REC_MODE_32000BPS,
		_WAVE_REC_MODE_44100BPS,
		_WAVE_REC_MODE_48000BPS

}_AUDIO_REC_MODE;

typedef enum
{
 
   RESOURCE_ID000_MIDI_DSP = 0,
   RESOURCE_ID001_MIDI_DRUM1  ,
   RESOURCE_ID002_MIDI_DRUM2  ,	
   RESOURCE_ID003_MIDI_DRUM3  ,		
   RESOURCE_ID004_MIDI_DRUM4  ,	
   RESOURCE_ID005_MIDI_WT0484 ,      
   RESOURCE_ID006_MIDI_WT0485 ,     
   RESOURCE_ID007_MIDI_WT0486 ,   
  
   
   RESOURCE_ID008_AMR_DSP_FASTSLOW ,
   RESOURCE_ID009_AMR_DATA122 ,
   RESOURCE_ID010_AMR_DATA475 ,
   RESOURCE_ID011_AMR_DATA795 ,
   RESOURCE_ID012_AMR_DATADTX ,
   RESOURCE_ID013_AMR_DATAOTHERMODE ,

   RESOURCE_ID014_AMR_DSP_475 ,
   RESOURCE_ID015_AMR_DSP_515,
   RESOURCE_ID016_AMR_DSP_59 ,
   RESOURCE_ID017_AMR_DSP_67 ,
   RESOURCE_ID018_AMR_DSP_74 ,
   RESOURCE_ID019_AMR_DSP_795 ,
   RESOURCE_ID020_AMR_DSP_102 ,
   RESOURCE_ID021_AMR_DSP_122 ,

   RESOURCE_ID022_MP3_DSP_LAYER1,
   RESOURCE_ID023_MP3_DSP_LAYER2,
   RESOURCE_ID024_MP3_DSP_LAYER3,
   RESOURCE_ID025_MP3_LAYER3_BDMA1,
   RESOURCE_ID026_MP3_LAYER3_BDMA2,
   RESOURCE_ID027_MP3_LAYER3_BDMA3,
   
   RESOURCE_ID028_WAVE_DSP_PLAY,
   RESOURCE_ID029_WAVE_DSP_ENCODE,

   RESOURCE_ID030_WMA_DSP,
   
   RESOURCE_ID031_AAC_DSP,
   RESOURCE_ID032_NES_DSP,
   RESOURCE_ID033_BT_DSP,
   RESOURCE_ID034_TV_REC_DSP,
   RESOURCE_ID035_SNES_DSP,

   RESOURCE_ID036_I2S_WAVE_DSP_ENCODE,
   RESOURCE_ID037_I2S_AMR_DSP_122,

   RESOURCE_ID038_LPC_WMA_DSP,

   RESOURCE_ID039_WMA_BDMA_16smOb,
   RESOURCE_ID040_WMA_BDMA_16ssOb,
   RESOURCE_ID041_WMA_BDMA_44smOb,
   RESOURCE_ID042_WMA_BDMA_44ssOb,
   RESOURCE_ID043_WMA_BDMA_44smQb,
   RESOURCE_ID044_WMA_BDMA_44ssQb,
   RESOURCE_ID045_WMA_BDMA_3tables,
   RESOURCE_ID046_WMA_BDMA_4tables,
   RESOURCE_ID047_WMA_BDMA_SqrtFraction,

   RESOURCE_ID048_AAC_HUFFMAN_TAB,
   RESOURCE_ID049_AAC_SINCOS_TAB,
   RESOURCE_ID050_AAC_SINWIN_TAB,
   RESOURCE_ID051_FLAC_DSP
}_SD_RESOUCE_ID;

typedef struct
{
    _SD_T_U32  m_SampleRate;		//sample rate, sample per second
    _SD_T_U16  m_Channels;			//channel number
    _SD_T_U16  m_BitsPerSample;		//bits per sample
    _SD_T_U32  m_Type;			//PCM flag.
}
_SD_T_MEDIA_INFO;


typedef struct 
{
	_SD_T_FILE   file;      //file
	_SD_T_U32    fileLen;   //file length
	_SD_T_U32    timeSlice;	//ms
	
	_SD_T_U32    totalTime; //total time
	_SD_T_U32    indexSize; //total index size
	_SD_T_U16    DtxFlag;   //DTX flag
} _SD_T_INDEX_INFO; 

#define _SD_NODE_BUFF_LEN	8192

typedef struct _SD_T_NODE
{
	struct _SD_T_NODE *pnext;
	_SD_T_U32 bufflen;
	_SD_T_U32 pbuff[_SD_NODE_BUFF_LEN];
} _SD_T_PNODE;


typedef struct
{
    _SD_T_U8  *pwpos;		//pointer of write pos
    _SD_T_U32  free_len;	//buffer free length
    _SD_T_U8  *pstart;		//buffer start address
    _SD_T_U32  start_len;	//start free length		
}
_SD_T_BUFFER_CONTROL;

//resample data structure
typedef struct
{
	_SD_T_U16 channel_in;
	_SD_T_U16 channel_out;
	_SD_T_U16 bitspersample_in;
	_SD_T_U16 bitspersample_out;
	_SD_T_U32 samplerate_in;
	_SD_T_U32 samplerate_out;
}_SD_T_AUDIO_RS_INIT;

typedef struct
{
	void *buf_in;
	void *buf_out;
	_SD_T_U32 len_in;
	_SD_T_U32 len_out;
}_SD_T_AUDIO_BUF_IO_STRC;

typedef _SD_T_AUDIO_BUF_IO_STRC _SD_T_AUDIO_RS_STRC;

/** @{@name Define the callback function type */
/** Callback function for end of play */
typedef void (*_SD_END_CALLBACK)(_SD_T_U8 user_stop);	

/** Callback function for sending message */
typedef void (*_SD_SEND_MSG_CALLBACK)(void);	

/** Callback function for ADPCM record over */
typedef void (*_SD_ADPCM_RECORD_CALLBACK)(_SD_T_U8 *buff, _SD_T_U32 bufflen);	

/** @} */
#ifdef _SD_CHANGE_WORKCLOCK
/** Callback function for setting dsp clock */
typedef void (*_SD_SET_DSPFREQDIV_CALLBACK)(_SD_T_U32 arm_freq_div, _SD_T_U32 dsp_freq_div);
#endif

typedef void (*_SD_VTIMERDELAYMS_CALLBACK)(_SD_T_U32);

/**CallBack function for LoadResource*/

typedef _SD_T_U32 (*_SD_CALLBACK_FUN_FREAD)(_SD_T_FILE stream, _SD_T_U8 *buffer, _SD_T_U32 count);

typedef _SD_T_U32 (*_SD_CALLBACK_FUN_FWRITE)(unsigned int hFile,  void	*pBuffer,  unsigned int	count);

typedef _SD_T_U32 (*_SD_CALLBACK_FUN_FSEEK)(_SD_T_FILE stream, _SD_T_U32 offset, _SD_T_U32 origin);

typedef _SD_T_S32 (*_SD_CALLBACK_FUN_FGETLEN)(unsigned int	hFile);		

typedef _SD_T_U32 (*_SD_CALLBACK_FUN_FTELL)(_SD_T_FILE hFile);	

typedef void (*_SD_CALLBACK_FUN_LOADRESOURCE)(_SD_T_LOADRESOURCE_CB_PARA *pPara);

typedef void (*_SD_CALLBACK_FUN_RELEASERESOURCE)(_SD_T_U8	*Buff );	

typedef void* (*_SD_CALLBACK_FUN_MALLOC)(_SD_T_U32 size, _SD_T_U8 *filename,_SD_T_U32 line );		
typedef void* (*_SD_CALLBACK_FUN_FREE)(void* mem);		
typedef void* (*_SD_CALLBACK_FUN_REMALLOC)(void* mem ,_SD_T_U32 size);		
typedef void* (*_SD_CALLBACK_FUN_DMAMEMCOPY)(void* dst,char* src,_SD_T_U32 count);		
typedef void (*_SD_CALLBACK_FUN_SHOWFRAME)(void* srcImg,_SD_T_U32 src_width,_SD_T_U32 src_height);		
typedef void (*_SD_CALLBACK_FUN_CAMERASHOWFRAME)(void* srcImg,_SD_T_U32 src_width,_SD_T_U32 src_height);		
typedef void (*_SD_CALLBACK_FUN_CAPSTART)();		
typedef _SD_T_BOOL (*_SD_CALLBACK_FUN_CAPCOMPLETE)();		
typedef void* (*_SD_CALLBACK_FUN_CAPGETDATA)();		
typedef _SD_T_U32 (*_SD_CALLBACK_FUN_GETTICKCOUNT)();		
typedef _SD_T_U32 (*_SD_CALLBACK_FUN_PRINTF)(_SD_T_pCSTR format, ...);	
typedef _SD_T_S32 (*_SD_CALLBACK_FUN_MBSTOWCS)(_SD_T_S8 *mbstr, _SD_T_U32 cbMultiByte, _SD_T_U16 *wcstr, _SD_T_U32 cchWideChar);
typedef _SD_T_S32 (*_SD_CALLBACK_FUN_WCSTOMBS)(_SD_T_U16 *wcstr, _SD_T_U32 cchWideChar, _SD_T_S8 *mbstr, _SD_T_U32 cbMultiByte);
typedef void (*_SD_CALLBACK_FUN_INVALIDATECACHE)(void);


typedef struct{
	_SD_CALLBACK_FUN_FREAD   			fread;
	_SD_CALLBACK_FUN_FWRITE  			fwrite;
	_SD_CALLBACK_FUN_FSEEK    			fseek;
	_SD_CALLBACK_FUN_FGETLEN            fgetlen;
	_SD_CALLBACK_FUN_FTELL 				ftell;
	_SD_CALLBACK_FUN_LOADRESOURCE  		LoadResource;
	_SD_CALLBACK_FUN_RELEASERESOURCE 	ReleaseResource;
	_SD_CALLBACK_FUN_MALLOC 			malloc;
	_SD_CALLBACK_FUN_FREE    			free;
	_SD_CALLBACK_FUN_REMALLOC 		 	remalloc;
	_SD_CALLBACK_FUN_DMAMEMCOPY 		DMAMemcpy;
	_SD_CALLBACK_FUN_SHOWFRAME 			ShowFrame;
	_SD_CALLBACK_FUN_CAMERASHOWFRAME 	CameraShowFrame;
	_SD_CALLBACK_FUN_CAPSTART			CapStart;
	_SD_CALLBACK_FUN_CAPCOMPLETE		CapComplete;
	_SD_CALLBACK_FUN_CAPGETDATA			CapGetData;
	_SD_CALLBACK_FUN_GETTICKCOUNT		GetTickCount;
	_SD_CALLBACK_FUN_PRINTF            	printf;
	_SD_CALLBACK_FUN_MBSTOWCS			mbstowcs;
	_SD_CALLBACK_FUN_WCSTOMBS			wcstombs;
	_SD_CALLBACK_FUN_INVALIDATECACHE    Invalidatecache;
} _SD_CB_FUNS;


/** Declaration of external dac operations */
#if defined( _SD_USE_I2S_DA) || defined(_SD_USE_I2S_AD)

/**
 * @brief reset external DA/AD chip
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_Reset(void);
/**
 * @brief power up external DA/AD chip
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_PoweUp(void);
/**
 * @brief power down external DA/AD chip
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_PowerDown(void);
/**
 * @brief set input gain volume  
 * @param _SD_T_BOOL IsMicIn: _SD_LINEIN_ON / _SD_MIC_ON
 * @param _SD_T_U16 gain: input gain (0 ~ 4)
 * @return T_VOID
 */
extern void _AK_HAL_ExtDAC_SetInputGain(_SD_T_U16 input_mode, _SD_T_U16 gain);
/**
 * @brief set output gain volume 
 * @param _SD_T_BOOL IsHeadphone: _SD_HEADPHONE_ON / _SD_SPEAKER_ON / _SD_ALL_ON
 * @param _SD_T_U16 volume: gain volume (0 ~ 3)
 * @return T_VOID
 */
extern void _AK_HAL_ExtDAC_SetOutputGain(_SD_T_U16 output_mode, _SD_T_U16 gain);
/**
 * @brief enable external AD 
 * @param input_mode: _SD_MIC_ON: mic in; _SD_LINEIN_ON: line in
 * @return void
 */
extern void _AK_HAL_ExtDAC_EnableAD(_SD_T_U16 input_mode);
/**
 * @brief disable external AD 
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_DisableAD(void);

/**
 * @brief enable external DA
 * @param output_mode: _SD_HEADPHONE_ON: headphone, _SD_SPEAKER_ON: speaker 
 * @return void
 */
extern void _AK_HAL_ExtDAC_EnableDA(_SD_T_U16 output_mode);
/**
 * @brief disable external DA 
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_DisableDA(void);

/**
 * @brief set line in bypass mode
 * @param void
 * @return void
 */
extern void _AK_HAL_ExtDAC_BypassLineIn(void);
/**
 * @brief set external DA/AD sample rate
 * @param _SD_T_U16 sampleRate: 8000, 16000, 22050, 32000, 44100, 48000
 * @return T_S16: result
 * @retval 0: succeed
 * @retval -1: unsupported sample reate
 */
extern _SD_T_S16 _AK_HAL_ExtDAC_SetSampleRate(_SD_T_U16 sampleRate);

#endif

#endif 
/** end of sdglobal.h */		 
