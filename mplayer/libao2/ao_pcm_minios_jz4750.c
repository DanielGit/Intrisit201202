#include "config.h"

#include "mplaylib.h"


#include "bswap.h"
#include "subopt-helper.h"
#include "libaf/af_format.h"
#include "audio_out.h"
#include "audio_out_internal.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "ao_jz4740_vol.h"

static ao_info_t info = 
{
	"RAW PCM/WAVE file writer audio output",
	"pcm",
	"Atmosfear",
	""
};

LIBAO_EXTERN(minios)

extern int vo_pts;

//static char *ao_outputfilename = NULL;
//static int ao_pcm_waveheader = 1;

//#undef printf

#define WAV_ID_RIFF 0x46464952 /* "RIFF" */
#define WAV_ID_WAVE 0x45564157 /* "WAVE" */
#define WAV_ID_FMT  0x20746d66 /* "fmt " */
#define WAV_ID_DATA 0x61746164 /* "data" */
#define WAV_ID_PCM  0x0001


//from /drv/codec/pcm.h
extern void pcm_set_write_pagesize(unsigned int pagesize);
#define	AFMT_U8			8
#define AFMT_S16_LE		16
#define PCM_SET_SAMPLE_RATE	0
#define PCM_SET_CHANNEL		1
#define PCM_SET_FORMAT		2
#define PCM_GET_SPACE   5
#define PCM_SET_HP_VOL		6
#define PCM_GET_HP_VOL		7

#define PCM_SET_PAUSE		8
#define PCM_SET_PLAY		9
#define PCM_RESET		10
#define PCM_SET_MUTE    13
#define PCM_SET_EQPLAY  14
#define PCM_SET_VOLFUNCTION 15
#define PCM_REINIT          18
#define PCM_DATA_FINISH     19
#define PCM_SET_REPLAY      11
static int audio_status = 1;
static void (*Pre_Play)(void *buffer,int size) = NULL;
static void (*Aft_Play)(void *buffer,int size) = NULL;



#define AUDIO_CTRL_FINISH 2
#define AUDIO_CTRL_DECSTATE  1
#define AUDIO_CTRL_INCSTATE  3

#define AUDIO_CTRL_DECING  4
#define AUDIO_CTRL_INCING  5




static int audio_vol_control = AUDIO_CTRL_FINISH;

static short audio_vol_time = 0; //ms
static int audio_vol = 0;
static int audio_vol_save = 0;

extern int videoshow;
//#undef printf

void Set3DPlay(void *p)
{
	Aft_Play = p;
}
void ao_pcm_minios_init()
{
	Pre_Play = 0;
	Aft_Play = 0;
}
void SetPostPlay(void *pre,void *aft)
{
	Pre_Play = pre;
	Aft_Play = aft;
}
#define MAX_AUDIO_CTRL_TIME 2000
static inline void play_fill_16bit_audio(void *block,int count,void *outbuf,int jz_audio_volume,int shift)
{
	if(Pre_Play)
		Pre_Play(block,count);
	if(Aft_Play)
		Aft_Play(block,count);
	if(AUDIO_CTRL_DECSTATE == audio_vol_control)
	{
				
			audio_vol_save = pcm_ioctl(PCM_GET_HP_VOL,0);
			audio_vol_control = AUDIO_CTRL_DECING;
			audio_vol_time = MAX_AUDIO_CTRL_TIME;
	}
	if(AUDIO_CTRL_DECING == audio_vol_control)
	{
		audio_vol_time -=  (count * 1000)/ ao_data.bps;
		int val = audio_vol_save * audio_vol_time / MAX_AUDIO_CTRL_TIME; 
		
		if(audio_vol_time <= 0)
		{	
			audio_vol_control = AUDIO_CTRL_FINISH;
			val = 0;
		}
		//F("DEC %d %d\n",audio_vol_time,val);
		pcm_ioctl(PCM_SET_HP_VOL,val);
	}
	
	if(AUDIO_CTRL_INCSTATE == audio_vol_control)
	{
			audio_vol_time = MAX_AUDIO_CTRL_TIME;
			audio_vol_control = AUDIO_CTRL_INCING;
	}
	if(AUDIO_CTRL_INCING == audio_vol_control)
	{
		audio_vol_time -=  (count * 1000)/ ao_data.bps;
		int val = audio_vol_save - audio_vol_time * audio_vol_save / MAX_AUDIO_CTRL_TIME; 
		
		if(audio_vol_time <= 0)
		{
			audio_vol_control = AUDIO_CTRL_FINISH;
			val = audio_vol_save;
			audio_vol_save = 0;
			
		}
		//F("INC %d %d\n",audio_vol_time,val);
		pcm_ioctl(PCM_SET_HP_VOL,val);
	}

	pcm_fill_buff(outbuf,block,count);
	
	
}

