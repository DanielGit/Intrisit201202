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
static int audio_vol_factor = 0;
static int audio_vol_factor_save = 0;
static int audio_vol_factor_control = 0;
static short audio_vol_factor_speed = 1;
static short audio_vol_count = 1;
extern int videoshow;
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
static int audio_volmid_count = 0;
		
static inline void play_fill_16bit_audio(void *block,int count,void *outbuf,int jz_audio_volume,int shift)
{
	if(Pre_Play)
		Pre_Play(block,count);
	if(audio_vol_factor_control != 0)
	{
		audio_volmid_count++;
		if(audio_vol_factor_speed > 0)
		{	
			audio_vol_factor = (int)(audio_vol_factor_speed * audio_volmid_count);
			audio_vol_factor = audio_vol_factor * audio_vol_factor;
			
				
		}else
		{
			if(audio_vol_count - audio_volmid_count > 0)
			{
				audio_vol_factor = (int)(audio_vol_factor_speed * (audio_vol_count - audio_volmid_count));
				audio_vol_factor = audio_vol_factor * audio_vol_factor;
			}else audio_vol_factor = 0;
		}
		if(audio_vol_factor > jz_audio_volume) audio_vol_factor = jz_audio_volume;
			
		if((audio_volmid_count > audio_vol_count + 1) )
		{
			audio_volmid_count = audio_vol_count + 1;
			if(audio_vol_factor_speed < 0)
			{
				audio_vol_factor = 0;
				audio_vol_factor_control = 2;
				pcm_ioctl(PCM_SET_MUTE,1);
			}
			if(audio_vol_factor_speed > 0)
			{
				audio_vol_factor = jz_audio_volume;
				audio_vol_factor_control = 2;
			}
			
		}	
		replay_fill_all_16_mid(block,count,outbuf,audio_vol_factor,shift);
	}else 
	{
		if(audio_vol_factor_save != jz_audio_volume)
		{
			audio_vol_factor = jz_audio_volume;
			audio_vol_factor_save = jz_audio_volume;
			
		}	
		replay_fill_all_16_mid(block,count,outbuf,jz_audio_volume,shift);
	}
		
	if(Aft_Play)
		Aft_Play(outbuf,count);
}

 
// open & setup audio device
// return: 1=success 0=fail
static int init(int rate,int channels,int format,int flags){
	int m_format;
	int bits = 8;

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
	
	int bps = channels*rate*(bits/8);
	int burst = bps / 4 / 4 /32 * 32;
	#define MAX_BUF (0x4000)
	
	if(burst > MAX_BUF) burst = MAX_BUF;
	if(videoshow == 0)
		burst = MAX_BUF;
	F("max burst = %d\n",burst);
	pcm_set_write_pagesize(burst);	
	ao_data.outburst = burst;
	ao_data.buffersize= 4 * burst;
	ao_data.channels=channels;
	ao_data.samplerate=rate;
	ao_data.format=format;
	ao_data.bps=bps;
	audio_vol_factor = 0;
	audio_vol_factor_control = 0;

	
	F("format = %x\n",format);
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
				pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)replay_fill_all_16_mid);
			}
			else
			{	
				pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)play_fill_16bit_audio);
				printf("3d eq!");
			}
	}
	else
		pcm_ioctl(PCM_SET_VOLFUNCTION,(unsigned long)replay_fill_all_8_mid);
  F("rate = %d channel = %d %d\n",rate,channels,m_format);
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
	Set3DPlay(0);
}

// stop playing and empty buffers (for seeking/pause)
static void reset(void){
	F("1\n");
	
	pcm_ioctl(PCM_RESET, 0);
}

// stop playing, keep buffers (for pause)
static void audio_pause(void)
{
	pcm_ioctl(PCM_SET_PAUSE, 0);
	F("%d\n",audio_status);
    // for now, just call reset();
    audio_status = 0;
}

