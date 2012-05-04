

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#else
#include <uclib.h>
#include <uclib.h>
#endif

#include "config.h"

#ifdef __LINUX__
#define HAVE_LIBDL
#ifdef HAVE_LIBDL
#include <dlfcn.h>
#endif
#endif

#include "mp_msg.h"
#include "help_mp.h"
#include "libavcodec/avcodec.h"

#include "vd_internal.h"

#ifdef __LINUX__
#include "loader/wine/windef.h"
#endif

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

static vd_info_t info = {
	"RealVideo decoder",
	"realvid",
	"Alex Beregszaszi",
	"Florian Schneider, Arpad Gereoffy, Alex Beregszaszi, Donnie Smith",
	"binary real video codecs"
};
#ifndef HX_RESULT
typedef int HX_RESULT;
#endif /* #ifndef HX_RESULT */

extern HX_RESULT _RV40toYUV420CustomMessage(unsigned int *msg_id, void *global);

extern HX_RESULT _RV40toYUV420HiveMessage(unsigned int *msg_id, void *global);

extern HX_RESULT _RV40toYUV420Transform    (
    unsigned char     *pRV20Packets,
    unsigned char     *pDecodedFrameBuffer,
    void      *pInputParams,
    void      *pOutputParams,
    void      *global
    );

extern HX_RESULT _RV40toYUV420Free(void *global);

extern HX_RESULT _RV40toYUV420Init(void *prv10Init, void **decoderState);

LIBVD_EXTERN(realvid)

#ifndef __LINUX__
vd_functions_t *vd_realvid_table (void)
{
	return &mpcodecs_vd_realvid;
}
#endif

/*
 * Structures for data packets.  These used to be tables of unsigned ints, but
 * that does not work on 64 bit platforms (e.g. Alpha).  The entries that are
 * pointers get truncated.  Pointers on 64 bit platforms are 8 byte longs.
 * So we have to use structures so the compiler will assign the proper space
 * for the pointer.
 */
typedef struct cmsg_data_s {
	uint32_t data1;
	uint32_t data2;
	uint32_t* dimensions;
} cmsg_data_t;

typedef struct transform_in_s {
	uint32_t len;
	uint32_t unknown1;
	uint32_t chunks;
	uint32_t* extra;
	uint32_t unknown2;
	uint32_t timestamp;
} transform_in_t;

static unsigned long (*rvyuv_custom_message)(cmsg_data_t* ,void*);
static unsigned long (*rvyuv_free)(void*);
static unsigned long (*rvyuv_init)(void*, void*); // initdata,context
static unsigned long (*rvyuv_transform)(char*, char*,transform_in_t*,unsigned int*,void*);
#ifdef USE_WIN32DLL
static unsigned long WINAPI (*wrvyuv_custom_message)(cmsg_data_t* ,void*);
static unsigned long WINAPI (*wrvyuv_free)(void*);
static unsigned long WINAPI (*wrvyuv_init)(void*, void*); // initdata,context
static unsigned long WINAPI (*wrvyuv_transform)(char*, char*,transform_in_t*,unsigned int*,void*);
#endif

static void *rv_handle=NULL;
static int inited=0;

static uint32_t *buffer = NULL;
static int bufsz = 0;
#ifdef USE_WIN32DLL
static int dll_type = 0; /* 0 = unix dlopen, 1 = win32 dll */
#endif

void *__builtin_vec_new(unsigned long size) {
	return malloc(size);
}

void __builtin_vec_delete(void *mem) {
	free(mem);
}

void __pure_virtual(void) {
	printf("FATAL: __pure_virtual() called!\n");
//	exit(1);
}

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
void ___brk_addr(void) {exit(0);}
char **__environ={NULL};
#undef stderr
FILE *stderr=NULL;
#endif

// to set/get/query special features/parameters
static int control(sh_video_t *sh,int cmd,void* arg,...){
//    switch(cmd){
//    case VDCTRL_QUERY_MAX_PP_LEVEL:
//	return 9;
//    case VDCTRL_SET_PP_LEVEL:
//	vfw_set_postproc(sh,10*(*((int*)arg)));
//	return CONTROL_OK;
//    }
    return CONTROL_UNKNOWN;
}

/* exits program when failure */
#ifdef HAVE_LIBDL

