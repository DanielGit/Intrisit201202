
#include "../config.h"
//#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern int (*ff_mpeg1_find_frame_end_funcp)();
extern int ff_mpeg1_find_frame_end ();
extern void (*ff_h261_loop_filter_funcp)();
extern void ff_h261_loop_filter();
extern void (*ff_cavsdsp_init_funcp)();
extern void ff_cavsdsp_init ();

extern unsigned int _fbss_plugin, _end_plugin;
void *_vstart_mpegvideo ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;
  
  first_avcodec = NULL;
  REGISTER_DECODER (MPEG1VIDEO, mpeg1video);
  REGISTER_DECODER (MPEG2VIDEO, mpeg2video);
  REGISTER_DECODER (MPEGVIDEO, mpegvideo);
  REGISTER_DECODER (MDEC, mdec);
  REGISTER_DECODER (ASV1, asv1);
  REGISTER_DECODER (ASV2, asv2);
  REGISTER_DECODER (AVS, avs);
  REGISTER_DECODER (CAVS, cavs);
  REGISTER_DECODER (SP5X, sp5x);
  REGISTER_DECODER (AMV, amv);
  REGISTER_DECODER (NUV, nuv);
  REGISTER_DECODER (H261, h261);
  REGISTER_DECODER (MJPEG, mjpeg);
  REGISTER_DECODER (THP, thp);
  REGISTER_DECODER (JPEGLS, jpegls);
  REGISTER_DECODER (MJPEGB, mjpegb);
  REGISTER_DECODER (SGI, sgi);
  REGISTER_DECODER (SMACKER, smacker);
  REGISTER_DECODER (TIERTEXSEQVIDEO, tiertexseqvideo);
  REGISTER_DECODER (TIFF, tiff);
  REGISTER_DECODER (TRUEMOTION1, truemotion1);
  REGISTER_DECODER (TRUEMOTION2, truemotion2);
  REGISTER_DECODER (TSCC, tscc);
  REGISTER_DECODER (TXD, txd);
  REGISTER_DECODER (ULTI, ulti);
  REGISTER_DECODER (VCR1, vcr1);
  REGISTER_DECODER (VMDVIDEO, vmdvideo);
  REGISTER_DECODER (VMNC, vmnc);
  REGISTER_DECODER (VQA, vqa);
  REGISTER_DECODER (WNV1, wnv1);
  REGISTER_DECODER (XAN_WC3, xan_wc3);
  REGISTER_DECODER (XL, xl);
  REGISTER_DECODER (XSUB, xsub);

// Audio
REGISTER_DECODER (SMACKAUD, smackaud);
REGISTER_DECODER (VMDAUDIO, vmdaudio);


// init ..._init funcp
   ff_mpeg1_find_frame_end_funcp = ff_mpeg1_find_frame_end;
   ff_h261_loop_filter_funcp = ff_h261_loop_filter;
   ff_cavsdsp_init_funcp = ff_cavsdsp_init;
  return NULL;
}

