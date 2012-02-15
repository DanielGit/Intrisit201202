/* setenv implementation for systems lacking it. */

#include "config.h"

#ifdef __LINUX__
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#endif
#ifndef MP_DEBUG
  #define NDEBUG
#endif
#include <assert.h>

int setenv(const char *name, const char *val, int overwrite)
{
  int len  = strlen(name) + strlen(val) + 2;
  char *env = malloc(len);
  if (!env) { return -1; }

  assert(overwrite != 0);

  strcpy(env, name);
  strcat(env, "=");
  strcat(env, val);
  putenv(env);

  return 0;
}
