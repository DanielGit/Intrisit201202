#ifndef _NOAH_MEDIA_MPLAYER
#define _NOAH_MEDIA_MPLAYER

#include "..\Include\libc\setjmp.h"

#define	OPEN_SUCCESS_ERR	0
#define	PLAY_FINISH_ERR	1						//playing finish
#define	STREAM_OPEN_ERR	-1					//stream open error
#define	DEMUXER_OPEN_ERR	-2					//demuxer open error
#define	AUDIO_VIDEO_OPEN_ERR	-3			//VIDEO,AUDIO is null
#define	CODEC_INFO_ERR	-4						//codec info error
#define	VIDEO_FRAMES_ERR	-5					//video frames is 0
#define	DEMUXER_CONTROL_ERR	-6
#define	MALLOC_ERROR	-7


typedef enum
{
	MEDIALIB_INIT,
	MEDIALIB_END,
	MEDIALIB_PLAYING,
	MEDIALIB_PAUSE,
	MEDIALIB_STOP,
	MEDIALIB_ERR,
	MEDIALIB_SEEK
}MEDIALIB_STATUS;

typedef struct _MEDIA_INFO
{
	int MediaType;
	char bHasAudio;
	char bHasVideo;
	char bAllowSeek;
	DWORD TotalTime;
	
	int AudioCodec;
	int AudioBitrate;
	int AudioSamplerate;
	int AudioChannels;

	int VideoCodec;
	int VideoWidth;
	int VideoHeight;
	int VideoFps;
	int VideoBitrate;
}MEDIA_INFO;
typedef MEDIA_INFO *PMEDIA_INFO;

typedef struct _Jz47_AV_Decoder
{
// display	
	unsigned char *lcd_frame_buffer;
	int		lcd_width, lcd_height, lcd_line_length;
	int		UsrLcdWidth,UsrLcdHeight,UsrLcdPosX,UsrLcdPosY;
	int		kprintf_enable;		//=0,不打印DEBUG信息；=1，打印DEBUG信息
	char	AudioCodec[64];		//audio 类型
	char	VideoCodec[64];		//video 类型
	char	FileName[256];		//文件名
	int OutFormat;
	
// file operation
  void *stream, *plugstream;
	long (*os_ftell)(void *stream);
	long (*os_fread)(void *ptr, long size, long nmemb, void *stream);
	long (*os_fwrite)(void *ptr, long size, long nmemb, void *stream);
	long (*os_fseek) (void *stream, long offset, int whence);
	void (*os_msleep) (unsigned long msec);

//audio control
  int   (*os_audio_init)      (int rate,int channels,int format,int flags);
  void  (*os_audio_uninit)    (int immed);
  void  (*os_audio_reset)     (void);
  void  (*os_audio_pause)     (void);
  void  (*os_audio_resume)    (void);
  int   (*os_audio_get_space) (void);
  int   (*os_audio_play)      (void* data,int len,int flags);
  float (*os_audio_get_delay) (void);
  int   (*os_audio_control)   (int cmd,void *arg);
  void  (*os_open_video_osd)  (int w, int h);
  int	(*os_audio_long_jump) (void* stream, jmp_buf buf, int ret_val);
  
// malloc usage	
	unsigned char *malloc_buf;
	unsigned int malloc_size;
	
//media info
	MEDIA_INFO	MediaInfo;
	DWORD		AudioSpace;
	BYTE		fIpuEnable;
	BYTE		fMplayerInit;
	jmp_buf		AudioJumpBuf;
	BYTE		*pJzDecoderBuf;
}Jz47_AV_Decoder;

#endif	//_MEDIA_MPLAYER

