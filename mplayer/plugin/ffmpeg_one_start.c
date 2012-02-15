
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern unsigned int _fbss_plugin, _end_plugin;

void *_start_one ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  REGISTER_DECODER (WMAV1, wmav1);
  REGISTER_DECODER (WMAV2, wmav2);
  REGISTER_DECODER (MACE3, mace3);
  REGISTER_DECODER (MACE6, mace6);
  REGISTER_DECODER (SHORTEN, shorten);
  REGISTER_DECODER (SONIC, sonic);
  REGISTER_DECODER (WS_SND1, ws_snd1);
#if 0
    /* audio codecs */
    REGISTER_DECODER (MPEG4AAC, mpeg4aac);
    REGISTER_ENCDEC  (AC3, ac3);
    REGISTER_DECODER (ALAC, alac);
    REGISTER_DECODER (APE, ape);
    REGISTER_DECODER (ATRAC3, atrac3);
    REGISTER_DECODER (COOK, cook);
    REGISTER_DECODER (DCA, dca);
    REGISTER_DECODER (DSICINAUDIO, dsicinaudio);
    REGISTER_ENCDEC  (FLAC, flac);
    REGISTER_DECODER (IMC, imc);
    REGISTER_ENCDEC  (LIBAMR_WB, libamr_wb);
    REGISTER_DECODER (LIBA52, liba52);
    REGISTER_ENCODER (LIBFAAC, libfaac);
    REGISTER_DECODER (LIBFAAD, libfaad);
    REGISTER_ENCDEC  (LIBGSM, libgsm);
    REGISTER_ENCDEC  (LIBGSM_MS, libgsm_ms);
    REGISTER_ENCODER (LIBMP3LAME, libmp3lame);
    REGISTER_ENCODER (LIBTHEORA, libtheora);
    if (!ENABLE_VORBIS_ENCODER)  REGISTER_ENCODER (LIBVORBIS, libvorbis);
    if (!ENABLE_VORBIS_DECODER)  REGISTER_DECODER (LIBVORBIS, libvorbis);
    REGISTER_DECODER (MACE3, mace3);
    REGISTER_DECODER (MACE6, mace6);
    REGISTER_ENCDEC  (MP2, mp2);
    REGISTER_DECODER (MP3, mp3);
    REGISTER_DECODER (MP3ADU, mp3adu);
    REGISTER_DECODER (MP3ON4, mp3on4);
    REGISTER_DECODER (MPC7, mpc7);
    REGISTER_DECODER (QDM2, qdm2);
    REGISTER_DECODER (RA_144, ra_144);
    REGISTER_DECODER (RA_288, ra_288);
    REGISTER_DECODER (SHORTEN, shorten);
    REGISTER_DECODER (SMACKAUD, smackaud);
    REGISTER_ENCDEC  (SONIC, sonic);
    REGISTER_ENCODER (SONIC_LS, sonic_ls);
    REGISTER_DECODER (TRUESPEECH, truespeech);
    REGISTER_DECODER (TTA, tta);
    REGISTER_DECODER (VMDAUDIO, vmdaudio);
    REGISTER_ENCDEC  (VORBIS, vorbis);
    REGISTER_DECODER (WAVPACK, wavpack);
    REGISTER_DECODER (WS_SND1, ws_snd1);

    /* pcm codecs */
    REGISTER_ENCDEC  (PCM_ALAW, pcm_alaw);
    REGISTER_ENCDEC  (PCM_MULAW, pcm_mulaw);
    REGISTER_ENCDEC  (PCM_S8, pcm_s8);
    REGISTER_ENCDEC  (PCM_S16BE, pcm_s16be);
    REGISTER_ENCDEC  (PCM_S16LE, pcm_s16le);
    REGISTER_ENCDEC  (PCM_S24BE, pcm_s24be);
    REGISTER_ENCDEC  (PCM_S24DAUD, pcm_s24daud);
    REGISTER_ENCDEC  (PCM_S24LE, pcm_s24le);
    REGISTER_ENCDEC  (PCM_S32BE, pcm_s32be);
    REGISTER_ENCDEC  (PCM_S32LE, pcm_s32le);
    REGISTER_ENCDEC  (PCM_U8, pcm_u8);
    REGISTER_ENCDEC  (PCM_U16BE, pcm_u16be);
    REGISTER_ENCDEC  (PCM_U16LE, pcm_u16le);
    REGISTER_ENCDEC  (PCM_U24BE, pcm_u24be);
    REGISTER_ENCDEC  (PCM_U24LE, pcm_u24le);
    REGISTER_ENCDEC  (PCM_U32BE, pcm_u32be);
    REGISTER_ENCDEC  (PCM_U32LE, pcm_u32le);
    REGISTER_ENCDEC  (PCM_ZORK , pcm_zork);

    /* dpcm codecs */
    REGISTER_DECODER (INTERPLAY_DPCM, interplay_dpcm);
    REGISTER_ENCDEC  (ROQ_DPCM, roq_dpcm);
    REGISTER_DECODER (SOL_DPCM, sol_dpcm);
    REGISTER_DECODER (XAN_DPCM, xan_dpcm);

    /* adpcm codecs */
    REGISTER_ENCDEC  (ADPCM_4XM, adpcm_4xm);
    REGISTER_ENCDEC  (ADPCM_ADX, adpcm_adx);
    REGISTER_ENCDEC  (ADPCM_CT, adpcm_ct);
    REGISTER_ENCDEC  (ADPCM_EA, adpcm_ea);
    REGISTER_ENCDEC  (ADPCM_G726, adpcm_g726);
    REGISTER_DECODER (ADPCM_IMA_AMV, adpcm_ima_amv);
    REGISTER_ENCDEC  (ADPCM_IMA_DK3, adpcm_ima_dk3);
    REGISTER_ENCDEC  (ADPCM_IMA_DK4, adpcm_ima_dk4);
    REGISTER_ENCDEC  (ADPCM_IMA_QT, adpcm_ima_qt);
    REGISTER_ENCDEC  (ADPCM_IMA_SMJPEG, adpcm_ima_smjpeg);
    REGISTER_ENCDEC  (ADPCM_IMA_WAV, adpcm_ima_wav);
    REGISTER_ENCDEC  (ADPCM_IMA_WS, adpcm_ima_ws);
    REGISTER_ENCDEC  (ADPCM_MS, adpcm_ms);
    REGISTER_ENCDEC  (ADPCM_SBPRO_2, adpcm_sbpro_2);
    REGISTER_ENCDEC  (ADPCM_SBPRO_3, adpcm_sbpro_3);
    REGISTER_ENCDEC  (ADPCM_SBPRO_4, adpcm_sbpro_4);
    REGISTER_ENCDEC  (ADPCM_SWF, adpcm_swf);
    REGISTER_DECODER (ADPCM_THP, adpcm_thp);
    REGISTER_ENCDEC  (ADPCM_XA, adpcm_xa);
    REGISTER_ENCDEC  (ADPCM_YAMAHA, adpcm_yamaha);
#endif


// init ..._init funcp
/*
  h264dsputil_init_funcp = h264dsputil_init;
  ff_vc1dsp_init_funcp = ff_vc1dsp_init;
  ff_mpeg4_decode_picture_header_funcp = ff_mpeg4_decode_picture_header;
  ff_wmv2_add_mb_funcp = ff_wmv2_add_mb;
  ff_mspel_motion_funcp = ff_mspel_motion;
*/

  return NULL;
}

