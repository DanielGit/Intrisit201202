
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

void *_start_five ()
{
  unsigned int *p = &_fbss_plugin;

  //printf ("++++++++ ffmpeg plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  /* audio codecs */
    REGISTER_DECODER  (AC3, ac3);
    REGISTER_DECODER  (VORBIS, vorbis);

  return NULL;
}

