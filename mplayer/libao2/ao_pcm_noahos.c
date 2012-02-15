
#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#else
#include <mplaylib.h>
#include "mplayertm.h"
#endif

#include "config.h"
#include "libaf/af_format.h"
#include "audio_out.h"
#include "audio_out_internal.h"

static ao_info_t info = 
{
	"Null audio output",
	"null",
	"Tobias Diedrich <ranma+mplayer@tdiedrich.de>",
	""
};

LIBAO_EXTERN(noahos)

struct	timeval last_tv;
int	buffer;

extern int noahos_audio_init(int rate,int channels,int format,int flags);
extern void noahos_audio_uninit(int immed);
extern void noahos_audio_reset(void);
extern void noahos_audio_pause(void);
extern void noahos_audio_resume(void);

extern int noahos_audio_get_space(void);
extern int noahos_audio_play(void* data,int len,int flags);
extern float noahos_audio_get_delay(void);
extern int noahos_audio_control(int cmd,void *arg);

static void drain(void){
 
    struct timeval now_tv;
    int temp, temp2;

    gettimeofday(&now_tv, 0);
    temp = now_tv.tv_sec - last_tv.tv_sec;
    temp *= ao_data.bps;
    
    temp2 = now_tv.tv_usec - last_tv.tv_usec;
    temp2 /= 1000;
    temp2 *= ao_data.bps;
    temp2 /= 1000;
    temp += temp2;

    buffer-=temp;
    if (buffer<0) buffer=0;

    if(temp>0) last_tv = now_tv;//mplayer is fast
}

// to set/get/query special features/parameters
static int control(int cmd,void *arg){
#if 0	
    return -1;
#else
    return noahos_audio_control (cmd, arg);
#endif    
}

// open & setup audio device
// return: 1=success 0=fail
static int init(int rate,int channels,int format,int flags){

    ao_data.buffersize= 65536;
    ao_data.outburst=1024;
    ao_data.channels=channels;
    ao_data.samplerate=rate;
    ao_data.format=format;
    ao_data.bps=channels*rate;
    if (format != AF_FORMAT_U8 && format != AF_FORMAT_S8)
	    ao_data.bps*=2; 
    buffer=0;
    
    return noahos_audio_init (rate, channels, format, flags);
}

// close audio device
static void uninit(int immed)
{
  noahos_audio_uninit (immed);
}

// stop playing and empty buffers (for seeking/pause)
static void reset(void){
    buffer=0;
    noahos_audio_reset ();
}

// stop playing, keep buffers (for pause)
static void audio_pause(void)
{
	buffer=0;
  noahos_audio_pause ();
}

// resume playing, after audio_pause()
static void audio_resume(void)
{
	return noahos_audio_resume();
}

// return: how many bytes can be played without blocking
static int get_space(void)
{
#if 0	
    drain();
    return ao_data.buffersize - buffer;
#endif    
  return noahos_audio_get_space ();
}

// plays 'len' bytes of 'data'
// it should round it down to outburst*n
// return: number of bytes played
static int play(void* data,int len,int flags)
{
#if 0
    int maxbursts = (ao_data.buffersize - buffer) / ao_data.outburst;
    int playbursts = len / ao_data.outburst;
    int bursts = playbursts > maxbursts ? maxbursts : playbursts;
    buffer += bursts * ao_data.outburst;
    return bursts * ao_data.outburst;
#else
  return noahos_audio_play (data, len, ((flags & AOPLAY_FINAL_CHUNK) != 0));
#endif    
}
extern float noahos_audio_get_delay(void);
// return: delay in seconds between first and last sample in buffer
static float get_delay(void){

#if 0
    drain();
    return (float) buffer / (float) ao_data.bps;
#else
  return noahos_audio_get_delay ();
#endif    
}