static inline void play_fill(void *block,int count,void *outbuf,int jz_audio_volume,int shift)
{
	memcpy(outbuf,block,count);
}
static unsigned int *g_pcmbuf = NULL;

#define AUDIO_PCM_BUFFER_SIZE (128 * 1024)

#define AUDIO_PCM_BUFFER_COUNT 8


#define VIDEO_PCM_BUFFER_SIZE (32 * 1024)
#define VIDEO_PCM_BUFFER_COUNT 8

static int audio_buf_size = AUDIO_PCM_BUFFER_SIZE;
static int audio_buf_count = AUDIO_PCM_BUFFER_COUNT; 



// open & setup audio device
// return: 1=success 0=fail
static int init(int rate,int channels,int format,int flags){
	int m_format;
	int bits = 8;
	//if(videoshow == 0)
	{
		if(videoshow != 0)
		{
			
		 	audio_buf_size = VIDEO_PCM_BUFFER_SIZE;
		  audio_buf_count = VIDEO_PCM_BUFFER_COUNT;
			if(ao_data.buffersize > 0)
			{
				audio_buf_size = ao_data.buffersize;
				audio_buf_count = ao_data.buffersize /ao_data.outburst;
		  }
	  }
		g_pcmbuf = memalign(32,audio_buf_size);
		if(g_pcmbuf == 0) return 0;
		int ret = Buffer_Init(g_pcmbuf,audio_buf_size,audio_buf_count);
		F("audio_buf_size = %d audio_buf_count = %d\n",audio_buf_size,audio_buf_count);
		if(ret == 0) return 0;
	}	
	m_format=AFMT_U8;

	switch(format){
	case AF_FORMAT_S8:
		
	case AF_FORMAT_U8:
	    break;
	default:
	    m_format=AFMT_S16_LE;
	    bits = 16;
	    break;
	}
	
	int bps = channels * rate * (bits/8);
	//int burst = bps / 4 / 4 /32 * 32;
	//#define MAX_BUF (0x4000)
	
	//if(burst > MAX_BUF) burst = MAX_BUF;
	//if(videoshow == 0)
	int burst = audio_buf_size / audio_buf_count;
	//F("max burst = %d\n",burst);
	//pcm_set_write_pagesize(burst);	
	ao_data.outburst = burst;
	ao_data.buffersize= audio_buf_count * burst;
	ao_data.channels=channels;
	ao_data.samplerate=rate;
	ao_data.format=format;
	ao_data.bps=bps;
	
	//F("format = %x\n",format);
	pcm_ioctl(PCM_SET_REPLAY,0);
	pcm_ioctl(PCM_SET_SAMPLE_RATE,rate);
	//printf("rate = %d\n",rate);
	
	pcm_ioctl(PCM_SET_CHANNEL,channels);
	pcm_ioctl(PCM_SET_FORMAT,m_format);

	if(m_format==AFMT_S16_LE)
	{
			if(videoshow == 1)
			{	
				printf("no 3d eq!");
				//pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)play_fill);
			}
			else
			{	
				pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)play_fill_16bit_audio);
				printf("3d eq!");
			}
	}
	else
		pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)play_fill);
  //F("rate = %d channel = %d %d\n",rate,channels,m_format);
	if(audio_status == 0)
	{
			audio_resume();	
	}
	pcm_ioctl(PCM_RESET,0);
	pcm_ioctl(PCM_REINIT,0);
	audio_status = 1;
	return 1;
}