static int load_syms_linux(char *path) {
#ifdef 	__LINUX__
	void *handle;

	mp_msg(MSGT_DECVIDEO,MSGL_V, "opening shared obj '%s'\n", path);
	handle = dlopen (path, RTLD_LAZY);
	if (!handle) {
		mp_msg(MSGT_DECVIDEO,MSGL_WARN,"Error: %s\n",dlerror());
		return 0;
	}

	rvyuv_custom_message = dlsym(handle, "RV20toYUV420CustomMessage");
	rvyuv_free = dlsym(handle, "RV20toYUV420Free");
	rvyuv_init = dlsym(handle, "RV20toYUV420Init");
	rvyuv_transform = dlsym(handle, "RV20toYUV420Transform");

    if(rvyuv_custom_message &&
       rvyuv_free &&
       rvyuv_init &&
       rvyuv_transform)
    {
		rv_handle = handle;
		return 1;
    }
	
	rvyuv_custom_message = dlsym(handle, "RV40toYUV420CustomMessage");
	rvyuv_free = dlsym(handle, "RV40toYUV420Free");
	rvyuv_init = dlsym(handle, "RV40toYUV420Init");
	rvyuv_transform = dlsym(handle, "RV40toYUV420Transform");
#else

	//rvyuv_custom_message = dlsym(handle, "RV40toYUV420CustomMessage");
	//rvyuv_free = dlsym(handle, "RV40toYUV420Free");
	//rvyuv_init = dlsym(handle, "RV40toYUV420Init");
	//rvyuv_transform = dlsym(handle, "RV40toYUV420Transform");


	rvyuv_custom_message = _RV40toYUV420CustomMessage;
	rvyuv_free = _RV40toYUV420Free;
	rvyuv_init = _RV40toYUV420Init;
	rvyuv_transform = _RV40toYUV420Transform;
#endif

		
    if(rvyuv_custom_message &&
       rvyuv_free &&
       rvyuv_init &&
       rvyuv_transform)
    {
#ifdef 	__LINUX__
		rv_handle = handle;
#else
		rv_handle = 0;
#endif
		return 1;
    }

    mp_msg(MSGT_DECVIDEO,MSGL_WARN,"Error resolving symbols! (version incompatibility?)\n");
#ifdef __LINUX__
    dlclose(handle);
#endif
    return 0;
}
#endif

#ifdef USE_WIN32DLL

#ifdef WIN32_LOADER
#include "loader/ldt_keeper.h"
#endif
void* WINAPI LoadLibraryA(char* name);
void* WINAPI GetProcAddress(void* handle,char* func);
int WINAPI FreeLibrary(void *handle);

#ifndef WIN32_LOADER
void * WINAPI GetModuleHandleA(char *);
static int patch_dll(uint8_t *patchpos, const uint8_t *oldcode,
                     const uint8_t *newcode, int codesize) {
	void *handle = GetModuleHandleA("kernel32");
	int WINAPI (*VirtProt)(void *, unsigned, int, int *);
	int res = 0;
	int prot, tmp;
	VirtProt = GetProcAddress(handle, "VirtualProtect");
	// change permissions to PAGE_WRITECOPY
	if (!VirtProt ||
		!VirtProt(patchpos, codesize, 0x08, &prot)) {
		mp_msg(MSGT_DECVIDEO, MSGL_WARN, "VirtualProtect failed at %p\n", patchpos);
		return 0;
	}
	if (memcmp(patchpos, oldcode, codesize) == 0) {
		memcpy(patchpos, newcode, codesize);
		res = 1;
	}
	VirtProt(patchpos, codesize, prot, &tmp);
	return res;
}
#endif

