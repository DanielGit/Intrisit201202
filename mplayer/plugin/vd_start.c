
#include "../libmpcodecs/vd_internal.h"

extern unsigned int _fbss_plugin, _end_plugin ;
extern vd_functions_t *func_table (void);

vd_functions_t *_start ()
{
  unsigned int *p = &_fbss_plugin;

  printf ("++++++++ video plugin start: BSS(0x%08x, 0x%08x) +++++++++\n", &_fbss_plugin, &_end_plugin);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  /* load func table */
  
  return func_table();
}

