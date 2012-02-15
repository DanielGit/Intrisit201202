
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

int8_t  table_4_3_exp[16];
uint32_t table_4_3_value[16];
uint32_t exp_table[16];
uint32_t expval_table[16];

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern unsigned int _fbss_plugin, _end_plugin;

void *_start_four ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

    /* audio codecs */
    REGISTER_DECODER (MPC7, mpc7);
    REGISTER_DECODER (QDM2, qdm2);
    REGISTER_DECODER (TRUESPEECH, truespeech);
    REGISTER_DECODER (TTA, tta);
    REGISTER_DECODER (WAVPACK, wavpack);
    REGISTER_DECODER (IMC, imc);
#if 0
    /* audio codecs */
//
    REGISTER_DECODER (MPEG4AAC, mpeg4aac);
    REGISTER_ENCDEC  (AC3, ac3);

    REGISTER_DECODER (LIBA52, liba52);
    REGISTER_DECODER (LIBFAAD, libfaad);
    REGISTER_ENCDEC  (VORBIS, vorbis);

    if (!ENABLE_VORBIS_ENCODER)  REGISTER_ENCODER (LIBVORBIS, libvorbis);
    if (!ENABLE_VORBIS_DECODER)  REGISTER_DECODER (LIBVORBIS, libvorbis);
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