// close audio device
static void uninit(int immed){
	
	pcm_set_write_pagesize(0);	
	Buffer_Init(0,0,0);
	Set3DPlay(0);
	if(g_pcmbuf)
		free(g_pcmbuf);
	g_pcmbuf = 0;
}

// stop playing and empty buffers (for seeking/pause)
static void reset(void){
	//F("1\n");
	
	pcm_ioctl(PCM_RESET, 0);
}

// stop playing, keep buffers (for pause)
static void audio_pause(void)
{
	audio_status = 0;
	pcm_ioctl(PCM_SET_PAUSE, 0);
	//F("%d\n",audio_status);
    // for now, just call reset();
}

// resume playing, after audio_pause()
static void audio_resume(void)
{
		F("%d\n",audio_status);	 
	  pcm_ioctl(PCM_SET_PLAY, 0);
		audio_status = 1;
}
// return: how many bytes can be played without blocking
static int get_space(void){
		#if 1
		if(audio_status)
		{	
		int d;
		d = pcm_ioctl(PCM_GET_SPACE,0);
		return d;
    
   }
   //mp_init = 1;
    return ao_data.outburst;
    #endif
  
}

// plays 'len' bytes of 'data'
// it should round it down to outburst*n
// return: number of bytes played
#define	AFMT_U8			8
#define AFMT_S16_LE		16

static int play(void* data,int len,int flags){

// let libaf to do the conversion...
  if(audio_status)
  {	
	  	//F("len = %d\n",len);
	  	len = pcm_write(data,len);
	}
	return len;
}

// return: delay in seconds between first and last sample in buffer
static float get_delay(void){

		
		int d;
		if(!audio_status)
			return (float)ao_data.buffersize / (float)ao_data.bps;
		d = pcm_ioctl(PCM_GET_SPACE,0);
		d = ao_data.buffersize - d;
		//d = ao_data.buffersize - d;
		
		if( d <= 0)
			return 0;
    return ((float)d)/(float)ao_data.bps;
}
static int control(int cmd,void *arg){
    
   switch (cmd)
	{
		case AOCONTROL_SET_BUFSIZE:
			audio_buf_size = *(int *)arg;
			return CONTROL_OK;
		case AOCONTROL_SET_BUFCOUNT:
			audio_buf_count = *(int *)arg;
			return CONTROL_OK;
		case AOCONTROL_GET_FINISH:
		{
			return pcm_ioctl(PCM_DATA_FINISH,0);
		}
		break;
		case AOCONTROL_GET_VOL_GRADED:
		{
			//printf("audio_vol_factor_control = %d\n",audio_vol_factor_control);
			return audio_vol_control;
		}
		case AOCONTROL_SET_MUTE:
		{
			pcm_ioctl(PCM_SET_MUTE,1);
			return CONTROL_OK;
		}
		case AOCONTROL_RESET_GRADED:
		{
			pcm_ioctl(PCM_SET_MUTE,0);
			audio_vol_control = AUDIO_CTRL_FINISH;
			return CONTROL_OK;
		}
		case AOCONTROL_SET_VOL_INC_GRADED:
		{
			if((AUDIO_CTRL_FINISH == audio_vol_control) && (audio_vol_save > 0))
				audio_vol_control = AUDIO_CTRL_INCSTATE;
		  	return CONTROL_OK;		  
		}
		case AOCONTROL_SET_VOL_DEC_GRADED:
		{
			if((AUDIO_CTRL_FINISH == audio_vol_control))
					audio_vol_control = AUDIO_CTRL_DECSTATE;
			return CONTROL_OK;
		}
	}
    return -1;
}
