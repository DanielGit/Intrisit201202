
#include "config.h"

#ifdef __LINUX__
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#include "stream.h"

static int open_s(stream_t *stream,int mode, void* opts, int* file_format) {
  stream->type = STREAMTYPE_DUMMY;

  return 1;
}


stream_info_t stream_info_null = {
  "Null stream",
  "null",
  "Albeu",
  "",
  open_s,
  { "null", NULL },
  NULL,
  0 // Urls are an option string
};
