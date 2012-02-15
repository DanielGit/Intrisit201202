
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

//#include <stddef.h>
#ifdef HAVE_LIBDL_AD
#include <dlfcn.h>
#endif
#include "help_mp.h"

#include "ad_internal.h"
#ifdef __LINUX__
#include "loader/wine/windef.h"
#endif
#include "ad_realaac.h"

static ad_info_t info =  {
	"RealAudio decoder",
	"realaac",
	"Alex Beregszaszi",
	"Florian Schneider, Arpad Gereoffy, Alex Beregszaszi, Donnie Smith",
	"binary real aac codecs"
};

LIBAD_EXTERN(realaac)
ad_functions_t *ad_realaac_table (void)
{
	return &mpcodecs_ad_realaac;
}

static  HAACDecoder *hAACDecoder;
static  AACFrameInfo frameInfo[1];

static int preinit(sh_audio_t *sh){
  sh->audio_out_minsize=AAC_MAX_NCHANS * AAC_MAX_NSAMPS * SBR_MUL;
  sh->audio_in_minsize=READBUF_SIZE;
  return 1;
}
static int aac_probe(unsigned char *buffer, int len)
{
  int i = 0, pos = 0;
  mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: %d bytes\n", len);
  while(i <= len-4) {
    if(
       ((buffer[i] == 0xff) && ((buffer[i+1] & 0xf6) == 0xf0)) ||
       (buffer[i] == 'A' && buffer[i+1] == 'D' && buffer[i+2] == 'I' && buffer[i+3] == 'F')
    ) {
      pos = i;
      break;
    }
    mp_msg(MSGT_DECAUDIO,MSGL_V, "AUDIO PAYLOAD: %x %x %x %x\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]);
    i++;
  }
  mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC_PROBE: ret %d\n", pos);
  return pos;
}
	
#define HXR_OK 0
static int init(sh_audio_t *sh){
  int pos = 0;
  int32_t result = HXR_OK;
  uint16_t *temp;
  uint8_t *inBuf;
  int in_len;
  int32_t NumChannels, SampleRate;
  hAACDecoder = (HAACDecoder *)AACInitDecoder();
  ga_config_data configData;
  int i;

  if (!hAACDecoder) {
          printf(" *** Error initializing decoder ***\n");
          return 0;
  }


  if(!sh->codecdata_len && sh->wf) {
    sh->codecdata_len = sh->wf->cbSize;
    sh->codecdata = (char*)(sh->wf+1);

    mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"realaac: codecdata extracted from WAVEFORMATEX\n");
  }

  if(!sh->codecdata_len){
    //return 0;
    sh->a_in_buffer_len = demux_read_data(sh->ds, sh->a_in_buffer, sh->a_in_buffer_size);

     inBuf  = sh->a_in_buffer;
     in_len = sh->a_in_buffer_len;
     temp = (uint16_t *)malloc(sizeof(uint16_t) * sh->audio_out_minsize);
     /* decode ADTS frame from opaque data to get config data */
     result = AACDecode(hAACDecoder,  &inBuf, &in_len, temp);
     free(temp);
     if (result != HXR_OK)
     {
        return 0;
     }

         /* get the config data struct from the decoder */
     AACGetLastFrameInfo(hAACDecoder, frameInfo);

     NumChannels = frameInfo->nChans;
     SampleRate = frameInfo->sampRateCore;
  }else{
    struct BITSTREAM *pBitstream ;

    pBitstream = (struct BITSTREAM *)malloc(sizeof(struct BITSTREAM)) ;
    if (!pBitstream)
        return 0 ;

    pBitstream->cacheBitsLeft = 0 ;
    pBitstream->buffer = sh->codecdata ;
    pBitstream->nBits  = sh->codecdata_len*8 ;
    pBitstream->pkptr = pBitstream->buffer + (0>>3) ;
    pBitstream->cacheBitsLeft = 0 ;
    pBitstream->cache = 0 ;
    pBitstream->inc = 1 ;

     result = ga_config_get_data(pBitstream, &configData);
    if(pBitstream){
      free(pBitstream);
    }

     if (result != HXR_OK) /* config data error */
     {
         return 0;
     }
      NumChannels = frameInfo->nChans=configData.numChannels;
      SampleRate = frameInfo->sampRateCore=configData.samplingFrequency;
     /* see MPEG4 spec for index of each object type */
     if (configData.audioObjectType == 2)
         frameInfo->profile = AAC_PROFILE_LC;
     else if (configData.audioObjectType == 1)
         frameInfo->profile = AAC_PROFILE_MP;
     else if (configData.audioObjectType == 3)
         frameInfo->profile = AAC_PROFILE_SSR;
     else
         return 0; /* don't know - assume LC */

    /* set decoder to handle raw data blocks */
    AACSetRawBlockParams(hAACDecoder, 0, frameInfo);
  }
    mp_msg(MSGT_DECAUDIO,MSGL_V,"realaac: Decoder init done (%dBytes)!\n", sh->a_in_buffer_len); // XXX: remove or move to debug!
    mp_msg(MSGT_DECAUDIO,MSGL_V,"realaac: Negotiated samplerate: %ldHz  channels: %d\n", SampleRate, NumChannels);

    sh->channels = NumChannels;
    if (audio_output_channels <= 2) sh->channels = NumChannels > 1 ? 2 : 1;
    sh->samplerate = SampleRate;
    sh->samplesize=2;

   if(!sh->i_bps) {
      mp_msg(MSGT_DECAUDIO,MSGL_WARN,"realaac: compressed input bitrate missing, assuming 128kbit/s!\n");
      sh->i_bps = 128*1000/8; // XXX: HACK!!! ::atmos
    } else
      mp_msg(MSGT_DECAUDIO,MSGL_V,"realaac: got %dkbit/s bitrate from MP4 header!\n",sh->i_bps*8/1000);

  return 1; // return values: 1=OK 0=ERROR
}

