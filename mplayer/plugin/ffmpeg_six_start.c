
#include "../config.h"
#include "../libavcodec/dsputil.h"
#include "../libavcodec/avcodec.h"
#define NULL ((void *)0)

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(ENABLE_##X##_DECODER)  register_avcodec(&x##_decoder); }

extern unsigned int _fbss_plugin, _end_plugin;

void *_start_six ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  REGISTER_DECODER (LIBAMR_NB, libamr_nb);
  REGISTER_DECODER (ATRAC3, atrac3);
  return NULL;
}

