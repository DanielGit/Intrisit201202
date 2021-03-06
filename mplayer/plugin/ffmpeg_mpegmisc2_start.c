
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }
/*
ff_mspel_motion_funcp
ff_h261_loop_filter_funcp
ff_mpeg1_find_frame_end_funcp
ff_wmv2_add_mb_funcp
h264dsputil_init_funcp
ff_vc1dsp_init_funcp
ff_mpeg4_decode_picture_header_funcp
ff_cavsdsp_init_funcp
*/


extern unsigned int _fbss_plugin, _end_plugin;

void *_vstart_mpegmisc2 ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;
  
  first_avcodec = NULL;
  REGISTER_DECODER  (SVQ1, svq1);
  REGISTER_DECODER (MMVIDEO, mmvideo);
  REGISTER_DECODER (MSRLE, msrle);
  REGISTER_DECODER (MSVIDEO1, msvideo1);
  REGISTER_DECODER (MSZH, mszh);
  REGISTER_DECODER (ZLIB, zlib);
  REGISTER_DECODER (SMC, smc);
  REGISTER_DECODER (TARGA, targa);
  REGISTER_DECODER (PTX, ptx);
  REGISTER_DECODER (QDRAW, qdraw);
  REGISTER_DECODER (QPEG, qpeg);
  REGISTER_DECODER (QTRLE, qtrle);
  REGISTER_DECODER (RAWVIDEO, rawvideo);
  REGISTER_DECODER (ROQ, roq);
  REGISTER_DECODER (RPZA, rpza);

  return NULL;
}

/*
ff_mspel_motion_funcp
ff_h261_loop_filter_funcp
ff_mpeg1_find_frame_end_funcp
ff_wmv2_add_mb_funcp
h264dsputil_init_funcp
ff_vc1dsp_init_funcp
ff_mpeg4_decode_picture_header_funcp
ff_cavsdsp_init_funcp
*/
