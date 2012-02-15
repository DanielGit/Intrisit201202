//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          直接音频播放控制函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _MEDIA_MPLAYER
#define _MEDIA_MPLAYER

#define	OPEN_SUCCESS_ERR	0
#define	PLAY_FINISH_ERR	1						//playing finish
#define	STREAM_OPEN_ERR	-1					//stream open error
#define	DEMUXER_OPEN_ERR	-2					//demuxer open error
#define	AUDIO_VIDEO_OPEN_ERR	-3			//VIDEO,AUDIO is null
#define	CODEC_INFO_ERR	-4						//codec info error
#define	VIDEO_FRAMES_ERR	-5					//video frames is 0
#define	DEMUXER_CONTROL_ERR	-6
#define	MALLOC_ERROR	-7

#define MPLAYER_NO_FREE			//mplayer不退出，一直驻留内存

#if defined(CONFIG_MCU_JZ4760B)
#define CONFIG_MPLAYER_OUT_FORMAT32		30		//32位图片
#define CONFIG_MPLAYER_OUT_FORMAT16		44		//16位图片
#else
#define CONFIG_MPLAYER_OUT_FORMAT32		6		//32位图片
#define CONFIG_MPLAYER_OUT_FORMAT16		9		//16位图片
#endif

void noah_os_init (PJz47_AV_Decoder priv);
int InitMplayerLib();
int OpenMplayerLib(int info);
int PLayMplayerLib();
int GetMplayerDataLib();
int MplayerGetInfo();
int MplayerCloseLib();
int  GetCurAudioTime();
int GetMplayerCurTime();
void SetMplayerSeek(int f);
void SetMplayerResume();
void SetMplayerStop();
void SetMplayerPause();
void SetMplayerError();
MEDIALIB_STATUS GetMplayerStatus();
void noah_set_lcd_size(int x, int y, int w, int h);
int GetMplayerVideoDataLib();

#endif	//_MEDIA_MPLAYER
