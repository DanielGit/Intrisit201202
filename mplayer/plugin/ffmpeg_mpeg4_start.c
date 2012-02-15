
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

/*
ff_h261_loop_filter_funcp
ff_mpeg1_find_frame_end_funcp
ff_wmv2_add_mb_funcp
h264dsputil_init_funcp
ff_vc1dsp_init_funcp
ff_mpeg4_decode_picture_header_funcp
ff_cavsdsp_init_funcp
*/

extern void h264dsputil_init(DSPContext* c, AVCodecContext *avctx);
extern void (*h264dsputil_init_funcp)(DSPContext* c, AVCodecContext *avctx);
extern void ff_vc1dsp_init(DSPContext* dsp, AVCodecContext *avctx);
extern void (*ff_vc1dsp_init_funcp)(DSPContext* c, AVCodecContext *avctx);
extern void (*ff_wmv2_add_mb_funcp)();
extern void ff_wmv2_add_mb ();
extern void (*ff_mspel_motion_funcp)();
extern void ff_mspel_motion ();
extern int (*ff_mpeg4_decode_picture_header_funcp)();
extern int ff_mpeg4_decode_picture_header;


extern void (*ff_vp3_idct_put_c_funcp)();
extern void ff_vp3_idct_put_c();

extern void (*ff_vp3_idct_add_c_funcp)();
extern void ff_vp3_idct_add_c();

extern void (*ff_vp3_idct_c_funcp)();
extern void ff_vp3_idct_c();


extern unsigned int _fbss_plugin, _end_plugin;

void *_vstart_mpeg4 ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  first_avcodec = NULL;
  REGISTER_DECODER  (MPEG4, mpeg4);
  REGISTER_DECODER  (H263, h263);
  REGISTER_DECODER  (MSMPEG4V1, msmpeg4v1);
  REGISTER_DECODER  (MSMPEG4V2, msmpeg4v2);
  REGISTER_DECODER  (MSMPEG4V3, msmpeg4v3);
  REGISTER_DECODER  (WMV1, wmv1);
  REGISTER_DECODER  (WMV2, wmv2);
  REGISTER_DECODER  (WMV3, wmv3);
  REGISTER_DECODER  (VC1, vc1);
  REGISTER_DECODER  (H263I, h263i);
  REGISTER_DECODER  (FLV, flv);
  REGISTER_DECODER  (RV10, rv10);
  REGISTER_DECODER  (RV20, rv20);

  REGISTER_DECODER (THEORA, theora);
  REGISTER_DECODER (VP3, vp3);
  REGISTER_DECODER (VP5, vp5);
  REGISTER_DECODER (VP6, vp6);
  REGISTER_DECODER (VP6F, vp6f);
  REGISTER_DECODER (VP6A, vp6a);

// init ..._init funcp
  h264dsputil_init_funcp = h264dsputil_init;
  ff_vc1dsp_init_funcp = ff_vc1dsp_init;
  ff_mpeg4_decode_picture_header_funcp = ff_mpeg4_decode_picture_header;
  ff_wmv2_add_mb_funcp = ff_wmv2_add_mb;
  ff_mspel_motion_funcp = ff_mspel_motion;

  ff_vp3_idct_put_c_funcp = ff_vp3_idct_put_c;
  ff_vp3_idct_add_c_funcp = ff_vp3_idct_add_c;
  ff_vp3_idct_c_funcp = ff_vp3_idct_c;

  return NULL;
}

