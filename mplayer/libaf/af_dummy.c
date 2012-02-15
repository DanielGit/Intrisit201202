/* The name speaks for itself this filter is a dummy and will not blow
   up regardless of what you do with it. */

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "af.h"
#else
#include <mplaylib.h>
#include "mplaylib.h"

#include "af.h"
#undef memcpy
#define memcpy uc_memcpy
#endif

// Initialization and runtime control
static int control(struct af_instance_s* af, int cmd, void* arg)
{
  switch(cmd){
  case AF_CONTROL_REINIT:
    memcpy(af->data,(af_data_t*)arg,sizeof(af_data_t));
    af_msg(AF_MSG_VERBOSE,"[dummy] Was reinitialized: %iHz/%ich/%s\n",
	af->data->rate,af->data->nch,af_fmt2str_short(af->data->format));
    return AF_OK;
  }
  return AF_UNKNOWN;
}

// Deallocate memory 
static void uninit(struct af_instance_s* af)
{
  if(af->data)
    free(af->data);
}

// Filter data through filter
static af_data_t* play(struct af_instance_s* af, af_data_t* data)
{
  // Do something necessary to get rid of annoying warning during compile
  if(!af)
    af_msg(AF_MSG_ERROR,"EEEK: Argument af == NULL in af_dummy.c play().");
  return data;
}

// Allocate memory and set function pointers
static int af_open(af_instance_t* af){
  af->control=control;
  af->uninit=uninit;
  af->play=play;
  af->mul.d=1;
  af->mul.n=1;
  af->data=malloc(sizeof(af_data_t));
  if(af->data == NULL)
    return AF_ERROR;
  return AF_OK;
}

// Description of this filter
af_info_t af_info_dummy = {
    "dummy",
    "dummy",
    "Anders",
    "",
    AF_FLAGS_REENTRANT,
    af_open
};
