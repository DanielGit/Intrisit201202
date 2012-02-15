
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern unsigned int _fbss_plugin, _end_plugin;

void *_start_two ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

    /* audio codecs */
    REGISTER_DECODER (MP2, mp2);
    REGISTER_DECODER (MP3, mp3);
    REGISTER_DECODER (MP3ADU, mp3adu);
    REGISTER_DECODER (MP3ON4, mp3on4);


#if 0
    /* audio codecs */
    REGISTER_ENCDEC  (LIBGSM, libgsm);
    REGISTER_ENCDEC  (LIBGSM_MS, libgsm_ms);
    if (!ENABLE_VORBIS_ENCODER)  REGISTER_ENCODER (LIBVORBIS, libvorbis);
    if (!ENABLE_VORBIS_DECODER)  REGISTER_DECODER (LIBVORBIS, libvorbis);
    REGISTER_DECODER (MACE3, mace3);
    REGISTER_DECODER (MACE6, mace6);
    REGISTER_DECODER (MPC7, mpc7);
    REGISTER_DECODER (QDM2, qdm2);
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
//

    REGISTER_ENCODER (LIBMP3LAME, libmp3lame);
    REGISTER_ENCODER (LIBTHEORA, libtheora);

    REGISTER_DECODER (MPEG4AAC, mpeg4aac);
    REGISTER_ENCDEC  (AC3, ac3);
    REGISTER_DECODER (ATRAC3, atrac3);
    REGISTER_DECODER (COOK, cook);

    REGISTER_ENCDEC  (LIBAMR_WB, libamr_wb);
    REGISTER_DECODER (LIBA52, liba52);
    REGISTER_ENCODER (LIBFAAC, libfaac);
    REGISTER_DECODER (LIBFAAD, libfaad);
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

