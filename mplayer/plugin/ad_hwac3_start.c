
#include <uclib.h>
#include "config.h"

#include "../libmpcodecs/ad_internal.h"

extern unsigned int _fbss_plugin, _end_plugin ;
extern ad_functions_t *ad_msgsm_table (void);

ad_functions_t *ad_hwac3_start ()
{
  unsigned int *p = &_fbss_plugin;

  printf ("++++++++ audio plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  /* load func table */
  
  return ad_hwac3_table();
}

