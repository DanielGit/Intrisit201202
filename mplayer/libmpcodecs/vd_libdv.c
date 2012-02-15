

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <uclib.h>
#include <uclib.h>
#include <uclib.h>
#include <sys/types.h>
#include <uclib.h>
#endif

#include <math.h>

#include "config.h"

#include "img_format.h"

#include <libdv/dv.h>
#include <libdv/dv_types.h>

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#include "vd_internal.h"

static vd_info_t info =
{
	"Raw DV Video Decoder",
	"libdv",
	"Alexander Neundorf <neundorf@kde.org>",
	"http://libdv.sf.net",
	"native codec"
};

LIBVD_EXTERN(libdv)

// to set/get/query special features/parameters
static int control(sh_video_t *sh,int cmd,void* arg,...){
    return CONTROL_UNKNOWN;
}

static dv_decoder_t* global_rawdv_decoder=NULL;

dv_decoder_t* init_global_rawdv_decoder(void)
{
 if(!global_rawdv_decoder){
   global_rawdv_decoder=dv_decoder_new(TRUE,TRUE,FALSE);
   global_rawdv_decoder->quality=DV_QUALITY_BEST;
   global_rawdv_decoder->prev_frame_decoded = 0;
 }
 return global_rawdv_decoder;
}

// init driver
static int init(sh_video_t *sh)
{
   sh->context = (void *)init_global_rawdv_decoder();
   return mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_YUY2);
}

// uninit driver
static void uninit(sh_video_t *sh){
}

// decode a frame
static mp_image_t* decode(sh_video_t *sh,void* data,int len,int flags)
{
   mp_image_t* mpi;
   dv_decoder_t *decoder=sh->context;
   
   if(len<=0 || (flags&3)){
//      fprintf(stderr,"decode() (rawdv) SKIPPED\n");
      return NULL; // skipped frame
   }

   dv_parse_header(decoder, data);

   mpi=mpcodecs_get_image(sh, MP_IMGTYPE_TEMP, MP_IMGFLAG_ACCEPT_STRIDE, sh->disp_w, sh->disp_h);
    
   if(!mpi){	// temporary!
      fprintf(stderr,"couldn't allocate image for stderr codec\n");
      return NULL;
   }

   dv_decode_full_frame(decoder, data, e_dv_color_yuv, mpi->planes, mpi->stride);
   
   return mpi;
}