static int load_syms_windows(char *path) {
    void *handle;

    mp_msg(MSGT_DECVIDEO,MSGL_V, "opening win32 dll '%s'\n", path);
#ifdef WIN32_LOADER
    Setup_LDT_Keeper();
#endif
    handle = LoadLibraryA(path);
    mp_msg(MSGT_DECVIDEO,MSGL_V,"win32 real codec handle=%p  \n",handle);
    if (!handle) {
		mp_msg(MSGT_DECVIDEO,MSGL_WARN,"Error loading dll\n");
		return 0;
    }

    wrvyuv_custom_message = GetProcAddress(handle, "RV20toYUV420CustomMessage");
    wrvyuv_free = GetProcAddress(handle, "RV20toYUV420Free");
    wrvyuv_init = GetProcAddress(handle, "RV20toYUV420Init");
    wrvyuv_transform = GetProcAddress(handle, "RV20toYUV420Transform");

    if(wrvyuv_custom_message &&
       wrvyuv_free &&
       wrvyuv_init &&
       wrvyuv_transform)
    {
		dll_type = 1;
		rv_handle = handle;
#ifndef WIN32_LOADER
		{
			int patched = 0;
			// drv43260.dll
			if (wrvyuv_transform == (void *)0x634114d0) {
				// patch away multithreaded decoding, it causes crashes
				static const uint8_t oldcode[13] = {
					0x83, 0xbb, 0xf8, 0x05, 0x00, 0x00, 0x01,
					0x0f, 0x86, 0xd0, 0x00, 0x00, 0x00 };
				static const uint8_t newcode[13] = {
					0x31, 0xc0,
					0x89, 0x83, 0xf8, 0x05, 0x00, 0x00,
					0xe9, 0xd0, 0x00, 0x00, 0x00 };
				patched = patch_dll((void *)0x634132fa, oldcode, newcode,
									sizeof(oldcode));
			}
			if (!patched)
				mp_msg(MSGT_DECVIDEO, MSGL_WARN, "Could not patch Real codec, this might crash on multi-CPU systems\n");
		}
#endif
		return 1;
    }
    mp_msg(MSGT_DECVIDEO,MSGL_WARN,"Error resolving symbols! (version incompatibility?)\n");
    FreeLibrary(handle);
    return 0; // error
}
#endif

/* we need exact positions */
struct rv_init_t {
	short unk1;
	short w;
	short h;
	short unk3;
	int unk2;
	int subformat;
	int unk5;
	int format;
} rv_init_t;

// init driver
static int init(sh_video_t *sh){
	//unsigned int out_fmt;
	char *path;
	int result;
	// we export codec id and sub-id from demuxer in bitmapinfohdr:
	unsigned char* extrahdr=(unsigned char*)(sh->bih+1);
	unsigned int extrahdr_size = sh->bih->biSize - sizeof(BITMAPINFOHEADER);
	struct rv_init_t init_data;
	if(extrahdr_size < 8) {
	    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"realvideo: extradata too small (%u)\n", sh->bih->biSize - sizeof(BITMAPINFOHEADER));
	    return 0;
	}
	init_data = (struct rv_init_t){11, sh->disp_w, sh->disp_h, 0, 0, be2me_32(((unsigned int*)extrahdr)[0]), 1, be2me_32(((unsigned int*)extrahdr)[1])}; // rv30

	mp_msg(MSGT_DECVIDEO,MSGL_V,"realvideo codec id: 0x%08X  sub-id: 0x%08X\n",be2me_32(((unsigned int*)extrahdr)[1]),be2me_32(((unsigned int*)extrahdr)[0]));

	path = malloc(strlen(REALCODEC_PATH)+strlen(sh->codec->dll)+2);
	if (!path) return 0;
	sprintf(path, REALCODEC_PATH "/%s", sh->codec->dll);

	/* first try to load linux dlls, if failed and we're supporting win32 dlls,
	   then try to load the windows ones */
#ifdef HAVE_LIBDL       
		if(strstr(sh->codec->dll,".dll") || !load_syms_linux(path))
#endif
#ifdef USE_WIN32DLL
	    if (!load_syms_windows(sh->codec->dll))
#endif
		{
#ifdef JZ47_OPT
	rvyuv_custom_message = _RV40toYUV420CustomMessage;
	rvyuv_free = _RV40toYUV420Free;
	rvyuv_init = _RV40toYUV420Init;
	rvyuv_transform = _RV40toYUV420Transform;
#else
			//mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_MissingDLLcodec,sh->codec->dll);
			mp_msg(MSGT_DECVIDEO,MSGL_HINT,"Read the RealVideo section of the DOCS!\n");
			//free(path);
			return 0;
#endif
		}
	free(path);
	// only I420 supported
//	if((sh->format!=0x30335652) && !mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_I420)) return 0;
	// init codec:
#ifdef __LINUX__
  #ifdef USE_IPU_THROUGH_MODE
        sh->context=1;
  #else
        sh->context=NULL;
  #endif
#else
	sh->context=get_display_is_direct();
#endif
	
#ifdef USE_WIN32DLL
	if (dll_type == 1)
	    result=(*wrvyuv_init)(&init_data, &sh->context);
	else
