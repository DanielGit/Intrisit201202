/*
  Android audio flinger output plugin for MPlayer

  (C) Ingenic Semicondutor Inc.
  
  Author:      Wolfgang <lgwang@ingenic.cn>, 05-12-2009
  Modified:    
  
  Any bugreports regarding to this driver are welcome.
*/

#include <errno.h>
#include <sys/time.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "config.h"
#include "subopt-helper.h"
#include "mixer.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "libao2/audio_out.h"
#include "libao2/audio_out_internal.h"
#include "libaf/af_format.h"
#include "mplayer.h"

#if defined(HAVE_ANDROID_OS)

//#define USE_SHM_MEM 1

//#define LOG_NDEBUG 0

#define LOG_TAG "mplayer-AO-Android"
#include <utils/Log.h>

#if 1
#define MPLAYER_DBG()							\
	do {								\
		printf("%s[%d], %s()::\n", __FILE__, __LINE__, __FUNCTION__); \
	}while(0)

#define MPLAYER_PRINTF(fff, aaa...) printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa)
#define MY_PRINTF(fff, aaa...) printf(fff, ##aaa)
#define ENTER()								\
	do {								\
		printf("%s[%d], %s():: *** ENTER ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#define LEAVE()								\
	do {								\
		printf("%s[%d], %s():: *** LEAVE ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#define please_use_av_log printf
#else
#define MY_PRINTF(fff, aaa...) do {} while (0)
#endif

static int control(int cmd, void *arg);
static ao_info_t info = 
{
	"Android audio flinger output",
	"AudioFlinger",
	"Wolfgang <lgwang@ingenic.cn>",
	"under developement"
};

LIBAO_EXTERN(audioflinger)

int audioStarted;

#if defined(USE_SHM_MEM)
        //////////////////
	/* share memory */
	int shmID;
        char * shmbuffer;
	key_t shmKey;
        /////////////////
#endif

static int control(int cmd, void *arg)
{
	MplayerAudioOutput_Control(cmd, arg);

	switch(cmd) {
	case AOCONTROL_QUERY_FORMAT:
		return CONTROL_TRUE;
	case AOCONTROL_GET_VOLUME:
	case AOCONTROL_SET_VOLUME:
		return CONTROL_OK;
	}

	return CONTROL_OK;
	//return(CONTROL_UNKNOWN);
}

/*
    open & setup audio device
    return: 1=success 0=fail
*/
static int init(int rate,int channels,int format,int flags)
{
	int ret = 0;
	int bytes_per_sample;
	
	audioStarted = 0;
	printf("3 rate=%d, channels=%d, format=%d, flags=%d", rate, 
	       channels, format, flags);
	
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	ao_data.samplerate = rate;
	ao_data.format = format;
	ao_data.channels = channels;

	int audio_system_format;
	switch (format) {
	case AF_FORMAT_S8:
	case AF_FORMAT_U8:
		audio_system_format = 2/*AudioSystem::PCM_8_BIT*/;
		break;
	case AF_FORMAT_U16_LE:
	case AF_FORMAT_U16_BE:
#ifndef WORDS_BEGINDIAN
	case AF_FORMAT_AC3:
#endif
	case AF_FORMAT_S16_LE:
#ifdef WORDS_BEGINDIAN
	case AF_FORMAT_AC3:
#endif
	case AF_FORMAT_S16_BE:
	default:
		audio_system_format = 1/*AudioSystem::PCM_16_BIT*/;
		break;
	}

	/* My AudioFinger Init */
	MplayerAudioOutput_Init( rate, channels, audio_system_format , flags);
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	ao_data.buffersize = MplayerAudioOutput_GetBufferSize();
	printf("%s:%s:%d  ao_data.buffersize=%d\n",__FILE__,__FUNCTION__,__LINE__,ao_data.buffersize);
	//ao_data.outburst = MplayerAudioOutput_GetFrameSize();
	ao_data.outburst = OUTBURST;

	ao_data.bps = rate * channels;
	if (format != AF_FORMAT_U8 && format != AF_FORMAT_S8)
		ao_data.bps*=2;
	
	bytes_per_sample = ao_data.bps;
	
	if ( ao_data.outburst != 4 ) {
		printf("*********** ao_data.outburst=%d\n", ao_data.outburst);
		printf("ao_data.buffersize=%d, ao_data.outburst=%d, ao_data.channels=%d, ao_data.samplerate=%d, ao_data.format=%d, ao_data.bps=%d\n", 
		       ao_data.buffersize, ao_data.outburst, ao_data.channels, ao_data.samplerate, ao_data.format, ao_data.bps);
	}

	printf("ao_data.buffersize=%d, ao_data.outburst=%d, ao_data.channels=%d, ao_data.samplerate=%d, ao_data.format=%d, ao_data.bps=%d\n", 
	       ao_data.buffersize, ao_data.outburst, ao_data.channels, ao_data.samplerate, ao_data.format, ao_data.bps);
	
	LOGV("alsa: %d Hz/%d channels/%d bpf/%d bytes buffer\n",
	     ao_data.samplerate, ao_data.channels, bytes_per_sample, ao_data.buffersize);
	

#if defined(USE_SHM_MEM)
	//////////////////
	/* share memory */
	shmKey = ftok("/system/bin/mplayer", 16);
	printf("1 shmKey = 0x%08x\n", shmKey);
	if(shmKey == -1) {
		printf("create key error. errno=%d\n",errno);
	}
	shmID = shmget(shmKey, MAX_OUTBURST, IPC_CREAT);
	if(shmID == -1) {
		printf("create share ID error  errno=%d\n",errno);
	}
	shmbuffer = (char *)shmat(shmID, NULL, 0);

	sleep(1);
	/* notify MediaPlayerService to get share memory */
	ret = MplayerAudioOutput_ObtainShm();
	if (ret == 0)
		printf("Obtain share memory error.!\n");
	////////////////

#endif       
	
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	return 1;
}

/* close audio device */
static void uninit(int immed)
{
	int ret = 0;
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	printf("\n\nao_audioflinger uninit()....\n\n");
	MplayerAudioOutput_Uninit(immed);
	audioStarted = 0;

#if defined(USE_SHM_MEM)
	//////////////////////////
	/* share memory */
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	ret = MplayerAudioOutput_FreeShm();
	if (ret ==0)
		printf("Free share memory error.!\n");
	sleep(1);
	shmdt(shmbuffer);
	sleep(1);
	shmctl(shmID, IPC_RMID, 0);
	//////////////////////////
#endif

}

/* stop playing and empty buffers (for seeking/pause) */
static void reset(void)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
//	MplayerAudioOutput_Reset();
	return;
}

/*
    plays 'len' bytes of 'data'
    returns: number of bytes played
*/
static int play(void* data,int len,int flags)
{
#if defined(USE_SHM_MEM)
	//////////////////////////
	/* share memory */
	int reallen = 0;
	int ret = 0;
	if (len > MAX_OUTBURST) {
		printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
		printf("len is too bigger than MAX_OUTBURST\n");
		reallen = MAX_OUTBURST;
	} else
		reallen = len;

	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	memcpy(shmbuffer, data, reallen);
	ret = MplayerAudioOutput_NotifyPlay(reallen);
	//printf("1 ret = %d start\n",ret);
	if (!audioStarted) {
		MplayerAudioOutput_Start();
		audioStarted = 1;
        }
	return ret;
	//////////////////////////
#else
	int ret = 0;
	ret = MplayerAudioOutput_PlayChunk(len, data);
	//printf("1 ret = %d start\n",ret);
	if (!audioStarted) {
		MplayerAudioOutput_Start();
		audioStarted = 1;
        }
	return ret;
#endif
}


/* how many byes are free in the buffer */
static int get_space(void)
{
	int ret;
	//printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	ret = MplayerAudioOutput_GetSpace();
	//printf("%s:%s:%d   ret=%d\n",__FILE__,__FUNCTION__,__LINE__,ret);
	return(ret);
}

/* delay in seconds between first and last sample in buffer */
static float get_delay(void)
{
	float ret;

	int byteReady = MplayerAudioOutput_GetDelay();
	//printf("%s:%s:%d   byteReady=%d\n",__FILE__,__FUNCTION__,__LINE__,byteReady);
	ret =  (float) byteReady / (float) ao_data.bps;
	
	return ret;
}

static void audio_pause(void)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	MplayerAudioOutput_Pause();
}

static void audio_resume(void)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	MplayerAudioOutput_Resume();
}

#endif	/* HAVE_ANDROID_OS */
