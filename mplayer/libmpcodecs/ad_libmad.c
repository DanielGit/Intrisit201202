// SAMPLE audio decoder - you can use this file as template when creating new codec!

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#else
#include <uclib.h>
#include <uclib.h>
#include <uclib.h>
#endif

#include "config.h"

#include "ad_internal.h"

static ad_info_t info =  {
	"libmad mpeg audio decoder",
	"libmad",
	"A'rpi",
	"libmad...",
	"based on Xine's libmad/xine_decoder.c"
};

LIBAD_EXTERN(libmad)

#ifndef __LINUX__
ad_functions_t *ad_libmad_table (void)
{
  return &mpcodecs_ad_libmad;
}
#endif

#include <mad.h>

typedef struct mad_decoder_s {

  struct mad_synth  synth; 
  struct mad_stream stream;
  struct mad_frame  frame;
  
  int have_frame;

  int               output_sampling_rate;
  int               output_open;
  int               output_mode;

} mad_decoder_t;

static int preinit(sh_audio_t *sh){

  mad_decoder_t *this = (mad_decoder_t *) malloc(sizeof(mad_decoder_t));
  memset(this,0,sizeof(mad_decoder_t));
  sh->context = this;
  
  mad_stream_init (&this->stream);
  mad_frame_init  (&this->frame);
  mad_synth_init  (&this->synth);
  sh->audio_out_minsize=2*4608;
  sh->audio_in_minsize=4096;
  
  return 1;
}

static int read_frame(sh_audio_t *sh)
{
	mad_decoder_t *this = (mad_decoder_t *) sh->context;
	int len;

	len=demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len], sh->a_in_buffer_size-sh->a_in_buffer_len);
	//F("len = %d",len);
	//F("sh->a_in_buffer_len = %d",sh->a_in_buffer_len);

	while( len >0 )
	{
		sh->a_in_buffer_len+=len;
		while(1)
		{
			int ret;
			mad_stream_buffer (&this->stream, sh->a_in_buffer, sh->a_in_buffer_len);
			ret=mad_frame_decode (&this->frame, &this->stream);
			if (this->stream.next_frame) 
			{
				int num_bytes =	(char*)sh->a_in_buffer+sh->a_in_buffer_len - (char*)this->stream.next_frame;
				memmove(sh->a_in_buffer, this->stream.next_frame, num_bytes);
				mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"libmad: %d bytes processed\n",sh->a_in_buffer_len-num_bytes);
				sh->a_in_buffer_len = num_bytes;
			}
			if((ret < 0) && (sh->a_in_buffer_len < 16))
				break;
			if (ret == 0)
				return 1; // OK!!!

			// error! try to resync!
			if(this->stream.error==MAD_ERROR_BUFLEN )
				break;
		}
		len=demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len], sh->a_in_buffer_size-sh->a_in_buffer_len);
	}

//	F("len = %d",len);
	if( len == 0 )
	{
	//	F("len = 0,package program");
	//	F("sh->a_in_buffer_len = %d",sh->a_in_buffer_len);
		while(sh->a_in_buffer_len > 0)
		{
			int ret;
			mad_stream_buffer (&this->stream, sh->a_in_buffer, sh->a_in_buffer_len);
			ret=mad_frame_decode (&this->frame, &this->stream);
			if( ret < 0 )
			{
				sh->a_in_buffer_len = 0;
				return 0;
			}
			
			if (this->stream.next_frame) 
			{
				int num_bytes =	(char*)sh->a_in_buffer+sh->a_in_buffer_len - (char*)this->stream.next_frame;
				memmove(sh->a_in_buffer, this->stream.next_frame, num_bytes);
				mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"libmad: %d bytes processed\n",sh->a_in_buffer_len-num_bytes);
				sh->a_in_buffer_len = num_bytes;
			}

			if (ret == 0)
				return 1; // OK!!!
		}
	}

#ifndef JZ47_OPT
	mp_msg(MSGT_DECAUDIO,MSGL_INFO,"Cannot sync MAD frame\n");
#endif
	return 0;
}

static int init(sh_audio_t *sh){
  mad_decoder_t *this = (mad_decoder_t *) sh->context;

  this->have_frame = read_frame(sh);
  if(!this->have_frame)
  {
	  sh->channels=sh->wf->nChannels;
	  sh->samplerate=sh->audio.dwRate;
	  sh->i_bps=sh->wf->nAvgBytesPerSec;
 	  return 0; // failed to sync...
  }
  
  sh->channels=(this->frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
  sh->samplerate=this->frame.header.samplerate;
  if(!sh->i_bps)
  	sh->i_bps=this->frame.header.bitrate/8;
  sh->samplesize=2;
  return 1;
}

static void uninit(sh_audio_t *sh){
  mad_decoder_t *this = (mad_decoder_t *) sh->context;
  mad_synth_finish (&this->synth);
  mad_frame_finish (&this->frame);
  mad_stream_finish(&this->stream);
  free(sh->context);
}

/* utility to scale and round samples to 16 bits */
static inline signed int scale(mad_fixed_t sample) {
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
	mad_decoder_t *this = (mad_decoder_t *) sh->context;
	int len=0;

	//F("1");
	while(len<minlen && len+4608<=maxlen)
	{
		this->have_frame = read_frame(sh);
		if(!this->have_frame)
			break;

		//F("3");
		mad_synth_frame (&this->synth, &this->frame);
		//F("4");

		{ 
			unsigned int         nchannels, nsamples;
			mad_fixed_t const   *left_ch, *right_ch;
			struct mad_pcm      *pcm = &this->synth.pcm;
			uint16_t            *output = (uint16_t*) buf;

			nchannels = pcm->channels;
			nsamples  = pcm->length;
			left_ch   = pcm->samples[0];
			right_ch  = pcm->samples[1];

			len+=2*nchannels*nsamples;
			buf+=2*nchannels*nsamples;

			while (nsamples--) 
			{
				/* output sample(s) in 16-bit signed little-endian PCM */

				*output++ = scale(*left_ch++);

				if (nchannels == 2) 
				  *output++ = scale(*right_ch++);
			}
		}
	}

	//F("2");
	return len?len:-1;
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
	mad_decoder_t *this = (mad_decoder_t *) sh->context;
	// various optional functions you MAY implement:
	switch(cmd)
	{
	case ADCTRL_RESYNC_STREAM:
		F("ADCTRL_RESYNC_STREAM");
		this->have_frame=0;
		mad_synth_init  (&this->synth);
		mad_stream_init (&this->stream);
		mad_frame_init  (&this->frame);
		return CONTROL_TRUE;
	case ADCTRL_SKIP_FRAME:
		F("ADCTRL_SKIP_FRAME");
		this->have_frame=read_frame(sh);
		return CONTROL_TRUE;
	}
	return CONTROL_UNKNOWN;
}