#endif
	  result=(*rvyuv_init)(&init_data, &sh->context);

	if (result){
	    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Couldn't open RealVideo codec, error code: 0x%X  \n",result);
	    return 0;
	}
	// setup rv30 codec (codec sub-type and image dimensions):
	if((sh->format<=0x30335652) && (be2me_32(((unsigned int*)extrahdr)[1])>=0x20200002)){
	    int i, cmsg_cnt;
	    uint32_t cmsg24[16]={sh->disp_w,sh->disp_h};
	    cmsg_data_t cmsg_data={0x24,1+(extrahdr[1]&7), &cmsg24[0]};

	    mp_msg(MSGT_DECVIDEO,MSGL_V,"realvideo: using cmsg24 with %u elements.\n",extrahdr[1]&7);
	    cmsg_cnt = (extrahdr[1]&7)*2;
	    if (extrahdr_size-8 < cmsg_cnt) {
	        mp_msg(MSGT_DECVIDEO,MSGL_WARN,"realvideo: not enough extradata (%u) to make %u cmsg24 elements.\n",extrahdr_size-8,extrahdr[1]&7);
	        cmsg_cnt = extrahdr_size-8;
	    }
	    for (i = 0; i < cmsg_cnt; i++)
	        cmsg24[2+i] = extrahdr[8+i]*4;
	    if (extrahdr_size-8 > cmsg_cnt)
	        mp_msg(MSGT_DECVIDEO,MSGL_WARN,"realvideo: %u bytes of unknown extradata remaining.\n",extrahdr_size-8-cmsg_cnt);

#ifdef USE_WIN32DLL
	    if (dll_type == 1)
			(*wrvyuv_custom_message)(&cmsg_data,sh->context);
	    else
#endif
			(*rvyuv_custom_message)(&cmsg_data,sh->context);
	}
	mp_msg(MSGT_DECVIDEO,MSGL_V,"INFO: RealVideo codec init OK!\n");
	return 1;
}

// uninit driver
static void uninit(sh_video_t *sh){
#ifdef USE_WIN32DLL
	if (dll_type == 1)
	{
	    if (wrvyuv_free) wrvyuv_free(sh->context);
	} else
#endif
		if(rvyuv_free) rvyuv_free(sh->context);

#ifdef USE_WIN32DLL
	if (dll_type == 1)
	{
	    if (rv_handle) FreeLibrary(rv_handle);
	} else
#endif
#ifdef HAVE_LIBDL
#ifdef __LINUX__
		if(rv_handle) dlclose(rv_handle);
#endif
#endif
		rv_handle=NULL;
	inited = 0;
	if (buffer)
	    free(buffer);
	buffer = NULL;
	bufsz = 0;
}

