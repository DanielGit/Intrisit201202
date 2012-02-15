
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

void *_vstart_mpegmisc ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;
  
  first_avcodec = NULL;
  REGISTER_DECODER (AASC, aasc);
  REGISTER_DECODER (BETHSOFTVID, bethsoftvid);
  REGISTER_DECODER (CINEPAK, cinepak);
  REGISTER_DECODER (CLJR, cljr);
  REGISTER_DECODER (CSCD, cscd);
  REGISTER_DECODER (CYUV, cyuv);
  REGISTER_DECODER (DNXHD, dnxhd);
  REGISTER_DECODER (FFV1, ffv1);
  REGISTER_DECODER (HUFFYUV, huffyuv);
  REGISTER_DECODER  (FFVHUFF, ffvhuff);
  REGISTER_DECODER (FLIC, flic);
  REGISTER_DECODER (FOURXM, fourxm);
  REGISTER_DECODER (FRAPS, fraps);
  REGISTER_DECODER (DSICINVIDEO, dsicinvideo);
  REGISTER_DECODER (DVVIDEO, dvvideo);
  REGISTER_DECODER (EIGHTBPS, eightbps);
  REGISTER_DECODER (GIF, gif);
  REGISTER_DECODER (BMP, bmp);
  REGISTER_DECODER (C93, c93);
  REGISTER_DECODER (IDCIN, idcin);
  REGISTER_DECODER (INDEO2, indeo2);
  REGISTER_DECODER (INDEO3, indeo3);
  REGISTER_DECODER (INTERPLAY_VIDEO, interplay_video);
  REGISTER_DECODER (KMVC, kmvc);
  REGISTER_DECODER (LOCO, loco);

//audio
REGISTER_DECODER (DSICINAUDIO, dsicinaudio);

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