static void uninit(sh_audio_t *sh){
        AACFreeDecoder(hAACDecoder);
}
static int aac_sync(sh_audio_t *sh)
{
  int pos = 0;
  if(!sh->codecdata_len) {
    if(sh->a_in_buffer_len < sh->a_in_buffer_size){
      sh->a_in_buffer_len +=
	demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len],
	sh->a_in_buffer_size - sh->a_in_buffer_len);
    }
    pos = aac_probe(sh->a_in_buffer, sh->a_in_buffer_len);
    if(pos) {
      sh->a_in_buffer_len -= pos;
      memmove(sh->a_in_buffer, &(sh->a_in_buffer[pos]), sh->a_in_buffer_len);
      mp_msg(MSGT_DECAUDIO,MSGL_V, "\nAAC SYNC AFTER %d bytes\n", pos);
    }
  }
  return pos;
}

#if 0
#define FRAC_CONST(a) ((int32_t)((a)*((1<<31)-1)+0.5))
#define DM_MUL FRAC_CONST(0.3203772410170407) // 1/(1+sqrt(2) + 1/sqrt(2))
#define RSQRT2 FRAC_CONST(0.7071067811865475244) // 1/sqrt(2)
#endif
#define DM_MUL 688004886 // 1/(1+sqrt(2) + 1/sqrt(2))
#define RSQRT2 1518500249 // 1/sqrt(2)
#define MUL_F(a,b)  ((int32_t)(((long long)(a) * (b))>>31))
void reorder_ch_pcm(int16_t *pcmBuf, int nSamps, int nChans)
{
    int i, ch, chanMap[6];
    int16_t tmpBuf[6], *outbuf;

    switch (nChans) {
    case 3:
        chanMap[0] = 1;    /* L */
        chanMap[1] = 2;    /* R */
        chanMap[2] = 0;    /* C */
        break;
    case 4:
        chanMap[0] = 1;    /* L */
        chanMap[1] = 2;    /* R */
        chanMap[2] = 0;    /* C */
        chanMap[3] = 3;    /* S */
        break;
    case 5:
        chanMap[0] = 1;    /* L */
        chanMap[1] = 2;    /* R */
        chanMap[2] = 0;    /* C */
        chanMap[3] = 3;    /* LS */
        chanMap[4] = 4;    /* RS */
        break;
    case 6:
        chanMap[0] = 1;    /* L */
        chanMap[1] = 2;    /* R */
        chanMap[2] = 0;    /* C */
        chanMap[3] = 5;    /* LFE */
        chanMap[4] = 3;    /* LS */
        chanMap[5] = 4;    /* RS */
        break;
    default:
        return;
    }

    if(audio_output_channels < nChans){
#if 0
       int32_t C, L_S, cum;
      outbuf = pcmBuf;
      for (i = 0; i < nSamps; i += nChans) {
        C   = MUL_F(pcmBuf[0], RSQRT2);
        L_S = MUL_F(pcmBuf[3], RSQRT2);
        cum = pcmBuf[1] + C + L_S;
        outbuf[0] = MUL_F(cum, DM_MUL);
        C   = MUL_F(pcmBuf[0], RSQRT2);
        L_S = MUL_F(pcmBuf[4], RSQRT2);
        cum = pcmBuf[2] + C + L_S;
        outbuf[1] = MUL_F(cum, DM_MUL);
        outbuf += 2;
        pcmBuf += nChans;
      }
#else
      outbuf = pcmBuf;
      for (i = 0; i < nSamps; i += nChans) {
        outbuf[0] = pcmBuf[0]+pcmBuf[3]+pcmBuf[1];
        outbuf[1] = pcmBuf[0]+pcmBuf[4]+pcmBuf[2];
        outbuf += 2;
        pcmBuf += nChans;
      }

#endif
    }else {
      for (i = 0; i < nSamps; i += nChans) {
        for (ch = 0; ch < nChans; ch++)
            tmpBuf[ch] = pcmBuf[chanMap[ch]];
        for (ch = 0; ch < nChans; ch++)
            pcmBuf[ch] = tmpBuf[ch];
        pcmBuf += nChans;
      }
    }
}

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen){
  int len = 0, last_dec_len = 1, err = 0, in_len;
  int outOfData = 0, errors=0;;
  uint8_t *inBuf;

  while(len < minlen && last_dec_len > 0 ) {
    /* update buffer for raw aac streams: */
  if(!sh->codecdata_len){
    if(sh->a_in_buffer_len < sh->a_in_buffer_size){
      sh->a_in_buffer_len +=
        demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len],
        sh->a_in_buffer_size - sh->a_in_buffer_len);
    }
//#define DUMP_AAC_COMPRESSED
#ifdef DUMP_AAC_COMPRESSED
    {int i;
     inBuf = sh->a_in_buffer;
    for (i = 0; i < 16; i++)
      printf ("%02x ", inBuf[i]);
    printf ("\n");}
#endif
    inBuf  = sh->a_in_buffer;
    in_len = sh->a_in_buffer_len;
    // raw aac stream:
    err = AACDecode(hAACDecoder,  &(inBuf), &(in_len), buf+len);
    if(in_len > 0) {
      memmove(sh->a_in_buffer,inBuf,in_len);
    }
    sh->a_in_buffer_len = in_len;

    if (err  == ERR_AAC_NONE)
    {
        AACGetLastFrameInfo(hAACDecoder, frameInfo);
        reorder_ch_pcm(buf+len, frameInfo->outputSamps, frameInfo->nChans);
        last_dec_len = sh->samplesize*frameInfo->outputSamps*sh->channels/frameInfo->nChans;
        len += last_dec_len;
        sh->pts_bytes += last_dec_len;
        if(errors)
          errors=0;
    }else if(err  == ERR_AAC_INDATA_UNDERFLOW){
      errors++;
    }else if(sh->a_in_buffer_len > 0){
      sh->a_in_buffer_len--;
      memmove(sh->a_in_buffer,&sh->a_in_buffer[1],sh->a_in_buffer_len);
      aac_sync(sh);
      errors++;
    }
    if(errors > 10){
        break;
    }
   }else{
    /* update buffer for raw aac streams: */
    // packetized (.mp4) aac stream:
    unsigned char* bufptr=NULL;
    double pts;
    int buflen=ds_get_packet_pts(sh->ds, &bufptr, &pts);
    if(buflen<=0) break;
    if (pts != MP_NOPTS_VALUE) {
	sh->pts = pts;
	sh->pts_bytes = 0;
    }
    err = AACDecode(hAACDecoder, &bufptr, &buflen, buf+len);
    if (err  == ERR_AAC_NONE)
    {
        AACGetLastFrameInfo(hAACDecoder, frameInfo);
        reorder_ch_pcm(buf+len, frameInfo->outputSamps, frameInfo->nChans);
        last_dec_len = sh->samplesize*frameInfo->outputSamps*sh->channels/frameInfo->nChans;
        len += last_dec_len;
        sh->pts_bytes += last_dec_len;
    }
   }
  }
  return len;
}
static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    switch(cmd)
    {
      case ADCTRL_RESYNC_STREAM:
         aac_sync(sh);
         AACFlushCodec(hAACDecoder);
	 return CONTROL_TRUE;
    }
  return CONTROL_UNKNOWN;
}
