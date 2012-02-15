
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern unsigned int _fbss_plugin, _end_plugin;

void *_start_three ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

    /* audio codecs */
    REGISTER_DECODER (APE, ape);
    REGISTER_DECODER (ALAC, alac);
    REGISTER_DECODER (DCA, dca);
    REGISTER_DECODER (FLAC, flac);
    REGISTER_DECODER (RA_144, ra_144);
    REGISTER_DECODER (RA_288, ra_288);
    REGISTER_DECODER (COOK, cook);

    /* pcm codecs */
     REGISTER_DECODER  (PCM_S32LE, pcm_s32le);
     REGISTER_DECODER  (PCM_S32BE, pcm_s32be);
     REGISTER_DECODER  (PCM_U32LE, pcm_u32le);
     REGISTER_DECODER  (PCM_U32BE, pcm_u32be);
     REGISTER_DECODER  (PCM_S24LE, pcm_s24le);
     REGISTER_DECODER  (PCM_S24BE, pcm_s24be);
     REGISTER_DECODER  (PCM_U24LE, pcm_u24le);
     REGISTER_DECODER  (PCM_U24BE, pcm_u24be);
     REGISTER_DECODER  (PCM_S24DAUD, pcm_s24daud);
     REGISTER_DECODER  (PCM_S16LE, pcm_s16le);
     REGISTER_DECODER  (PCM_S16BE, pcm_s16be);
     REGISTER_DECODER  (PCM_U16LE, pcm_u16le);
     REGISTER_DECODER  (PCM_U16BE, pcm_u16be);
     REGISTER_DECODER  (PCM_S8, pcm_s8);
     REGISTER_DECODER  (PCM_U8, pcm_u8);
     REGISTER_DECODER  (PCM_ALAW, pcm_alaw);
     REGISTER_DECODER  (PCM_MULAW, pcm_mulaw);
     REGISTER_DECODER  (PCM_ZORK , pcm_zork);

    /* dpcm codecs */
    REGISTER_DECODER (ROQ_DPCM, roq_dpcm);
    REGISTER_DECODER (INTERPLAY_DPCM, interplay_dpcm);
    REGISTER_DECODER (XAN_DPCM, xan_dpcm);
    REGISTER_DECODER (SOL_DPCM, sol_dpcm);

    /* adpcm codecs */
     REGISTER_DECODER  (ADPCM_4XM, adpcm_4xm);
     REGISTER_DECODER  (ADPCM_CT, adpcm_ct);
     REGISTER_DECODER  (ADPCM_EA, adpcm_ea);
     REGISTER_DECODER  (ADPCM_IMA_AMV, adpcm_ima_amv);
     REGISTER_DECODER  (ADPCM_IMA_DK3, adpcm_ima_dk3);
     REGISTER_DECODER  (ADPCM_IMA_DK4, adpcm_ima_dk4);
     REGISTER_DECODER  (ADPCM_IMA_QT, adpcm_ima_qt);
     REGISTER_DECODER  (ADPCM_IMA_SMJPEG, adpcm_ima_smjpeg);
     REGISTER_DECODER  (ADPCM_IMA_WAV, adpcm_ima_wav);
     REGISTER_DECODER  (ADPCM_IMA_WS, adpcm_ima_ws);
     REGISTER_DECODER  (ADPCM_MS, adpcm_ms);
     REGISTER_DECODER  (ADPCM_SBPRO_2, adpcm_sbpro_2);
     REGISTER_DECODER  (ADPCM_SBPRO_3, adpcm_sbpro_3);
     REGISTER_DECODER  (ADPCM_SBPRO_4, adpcm_sbpro_4);
     REGISTER_DECODER  (ADPCM_SWF, adpcm_swf);
     REGISTER_DECODER  (ADPCM_THP, adpcm_thp);
     REGISTER_DECODER  (ADPCM_XA, adpcm_xa);
     REGISTER_DECODER  (ADPCM_YAMAHA, adpcm_yamaha);
     REGISTER_DECODER  (ADPCM_ADX, adpcm_adx);
     REGISTER_DECODER  (ADPCM_G726, adpcm_g726);

  return NULL;
}