// resume playing, after audio_pause()
static void audio_resume(void)
{
		F("%d\n",audio_status);
		audio_status = 1;
	 
	  pcm_ioctl(PCM_SET_PLAY, 0);
}
// return: how many bytes can be played without blocking
static int get_space(void){
		#if 1
//		if(audio_status)
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
//#ifdef WORDS_BIGENDIAN
#if 0
	if (ao_data.format  == AF_FORMAT_S16_LE) {
	  unsigned short *buffer = (unsigned short *) data;
	  register int i;
	  for(i = 0; i < len/2; ++i) {
	    buffer[i] = le2me_16(buffer[i]);
	  }
	}
#endif
	//printf("PCM: Writing chunk!\n");
	//F("%d\n",len);
  if(audio_status)
	  	len = pcm_write(data,len);
  //F("%d\n",len);
	
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

// to set/get/query special features/parameters

static inline char get_sqrt(int d)
{
	unsigned char sqrt_dat[]={1,4,9,16,25,36,49,64,81,100,121,144,169,196,225,255};
	int i;
	for(i = 0;i < 16;i++)
	{
		if(d <= sqrt_dat[i])
			return i + 1;
	}
	return i;
}
static int control(int cmd,void *arg){
    
   switch (cmd)
	{
		case AOCONTROL_GET_FINISH:
		{
			return pcm_ioctl(PCM_DATA_FINISH,0);
		}
		break;
		case AOCONTROL_GET_VOL_GRADED:
		{
			//printf("audio_vol_factor_control = %d\n",audio_vol_factor_control);
			return audio_vol_factor_control;
		}
		case AOCONTROL_SET_MUTE:
		{
			pcm_ioctl(PCM_SET_MUTE,1);
			return CONTROL_OK;
		}
		case AOCONTROL_RESET_GRADED:
		{
			pcm_ioctl(PCM_SET_MUTE,0);
			audio_vol_factor_control = 0;
			return CONTROL_OK;
		}
		case AOCONTROL_SET_VOL_INC_GRADED:
		{
			audio_volmid_count = 0;
            if(audio_vol_factor_save != 0)
            {
			    audio_vol_factor = 0;
			    pcm_ioctl(PCM_SET_MUTE,0);
			    audio_vol_count = ao_data.bps / ao_data.outburst - 1;
			    if((audio_vol_factor_save > 0) && (audio_vol_count > 0))
			    {	
				    audio_vol_factor_speed =  get_sqrt(audio_vol_factor_save) / audio_vol_count;  
			    }else audio_vol_factor_speed = 1;
				
			    if(audio_vol_factor_speed <= 0) audio_vol_factor_speed = 1;
			    if(audio_vol_factor_speed > audio_vol_factor_save) audio_vol_factor_speed = audio_vol_factor_save;
			    audio_vol_factor -= audio_vol_factor_speed;
			    audio_vol_factor_control = 1;
            }else
                audio_vol_factor_control = 2;
			return CONTROL_OK;
		}
		case AOCONTROL_SET_VOL_DEC_GRADED:
		{
			audio_volmid_count = 0;
			audio_vol_factor = audio_vol_factor_save;
			if(audio_vol_factor != 0)
			{	
				audio_vol_count = ao_data.bps / ao_data.outburst - 1;
				
				if((audio_vol_factor_save > 0) && (audio_vol_count > 0))
				{	
					audio_vol_factor_speed =  get_sqrt(audio_vol_factor_save) / audio_vol_count;  
				}else audio_vol_factor_speed = 1;
				//audio_vol_factor_speed = ao_data.outburst * audio_vol_factor_save / ao_data.bps;
				
				if(audio_vol_factor_speed <= 0) audio_vol_factor_speed = 1;
				if(audio_vol_factor_speed > audio_vol_factor_save) audio_vol_factor_speed = audio_vol_factor_save;
				
				audio_vol_factor_speed = 0 - audio_vol_factor_speed;
				audio_vol_factor_control = 1;
			}else
				audio_vol_factor_control = 2;
				
			
			return CONTROL_OK;
		}
	}
    return -1;
}