// copypaste from demux_real.c - it should match to get it working!
typedef struct dp_hdr_s {
    uint32_t chunks;	// number of chunks
    uint32_t timestamp; // timestamp from packet header
    uint32_t len;	// length of actual data
    uint32_t chunktab;	// offset to chunk offset array
    uint32_t flags;
} dp_hdr_t;
// decode a frame
static mp_image_t* decode(sh_video_t *sh,void* data,int len,int flags){
	mp_image_t* mpi;
	unsigned long result;
	dp_hdr_t* dp_hdr=(dp_hdr_t*)data;
	unsigned char* dp_data=((unsigned char*)data)+sizeof(dp_hdr_t);
	uint32_t* extra=(uint32_t*)(((char*)data)+dp_hdr->chunktab);

	unsigned int transform_out[5];
	transform_in_t transform_in;
	if(flags==1)
	{
		transform_in=(transform_in_t){
			dp_hdr->len,	// length of the packet (sub-packets appended)
			0,		// unknown, seems to be unused
			dp_hdr->chunks,	// number of sub-packets - 1
			extra,		// table of sub-packet offsets
			0x206,		// unknown, seems to be unused
			dp_hdr->timestamp,// timestamp (the integer value from the stream)
		};
	
	}else
	{
		transform_in=(transform_in_t){
			dp_hdr->len,	// length of the packet (sub-packets appended)
			0,		// unknown, seems to be unused
			dp_hdr->chunks,	// number of sub-packets - 1
			extra,		// table of sub-packet offsets
			0x4,		// unknown, seems to be unused
			dp_hdr->timestamp,// timestamp (the integer value from the stream)
		};
	}	 
	//F("flags = %x\n",flags);
	if(len<=0 || flags&2) return NULL; // skipped frame || hardframedrop
		
/*
  if (bufsz < sh->disp_w*sh->disp_h*3/2) {
  if (buffer) free(buffer);
  bufsz = sh->disp_w*sh->disp_h*3/2;
  buffer=malloc(bufsz);
  if (!buffer) return 0;
  }
*/
	if(buffer == NULL)
	{
		buffer = malloc(sizeof(unsigned int) * 16);
		//return 0;
	}
#ifdef USE_WIN32DLL
	if (dll_type == 1)
	    result=(*wrvyuv_transform)(dp_data, buffer, &transform_in,
								   transform_out, sh->context);
	else
#endif
		// F("%x\n",buffer);
		//static unsigned int framecount = 0;
		//framecount++;
		//extern int rv9_skip_dblk;
		//if((framecount & 0xf) == 0)
		//	rv9_skip_dblk = 0;
		static int drop_image = 0;
	
		if(demuxer_get_drop_image())
		{
			
			if(dp_hdr->flags)
			{
				drop_image = 1;
				demuxer_clear_drop_image();
			}else
			{
				return 0;
			}
		}

	result=(*rvyuv_transform)(dp_data, buffer, &transform_in,
							  transform_out, sh->context);
#if (RELEASE_TO_AUDIO==0)		
	if(transform_out[0] == 0)
      	return NULL;
#endif
	
	if(!inited && (!(transform_out[1] & 0x100))){  // rv30 width/height now known
	    sh->aspect=(float)sh->disp_w /(float)sh->disp_h;
	    sh->disp_w=transform_out[3];
	    sh->disp_h=transform_out[4];
	    if (!mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_I420)) return 0;
	    //if (!mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_YV12)) return 0;
	    inited=1;
	} 
	mpi=mpcodecs_get_image(sh, MP_IMGTYPE_EXPORT, 0 /*MP_IMGFLAG_ACCEPT_STRIDE*/,
						   sh->disp_w, sh->disp_h);

#if RELEASE_TO_AUDIO
	if(transform_out[1] & 0x100)
	{
		mpi->pict_type = 0x100;
#ifndef __LINUX__
		int dcycle = *(unsigned int *)0xB380C204 - *(unsigned int *)0xB380C200;
		set_release_cycle(dcycle * 6000);
#endif
		return mpi;
	}
	if(transform_out[0] == 0)
      	return NULL;
    
#endif      	
	static int timestamp = 0;
	if(drop_image)
	{
		if(drop_image == 1)
		{
			timestamp = dp_hdr->timestamp;
			drop_image = 2;
		}else if(drop_image == 2)
		{
			if(dp_hdr->timestamp - timestamp > 1)
			{
				drop_image = 0;
				
			}else
			{
				timestamp = dp_hdr->timestamp;
				return NULL;	
			}
			
		}
		
	}
	
	if(!mpi) 
	{
		return NULL;
	}
	//#undef printf
	mpi->planes[0] = *(buffer + 2);
	mpi->stride[0] = *(buffer + 3);
	mpi->planes[1] = *(buffer + 4);
	mpi->stride[1] = *(buffer + 5);
	mpi->planes[2] = *(buffer + 6);
	mpi->stride[2] = *(buffer + 7);

	//mpi->pict_type = (transform_out[1] & 4)?1:0;
	if (transform_out[1] & 4)
	  mpi->pict_type = FF_I_TYPE;
	else if (transform_out[1] & 8)
	  mpi->pict_type = FF_B_TYPE;
	else
	  mpi->pict_type = FF_P_TYPE;

	mpi->stride[3] = *(buffer + 9);    // error frame
    // sh->pts = (float)(transform_out[2]) / 1000.0;
	//printf("sh->pts = %f\n",sh->pts);
	//mpi->pict_type = (*(buffer + 8) & 4)?1:0;
	//#undef printf
	//printf("transform_out[1] = %x %x\n",transform_out[1],*(buffer + 8));
	//printf("%x\n",*(buffer + 8));
	//printf("%x\n",*(buffer + 9));
	if(transform_out[0] == 0) return 0;
	if(transform_out[0] &&
	   (sh->disp_w != transform_out[3] || sh->disp_h != transform_out[4]))
	{    
	    inited = 0;
	}
	return (result?NULL:mpi);
}
