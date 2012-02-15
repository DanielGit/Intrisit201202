
/// \file
/// \ingroup Properties Command2Property OSDMsgStack


#include "mplaylib.h"
#include "excpt.h"
#include "config.h"

#ifdef WIN32
#define _UWIN 1  /*disable Non-underscored versions of non-ANSI functions as otherwise int eof would conflict with eof()*/
#include <windows.h>
#endif

#include "mplaylib.h"



// #include <sys/mman.h>
//#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/ioctl.h>
#include <sys/wait.h>
#else
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGPIPE	13	/* broken pipe */
#endif
#include "mplayertm.h"
#include "version.h"
#include "mp_msg.h"

#define HELP_MP_DEFINE_STATIC
#include "help_mp.h"

#include "m_option.h"
#include "m_config.h"
#include "m_property.h"

#include "cfg-mplayer-def.h"

#include "subreader.h"

#include "libvo/video_out.h"

#include "libvo/font_load.h"
#include "libvo/sub.h"

#ifdef HAVE_X11
#include "libvo/x11_common.h"
#endif

#include "libao2/audio_out.h"

#include "codec-cfg.h"

#include "edl.h"

#include "spudec.h"
#include "vobsub.h"

#include "osdep/getch2.h"
#include "osdep/timer.h"

#include "cpudetect.h"

#ifdef HAVE_NEW_GUI
#include "gui/interface.h"
#endif

#include "input/input.h"
#define memcpy uc_memcpy
int slave_mode=0;
int player_idle_mode=0;
#define quiet 1
int enable_mouse_movements=0;

#ifdef WIN32
char * proc_priority=NULL;
#endif

#define ROUND(x) ((int)((x)<0 ? (x)-0.5 : (x)+0.5))

#ifdef HAVE_RTC
#ifdef __linux__
#include <linux/rtc.h>
#else
#include <rtc.h>
#define RTC_IRQP_SET RTCIO_IRQP_SET
#define RTC_PIE_ON   RTCIO_PIE_ON
#endif /* __linux__ */
#endif /* HAVE_RTC */

#ifdef USE_TV
#include "stream/tv.h"
#endif
#ifdef USE_RADIO
#include "stream/stream_radio.h"
#endif

#ifdef HAS_DVBIN_SUPPORT
#include "stream/dvbin.h"
#include "stream/cache2.h"
#endif

//**************************************************************************//
//             Playtree
//**************************************************************************//
#include "playtree.h"
#include "playtreeparser.h"

#ifdef HAVE_NEW_GUI
extern int import_playtree_playlist_into_gui(play_tree_t* my_playtree, m_config_t* config);
extern int import_initial_playtree_into_gui(play_tree_t* my_playtree, m_config_t* config, int enqueue);
#endif

//**************************************************************************//
//             Config
//**************************************************************************//
#include "parser-cfg.h"
#include "parser-mpcmd.h"

m_config_t* mconfig;

//**************************************************************************//
//             Config file
//**************************************************************************//

static int cfg_inc_verbose(m_option_t *conf){ ++verbose; return 0;}


#ifndef USE_16M_SDRAM
static int cfg_include(m_option_t *conf, char *filename){
	return m_config_parse_config_file(mconfig, filename);
}
#endif

#include "get_path.h"

//**************************************************************************//
//             XScreensaver
//**************************************************************************//

#ifdef HAVE_X11
void xscreensaver_heartbeat(void);
#endif

//**************************************************************************//
//**************************************************************************//
//             Input media streaming & demultiplexer:
//**************************************************************************//

static int max_framesize=0;

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
//#include "parse_es.h"
#include "libmpdemux/matroska.h"

#ifdef USE_DVDREAD
#include "stream/stream_dvd.h"
#endif

#ifdef USE_DVDNAV
#include "stream/stream_dvdnav.h"
#endif

#include "libmpcodecs/dec_audio.h"
#include "libmpcodecs/dec_video.h"
#include "libmpcodecs/mp_image.h"
#include "libmpcodecs/vf.h"
#include "libmpcodecs/vd.h"

#include "mixer.h"

#include "mp_core.h"

#if 0 //#ifdef USE_16M_SDRAM
float a52_drc_level = 1.0;
#endif
//**************************************************************************//
//**************************************************************************//

// Common FIFO functions, and keyboard/event FIFO code
#include "mp_fifo.h"
int noconsolecontrols=1;
//**************************************************************************//

// Not all functions in mplayer.c take the context as an argument yet
static MPContext mpctx_s = {
    .osd_function = OSD_PLAY,
    .begin_skip = MP_NOPTS_VALUE,
    .play_tree_step = 1,
    .global_sub_pos = -1,
    .set_of_sub_pos = -1,
    .file_format = DEMUXER_TYPE_UNKNOWN,
    .loop_times = -1,
#ifdef HAS_DVBIN_SUPPORT
    .last_dvb_step = 1,
#endif
};

MPContext *mpctx = &mpctx_s;
int fixed_vo=0;
static unsigned int mp_mode = 0;


#define AUDIO_DEC_GRADED 0x10
#define AUDIO_INC_GRADED 0x20

// benchmark:
int64_t video_time_usage=0;
int64_t vout_time_usage=0;
static int64_t audio_time_usage=0;
static int total_time_usage_start=0;
static int total_frame_cnt=0;
int drop_frame_cnt=0; // total number of dropped frames
int benchmark=0;

// options:
       int auto_quality=0;
static int output_quality=0;

//#define playback_speed 1.0

float playback_speed=1.0;

int use_gui=0;

#ifdef HAVE_NEW_GUI
int enqueue=0;
#endif

static int list_properties = 0;

int osd_level=0;
// if nonzero, hide current OSD contents when GetTimerMS() reaches this
unsigned int osd_visible;
int osd_duration = 1000;

int term_osd = 0;
static char* term_osd_esc = "\x1b[A\r\x1b[K";
static char* playing_msg = NULL;            
// seek:    
static double seek_to_sec = 0.0;
static off_t seek_to_byte=0;
static off_t step_sec=0;
static int loop_seek=0;

static m_time_size_t end_at = { .type = END_AT_NONE, .pos = 0 };

// A/V sync:
       int autosync=30; // 30 might be a good default value.

// may be changed by GUI:  (FIXME!)
float rel_seek_secs=0.0;
int abs_seek_pos=0;

// codecs:
char **audio_codec_list=NULL; // override audio codec
char **video_codec_list=NULL; // override video codec
char **audio_fm_list=NULL;    // override audio codec family 
char **video_fm_list=NULL;    // override video codec family 

// demuxer:
extern char *demuxer_name; // override demuxer
extern char *audio_demuxer_name; // override audio demuxer
extern char *sub_demuxer_name; // override sub demuxer

// streaming:
int audio_id=-1;
int video_id=-1;
int dvdsub_id=-2;
int vobsub_id=-1;
char* audio_lang=NULL;
char* dvdsub_lang=NULL;
static char* spudec_ifo=NULL;
char* filename=NULL; //"MI2-Trailer.avi";
int forced_subs_only=0;
int file_filter=0;

// cache2:
int stream_cache_size=-1;
#ifdef USE_STREAM_CACHE
extern int cache_fill_status;

float stream_cache_min_percent=20.0;
float stream_cache_seek_min_percent=50.0;
#else
#define cache_fill_status 0
#endif

// dump:
static char *stream_dump_name="stream.dump";
       int stream_dump_type=0;

// A-V sync:
static float default_max_pts_correction=-1;//0.01f;
static float max_pts_correction=0;//default_max_pts_correction;
static float c_total=0;
float audio_delay=0.0;//(64000.0 * 3.0)/2.0/22050.0/2.0;
static int ignore_start=0;

#define AUDIO_BUFF_SIZE 0x40000

#define softsleep 0
//static int softsleep=0;

 float force_fps=0;
static int force_srate=0;
static int audio_output_format=-1; // AF_FORMAT_UNKNOWN
int frame_dropping=0; // option  0=no drop  1= drop vo  2= drop decode
static int av_drop = 0; 
	
static int play_n_frames=-1;
static int play_n_frames_mf=-1;

// screen info:
char** video_driver_list=NULL;
char** audio_driver_list=NULL;

// sub:
char *font_name=NULL;
char *sub_font_name=NULL;
#ifdef HAVE_FONTCONFIG
extern int font_fontconfig;
#endif
float font_factor=0.75;
char **sub_name=NULL;
float sub_delay=0;
float sub_fps=0;
int   sub_auto = 0;
char *vobsub_name=NULL;
/*DSP!!char *dsp=NULL;*/
int   subcc_enabled=0;
int suboverlap_enabled = 1;

#ifdef JZ4740_MXU_OPT
unsigned int mxucr = 0;
#endif

//#define JZ47_ANL_TIME
#ifdef JZ47_ANL_TIME
#define ANL_CNT 8
unsigned int anl[ANL_CNT], anltime[ANL_CNT];
#endif

#ifdef USE_ASS
#include "libass/ass.h"
#include "libass/ass_mp.h"
#endif

extern int mp_msg_levels[MSGT_MAX];
extern int mp_msg_level_all;

char* current_module=NULL; // for debugging


// ---

#ifdef HAVE_MENU
#include "m_struct.h"
#include "libmenu/menu.h"
extern void vf_menu_pause_update(struct vf_instance_s* vf);
extern vf_info_t vf_info_menu;
static vf_info_t* libmenu_vfs[] = {
  &vf_info_menu,
  NULL
};
static vf_instance_t* vf_menu = NULL;
static int use_menu = 0;
static char* menu_cfg = NULL;
static char* menu_root = "main";
#endif


#ifdef HAVE_RTC
static int nortc = 1;
static char* rtc_device;
#endif

edl_record_ptr edl_records = NULL; ///< EDL entries memory area
edl_record_ptr next_edl_record = NULL; ///< only for traversing edl_records
short edl_decision = 0; ///< 1 when an EDL operation has been made.
FILE* edl_fd = NULL; ///< fd to write to when in -edlout mode.
int use_filedir_conf;
static unsigned int inited_flags=0;
static int mp_excptid = 0;
#include "mpcommon.h"
#include "command.h"
#include "metadata.h"

#define mp_basename2(s) (strrchr(s,'/')==NULL?(char*)s:(strrchr(s,'/')+1))

static void * mp_sem_pause = NULL;
void *audio_sem_filemode = NULL; 
static void * mp_sem_seek = NULL;
static unsigned int saveregister[2];
static char *singlefile = "SingleFile";
static char *audiofile = "AudioFile";
#include "GetAudioFunction.c"
extern void BUFF_SetPriority(unsigned int prio);

static void wait_audio_empty()
{
	if(!mpctx->audio_out) return;
	int timeoutcount = 100;
	while((mpctx->audio_out->get_space() != ao_data.buffersize) && (timeoutcount--))
		usec_sleep(10000);
	printf("wait audio empty mpctx->audio_out->get_space() = %d\n",mpctx->audio_out->get_space());
}	
#define MP_MODE(x) \
do{                \
	mp_mode &= 0xffff0000; \
	mp_mode |= x;          \
}while(0)
void seterrorseek()
{
	MP_MODE(3);
}
	
void mplayer_set_atob(int v)
{
		switch(v)
		{
			case 0:
				mp_mode &= 0xffff;
				mp_mode |= 0x40000;
				while((mp_mode & 0xffff0000) != 0x0000)
				{
					usec_sleep(10000);
				}
				break;
			case 1:
				mp_mode &= 0xffff;
				mp_mode |= 0x10000;
				while((mp_mode & 0xffff0000) != 0x0000)
				{
					usec_sleep(10000);
				}
				break;
		    case 2:
		 		mp_mode &= 0xffff;
		 		mp_mode |= 0x20000;
		 		while((mp_mode & 0xffff0000) != 0x30000)
				{
					usec_sleep(10000);
				}
				break;
		 }
		 
}
static int get_cur_play_time()
{
	if(videoshow)
	{
		return (int)(demuxer_get_current_time(mpctx->demuxer)*1000.0 );
	}else
	{
		if(!mpctx->audio_out) return;
		float f = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
		f = f* 1000.0;
		return (int)f;
	}
}
void SetPauseState(unsigned int value)
{
  if(mpctx == NULL)
  	return;
  if(mpctx->eof != 0)
  	return;
  	
   if(videoshow == 1)
   {
		if(value)
		{
			unsigned char err;
			os_SemaphoreSet(mp_sem_pause,0,&err);
			mpctx->osd_function = OSD_PAUSE;
		}else
		{
			mpctx->osd_function =OSD_PLAY;
	    	os_SemaphorePost(mp_sem_pause);
		}
	}else
	{
		
			if(value)
			{
				
				MP_MODE(0x10);
				if(mpctx->audio_out) 
				{	
					mpctx->audio_out->control(AOCONTROL_SET_VOL_DEC_GRADED,0);
					while((mp_mode & 0xffff)!= 0)
						usec_sleep(10000);
				}
			}
			else
			{
				printf("############################ pause \n");
				MP_MODE(0x20);
				mpctx->osd_function =OSD_PLAY;
				if(mpctx->audio_out)
				{
					mpctx->audio_out->control(AOCONTROL_SET_VOL_INC_GRADED,0);
					os_SemaphorePost(mp_sem_pause);
					while((mp_mode & 0xffff)!= 0)
						usec_sleep(10000);
				}
			}
		
	}
}
typedef void (*PFUN_StartPlay)(void);
PFUN_StartPlay g_StartPlay=NULL;
void SetStartPlay(PFUN_StartPlay pfun) 
{
    //printf("============ PFUN_StartPlay ==========\n");
    g_StartPlay = pfun;
}
PFUN_StartPlay g_ExitPlay=NULL;
void SetExitPlay(PFUN_StartPlay pfun) 
{
    //printf("============ PFUN_StartPlay ==========\n");
    g_ExitPlay = pfun;
}

int  GetTotalTime()
{
  if(mpctx == NULL)
        return -1;

        if(mpctx->eof != 0)
        return -1;
        //printf("-----%d\n",(unsigned int)demuxer_get_time_length(mpctx->demuxer)); 
        return (int)(demuxer_get_time_length(mpctx->demuxer)*1000.0);
}
static float cur_video_payer_time = 0;
static float seektime = 0;         
int GetCurPlayTime()
{
  if(mpctx == NULL)
        return -1;

        if(mpctx->eof != 0)
        return -1;
        if(seektime)
        {	
			cur_video_payer_time = seektime;
			printf("cur_video_payer_time = %f\n",cur_video_payer_time);
			seektime = 0;
		}
		else
			cur_video_payer_time = demuxer_get_current_time(mpctx->demuxer);
        return (int)(cur_video_payer_time*1000.0 );
}

static float curseektime = 0;
static float seekmode = 0;
int  GetCurAudioTime()
{
	if(mpctx == NULL)
    return 0;

	if(mpctx->eof != 0)
  	return 0;
  	if(!mpctx->audio_out) return 0;
	if(seekmode == 0)
	{	
		if((mp_mode & 0xffff) == 0)
		{	
		float f = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
		if((f<curseektime) && (curseektime > 0))
			f = curseektime;
		return (int)(f*1000.0);
	 }else
	 	 return (int)(curseektime * 1000.0);
	}
	else
	{	
		return (int)(curseektime * 1000.0);
	}
}
void SetSeekTime( int f)
{
  if(seekmode == 0)
  {	
   unsigned char err;
   rel_seek_secs=(float)f/1000.0;
   os_SemaphoreSet(mp_sem_seek,0,&err);
   os_SemaphorePend(mp_sem_seek,50,&err);
  }else if(seekmode == 1)
	{
		curseektime += (float)f/1000.0; 
		usec_sleep(20000);	
	}	
}
#undef printf
void mplayer_seekabstime(int f)
{
	unsigned char err;
	if(mpctx == NULL)
        return;

    if(mpctx->eof != 0)
        return;
	
	if(videoshow)
	{
		demuxer_t *demuxer = mpctx->demuxer;
		//if(f > (int)(cur_video_payer_time * 1000.0))
		//{
		//	//向前	
		//	float f1;
		//	abs_seek_pos = 0;
		// 	rel_seek_secs=(float)f/1000.0;
		//	
		//}else
		{
			//向后   
			
			abs_seek_pos = 1;
		 	seektime = (float)f/1000.0;
		 	printf("seektime = %f\n",seektime);
		 	rel_seek_secs = seektime;
		 	
		}	
		 
	   os_SemaphoreSet(mp_sem_seek,0,&err);
	   os_SemaphorePend(mp_sem_seek,100,&err);
   	}
}
void SetSeekAbsTime( int f)
{
  rel_seek_secs=(float)f/1000.0;
}

void Set3DSoundPlay(void *f)
{
	Set3DPlay(f);
}

void mplayer_CloseAudio()
{
	if(mpctx == NULL)
    return;
	if(mpctx->eof != 0)
    	return;
	MP_MODE(1);
	if(videoshow == 0)
	{
		if(!mpctx->audio_out) return;
		curseektime = (float)playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
		unsigned char err;
		os_SemaphoreSet(mp_sem_pause,0,&err);
		mpctx->osd_function = OSD_PAUSE;		
		mpctx->audio_out->control(AOCONTROL_SET_MUTE,0);	
		seekmode = 1;
		
	}	
	//mpctx->d_audio->eof = 1;
	F("===============mplayer_CloseAudio=======================\n");
}
void mplayer_OpenAudio()
{
	if(mpctx == NULL)
    return;
	if(mpctx->eof != 0)
    	return;
	if(videoshow == 0)
	{
		mpctx->osd_function =OSD_PLAY;
    	os_SemaphorePost(mp_sem_pause);
		//usec_sleep(20000);	
		MP_MODE(4);
		seekmode = 0;
		
	}else
		MP_MODE(2);
	while((mp_mode & 0xffff) != 0)
	 usec_sleep(10000);
 	 
  //mpctx->d_audio->eof = 0;
	F("===============mplayer_OpenAudio=======================\n");
}
extern void Set3DPlay(void *p);
void* mplayer_malloc(unsigned int size)
{
	return uc_malloc(size);
}
void mplayer_free(void *addr)
{
	return uc_free(addr);
}
void* mplayer_malloc_audio(unsigned int size)
{
	return Module_alignAlloc(32,size);
}
void mplayer_free_audio(void *addr)
{
	return GM_Dealloc(addr);
}

void mplayer_audio_mem(int mode)
{
	if(mpctx == 0) return;
	if(mpctx->eof) return;	
	while((inited_flags &INITED_DEMUXER) == 0)
		usec_sleep(10 * 1000);
	stream_change_mode(mode);	
}
static void initmplayer()
{
	//***************************************************//
	int mode;
	 mp_memory_init(videoshow);
	 
	 if(videoshow)
	 {
	 	BUFF_Init(mplayer_malloc,mplayer_free);
	 	BUFF_SetBufSize(0x280000,0x20000,0);
	 }
	 else
	 {	
	 	
	 	BUFF_Init(mplayer_malloc_audio,mplayer_free_audio);
	 	mode = GetAudioMemMode();
	 	set_change_mode(mode);
	 	BUFF_SetBufSize(0x80000,AUDIO_STREAM_BUFFER_SIZE,mode);
	 	SetMplayerAudioModeFun((void *)mplayer_audio_mem);
	 }	
#ifndef USE_16M_SDRAM
extern void mpeg2_init_var();

	 mpeg2_init_var();
#endif
   Set3DPlay(0);
	 SetPostPlay(0,0);
	 slave_mode=0;
	 player_idle_mode=0;
	 //quiet=1;
	 mp_mode = 0;

	 enable_mouse_movements=0;
   noconsolecontrols=1;
//**************************************************************************//
  mp_sem_pause = os_SemaphoreCreate(0) ;  // create sem
  mp_sem_seek = os_SemaphoreCreate(0) ;  // create sem
  audio_sem_filemode = os_SemaphoreCreate(0);
// Not all functions in mplayer.c take the context as an argument yet
   memset(&mpctx_s,0,sizeof(mpctx_s));
 {
    mpctx_s.osd_function = OSD_PLAY;
    mpctx_s.begin_skip = MP_NOPTS_VALUE;
    mpctx_s.play_tree_step = 1;
    mpctx_s.global_sub_pos = -1;
    mpctx_s.set_of_sub_pos = -1;
    mpctx_s.file_format = DEMUXER_TYPE_UNKNOWN;
    mpctx_s.loop_times = -1;
#ifdef HAS_DVBIN_SUPPORT
    mpctx_s.last_dvb_step = 1;
#endif
   }

   mpctx = &mpctx_s;
   fixed_vo=0;

// benchmark:
   video_time_usage=0;
   vout_time_usage=0;
   audio_time_usage=0;
   total_time_usage_start=0;
   total_frame_cnt=0;
   drop_frame_cnt=0; // total number of dropped frames
   benchmark=0;

// options:
    auto_quality=0;
    output_quality=0;

    //playback_speed=1.0;


    list_properties = 0;

    osd_level=0;
// if nonzero, hide current OSD contents when GetTimerMS() reaches this

    term_osd = 0;
    playing_msg = NULL;            
// seek:    
   seek_to_sec = 0.0;
   seek_to_byte=0;
   step_sec=0;
   loop_seek=0;

   m_time_size_t end_at = { .type = END_AT_NONE, .pos = 0 };

// A/V sync:
   autosync=200; // 30 might be a good default value.

// may be changed by GUI:  (FIXME!)
   rel_seek_secs=0.0;
   abs_seek_pos=0;

// codecs:
  audio_codec_list=NULL; // override audio codec
  video_codec_list=NULL; // override video codec
  audio_fm_list=NULL;    // override audio codec family 
  video_fm_list=NULL;    // override video codec family 

// demuxer:
  demuxer_name = NULL; // override demuxer
  audio_demuxer_name = NULL; // override audio demuxer
  sub_demuxer_name = NULL; // override sub demuxer

// streaming:
  audio_id=-1;
  video_id=-1;
  dvdsub_id=-2;
  vobsub_id=-1;
  audio_lang=NULL;
  dvdsub_lang=NULL;
  spudec_ifo=NULL;
  filename=NULL; //"MI2-Trailer.avi";
  forced_subs_only=0;
  file_filter=0;

// A-V sync:
   default_max_pts_correction=-1;//0.01f;
   max_pts_correction=0;//default_max_pts_correction;
   c_total=0;
   audio_delay=0.0;//(64000.0 * 3.0)/2.0/22050.0/2.0;
   ignore_start=0;



   //softsleep=0;

   force_fps=0;
   force_srate=0;
   audio_output_format=-1; // AF_FORMAT_UNKNOWN
   frame_dropping=1; // option  0=no drop  1= drop vo  2= drop decode
   play_n_frames=-1;
   play_n_frames_mf=-1;

// screen info:
    video_driver_list=NULL;
    audio_driver_list=NULL;

// sub:
    font_name=NULL;
    sub_font_name=NULL;
    font_factor=0.75;
    sub_name=NULL;
    sub_delay=0;
    sub_fps=0;
    sub_auto = 0;
		vobsub_name=NULL;
/*DSP!!char *dsp=NULL;*/
    subcc_enabled=0;
    suboverlap_enabled = 1;
    current_module=NULL; // for debugging
	
}


void *mpctx_get_video_out(MPContext *mpctx)
{
    return mpctx->video_out;
}

void *mpctx_get_audio_out(MPContext *mpctx)
{
    return mpctx->audio_out;
}

void *mpctx_get_playtree_iter(MPContext *mpctx)
{
    return mpctx->playtree_iter;
}

void *mpctx_get_mixer(MPContext *mpctx)
{
    return &mpctx->mixer;
}

int mpctx_get_global_sub_size(MPContext *mpctx)
{
    return mpctx->global_sub_size;
}

char *get_metadata (metadata_t type) {
  char *meta = NULL;
  sh_audio_t * const sh_audio = mpctx->sh_audio;
  sh_video_t * const sh_video = mpctx->sh_video;

  if (!is_valid_metadata_type (type))
    return NULL;
  
  switch (type)
  {
  case META_NAME:
  {
    return strdup (mp_basename2 (filename));
  }
    
  case META_VIDEO_CODEC:
  {
    if (sh_video->format == 0x10000001)
      meta = strdup ("mpeg1");
    else if (sh_video->format == 0x10000002)
      meta = strdup ("mpeg2");
    else if (sh_video->format == 0x10000004)
      meta = strdup ("mpeg4");
    else if (sh_video->format == 0x10000005)
      meta = strdup ("h264");
    else if (sh_video->format >= 0x20202020)
    {
      meta = malloc (8);
      sprintf (meta, "%.4s", (char *) &sh_video->format);
    }
    else
    {
      meta = malloc (8);
      sprintf (meta, "0x%08X", sh_video->format);
    }
    return meta;
  }
  
  case META_VIDEO_BITRATE:
  {
    meta = malloc (16);
    sprintf (meta, "%d kbps", (int) (sh_video->i_bps * 8 / 1024));
    return meta;
  }
  
  case META_VIDEO_RESOLUTION:
  {
    meta = malloc (16);
    sprintf (meta, "%d x %d", sh_video->disp_w, sh_video->disp_h);
    return meta;
  }

  case META_AUDIO_CODEC:
  {
    if (sh_audio->codec && sh_audio->codec->name)
      meta = strdup (sh_audio->codec->name);
    return meta;
  }
  
  case META_AUDIO_BITRATE:
  {
    meta = malloc (16);
    sprintf (meta, "%d kbps", (int) (sh_audio->i_bps * 8/1000));
    return meta;
  }
  
  case META_AUDIO_SAMPLES:
  {
    meta = malloc (16);
    sprintf (meta, "%d Hz, %d ch.", sh_audio->samplerate, sh_audio->channels);
    return meta;
  }

  /* check for valid demuxer */
  case META_INFO_TITLE:
    return get_demuxer_info ("Title");
  
  case META_INFO_ARTIST:
    return get_demuxer_info ("Artist");

  case META_INFO_ALBUM:
    return get_demuxer_info ("Album");

  case META_INFO_YEAR:
    return get_demuxer_info ("Year");

  case META_INFO_COMMENT:
    return get_demuxer_info ("Comment");

  case META_INFO_TRACK:
    return get_demuxer_info ("Track");

  case META_INFO_GENRE:
    return get_demuxer_info ("Genre");

  default:
    break;
  }

  return meta;
}

/// step size of mixer changes
int volstep = 3;

void uninit_player(unsigned int mask){
	
  			
  mask=inited_flags&mask;
  F("free:mask = %x,inited_flag = %x\n",mask,inited_flags);

  mp_msg(MSGT_CPLAYER,MSGL_DBG2,"\n*** uninit(0x%X)\n",mask);
  F("free:demuxer\n");

 // if(mask&INITED_ACODEC)
  {
   
    inited_flags&=~INITED_ACODEC;
    current_module="uninit_acodec";
    
    if(mpctx->sh_audio) uninit_audio(mpctx->sh_audio);
#ifdef HAVE_NEW_GUI
    if (use_gui) guiGetEvent(guiSetAfilter, (char *)NULL);
#endif
    mpctx->sh_audio=NULL;
    mpctx->mixer.afilter = NULL;
  }

  //if(mask&INITED_VCODEC)
  {
    inited_flags&=~INITED_VCODEC;
    current_module="uninit_vcodec";
    if(mpctx->sh_video) uninit_video(mpctx->sh_video);
    mpctx->sh_video=NULL;
#ifdef HAVE_MENU
    vf_menu=NULL;
#endif
  }
 
  if(mask&INITED_DEMUXER){
    inited_flags&=~INITED_DEMUXER;
    current_module="free_demuxer";
    if(mpctx->demuxer){
	mpctx->stream=mpctx->demuxer->stream;
	free_demuxer(mpctx->demuxer);
    }
    mpctx->demuxer=NULL;
  }

  // kill the cache process:
  if(mask&INITED_STREAM){
    inited_flags&=~INITED_STREAM;
    current_module="uninit_stream";
    if(mpctx->stream) free_stream(mpctx->stream);
    mpctx->stream=NULL;
  }

  if(mask&INITED_VO){
    inited_flags&=~INITED_VO;
    current_module="uninit_vo";
    mpctx->video_out->uninit();
    mpctx->video_out=NULL;
  }
#if 0
  // Must be after libvo uninit, as few vo drivers (svgalib) have tty code.
  if(mask&INITED_GETCH2){
    inited_flags&=~INITED_GETCH2;
    current_module="uninit_getch2";
    mp_msg(MSGT_CPLAYER,MSGL_DBG2,"\n[[[uninit getch2]]]\n");
  // restore terminal:
    getch2_disable();
  }
#endif
  if(mask&INITED_VOBSUB){
    inited_flags&=~INITED_VOBSUB;
    current_module="uninit_vobsub";
    if(vo_vobsub) vobsub_close(vo_vobsub);
    vo_vobsub=NULL;
  }

  if (mask&INITED_SPUDEC){
    inited_flags&=~INITED_SPUDEC;
    current_module="uninit_spudec";
    spudec_free(vo_spudec);
    vo_spudec=NULL;
  }

  if(mask&INITED_AO){
    inited_flags&=~INITED_AO;
    current_module="uninit_ao";
#ifndef USE_16M_SDRAM
    if (mpctx->edl_muted) mixer_mute(&mpctx->mixer); 
#endif
    mpctx->audio_out->uninit(mpctx->eof?0:1); mpctx->audio_out=NULL;
  }

#ifdef HAVE_NEW_GUI
  if(mask&INITED_GUI){
    inited_flags&=~INITED_GUI;
    current_module="uninit_gui";
    guiDone();
  }
#endif

  if(mask&INITED_INPUT){
    inited_flags&=~INITED_INPUT;
    current_module="uninit_input";
    mp_input_uninit();
  }
unsigned char my_err;
//OS_DELETE_ALWAYS = 1
	//printf("+++os_SemaphoreDelete\n");
	if(mp_sem_pause)
		os_SemaphoreDelete ( mp_sem_pause , 1 , &my_err) ;
	if(mp_sem_seek)
		os_SemaphoreDelete ( mp_sem_seek , 1 , &my_err) ;
	if(audio_sem_filemode)
		os_SemaphoreDelete ( audio_sem_filemode , 1 , &my_err) ;
	mp_sem_pause = NULL;
	mp_sem_seek = NULL;
	audio_sem_filemode = NULL;
	//printf("---os_SemaphoreDelete\n");
	video_driver_list=NULL;
	audio_driver_list=NULL;
  current_module=NULL;
}
static mplayer_opts_free(m_option_t *arg)
{
	int i;
	if(arg->type->flags & M_OPT_TYPE_HAS_CHILD) {
    m_option_t *ol = arg->p;
    for(i = 0 ; ol[i].name != NULL ; i++)
    	mplayer_opts_free(&ol[i]);
  }
 	if((arg->type->flags & M_OPT_TYPE_DYNAMIC) && arg->p && (*(void**)arg->p)) 
      	  	m_option_free(arg,arg->p);
}

void exit_player_with_rc(const char* how, int rc){
 
 	
#ifndef USE_16M_SDRAM
  if (mpctx->user_muted && !mpctx->edl_muted) mixer_mute(&mpctx->mixer); 
#endif
  uninit_player(INITED_ALL);
#ifdef HAVE_X11
#ifdef HAVE_NEW_GUI
  if ( !use_gui )
#endif
  vo_uninit();	// Close the X11 connection (if any is open).
#endif
 
#ifdef HAVE_FREETYPE
  current_module="uninit_font";
  if (vo_font) free_font_desc(vo_font);
  vo_font = NULL;
  done_freetype();
#endif
  free_osd_list();
  
#ifdef USE_ASS
  ass_library_done(ass_library);
#endif

  current_module="exit_player";

// free mplayer config
  if(mconfig)
    m_config_free(mconfig);

  if(mpctx->playtree)
    play_tree_free(mpctx->playtree, 1);


  if(edl_records != NULL) free(edl_records); // free mem allocated for EDL
  if(how) mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_ExitingHow,how);
  mp_msg(MSGT_CPLAYER,MSGL_DBG2,"max framesize was %d bytes\n",max_framesize);
  
#ifdef JZ4740_IPU
#endif
  do_free();
  codecs_uninit_free();
  
  g_StartPlay = NULL;
  mpctx = NULL;
#ifdef JZ4740_MXU_OPT
	if(mxucr)
	{	
	  disable_jz4740_mxu (mxucr);
	  mxucr = 0;
	}
#endif
  mp_memory_deinit(videoshow);
  JZ_StopTimer();

   #if EXCEPTION_MPLAYER
  DecExcept(mp_excptid,saveregister);
  
  F("exit\n");
  excpt_exit(1,saveregister);
  #endif
  
}
void mplayer_fexit()
{
  SetPauseState(1);
  usec_sleep(200); //wait pause_sem send to mplayer_main
	mp_memory_deinit(videoshow);
}
void exit_player(const char* how){
  exit_player_with_rc(how, 1);
}

#ifndef __MINGW32__
static void child_sighandler(int x){  
  pid_t pid;
  while((pid=waitpid(-1,NULL,WNOHANG)) > 0);
}
#endif

#ifdef CRASH_DEBUG
static char *prog_path;
static int crash_debug = 0;
#endif
#if 0
static void exit_sighandler(int x){
  static int sig_count=0;
#ifdef CRASH_DEBUG
  if (!crash_debug || x != SIGTRAP)
#endif
  ++sig_count;
  if(inited_flags==0 && sig_count>1) exit(1);
  if(sig_count==5)
    {
      /* We're crashing bad and can't uninit cleanly :( 
       * by popular request, we make one last (dirty) 
       * effort to restore the user's 
       * terminal. */
      getch2_disable();
      exit(1);
    }
  if(sig_count==6) exit(1);
  if(sig_count>6){
    // can't stop :(
#ifndef __MINGW32__
    kill(getpid(),SIGKILL);
#endif
  }
  mp_msg(MSGT_CPLAYER,MSGL_FATAL,"\n" MSGTR_IntBySignal,x,
      current_module?current_module:"unknown"
  );
  mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SIGNAL=%d\n", x);
  if(sig_count<=1)
  switch(x){
  case SIGINT:
  case SIGQUIT:
  case SIGTERM:
  case SIGKILL:
      break;  // killed from keyboard (^C) or killed [-9]
  case SIGILL:
#ifdef RUNTIME_CPUDETECT
      mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGILL_RTCpuSel);
#else
      mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGILL);
#endif
  case SIGFPE:
  case SIGSEGV:
      mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGSEGV_SIGFPE);
  default:
      mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_Exit_SIGCRASH);
#ifdef CRASH_DEBUG
      if (crash_debug) {
        int gdb_pid;
        char spid[20];
        snprintf(spid, 19, "%i", getpid());
        spid[19] = 0;
        mp_msg(MSGT_CPLAYER, MSGL_INFO, "Forking...\n");
        gdb_pid = fork();
        mp_msg(MSGT_CPLAYER, MSGL_INFO, "Forked...\n");
        if (gdb_pid == 0) { // We are the child
          if (execlp("gdb", "gdb", prog_path, spid, NULL) == -1)
            mp_msg(MSGT_CPLAYER, MSGL_ERR, "Couldn't start gdb\n");
        } else if (gdb_pid < 0) 
          mp_msg(MSGT_CPLAYER, MSGL_ERR, "Couldn't fork\n");
        else {
          waitpid(gdb_pid, NULL, 0);
        }
        if (x == SIGTRAP) return;
      }
#endif  
  }
  exit_player(NULL);
}
#endif
extern void mp_input_register_options(m_config_t* cfg);

#include "cfg-mplayer.h"

static void parse_cfgfiles( m_config_t* conf )
{
	#if 0
char *conffile;
int conffile_fd;
if (m_config_parse_config_file(conf, MPLAYER_CONFDIR "/mplayer.conf") < 0)
  exit_player(NULL);
if ((conffile = get_path("")) == NULL) {
  mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_NoHomeDir);
} else {
#ifdef __MINGW32__
  mkdir(conffile);
#else
  mkdir(conffile, 0777);
#endif
  free(conffile);
  if ((conffile = get_path("config")) == NULL) {
    mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_GetpathProblem);
  } else {
    if ((conffile_fd = open(conffile, O_CREAT | O_EXCL | O_WRONLY, 0666)) != -1) {
      mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_CreatingCfgFile, conffile);
      write(conffile_fd, default_config, strlen(default_config));
      close(conffile_fd);
    }
    if (m_config_parse_config_file(conf, conffile) < 0)
      exit_player(NULL);
    free(conffile);
  }
}
#endif
}
#if 0
void load_per_file_config (m_config_t* conf, const char *const file)
{
  
    char *confpath;
    char cfg[strlen(file)+10];
    struct stat st;
    char *name;

    sprintf (cfg, "%s.conf", file);
    
    if (use_filedir_conf && !stat (cfg, &st))
    {
	mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingConfig, cfg);
	m_config_parse_config_file (conf, cfg);
	return;
    }

    if ((name = strrchr (cfg, '/')) == NULL)
	name = cfg;
    else
	name++;

    if ((confpath = get_path (name)) != NULL)
    {
	if (!stat (confpath, &st))
	{
	    mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_LoadingConfig, confpath);
	    m_config_parse_config_file (conf, confpath);
	}

	free (confpath);
    }
}

#endif
/* When libmpdemux performs a blocking operation (network connection or
 * cache filling) if the operation fails we use this function to check
 * if it was interrupted by the user.
 * The function returns a new value for eof. */
static int libmpdemux_was_interrupted(int eof) {
  mp_cmd_t* cmd;
  if((cmd = mp_input_get_cmd(0,0,0)) != NULL) {
       switch(cmd->id) {
       case MP_CMD_QUIT:
	 exit_player_with_rc(MSGTR_Exit_quit, (cmd->nargs > 0)? cmd->args[0].v.i : 0);
       case MP_CMD_PLAY_TREE_STEP: {
	 eof = (cmd->args[0].v.i > 0) ? PT_NEXT_ENTRY : PT_PREV_ENTRY;
	 mpctx->play_tree_step = (cmd->args[0].v.i == 0) ? 1 : cmd->args[0].v.i;
       } break;
       case MP_CMD_PLAY_TREE_UP_STEP: {
	 eof = (cmd->args[0].v.i > 0) ? PT_UP_NEXT : PT_UP_PREV;
       } break;	  
       case MP_CMD_PLAY_ALT_SRC_STEP: {
	 eof = (cmd->args[0].v.i > 0) ?  PT_NEXT_SRC : PT_PREV_SRC;
       } break;
       }
       mp_cmd_free(cmd);
  }
  return eof;
}

#define mp_basename(s) (strrchr(s,'\\')==NULL?(mp_basename2(s)):(strrchr(s,'\\')+1))

#ifndef USE_16M_SDRAM
int playtree_add_playlist(play_tree_t* entry)
{
  play_tree_add_bpf(entry,filename);

#ifdef HAVE_NEW_GUI
  if (use_gui) {
    if (entry) {
      import_playtree_playlist_into_gui(entry, mconfig);
      play_tree_free_list(entry,1);
    }
  } else
#endif
  {
  if(!entry) {      
    entry = mpctx->playtree_iter->tree;
    if(play_tree_iter_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
        return PT_NEXT_ENTRY;
    }
    if(mpctx->playtree_iter->tree == entry ) { // Loop with a single file
      if(play_tree_iter_up_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
	return PT_NEXT_ENTRY;
      }
    }
    play_tree_remove(entry,1,1);
    return PT_NEXT_SRC;
  }
  play_tree_insert_entry(mpctx->playtree_iter->tree,entry);
  play_tree_set_params_from(entry,mpctx->playtree_iter->tree);
  entry = mpctx->playtree_iter->tree;
  if(play_tree_iter_step(mpctx->playtree_iter,1,0) != PLAY_TREE_ITER_ENTRY) {
    return PT_NEXT_ENTRY;
        }
  play_tree_remove(entry,1,1);
    }
  return PT_NEXT_SRC;
}
#endif

void add_subtitles(char *filename, float fps, int silent)
{
    sub_data *subd;
#ifdef USE_ASS
    ass_track_t *asst = 0;
#endif

    if (filename == NULL || mpctx->set_of_sub_size >= MAX_SUBTITLE_FILES) {
	return;
    }

    subd = sub_read_file(filename, fps);
#ifdef USE_ASS
    if (ass_enabled)
#ifdef USE_ICONV
        asst = ass_read_file(ass_library, filename, sub_cp);
#else
        asst = ass_read_file(ass_library, filename, 0);
#endif
    if (ass_enabled && subd && !asst)
        asst = ass_read_subdata(ass_library, subd, fps);

    if (!asst && !subd && !silent)
#else
    if(!subd && !silent) 
#endif
        mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_CantLoadSub,
		filename_recode(filename));
    
#ifdef USE_ASS
    if (!asst && !subd) return;
    mpctx->set_of_ass_tracks[mpctx->set_of_sub_size] = asst;
#else
    if (!subd) return;
#endif
    mpctx->set_of_subtitles[mpctx->set_of_sub_size] = subd;
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_FILE_SUB_ID=%d\n", mpctx->set_of_sub_size);
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_FILE_SUB_FILENAME=%s\n",
	   filename_recode(filename));
    ++mpctx->set_of_sub_size;
    mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AddedSubtitleFile, mpctx->set_of_sub_size,
	    filename_recode(filename));
}

// FIXME: if/when the GUI calls this, global sub numbering gets (potentially) broken.
void update_set_of_subtitles(void)
    // subdata was changed, set_of_sub... have to be updated.
{
    sub_data ** const set_of_subtitles = mpctx->set_of_subtitles;
    int i;
    if (mpctx->set_of_sub_size > 0 && subdata == NULL) { // *subdata was deleted
        for (i = mpctx->set_of_sub_pos + 1; i < mpctx->set_of_sub_size; ++i)
            set_of_subtitles[i-1] = set_of_subtitles[i];
        set_of_subtitles[mpctx->set_of_sub_size-1] = NULL;
        --mpctx->set_of_sub_size;
        if (mpctx->set_of_sub_size > 0) subdata = set_of_subtitles[mpctx->set_of_sub_pos=0];
    }
    else if (mpctx->set_of_sub_size > 0 && subdata != NULL) { // *subdata was changed
        set_of_subtitles[mpctx->set_of_sub_pos] = subdata;
    }
    else if (mpctx->set_of_sub_size <= 0 && subdata != NULL) { // *subdata was added
        set_of_subtitles[mpctx->set_of_sub_pos=mpctx->set_of_sub_size] = subdata;
        ++mpctx->set_of_sub_size;
    }
}

void init_vo_spudec(void) {
  if (vo_spudec)
    spudec_free(vo_spudec);
  inited_flags &= ~INITED_SPUDEC;
  vo_spudec = NULL;
  if (spudec_ifo) {
    unsigned int palette[16], width, height;
    current_module="spudec_init_vobsub";
    if (vobsub_parse_ifo(NULL,spudec_ifo, palette, &width, &height, 1, -1, NULL) >= 0)
      vo_spudec=spudec_new_scaled(palette, width, height);
  }

#ifdef USE_DVDREAD
  if (vo_spudec==NULL && mpctx->stream->type==STREAMTYPE_DVD) {
    current_module="spudec_init_dvdread";
    vo_spudec=spudec_new_scaled(((dvd_priv_t *)(mpctx->stream->priv))->cur_pgc->palette,
                                mpctx->sh_video->disp_w, mpctx->sh_video->disp_h);
  }
#endif

#ifdef USE_DVDNAV
  if (vo_spudec==NULL && mpctx->stream->type==STREAMTYPE_DVDNAV) {
    unsigned int *palette = mp_dvdnav_get_spu_clut(mpctx->stream);
    current_module="spudec_init_dvdnav";
    vo_spudec=spudec_new_scaled(palette, mpctx->sh_video->disp_w, mpctx->sh_video->disp_h);
  }
#endif

  if ((vo_spudec == NULL) && (mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA) &&
      (mpctx->d_sub->sh != NULL) && (((sh_sub_t *)mpctx->d_sub->sh)->type == 'v')) {
    sh_sub_t *mkv_sh_sub = (sh_sub_t *)mpctx->d_sub->sh;
    current_module = "spudec_init_matroska";
    vo_spudec =
      spudec_new_scaled_vobsub(mkv_sh_sub->palette, mkv_sh_sub->colors,
                             mkv_sh_sub->custom_colors, mkv_sh_sub->width,
                               mkv_sh_sub->height);
    forced_subs_only = mkv_sh_sub->forced_subs_only;
  }

  if (vo_spudec==NULL) {
    sh_sub_t *sh = (sh_sub_t *)mpctx->d_sub->sh;
    unsigned int *palette = NULL;
    if (sh && sh->has_palette)
      palette = sh->palette;
    current_module="spudec_init_normal";
    vo_spudec=spudec_new_scaled(palette, mpctx->sh_video->disp_w, mpctx->sh_video->disp_h);
    spudec_set_font_factor(vo_spudec,font_factor);
  }

  if (vo_spudec!=NULL)
    inited_flags|=INITED_SPUDEC;
}

/*
 * In Mac OS X the SDL-lib is built upon Cocoa. The easiest way to
 * make it all work is to use the builtin SDL-bootstrap code, which 
 * will be done automatically by replacing our main() if we include SDL.h.
 */
#if defined(SYS_DARWIN) && defined(HAVE_SDL)
#include <SDL.h>
#endif

/**
 * \brief append a formatted string
 * \param buf buffer to print into
 * \param pos position of terminating 0 in buf
 * \param len maximum number of characters in buf, not including terminating 0
 * \param format printf format string
 */
static void saddf(char *buf, unsigned *pos, int len, const char *format, ...)
{
  va_list va;
  va_start(va, format);
  *pos += vsnprintf(&buf[*pos], len - *pos, format, va);
  va_end(va);
  if (*pos >= len ) {
    buf[len] = 0;
    *pos = len;
  }
}

/**
 * \brief append time in the hh:mm:ss.f format
 * \param buf buffer to print into
 * \param pos position of terminating 0 in buf
 * \param len maximum number of characters in buf, not including terminating 0
 * \param time time value to convert/append
 */
static void sadd_hhmmssf(char *buf, unsigned *pos, int len, float time) {
  long tenths = 10 * time;
  int f1 = tenths % 10;
  int ss = (tenths /  10) % 60;
  int mm = (tenths / 600) % 60;
  int hh = tenths / 36000;
  if (time <= 0) {
    saddf(buf, pos, len, "unknown");
    return;
  }
  if (hh > 0)
    saddf(buf, pos, len, "%2d:", hh);
  if (hh > 0 || mm > 0)
    saddf(buf, pos, len, "%02d:", mm);
  saddf(buf, pos, len, "%02d.%1d", ss, f1);
}

/**
 * \brief print the status line
 * \param a_pos audio position
 * \param a_v A-V desynchronization
 * \param corr amount out A-V synchronization
 */
static void print_status(float a_pos, float a_v, float corr)
{
  sh_video_t * const sh_video = mpctx->sh_video;
  int width;
  char *line;
  unsigned pos = 0;
  get_screen_size();
  if (screen_width > 0)
    width = screen_width;
  else
  width = 80;
#ifdef WIN32
  /* Windows command line is broken (MinGW's rxvt works, but we
   * should not depend on that). */
  width--;
#endif
  line = malloc(width + 1); // one additional char for the terminating null
  
  // Audio time
  if (mpctx->sh_audio) {
    saddf(line, &pos, width, "A:%6.1f ", a_pos);
    if (!sh_video) {
      float len = demuxer_get_time_length(mpctx->demuxer);
      saddf(line, &pos, width, "(");
      sadd_hhmmssf(line, &pos, width, a_pos);
      saddf(line, &pos, width, ") of %.1f (", len);
      sadd_hhmmssf(line, &pos, width, len);
      saddf(line, &pos, width, ") ");
    }
  }

  // Video time
  if (sh_video)
    saddf(line, &pos, width, "V:%6.1f ", sh_video->pts);

  // A-V sync
  if (mpctx->sh_audio && sh_video)
    saddf(line, &pos, width, "A-V:%7.3f ct:%7.3f ", a_v, corr);

  // Video stats
  if (sh_video)
    saddf(line, &pos, width, "%3d/%3d ",
      (int)sh_video->num_frames,
      (int)sh_video->num_frames_decoded);

#if 0
  // CPU usage
  if (sh_video) {
    if (sh_video->timer > 0.5)
      saddf(line, &pos, width, "%2d%% %2d%% %4.1f%% ",
        (int)(100.0*video_time_usage*playback_speed/(double)sh_video->timer),
        (int)(100.0*vout_time_usage*playback_speed/(double)sh_video->timer),
        (100.0*audio_time_usage*playback_speed/(double)sh_video->timer));
    else
      saddf(line, &pos, width, "??%% ??%% ??,?%% ");
  } else if (mpctx->sh_audio) {
    if (mpctx->delay > 0.5)
      saddf(line, &pos, width, "%4.1f%% ",
        100.0*audio_time_usage/(double)mpctx->delay);
    else
      saddf(line, &pos, width, "??,?%% ");
  }
#endif
  // VO stats
  if (sh_video) 
    saddf(line, &pos, width, "%d %d ", drop_frame_cnt, output_quality);

#ifdef USE_STREAM_CACHE
  // cache stats
  if (stream_cache_size > 0)
    saddf(line, &pos, width, "%d%% ", cache_fill_status);
#endif

  // other
  if (playback_speed != 1)
    saddf(line, &pos, width, "%4.2fx ", playback_speed);

  // end
  if (erase_to_end_of_line) {
    line[pos] = 0;
    mp_msg(MSGT_STATUSLINE, MSGL_STATUS, "%s%s\r", line, erase_to_end_of_line);
  } else {
    memset(&line[pos], ' ', width - pos);
    line[width] = 0;
    mp_msg(MSGT_STATUSLINE, MSGL_STATUS, "%s\r", line);
  }
  free(line);
}

/**
 * \brief build a chain of audio filters that converts the input format
 * to the ao's format, taking into account the current playback_speed.
 * \param sh_audio describes the requested input format of the chain.
 * \param ao_data describes the requested output format of the chain.
 */
int build_afilter_chain(sh_audio_t *sh_audio, ao_data_t *ao_data)
{
  int new_srate;
  int result;
  if (!sh_audio)
  {
#ifdef HAVE_NEW_GUI
    if (use_gui) guiGetEvent(guiSetAfilter, (char *)NULL);
#endif
    mpctx->mixer.afilter = NULL;
    return 0;
  }
  new_srate = sh_audio->samplerate * playback_speed;
  if (new_srate != ao_data->samplerate) {
    // limits are taken from libaf/af_resample.c
    if (new_srate < 8000)
      new_srate = 8000;
    if (new_srate > 192000)
      new_srate = 192000;
    //playback_speed = (float)new_srate / (float)sh_audio->samplerate;
  }
  result =  init_audio_filters(sh_audio, new_srate,
           sh_audio->channels, sh_audio->sample_format,
           &ao_data->samplerate, &ao_data->channels, &ao_data->format,
           ao_data->outburst * 4, ao_data->buffersize);
  mpctx->mixer.afilter = NULL;//sh_audio->afilter;
#ifdef HAVE_NEW_GUI
  if (use_gui) guiGetEvent(guiSetAfilter, (char *)sh_audio->afilter);
#endif
  return result;
}

#if 1
typedef struct mp_osd_msg mp_osd_msg_t;
struct mp_osd_msg {
    /// Previous message on the stack.
    mp_osd_msg_t* prev;
    /// Message text.
    char msg[64];
    int  id,level,started;
    /// Display duration in ms.
    unsigned  time;
};

/// OSD message stack.
static mp_osd_msg_t* osd_msg_stack = NULL;
#endif
/**
 *  \brief Add a message on the OSD message stack
 * 
 *  If a message with the same id is already present in the stack
 *  it is pulled on top of the stack, otherwise a new message is created.
 *  
 */

void set_osd_msg(int id, int level, int time, const char* fmt, ...) {
	#if 0
    mp_osd_msg_t *msg,*last=NULL;
    va_list va;
    int r;
   
    // look if the id is already in the stack
    for(msg = osd_msg_stack ; msg && msg->id != id ;
	last = msg, msg = msg->prev);
    // not found: alloc it
    if(!msg) {
        msg = calloc(1,sizeof(mp_osd_msg_t));
        msg->prev = osd_msg_stack;
        osd_msg_stack = msg;
    } else if(last) { // found, but it's not on top of the stack
        last->prev = msg->prev;
        msg->prev = osd_msg_stack;
        osd_msg_stack = msg;
    }
    // write the msg
    va_start(va,fmt);
    r = vsnprintf(msg->msg, 64, fmt, va);
    va_end(va);
    if(r >= 64) msg->msg[63] = 0;
    // set id and time
    msg->id = id;
    msg->level = level;
    msg->time = time;
    
 
 #endif //end 0
}

/**
 *  \brief Remove a message from the OSD stack
 * 
 *  This function can be used to get rid of a message right away.
 * 
 */

void rm_osd_msg(int id) {
    #if 0
    mp_osd_msg_t *msg,*last=NULL;
    
    // Search for the msg
    for(msg = osd_msg_stack ; msg && msg->id != id ;
	last = msg, msg = msg->prev);
    if(!msg) return;

    // Detach it from the stack and free it
    if(last)
        last->prev = msg->prev;
    else
        osd_msg_stack = msg->prev;
    free(msg);
    #endif
}

/**
 *  \brief Remove all messages from the OSD stack
 * 
 */

static void clear_osd_msgs(void) {
	#if 0
    mp_osd_msg_t* msg = osd_msg_stack, *prev = NULL;
    while(msg) {
        prev = msg->prev;
        free(msg);
        msg = prev;
    }
    osd_msg_stack = NULL;
   #endif
}

/**
 *  \brief Get the current message from the OSD stack.
 * 
 *  This function decrements the message timer and destroys the old ones.
 *  The message that should be displayed is returned (if any).
 *  
 */

static mp_osd_msg_t* get_osd_msg(void) {
    // Nothing found
    return NULL;
}

/**
 * \brief Display the OSD bar.
 *
 * Display the OSD bar or fall back on a simple message.
 *
 */

void set_osd_bar(int type,const char* name,double min,double max,double val) {
    
}


/**
 * \brief Update the OSD message line.
 *
 * This function displays the current message on the vo OSD or on the term.
 * If the stack is empty and the OSD level is high enough the timer
 * is displayed (only on the vo OSD).
 * 
 */

static void update_osd_msg(void) {
}

///@}
// OSDMsgStack
void reinit_audio_chain(void) {
if(mpctx->sh_audio){
  current_module="init_audio_codec";
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
  if(!init_best_audio_codec(mpctx->sh_audio,audio_codec_list,audio_fm_list)){
    mpctx->sh_audio=mpctx->d_audio->sh=NULL; // failed to init :(
    mpctx->d_audio->id = -2;
    return;
  } else
    inited_flags|=INITED_ACODEC;
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
  

  //const ao_info_t *info=audio_out->info;
  current_module="af_preinit";
  ao_data.samplerate=force_srate;
  ao_data.channels=0;
  ao_data.format=audio_output_format;
#if 1
  // first init to detect best values
  if(!preinit_audio_filters(mpctx->sh_audio,
        // input:
        (int)(mpctx->sh_audio->samplerate*playback_speed),
	mpctx->sh_audio->channels, mpctx->sh_audio->sample_format,
	// output:
	&ao_data.samplerate, &ao_data.channels, &ao_data.format)){
      mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_AudioFilterChainPreinitError);
      exit_player(MSGTR_Exit_error);
  }
#endif  
  current_module="ao2_init";
  if(!(mpctx->audio_out=init_best_audio_out(audio_driver_list,
      0, // plugin flag
      ao_data.samplerate,
      ao_data.channels,
      ao_data.format,0))){
    // FAILED:
    mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CannotInitAO);
    uninit_player(INITED_ACODEC); // close codec
    mpctx->sh_audio=mpctx->d_audio->sh=NULL; // -> nosound
    mpctx->d_audio->id = -2;
    return;
  } else {
    // SUCCESS:
    inited_flags|=INITED_AO;
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"AO: [%s] %dHz %dch %s (%d bytes per sample)\n",
      mpctx->audio_out->info->short_name,
      ao_data.samplerate, ao_data.channels,
      af_fmt2str_short(ao_data.format),
      af_fmt2bits(ao_data.format)/8 );
    mp_msg(MSGT_CPLAYER,MSGL_V,"AO: Description: %s\nAO: Author: %s\n",
      mpctx->audio_out->info->name, mpctx->audio_out->info->author);
    if(strlen(mpctx->audio_out->info->comment) > 0)
      mp_msg(MSGT_CPLAYER,MSGL_V,"AO: Comment: %s\n", mpctx->audio_out->info->comment);
    // init audio filters:
#if 1
    current_module="af_init";
    if(!build_afilter_chain(mpctx->sh_audio, &ao_data)) {
      mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_NoMatchingFilter);
//      mp_msg(MSGT_CPLAYER,MSGL_ERR,"Couldn't find matching filter / ao format! -> NOSOUND\n");
//      uninit_player(INITED_ACODEC|INITED_AO); // close codec & ao
//      sh_audio=mpctx->d_audio->sh=NULL; // -> nosound
    }
#endif
  }
  mpctx->mixer.audio_out = mpctx->audio_out;
  mpctx->mixer.volstep = volstep;
}
}


///@}
// Command2Property


// Return pts value corresponding to the end point of audio written to the
// ao so far.
static double written_audio_pts(sh_audio_t *sh_audio, demux_stream_t *d_audio)
{
    // first calculate the end pts of audio that has been output by decoder
    double a_pts = sh_audio->pts;
   //F("a_pts %f\n",a_pts);
    #if 1
    if (a_pts != MP_NOPTS_VALUE)
	// Good, decoder supports new way of calculating audio pts.
	// sh_audio->pts is the timestamp of the latest input packet with
	// known pts that the decoder has decoded. sh_audio->pts_bytes is
	// the amount of bytes the decoder has written after that timestamp.
	a_pts += sh_audio->pts_bytes / (double) sh_audio->o_bps;
    else {
	// Decoder doesn't support new way of calculating pts (or we're
	// being called before it has decoded anything with known timestamp).
	// Use the old method of audio pts calculation: take the timestamp
	// of last packet with known pts the decoder has read data from,
	// and add amount of bytes read after the beginning of that packet
	// divided by input bps. This will be inaccurate if the input/output
	// ratio is not constant for every audio packet or if it is constant
	// but not accurately known in sh_audio->i_bps.

	a_pts = d_audio->pts;
	// ds_tell_pts returns bytes read after last timestamp from
	// demuxing layer, decoder might use sh_audio->a_in_buffer for bytes
	// it has read but not decoded
	if (sh_audio->i_bps)
	    a_pts += (ds_tell_pts(d_audio) - sh_audio->a_in_buffer_len) /
		(double)sh_audio->i_bps;
    }
    #endif
    // Now a_pts hopefully holds the pts for end of audio from decoder.
    // Substract data in buffers between decoder and audio out.

    // Decoded but not filtered
    a_pts -= sh_audio->a_buffer_len / (double)sh_audio->o_bps;

    // Data that was ready for ao but was buffered because ao didn't fully
    // accept everything to internal buffers yet
    a_pts -= sh_audio->a_out_buffer_len * playback_speed / (double)ao_data.bps;

    return a_pts;
}

// Return pts value corresponding to currently playing audio.
double playing_audio_pts(sh_audio_t *sh_audio, demux_stream_t *d_audio,
				ao_functions_t *audio_out)
{
    return written_audio_pts(sh_audio, d_audio) - playback_speed *
		audio_out->get_delay();
}

static int generate_video_frame(sh_video_t *sh_video, demux_stream_t *d_video)
{
    unsigned char *start;
    int in_size;
    int hit_eof=0;
    double pts;

    while (1) {
	void *decoded_frame;
	current_module = "decode video";
	// XXX Time used in this call is not counted in any performance
	// timer now, OSD is not updated correctly for filter-added frames
	if (vf_output_queued_frame(sh_video->vfilter))
	    break;
	current_module = "video_read_frame";
	in_size = ds_get_packet_pts(d_video, &start, &pts);
	if (in_size < 0) {
	    // try to extract last frames in case of decoder lag
	    in_size = 0;
	    pts = 1e300;
	    hit_eof = 1;
	}
	if (in_size > max_framesize)
	    max_framesize = in_size;
	current_module = "decode video";
	decoded_frame = decode_video(sh_video, start, in_size, 0, pts);
	if (decoded_frame) {
	    update_subtitles(sh_video, mpctx->d_sub, 0);
	    update_teletext(sh_video, mpctx->demuxer, 0);
	    update_osd_msg();
	    current_module = "filter video";
	    if (filter_video(sh_video, decoded_frame, sh_video->pts))
		break;
	}
	if (hit_eof)
	    return 0;
    }
    return 1;
}

#ifdef HAVE_RTC
    int rtc_fd = -1;
#endif

static float timing_sleep(float time_frame)
{
#ifdef HAVE_RTC
    if (rtc_fd >= 0){
	// -------- RTC -----------
	current_module="sleep_rtc";
        while (time_frame > 0.000) {
	    unsigned long rtc_ts;
	    if (read(rtc_fd, &rtc_ts, sizeof(rtc_ts)) <= 0)
		mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_LinuxRTCReadError, strerror(errno));
    	    time_frame -= GetRelativeTime();
	}
    } else
#endif
    {
	// assume kernel HZ=100 for softsleep, works with larger HZ but with
	// unnecessarily high CPU usage
	//dsqiu
	float margin = softsleep ? 0.011 : 0.010;
	current_module = "sleep_timer";
	while (time_frame > margin) {
	    
	    usec_sleep((int)(1000000.0 * (time_frame - margin)));
	    time_frame -= GetRelativeTime();
	}
	if (softsleep){
	    current_module = "sleep_soft";
	    if (time_frame < 0)
		mp_msg(MSGT_AVSYNC, MSGL_WARN, MSGTR_SoftsleepUnderflow);
	    while (time_frame > 0)
		time_frame-=GetRelativeTime(); // burn the CPU
	}
    }
    return time_frame;
}
float AV_delay = 0.0;
float Audio_PTS = 0.0;
#undef printf
static void adjust_sync_and_print_status(int between_frames, float timing_error)
{
    current_module="av_sync";

    if(mpctx->sh_audio){
	double a_pts, v_pts;

	if (autosync)
	{	
	    /*
	     * If autosync is enabled, the value for delay must be calculated
	     * a bit differently.  It is set only to the difference between
	     * the audio and video timers.  Any attempt to include the real
	     * or corrected delay causes the pts_correction code below to
	     * try to correct for the changes in delay which autosync is
	     * trying to measure.  This keeps the two from competing, but still
	     * allows the code to correct for PTS drift *only*.  (Using a delay
	     * value here, even a "corrected" one, would be incompatible with
	     * autosync mode.)
	     */
	     
	    Audio_PTS = written_audio_pts(mpctx->sh_audio, mpctx->d_audio);
	    a_pts = Audio_PTS - mpctx->delay;
	}
	else
	{	
	    a_pts = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
	    Audio_PTS = a_pts;
  }
	v_pts = mpctx->sh_video->pts;
	
	//Audio_PTS = a_pts;
	
#if 1 //dsqiu
	{
	    static int drop_message=0;
	    //double AV_delay = a_pts - audio_delay - v_pts;
	    AV_delay = a_pts - audio_delay - v_pts;
	    double x;
	    //F("v_pts = %f,%f,%f,%f\n",v_pts,a_pts,audio_delay,AV_delay);
	    /*
	    if (AV_delay>0.5 && drop_frame_cnt>50 && drop_message==0){
		++drop_message;
		mp_msg(MSGT_AVSYNC,MSGL_WARN,MSGTR_SystemTooSlow);
	    }
	    */
	    if (autosync)
				x = AV_delay*0.1f;
	    else
		/* Do not correct target time for the next frame if this frame
		 * was late not because of wrong target time but because the
		 * target time could not be met */
		x = (AV_delay + timing_error * playback_speed) * 0.1f;
	    if (x < -max_pts_correction)
		x = -max_pts_correction;
	    else if (x> max_pts_correction)
		x = max_pts_correction;
	    if (default_max_pts_correction >= 0)
		max_pts_correction = default_max_pts_correction;
	    else
			max_pts_correction = mpctx->sh_video->frametime*0.10; // +-10% of time
	    if (!between_frames) {
		mpctx->delay+=x;
		c_total+=x;
	    }
	    if(!quiet)
		print_status(a_pts - audio_delay, AV_delay, c_total);
	}
#endif
    } else {
	// No audio:
    
	if (!quiet)
	    print_status(0, 0, 0);
    }
}

static int fill_audio_out_buffers(void)
{
    unsigned int t;
    int64_t tt;
    int playsize;
    int playflags=0;
    int audio_eof=0;
    int bytes_to_write;
    sh_audio_t * const sh_audio = mpctx->sh_audio;

    current_module="play_audio";

    while (1) {
	// all the current uses of ao_data.pts seem to be in aos that handle
	// sync completely wrong; there should be no need to use ao_data.pts
	// in get_space()
	ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
	bytes_to_write = mpctx->audio_out->get_space();
	if (mpctx->sh_video || bytes_to_write >= ao_data.outburst)
	    break;

	// handle audio-only case:
	// this is where mplayer sleeps during audio-only playback
	// to avoid 100% CPU use
	usec_sleep(10000); // Wait a tick before retry
    }

    while (bytes_to_write) {
	playsize = bytes_to_write;
	if (playsize > MAX_OUTBURST)
	    playsize = MAX_OUTBURST;
	bytes_to_write -= playsize;

	//Fill buffer if needed:
	current_module="decode_audio";
	t = GetTimer();
	while (sh_audio->a_out_buffer_len < playsize) {
	    int buflen = sh_audio->a_out_buffer_len;
	    int ret = decode_audio(sh_audio, &sh_audio->a_out_buffer[buflen],
				   playsize - buflen, // min bytes
				   sh_audio->a_out_buffer_size - buflen // max
				   );
    if (ret <= 0) { // EOF?
		if (mpctx->d_audio->eof) {
		    audio_eof = 1;
		    if (sh_audio->a_out_buffer_len == 0)
			return 0;
		}
		break;
	    }
	    
	    sh_audio->a_out_buffer_len += ret;
	    
	}
	t = GetTimer() - t;
	audio_time_usage+=t;
	if (playsize > sh_audio->a_out_buffer_len) {
	    playsize = sh_audio->a_out_buffer_len;
	    if (audio_eof)
			playflags |= AOPLAY_FINAL_CHUNK;
			
	}
	if (!playsize)
	    break;
   
	// play audio:  
	current_module="play_audio";

	// Is this pts value actually useful for the aos that access it?
	// They're obviously badly broken in the way they handle av sync;
	// would not having access to this make them more broken?
	ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
	playsize = mpctx->audio_out->play(sh_audio->a_out_buffer, playsize, playflags);

	if (playsize > 0) {
	    sh_audio->a_out_buffer_len -= playsize;
	    memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
		    sh_audio->a_out_buffer_len);
	    //memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
		  //  sh_audio->a_out_buffer_len);
	    mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
	}
	else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
	    // Sanity check to avoid hanging in case current ao doesn't output
	    // partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
	    mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
	    sh_audio->a_out_buffer_len = 0;
	}
    }
    return 1;
}


static int sleep_until_update(float *time_frame, float *aq_sleep_time)
{
    int frame_time_remaining = 0;
    current_module="calc_sleep_time";

    *time_frame -= GetRelativeTime(); // reset timer

    if (mpctx->sh_audio && !mpctx->d_audio->eof) {
	float delay = mpctx->audio_out->get_delay();
	if(delay < 0)
		F("audio delay=%f\n", delay);
	mp_dbg(MSGT_AVSYNC, MSGL_DBG2, "delay=%f\n", delay);

	if (autosync) {
	    /*
	     * Adjust this raw delay value by calculating the expected
	     * delay for this frame and generating a new value which is
	     * weighted between the two.  The higher autosync is, the
	     * closer to the delay value gets to that which "-nosound"
	     * would have used, and the longer it will take for A/V
	     * sync to settle at the right value (but it eventually will.)
	     * This settling time is very short for values below 100.
	     */
	    float predicted = mpctx->delay / playback_speed + *time_frame;
	    float difference = delay - predicted;
	    delay = predicted + difference / (float)autosync;
	}

	*time_frame = delay - mpctx->delay / playback_speed;

	// delay = amount of audio buffered in soundcard/driver
	
	
	
	//if (delay > 0.25) delay=0.25; else
	//if (delay < 0.10) delay=0.10;
		if (*time_frame > delay*0.6) {
	    // sleep time too big - may cause audio drops (buffer underrun)
	    frame_time_remaining = 1;
	    *time_frame = delay*0.5;
	}
    } else {
	// If we're lagging more than 200 ms behind the right playback rate,
	// don't try to "catch up".
	// If benchmark is set always output frames as fast as possible
	// without sleeping.
	if (*time_frame < -0.2 || benchmark)
	    *time_frame = 0;
    }

    *aq_sleep_time += *time_frame;


    //============================== SLEEP: ===================================

    // flag 256 means: libvo driver does its timing (dvb card)
    if (*time_frame > 0.001 && !(vo_flags&256))
				*time_frame = timing_sleep(*time_frame);
    return frame_time_remaining;
}

int reinit_video_chain(void) {
    sh_video_t * const sh_video = mpctx->sh_video;
    double ar=-1.0;
    //================== Init VIDEO (codec & libvo) ==========================
    if(!fixed_vo || !(inited_flags&INITED_VO)){
    current_module="preinit_libvo";

    //shouldn't we set dvideo->id=-2 when we fail?
    vo_config_count=0;
    //if((mpctx->video_out->preinit(vo_subdevice))!=0){
    if(!(mpctx->video_out=init_best_video_out(video_driver_list))){
      mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_ErrorInitializingVODevice);
      goto err_out;
    }
    sh_video->video_out=mpctx->video_out;
    inited_flags|=INITED_VO;
  }

  if(stream_control(mpctx->demuxer->stream, STREAM_CTRL_GET_ASPECT_RATIO, &ar) != STREAM_UNSUPPORTED)
      mpctx->sh_video->stream_aspect = ar;
  current_module="init_video_filters";
  {
    char* vf_arg[] = { "_oldargs_", (char*)mpctx->video_out , NULL };
    sh_video->vfilter=(void*)vf_open_filter(NULL,"vo",vf_arg);
  }
  sh_video->vfilter=(void*)append_filters(sh_video->vfilter);


  current_module="init_video_codec";

  mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");
  init_best_video_codec(sh_video,video_codec_list,video_fm_list);
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"==========================================================================\n");

  if(!sh_video->inited){
    if(!fixed_vo) uninit_player(INITED_VO);
    goto err_out;
  }

  inited_flags|=INITED_VCODEC;

  if (sh_video->codec)
    mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_CODEC=%s\n", sh_video->codec->name);

  sh_video->last_pts = MP_NOPTS_VALUE;
  sh_video->num_buffered_pts = 0;
  sh_video->next_frame_time = 0;

  if(auto_quality>0){
    // Auto quality option enabled
    output_quality=get_video_quality_max(sh_video);
    if(auto_quality>output_quality) auto_quality=output_quality;
    else output_quality=auto_quality;
    mp_msg(MSGT_CPLAYER,MSGL_V,"AutoQ: setting quality to %d.\n",output_quality);
    set_video_quality(sh_video,output_quality);
  }
  // ========== Init display (sh_video->disp_w*sh_video->disp_h/out_fmt) ============

  current_module="init_vo";

  return 1;

err_out:
  mpctx->sh_video = mpctx->d_video->sh = NULL;
  return 0;
}
int dropped_frames = 0;
int AV_presync(float frame_time);
//#undef printf
static double update_video(int *blit_frame)
{
    sh_video_t * const sh_video = mpctx->sh_video;
    //--------------------  Decode a frame: -----------------------
    double frame_time = 0.0;
    *blit_frame = 0; // Don't blit if we hit EOF
    if (!correct_pts) {
	unsigned char* start=NULL;
	void *decoded_frame;
	int drop_frame=0;
	int in_size;
	current_module = "video_read_frame";
	frame_time = sh_video->next_frame_time;
	//F("%d\n",(int)(frame_time* 1000.0));
	#if 0
	if(frame_dropping == 0)
	{	
		in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
						   &start, force_fps);
			if(mpctx->sh_audio && !mpctx->d_audio->eof)
				av_drop = AV_presync(frame_time);
			
			if(av_drop)
			{
					drop_image();
			}
  }else
  	in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
					   &start, force_fps);
	#endif
	//printf("frame_dropping = %d\n",frame_dropping);
	if(frame_dropping == 0)
	{	
			
			
			in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
								   &start, force_fps);
			//printf("mpctx->sh_audio = %x mpctx->d_audio->eof = %x\n",mpctx->sh_audio,mpctx->d_audio->eof);
			if(mpctx->sh_audio && !mpctx->d_audio->eof)
				av_drop = AV_presync(frame_time);
			if(av_drop)
			{
				mpctx->delay -= frame_time;
				float ds_fill_keyframe_buffer(demux_stream_t *ds);
				 float f = ds_fill_keyframe_buffer(sh_video->ds);
				 //printf("mpctx->delay = %f f = %f\n",mpctx->delay,f);
				 mpctx->delay -= f;
				 drop_image();
				 demuxer_set_drop_image();		   
				 in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
								   &start, force_fps);
			   av_drop = AV_presync(frame_time);
			}
			
  }else
  	in_size = video_read_frame(sh_video, &sh_video->next_frame_time,
					   &start, force_fps);
		
	if (in_size < 0)
	    return -1;
	if (in_size > max_framesize)
	    max_framesize = in_size; // stats
	sh_video->timer += frame_time;
	if (mpctx->sh_audio)
	    mpctx->delay -= frame_time;
	// video_read_frame can change fps (e.g. for ASF video)
	vo_fps = sh_video->fps;
	// check for frame-drop:
	if(frame_dropping)
	{	
		current_module = "check_framedrop";
		
		if (mpctx->sh_audio && !mpctx->d_audio->eof) {
		    float delay = playback_speed * mpctx->audio_out->get_delay();
		    float d = delay-mpctx->delay;
		    //F("%d   %d    %d   %d\n",(int)(delay * 1000.0),(int)(mpctx->delay * 1000.0),(int)(d* 1000.0),(int)(frame_time* 1000.0));
		    // we should avoid dropping too many frames in sequence unless we
		    // are too late. and we allow 100ms A-V delay here:
		    float a_v_delay = 0.100;//1.000; //0.100
		    //////////////////
		    #if 1
		    if (d < -dropped_frames*frame_time - a_v_delay &&		
					mpctx->osd_function != OSD_PAUSE) {
					//F("%d   %d    %d   %d\n",(int)(delay * 1000.0),(int)(mpctx->delay * 1000.0),(int)(d* 1000.0),(int)(frame_time* 1000.0));
			
		    //a_v_delay = 2.000;	
				drop_frame = frame_dropping;
				++drop_frame_cnt;
				++dropped_frames;
		  
			//BUFF_SetPriority(0);
		    } else
		    {	
		    	//a_v_delay = 0.200;
		    	//BUFF_SetPriority(1);
			    drop_frame = dropped_frames = 0;
		    }
		    #endif
	#ifndef JZ47_OPT
		    ++total_frame_cnt;
	#endif
		}
  }
	
#ifdef JZ47_OPT
        ++total_frame_cnt;
#endif
	//update_subtitles(sh_video, mpctx->d_sub, 0);
	//update_teletext(sh_video, mpctx->demuxer, 0);
	//update_osd_msg();
		if(av_drop == 0)
		{	
				current_module = "decode_video";
				decoded_frame = decode_video(sh_video, start, in_size, drop_frame,
							     sh_video->pts);
				
				current_module = "filter_video";
				*blit_frame = (decoded_frame && filter_video(sh_video, decoded_frame,
							    sh_video->pts));
		}					    
    }
    else {
    	
	if (!generate_video_frame(sh_video, mpctx->d_video))
	    return -1;

	((vf_instance_t *)sh_video->vfilter)->control(sh_video->vfilter,
					    VFCTRL_GET_PTS, &sh_video->pts);
	if (sh_video->pts == MP_NOPTS_VALUE) {
	    mp_msg(MSGT_CPLAYER, MSGL_ERR, "pts after filters MISSING\n");
	    sh_video->pts = sh_video->last_pts;
	}
	if (sh_video->last_pts == MP_NOPTS_VALUE)
	    sh_video->last_pts= sh_video->pts;
	else if (sh_video->last_pts >= sh_video->pts) {
	    sh_video->last_pts = sh_video->pts;
	    mp_msg(MSGT_CPLAYER, MSGL_INFO, "pts value <= previous\n");
	}
	frame_time = sh_video->pts - sh_video->last_pts;
	sh_video->last_pts = sh_video->pts;
	sh_video->timer += frame_time;
	if(mpctx->sh_audio)
	    mpctx->delay -= frame_time;
	*blit_frame = 1;
   
    }
    return frame_time;
}

void pause_loop(void)
{

    if (mpctx->video_out && mpctx->sh_video && vo_config_count)
	mpctx->video_out->control(VOCTRL_PAUSE, NULL);

    if (mpctx->audio_out && mpctx->sh_audio)
    {	
	if(!videoshow)	
		wait_audio_empty();
		mpctx->audio_out->pause();	// pause audio, keep data if possible
    }
    unsigned char err;
    Flush_Screen(); 
    if(mp_sem_pause)
    {
    	F("===========    os_SemaphorePend(mp_sem_pause,0,&err);  ========\n");
    	
  	os_SemaphorePend(mp_sem_pause,0,&err);
        F("===========    os_SemaphorePend(mp_sem_pause,0,&err);  ========\n");
        //lcd_clean_frame_all();
	//lcd_flush_frame_all();
    }
    
    #if 0
    while ( (cmd = mp_input_get_cmd(20, 1, 1)) == NULL) {
	if (mpctx->sh_video && mpctx->video_out && vo_config_count)
	    mpctx->video_out->check_events();
#ifdef HAVE_NEW_GUI
	if (use_gui) {
	    guiEventHandling();
	    guiGetEvent(guiReDraw, NULL);
	    if (guiIntfStruct.Playing!=2 || (rel_seek_secs || abs_seek_pos))
		break;
	}
#ifdef HAVE_MENU
	if (vf_menu)
	    vf_menu_pause_update(vf_menu);
#endif
	usec_sleep(20000);
    }

    if (cmd && cmd->id == MP_CMD_PAUSE) {
	cmd = mp_input_get_cmd(0,1,0);
	mp_cmd_free(cmd);
    }
   #endif
#endif
    mpctx->osd_function=OSD_PLAY;
    
    printf("========= loop======== %d \n",mpctx->eof);
   if(mpctx->eof == 0)
    {	
    if (mpctx->audio_out && mpctx->sh_audio)
        mpctx->audio_out->resume();	// resume audio
    if (mpctx->video_out && mpctx->sh_video && vo_config_count)
        mpctx->video_out->control(VOCTRL_RESUME, NULL);	// resume video
    }else
    	mpctx->audio_out->control(AOCONTROL_SET_MUTE,0);
    GetRelativeTime();	// ignore time that passed during pause
   	
#ifdef HAVE_NEW_GUI
    if (use_gui) {
	if (guiIntfStruct.Playing == guiSetStop)
	    mpctx->eof = 1;
	else
	    guiGetEvent(guiCEvent, (char *)guiSetPlay);
    }
#endif

}

void print_version(void){
  mp_msg(MSGT_CPLAYER, MSGL_INFO, "%s\n", MP_TITLE);

/* Test for CPU capabilities (and corresponding OS support) for optimizing */
  //GetCpuCaps(&gCpuCaps);
#ifdef ARCH_X86
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"CPUflags:  MMX: %d MMX2: %d 3DNow: %d 3DNow2: %d SSE: %d SSE2: %d\n",
      gCpuCaps.hasMMX,gCpuCaps.hasMMX2,
      gCpuCaps.has3DNow, gCpuCaps.has3DNowExt,
      gCpuCaps.hasSSE, gCpuCaps.hasSSE2);
#ifdef RUNTIME_CPUDETECT
  mp_msg(MSGT_CPLAYER,MSGL_INFO, MSGTR_CompiledWithRuntimeDetection);
#else
  mp_msg(MSGT_CPLAYER,MSGL_INFO, MSGTR_CompiledWithCPUExtensions);
#ifdef HAVE_MMX
  mp_msg(MSGT_CPLAYER,MSGL_INFO," MMX");
#endif
#ifdef HAVE_MMX2
  mp_msg(MSGT_CPLAYER,MSGL_INFO," MMX2");
#endif
#ifdef HAVE_3DNOW
  mp_msg(MSGT_CPLAYER,MSGL_INFO," 3DNow");
#endif
#ifdef HAVE_3DNOWEX
  mp_msg(MSGT_CPLAYER,MSGL_INFO," 3DNowEx");
#endif
#ifdef HAVE_SSE
  mp_msg(MSGT_CPLAYER,MSGL_INFO," SSE");
#endif
#ifdef HAVE_SSE2
  mp_msg(MSGT_CPLAYER,MSGL_INFO," SSE2");
#endif
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"\n");
#endif /* RUNTIME_CPUDETECT */
#endif /* ARCH_X86 */
}

// Find the right mute status and record position for new file position
static void edl_seek_reset(MPContext *mpctx)
{
    mpctx->edl_muted = 0;
    next_edl_record = edl_records;

    while (next_edl_record) {
	if (next_edl_record->start_sec >= mpctx->sh_video->pts)
	    break;

	if (next_edl_record->action == EDL_MUTE)
	    mpctx->edl_muted = !mpctx->edl_muted;
	next_edl_record = next_edl_record->next;
    }
#ifndef USE_16M_SDRAM
    if ((mpctx->user_muted | mpctx->edl_muted) != mpctx->mixer.muted)
		
		mixer_mute(&mpctx->mixer);
#endif
}


// Execute EDL command for the current position if one exists
static void edl_update(MPContext *mpctx)
{
	#if 0
    if (!next_edl_record)
	return;

    if (!mpctx->sh_video) {
	mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_EdlNOsh_video);
	free_edl(edl_records);
	next_edl_record = NULL; 
	edl_records = NULL;
	return;
    }

    if (mpctx->sh_video->pts >= next_edl_record->start_sec) {
	if (next_edl_record->action == EDL_SKIP) {
	    mpctx->osd_function = OSD_FFW;
	    abs_seek_pos = 0;
	    rel_seek_secs = next_edl_record->length_sec;
	    mp_msg(MSGT_CPLAYER, MSGL_DBG4, "EDL_SKIP: start [%f], stop "
		   "[%f], length [%f]\n", next_edl_record->start_sec,
		   next_edl_record->stop_sec, next_edl_record->length_sec);
	    edl_decision = 1;
	}
	else if (next_edl_record->action == EDL_MUTE) {
	    mpctx->edl_muted = !mpctx->edl_muted;
	    if ((mpctx->user_muted | mpctx->edl_muted) != mpctx->mixer.muted)
		mixer_mute(&mpctx->mixer);
	    mp_msg(MSGT_CPLAYER, MSGL_DBG4, "EDL_MUTE: [%f]\n",
		   next_edl_record->start_sec );
	}
	next_edl_record = next_edl_record->next;
    }
    #endif
}


// style & 1 == 0 means seek relative to current position, == 1 means absolute
// style & 2 == 0 means amount in seconds, == 2 means fraction of file length
// return -1 if seek failed (non-seekable stream?), 0 otherwise
static int seek(MPContext *mpctx, double amount, int style)
{
    current_module = "seek";
    
    if (demux_seek(mpctx->demuxer, amount, audio_delay, style) == 0)
	return -1;
#if 1
    if (mpctx->sh_video) {
	current_module = "seek_video_reset";
	
	resync_video_stream(mpctx->sh_video);
	
	if (vo_config_count)
	    mpctx->video_out->control(VOCTRL_RESET, NULL);
	mpctx->sh_video->num_buffered_pts = 0;
	mpctx->sh_video->last_pts = MP_NOPTS_VALUE;
	mpctx->num_buffered_frames = 0;
	mpctx->delay = 0;
	// Not all demuxers set d_video->pts during seek, so this value
	// (which is used by at least vobsub and edl code below) may
	// be completely wrong (probably 0).
	mpctx->sh_video->pts = mpctx->d_video->pts;
	//update_subtitles(mpctx->sh_video, mpctx->d_sub, 1);
	//update_teletext(mpctx->sh_video, mpctx->demuxer, 1);
    }
      
    if (mpctx->sh_audio) {
	current_module = "seek_audio_reset";
	//mpctx->audio_out->reset(); // stop audio, throwing away buffered data
	mpctx->sh_audio->a_buffer_len = 0;
	mpctx->sh_audio->a_out_buffer_len = 0;
    }

    if (vo_vobsub) {
	current_module = "seek_vobsub_reset";
	vobsub_seek(vo_vobsub, mpctx->sh_video->pts);
    }
     max_pts_correction = 0.1;
    
#endif
GetRelativeTime();
    c_total = 0;

    audio_time_usage = 0; video_time_usage = 0; vout_time_usage = 0;
    drop_frame_cnt = 0;
    current_module = NULL;
    return 0;
}

int opt_exit = 0;
/* Flag indicating whether MPlayer should exit without playing anything. */
void SetMPlayerQuit( void )
{
     opt_exit = 1;
	 if(mpctx != NULL)
	 {	
	 	 if(videoshow == 0)
		 {
		 	if(mpctx->osd_function == OSD_PAUSE)
		 	{
		 		mpctx->eof = 1;
		 		mpctx->osd_function = OSD_PLAY;
	    		os_SemaphorePost(mp_sem_pause);
	    		printf("############################ quit \n");
		 		
		 	}else
		 	{	
			 	MP_MODE(0x11);
			 	if(mpctx->audio_out)
			 	{	
					mpctx->audio_out->control(AOCONTROL_SET_VOL_DEC_GRADED,0);
					while((mp_mode & 0xffff) != 0)
						usec_sleep(10000);
				}
			}
		 }
		 mpctx->eof = 1;
	 }
	 		
}

#ifdef USE_16M_SDRAM
#include "av_plugin.h"
void *load_plugin (char *drv_name, char *dll_name, char video_plg, unsigned int arg0);
#endif
#if EXCEPTION_MPLAYER
#ifdef JZ4750_OPT
#include "jz4750_ipu.h"
#endif
void mplayer_excpt(void)
{
	mp_mode = 0;
	if(videoshow)
	{ 
		if(!get_display_is_direct())
				 lcd_reset_frame();
	}
	if(mpctx)
	{	
		mpctx->eof = 1;
		if(mpctx->audio_out)
		{
			mpctx->audio_out->resume();
			wait_audio_empty();	
			mpctx->audio_out->pause();	
			
  		}
	//应该关闭
	}
	if(g_ExitPlay)
		g_ExitPlay();
	
	
	#ifndef USE_16M_SDRAM
  //if (mpctx->user_muted && !mpctx->edl_muted) if(mpctx->mixer) mixer_mute(&mpctx->mixer); 
#endif
	unsigned char my_err;
	if(mp_sem_pause)
		os_SemaphoreDelete ( mp_sem_pause , 1 , &my_err) ;
	if(mp_sem_seek)
		os_SemaphoreDelete ( mp_sem_seek , 1 , &my_err) ;
	if(audio_sem_filemode)
		os_SemaphoreDelete ( audio_sem_filemode , 1 , &my_err) ;
	mp_sem_pause = NULL;
	mp_sem_seek = NULL;
	audio_sem_filemode = NULL;
	
  //uninit_player(INITED_ALL);
 	BUFF_Exception();
 	if((mpctx ) && (mpctx->video_out))
 	{
 		mpctx->video_out->uninit();
  	mpctx->video_out=NULL;
  }	
 	if((mpctx ) && (mpctx->audio_out))
	{
   	mpctx->audio_out->uninit(mpctx->eof?0:1); 
   	mpctx->audio_out=NULL;
	}
	F("1\n");

  current_module="exit_player";

// free mplayer config
  if(mconfig)
    m_config_free(mconfig);

  if(mpctx->playtree)
    play_tree_free(mpctx->playtree, 1);


  if(edl_records != NULL) free(edl_records); // free mem allocated for EDL
  do_free();
  codecs_uninit_free();
  
  g_StartPlay = NULL;
  mpctx = NULL;
#ifdef JZ4740_MXU_OPT
	if(mxucr)
	{	
	  disable_jz4740_mxu (mxucr);
	  mxucr = 0;
	}
#endif
  mp_memory_deinit(videoshow);
  JZ_StopTimer();
  excpt_exit(0,saveregister);
}
#endif

#ifdef VERIFY_CRC32
extern unsigned int image_crc32;
#endif


int c_mplay_main(int argc,char* argv[]);
int mplay_main(int argc,char* argv[])
{
	#if EXCEPTION_MPLAYER
	SAVE_PROCESS_REGISTER(saveregister);
	#endif
	return c_mplay_main(argc,argv);
}
int rv9_skip_dblk = 0;    

int c_mplay_main(int argc,char* argv[]){

	#if EXCEPTION_MPLAYER
	mp_excptid = AddExcept(mplayer_excpt,saveregister);
	#endif
#ifdef JZ4740_MXU_OPT
  mxucr = enable_jz4740_mxu ();
#endif
char * mem_ptr;

// movie info:


//float a_frame=0;    // Audio

int i;

int gui_no_filename=0;
  initmplayer();
  //__asm__ __volatile__ (".long 0x7000003f");
//F("-----------frame_dropping = %d\n",frame_dropping);
	//InitSysTime();
	

#ifdef USE_16M_SDRAM
load_plugin ("table", CODEC_TABLE_NAME, 0, FFMPEG_PLUGIN_TABLE);
#endif


#ifdef JZ47_ANL_TIME
   {
     int i;
     for (i = 0; i < ANL_CNT; i++)
       anltime[i] = 0;
   }
#endif
  //srand((int) time(NULL)); 

  InitTimer();

  mp_msg_init();
  
  for(i=1; i<argc; i++)
    if(!strcmp(argv[i], "-really-quiet"))
      verbose= -10;
//  F("1");
//  print_version();
 // F("2");
#if defined(WIN32) && defined(USE_WIN32DLL)
  set_path_env();
#endif /*WIN32 && USE_WIN32DLL*/
 F("3");
#ifdef USE_TV
  stream_tv_defaults.immediate = 1;
#endif

#ifdef JZ4750_OPT
  REG32(CPM_CLKGR_VADDR) &= ~(IDCT_CLOCK);
  REG32(CPM_CLKGR_VADDR) &= ~(DBLK_CLOCK);
  REG32(CPM_CLKGR_VADDR) &= ~(ME_CLOCK);
  REG32(CPM_CLKGR_VADDR) &= ~(MC_CLOCK);
  REG32(CPM_CLKGR_VADDR) &= ~(IPU_CLOCK);
#endif

ipu_image_start();
mplayer_showmode(1);

#if 0

  if (argc > 1 && argv[1] &&
      (!strcmp(argv[1], "-gui") || !strcmp(argv[1], "-nogui"))) {
    use_gui = !strcmp(argv[1], "-gui");
  } else
  if ( argv[0] )
  {
    char *base = strrchr(argv[0], '/');
    if (!base)
      base = strrchr(argv[0], '\\');
    if (!base)
      base = argv[0];
    if(strstr(base, "gmplayer"))
          use_gui=1;
  }
F("4");
    mconfig = m_config_new();
F("5");    
    m_config_register_options(mconfig,mplayer_opts);
F("6");    

    // TODO : add something to let modules register their options
    mp_input_register_options(mconfig);
F("7");    
    
    parse_cfgfiles(mconfig);
F("8");    

#ifdef HAVE_NEW_GUI
    if ( use_gui ) cfg_read();
#endif

    mpctx->playtree = m_config_parse_mp_command_line(mconfig, argc, argv);
F("9");    
    if(mpctx->playtree == NULL)
      opt_exit = 1;
    else {
    mpctx->playtree = play_tree_cleanup(mpctx->playtree);
    F("10");
    if(mpctx->playtree) {
      mpctx->playtree_iter = play_tree_iter_new(mpctx->playtree,mconfig);
      if(mpctx->playtree_iter) {  
	if(play_tree_iter_step(mpctx->playtree_iter,0,0) != PLAY_TREE_ITER_ENTRY) {
	  play_tree_iter_free(mpctx->playtree_iter);
	  mpctx->playtree_iter = NULL;
	}
	filename = play_tree_iter_get_file(mpctx->playtree_iter,1);
      }
    }
    }
F("11"); 	
#if defined(WIN32) && defined(HAVE_NEW_GUI)
    void *runningmplayer = FindWindow("MPlayer GUI for Windows", "MPlayer for Windows");
    if(runningmplayer && filename && use_gui){
        COPYDATASTRUCT csData;
        char file[MAX_PATH];
        char *filepart = filename;
        if(GetFullPathName(filename, MAX_PATH, file, &filepart)){
            csData.dwData = 0;
            csData.cbData = strlen(file)*2;
            csData.lpData = file;
            SendMessage(runningmplayer, WM_COPYDATA, (WPARAM)runningmplayer, (LPARAM)&csData);
        }
    }
#endif

#ifdef WIN32
	if(proc_priority){
		int i;
        	for(i=0; priority_presets_defs[i].name; i++){
        		if(strcasecmp(priority_presets_defs[i].name, proc_priority) == 0)
				break;
		}
		mp_msg(MSGT_CPLAYER,MSGL_STATUS,"Setting process priority: %s\n",
				priority_presets_defs[i].name);
		SetPriorityClass(GetCurrentProcess(), priority_presets_defs[i].prio);
	}
#endif	
#ifndef HAVE_NEW_GUI
    if(use_gui){
      mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_NoGui);
      use_gui=0;
    }
#else
#ifndef WIN32
    if(use_gui && !vo_init()){
      mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_GuiNeedsX);
      use_gui=0;
    }
#endif
    if (use_gui && mpctx->playtree_iter){
      char cwd[PATH_MAX+2];
      // Free Playtree and Playtree-Iter as it's not used by the GUI.
      play_tree_iter_free(mpctx->playtree_iter);
      mpctx->playtree_iter=NULL;
      
      if (getcwd(cwd, PATH_MAX) != (char *)NULL)
      {
	  strcat(cwd, "/");
          // Prefix relative paths with current working directory
          play_tree_add_bpf(mpctx->playtree, cwd);
      }      
      // Import initital playtree into GUI.
      import_initial_playtree_into_gui(mpctx->playtree, mconfig, enqueue);
    }
#endif /* HAVE_NEW_GUI */
/*
    if(video_driver_list && strcmp(video_driver_list[0],"help")==0){
      list_video_out();
      opt_exit = 1;
    }

    if(audio_driver_list && strcmp(audio_driver_list[0],"help")==0){
      list_audio_out();
      opt_exit = 1;
    }*/
F("12");
/* Check codecs.conf. */
parse_codec_cfg(NULL);
F("13");
F("14");
    if(audio_codec_list && strcmp(audio_codec_list[0],"help")==0){
      mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AvailableAudioCodecs);
      mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AUDIO_CODECS\n");
      list_codecs(1);
      mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
      opt_exit = 1;
    }
    if(video_codec_list && strcmp(video_codec_list[0],"help")==0){
      mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_AvailableVideoCodecs);
      mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_CODECS\n");
      list_codecs(0);
      mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
      opt_exit = 1;
    }
    if(video_fm_list && strcmp(video_fm_list[0],"help")==0){
      vfm_help();
      mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
      opt_exit = 1;
    }
    if(audio_fm_list && strcmp(audio_fm_list[0],"help")==0){
      afm_help();
      mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
      opt_exit = 1;
    }
    if(af_cfg.list && strcmp(af_cfg.list[0],"help")==0){
      af_help();
      printf("\n");
      opt_exit = 1;
    }
F("15");
    
#ifdef HAVE_X11
    if(vo_fstype_list && strcmp(vo_fstype_list[0],"help")==0){
      fstype_help();
      mp_msg(MSGT_FIXME, MSGL_FIXME, "\n");
      opt_exit = 1;
    }
#endif
F("16");

    if((demuxer_name && strcmp(demuxer_name,"help")==0) ||
       (audio_demuxer_name && strcmp(audio_demuxer_name,"help")==0) ||
       (sub_demuxer_name && strcmp(sub_demuxer_name,"help")==0)){
      demuxer_help();
      mp_msg(MSGT_CPLAYER, MSGL_INFO, "\n");
      opt_exit = 1;
    }
F("17");
    
    if(list_properties) {
      property_print_help();
      opt_exit = 1;
    }
F("18");

    if(opt_exit)
      exit_player(NULL);
 F("19");
   
    if (player_idle_mode && use_gui) {
        mp_msg(MSGT_CPLAYER, MSGL_FATAL, MSGTR_NoIdleAndGui);
        exit_player_with_rc(NULL, 1);
    }

    if(!filename && !player_idle_mode){
      if(!use_gui){
	// no file/vcd/dvd -> show HELP:
	mp_msg(MSGT_CPLAYER, MSGL_INFO, help_text);
        exit_player_with_rc(NULL, 0);
      } else gui_no_filename=1;
    }
F("20");

    /* Display what configure line was used */
    mp_msg(MSGT_CPLAYER, MSGL_V, "Configuration: " CONFIGURATION "\n");

    // Many users forget to include command line in bugreports...
    if( mp_msg_test(MSGT_CPLAYER,MSGL_V) ){
      mp_msg(MSGT_CPLAYER, MSGL_INFO, MSGTR_CommandLine);
      for(i=1;i<argc;i++)mp_msg(MSGT_CPLAYER, MSGL_INFO," '%s'",argv[i]);
      mp_msg(MSGT_CPLAYER, MSGL_INFO, "\n");
    }
F("21");

//------ load global data first ------

// check font
#ifdef HAVE_FREETYPE
  init_freetype();
#endif
#ifdef HAVE_FONTCONFIG
  if(!font_fontconfig)
  {
#endif
#ifdef HAVE_BITMAP_FONT
  if(font_name){
  	
       vo_font=read_font_desc(font_name,font_factor,verbose>1);
       if(!vo_font) mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CantLoadFont,
		filename_recode(font_name));
  } else {
      // try default:
       vo_font=read_font_desc( mem_ptr=get_path("font/font.desc"),font_factor,verbose>1);
       free(mem_ptr); // release the buffer created by get_path()
       if(!vo_font)
       vo_font=read_font_desc(MPLAYER_DATADIR "/font/font.desc",font_factor,verbose>1);
  }
  if (sub_font_name)
    sub_font = read_font_desc(sub_font_name, font_factor, verbose>1);
  else
    sub_font = vo_font;
#endif
#ifdef HAVE_FONTCONFIG
  }
#endif

#endif // end 0

parse_codec_cfg(NULL);
F("22");
//vo_init_osd();
F("23");
#ifdef USE_ASS
  ass_library = ass_init();
#endif

#ifdef HAVE_RTC
  if(!nortc)
  {
    // seteuid(0); /* Can't hurt to try to get root here */
    if ((rtc_fd = open(rtc_device ? rtc_device : "/dev/rtc", O_RDONLY)) < 0)
	mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_RTCDeviceNotOpenable,
	    rtc_device ? rtc_device : "/dev/rtc", strerror(errno));
     else {
	unsigned long irqp = 1024; /* 512 seemed OK. 128 is jerky. */

	if (ioctl(rtc_fd, RTC_IRQP_SET, irqp) < 0) {
    	    mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_LinuxRTCInitErrorIrqpSet, irqp, strerror(errno));
    	    mp_msg(MSGT_CPLAYER, MSGL_HINT, MSGTR_IncreaseRTCMaxUserFreq, irqp);
   	    close (rtc_fd);
    	    rtc_fd = -1;
	} else if (ioctl(rtc_fd, RTC_PIE_ON, 0) < 0) {
	    /* variable only by the root */
    	    mp_msg(MSGT_CPLAYER, MSGL_ERR, MSGTR_LinuxRTCInitErrorPieOn, strerror(errno));
    	    close (rtc_fd);
	    rtc_fd = -1;
	} else
	    mp_msg(MSGT_CPLAYER, MSGL_V, MSGTR_UsingRTCTiming, irqp);
    }
  }
#ifdef HAVE_NEW_GUI
// breaks DGA and SVGAlib and VESA drivers:  --A'rpi
// and now ? -- Pontscho
    if(use_gui) setuid( getuid() ); // strongly test, please check this.
#endif
    if(rtc_fd<0)
#endif /* HAVE_RTC */
      mp_msg(MSGT_CPLAYER, MSGL_V, "Using %s timing\n",
	     softsleep?"software":timer_name);

#ifdef USE_TERMCAP
  if ( !use_gui ) load_termcap(NULL); // load key-codes
#endif

// ========== Init keyboard FIFO (connection to libvo) ============

// Init input system
F("24");
#if 0
current_module = "init_input";
mp_input_init(use_gui);
F("25");
mp_input_add_key_fd(-1,0,mplayer_get_key,NULL);
F("26");
if(slave_mode)
#ifndef __MINGW32__
   mp_input_add_cmd_fd(0,1,NULL,NULL);
#else
  mp_input_add_cmd_fd(0,0,mp_input_win32_slave_cmd_func,NULL);
#endif
#if 0
else if(!noconsolecontrols)
    mp_input_add_event_fd(0, getch2);
#endif
F("27");
inited_flags|=INITED_INPUT;
current_module = NULL;

#ifdef HAVE_MENU
 if(use_menu) {
   if(menu_cfg && menu_init(mpctx, menu_cfg))
     mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MenuInitialized, menu_cfg);
   else {
     menu_cfg = get_path("menu.conf");
     if(menu_init(mpctx, menu_cfg))
       mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MenuInitialized, menu_cfg);
     else {
       if(menu_init(mpctx, MPLAYER_CONFDIR "/menu.conf"))
         mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MenuInitialized, MPLAYER_CONFDIR"/menu.conf");
       else {
         mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MenuInitFailed);
         use_menu = 0;
       }
     }
   }
 }
#endif
#endif  
  /// Catch signals
#ifndef __MINGW32__
//  signal(SIGCHLD,child_sighandler);
#endif

#ifdef CRASH_DEBUG
  prog_path = argv[0];
#endif
  //========= Catch terminate signals: ================
  // terminate requests:

#ifdef HAVE_NEW_GUI
  if(use_gui){
       guiInit();
       guiGetEvent(guiSetContext, mpctx);
       inited_flags|=INITED_GUI;
       guiGetEvent( guiCEvent,(char *)((gui_no_filename) ? 0 : 1) );
  }
#endif

// ******************* Now, let's see the per-file stuff ********************
	char **options = 0;
  if(videoshow == 0)
  {
  	
  	options = &audiofile;
  }	
play_next_file:

  // init global sub numbers
  mpctx->global_sub_size = 0;
  { int i; for (i = 0; i < SUB_SOURCES; i++) mpctx->global_sub_indices[i] = -1; }
//============ Open & Sync STREAM --- fork cache2 ====================
  filename = argv[1];
  mpctx->stream=NULL;
  mpctx->demuxer=NULL;
  if (mpctx->d_audio) {
    //free_demuxer_stream(mpctx->d_audio);
    mpctx->d_audio=NULL;
  }
  if (mpctx->d_video) {
    //free_demuxer_stream(d_video);
    mpctx->d_video=NULL;
  }
  mpctx->sh_audio=NULL;
  mpctx->sh_video=NULL;
	
  current_module="open_stream";
  
  mpctx->stream=open_stream(filename,options,&mpctx->file_format);
    if(!mpctx->stream) { // error...
    mpctx->eof = 1;
    goto goto_next_file;
  }
  inited_flags|=INITED_STREAM;
F("open stream = %s\n",filename);  
  if(mpctx->file_format == DEMUXER_TYPE_PLAYLIST) {
#ifdef USE_16M_SDRAM
    exit_player(MSGTR_Exit_error);
#else
    play_tree_t* entry;
    // Handle playlist
    current_module="handle_playlist";
    mp_msg(MSGT_CPLAYER,MSGL_V,"Parsing playlist %s...\n",
	    filename_recode(filename));
    entry = parse_playtree(mpctx->stream,0);
    mpctx->eof=playtree_add_playlist(entry);
    goto goto_next_file;
#endif
  }
  mpctx->stream->start_pos+=seek_to_byte;

if(stream_dump_type==5){

  mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_CoreDumped);
  exit_player_with_rc(MSGTR_Exit_eof, 0);
}





// CACHE2: initial prefill: 20%  later: 5%  (should be set by -cacheopts)

goto_enable_cache:
	
if(stream_cache_size>0){
  current_module="enable_cache";
  if(!stream_enable_cache(mpctx->stream,stream_cache_size*1024,
                          stream_cache_size*1024*(stream_cache_min_percent / 100.0),
                          stream_cache_size*1024*(stream_cache_seek_min_percent / 100.0)))
    if((mpctx->eof = libmpdemux_was_interrupted(PT_NEXT_ENTRY))) goto goto_next_file;
}

//============ Open DEMUXERS --- DETECT file type =======================
current_module="demux_open";
mpctx->demuxer=demux_open(mpctx->stream,mpctx->file_format,audio_id,video_id,dvdsub_id,filename);

if(!mpctx->demuxer) 
  goto goto_next_file;
inited_flags|=INITED_DEMUXER;
extern demuxer_desc_t demuxer_desc_mov;
if(((int)mpctx->demuxer->desc == (int)&demuxer_desc_mov) && (options == 0))
{
		inited_flags&=~INITED_DEMUXER;
    current_module="free_demuxer";
    if(mpctx->demuxer){
					mpctx->stream=mpctx->demuxer->stream;
					free_demuxer(mpctx->demuxer);
    }
    mpctx->demuxer=NULL;
  // kill the cache process:
    inited_flags&=~INITED_STREAM;
    current_module="uninit_stream";
    if(mpctx->stream) free_stream(mpctx->stream);
    mpctx->stream=NULL;
    options = &singlefile;
    goto play_next_file;
  
}	
if (mpctx->stream->type != STREAMTYPE_DVD && mpctx->stream->type != STREAMTYPE_DVDNAV) {
  int i;
  int maxid = -1;
  // setup global sub numbering
  mpctx->global_sub_indices[SUB_SOURCE_DEMUX] = mpctx->global_sub_size; // the global # of the first demux-specific sub.
  for (i = 0; i < MAX_S_STREAMS; i++)
    if (mpctx->demuxer->s_streams[i])
      maxid = FFMAX(maxid, ((sh_sub_t *)mpctx->demuxer->s_streams[i])->sid);
  mpctx->global_sub_size += maxid + 1;
}
if (mpctx->global_sub_size <= dvdsub_id) mpctx->global_sub_size = dvdsub_id + 1;
F("34\n");
current_module="demux_open2";

//file_format=demuxer->file_format;

mpctx->d_audio=mpctx->demuxer->audio;
mpctx->d_video=mpctx->demuxer->video;
mpctx->d_sub=mpctx->demuxer->sub;
F("34\n");
// DUMP STREAMS:
 
 F("35\n");  
mpctx->sh_audio=mpctx->d_audio->sh;
mpctx->sh_video=mpctx->d_video->sh;

if((videoshow == 0)&& mpctx->sh_video)
{	
	uninit_video(mpctx->sh_video);
	mpctx->sh_video = NULL;
}else
{
	//#undef printf
	//F("mpctx->sh_video->disp_w = %d\n",mpctx->sh_video->disp_w);
	//F("mpctx->sh_video->disp_h = %d\n",mpctx->sh_video->disp_h);
	if(mpctx->sh_video)
	{
			if(mpctx->sh_video->disp_w * mpctx->sh_video->disp_h > 1024 * 576)
				rv9_skip_dblk = 1;
			jz47_auto_select(mpctx->sh_video->disp_w,mpctx->sh_video->disp_h);
	}
}
 F("36\n");
if(mpctx->sh_video){

  current_module="video_read_properties";
  if(!video_read_properties(mpctx->sh_video)) {
    mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CannotReadVideoProperties);
    mpctx->sh_video=mpctx->d_video->sh=NULL;
  } else {
    mp_msg(MSGT_CPLAYER,MSGL_V,"[V] filefmt:%d  fourcc:0x%X  size:%dx%d  fps:%5.2f  ftime:=%6.4f\n",
	   mpctx->demuxer->file_format,mpctx->sh_video->format, mpctx->sh_video->disp_w,mpctx->sh_video->disp_h,
	   mpctx->sh_video->fps,mpctx->sh_video->frametime
	   );

    /* need to set fps here for output encoders to pick it up in their init */
    if(force_fps){
      mpctx->sh_video->fps=force_fps;
      mpctx->sh_video->frametime=1.0f/mpctx->sh_video->fps;
    }
    vo_fps = mpctx->sh_video->fps;

    if(!mpctx->sh_video->fps && !force_fps){
      mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_FPSnotspecified);
      mpctx->sh_video=mpctx->d_video->sh=NULL;
    }
  }

}
 F("37\n");
if(!mpctx->sh_video && !mpctx->sh_audio){
    mp_msg(MSGT_CPLAYER,MSGL_FATAL, MSGTR_NoStreamFound);
#ifdef HAS_DVBIN_SUPPORT
	if((mpctx->stream->type == STREAMTYPE_DVB) && mpctx->stream->priv)
	{
	  dvb_priv_t *priv = (dvb_priv_t*) mpctx->stream->priv;
	  if(priv->is_on)
	  {
		int dir;
		int v = mpctx->last_dvb_step;
		if(v > 0)
			dir = DVB_CHANNEL_HIGHER;
		else
			dir = DVB_CHANNEL_LOWER;
			
		if(dvb_step_channel(priv, dir))
			mpctx->eof = mpctx->dvbin_reopen = 1;
	  }
	}
#endif	
    goto goto_next_file; // exit_player(MSGTR_Exit_error);
}
 F("38\n");
/* display clip info */
demux_info_print(mpctx->demuxer);
 F("39\n");
//================== Read SUBTITLES (DVD & TEXT) ==========================
if(vo_spudec==NULL && mpctx->sh_video &&
     (mpctx->stream->type==STREAMTYPE_DVD || mpctx->stream->type == STREAMTYPE_DVDNAV || mpctx->d_sub->id >= 0)){
  init_vo_spudec();
}
 F("40\n");
// Apply current settings for forced subs 

if (vo_spudec!=NULL)
  spudec_set_forced_subs_only(vo_spudec,forced_subs_only);
 F("43\n");
 //mpctx->sh_video = NULL;

if(mpctx->sh_video) {
// after reading video params we should load subtitles because
// we know fps so now we can adjust subtitle time to ~6 seconds AST
// check .sub
  current_module="read_subtitles_file";
  if(sub_name){
    for (i = 0; sub_name[i] != NULL; ++i) 
        add_subtitles (sub_name[i], mpctx->sh_video->fps, 0); 
  } 
  if(sub_auto) { // auto load sub file ...
    char *psub = get_path( "sub/" );
    char **tmp = sub_filenames((psub ? psub : ""), filename);
    int i = 0;
    free(psub); // release the buffer created by get_path() above
    while (tmp[i]) {
        add_subtitles (tmp[i], mpctx->sh_video->fps, 0);
        free(tmp[i++]);
    }
    free(tmp);
  }
  if (mpctx->set_of_sub_size > 0)  {
      // setup global sub numbering
      mpctx->global_sub_indices[SUB_SOURCE_SUBS] = mpctx->global_sub_size; // the global # of the first sub.
      mpctx->global_sub_size += mpctx->set_of_sub_size;
  }
}
 F("41 %d\n",mpctx->global_sub_size);
if (mpctx->global_sub_size) {
  // find the best sub to use
  if (vobsub_id >= 0) {
    // if user asks for a vobsub id, use that first.
    mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_VOBSUB] + vobsub_id;
  } else if (dvdsub_id >= 0 && mpctx->global_sub_indices[SUB_SOURCE_DEMUX] >= 0) {
    // if user asks for a dvd sub id, use that next.
    mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_DEMUX] + dvdsub_id;
  } else if (mpctx->global_sub_indices[SUB_SOURCE_SUBS] >= 0) {
    // if there are text subs to use, use those.  (autosubs come last here)
    mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_SUBS];
/*
  } else if (mpctx->global_sub_indices[SUB_SOURCE_DEMUX] >= 0) {
    // if nothing else works, get subs from the demuxer.
    mpctx->global_sub_pos = mpctx->global_sub_indices[SUB_SOURCE_DEMUX];
*/
  } else {
    // nothing worth doing automatically.
    mpctx->global_sub_pos = -1;
  }
  // rather than duplicate code, use the SUB_SELECT handler to init the right one.
  mpctx->global_sub_pos--;
  mp_property_do("sub",M_PROPERTY_STEP_UP,NULL, mpctx);
  if(subdata)
    switch (stream_dump_type) {
        case 3: list_sub_file(subdata); break;
        case 4: dump_mpsub(subdata, mpctx->sh_video->fps); break;
        case 6: dump_srt(subdata, mpctx->sh_video->fps); break;
        case 7: dump_microdvd(subdata, mpctx->sh_video->fps); break;
        case 8: dump_jacosub(subdata, mpctx->sh_video->fps); break;
        case 9: dump_sami(subdata, mpctx->sh_video->fps); break;
    }
}
 F("44\n");
  mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_FILENAME=%s\n",
	  filename_recode(filename));
  mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_DEMUXER=%s\n", mpctx->demuxer->desc->name);
  if (mpctx->sh_video) {
    /* Assume FOURCC if all bytes >= 0x20 (' ') */
    if (mpctx->sh_video->format >= 0x20202020)
	mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FORMAT=%.4s\n", (char *)&mpctx->sh_video->format);
    else
	mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FORMAT=0x%08X\n", mpctx->sh_video->format);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_BITRATE=%d\n", mpctx->sh_video->i_bps*8);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_WIDTH=%d\n", mpctx->sh_video->disp_w);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_HEIGHT=%d\n", mpctx->sh_video->disp_h);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_FPS=%5.3f\n", mpctx->sh_video->fps);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_VIDEO_ASPECT=%1.4f\n", mpctx->sh_video->aspect);
  }
   F("45\n");
  if (mpctx->sh_audio) {
    /* Assume FOURCC if all bytes >= 0x20 (' ') */
    if (mpctx->sh_audio->format >= 0x20202020)
      mp_msg(MSGT_IDENTIFY,MSGL_INFO, "ID_AUDIO_FORMAT=%.4s\n", (char *)&mpctx->sh_audio->format);
    else
      mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_FORMAT=%d\n", mpctx->sh_audio->format);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_BITRATE=%d\n", mpctx->sh_audio->i_bps*8);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_RATE=%d\n", mpctx->sh_audio->samplerate);
    mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_AUDIO_NCH=%d\n", mpctx->sh_audio->channels);
  }
  mp_msg(MSGT_IDENTIFY,MSGL_INFO,"ID_LENGTH=%.2lf\n", demuxer_get_time_length(mpctx->demuxer));
 F("42\n");
if(!mpctx->sh_video) goto main; // audio-only

if(!reinit_video_chain()) {
  if(!mpctx->sh_video){
    if(!mpctx->sh_audio) goto goto_next_file;
    goto main; // exit_player(MSGTR_Exit_error);
}
}

   if(vo_flags & 0x08 && vo_spudec)
      spudec_set_hw_spu(vo_spudec,mpctx->video_out);

#ifdef HAVE_FREETYPE
   force_load_font = 1;
#endif

//================== MAIN: ==========================
main:
current_module="main";
 F("44\n");
    if(playing_msg) {
        char* msg = property_expand_string(mpctx, playing_msg);
        mp_msg(MSGT_CPLAYER,MSGL_INFO,"%s",msg);
        free(msg);
    }
        

// Disable the term OSD in verbose mode
if(verbose) term_osd = 0;

{
//int frame_corr_num=0;   //
//float v_frame=0;    // Video
float time_frame=0; // Timer
//float num_frames=0;      // number of frames played

int frame_time_remaining=0; // flag
int blit_frame=0;
mpctx->num_buffered_frames=0;

// Make sure old OSD does not stay around,
// e.g. with -fixed-vo and same-resolution files
//clear_osd_msgs();
//update_osd_msg();

//================ SETUP AUDIO ==========================

if(mpctx->sh_audio){
  reinit_audio_chain();
  if (mpctx->sh_audio && mpctx->sh_audio->codec)
    mp_msg(MSGT_IDENTIFY,MSGL_INFO, "ID_AUDIO_CODEC=%s\n", mpctx->sh_audio->codec->name);
}
if(mpctx->sh_audio)
{	
	//audio_delay += (float)(AUDIO_BUFF_SIZE -  MAX_OUTBURST - MAX_OUTBURST / 2) / (float)mpctx->sh_audio->o_bps;
	//audio_delay = (float)(ao_data.buffersize) / (float)mpctx->sh_audio->o_bps;
	//F("audio_delay = %d\n",(int)(audio_delay * 1000.0));
}
	current_module="av_init";

if(mpctx->sh_video){
  mpctx->sh_video->timer=0;
  if (! ignore_start)
    audio_delay += mpctx->sh_video->stream_delay;
  F("audio_delay = %d\n",(int)(audio_delay * 1000.0));  
}
if(mpctx->sh_audio){
  if (! ignore_start)
    audio_delay -= mpctx->sh_audio->stream_delay;
    
  mpctx->delay=-audio_delay;
  F("audio_delay = %d\n",(int)(audio_delay * 1000.0));
}

if(!mpctx->sh_audio){
  mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_NoSound);
  mp_msg(MSGT_CPLAYER,MSGL_V,"Freeing %d unused audio chunks.\n",mpctx->d_audio->packs);
  ds_free_packs(mpctx->d_audio); // free buffered chunks
  //mpctx->d_audio->id=-2;         // do not read audio chunks
  //uninit_player(INITED_AO); // close device
}
if(!mpctx->sh_video){
   mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_Video_NoVideo);
   mp_msg(MSGT_CPLAYER,MSGL_V,"Freeing %d unused video chunks.\n",mpctx->d_video->packs);
   ds_free_packs(mpctx->d_video);
   mpctx->d_video->id=-2;
   //if(!fixed_vo) uninit_player(INITED_VO);
}

if (!mpctx->sh_video && !mpctx->sh_audio)
    goto goto_next_file;

//if(demuxer->file_format!=DEMUXER_TYPE_AVI) pts_from_bps=0; // it must be 0 for mpeg/asf!
if(force_fps && mpctx->sh_video){
  vo_fps = mpctx->sh_video->fps=force_fps;
  mpctx->sh_video->frametime=1.0f/mpctx->sh_video->fps;
  mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_FPSforced,mpctx->sh_video->fps,mpctx->sh_video->frametime);
}
//==================== START PLAYING =======================

if(mpctx->loop_times>1) mpctx->loop_times--; else
if(mpctx->loop_times==1) mpctx->loop_times = -1;

mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_StartPlaying);

total_time_usage_start=GetTimer();
audio_time_usage=0; video_time_usage=0; vout_time_usage=0;
total_frame_cnt=0; drop_frame_cnt=0; // fix for multifile fps benchmark
play_n_frames=play_n_frames_mf;

if(g_StartPlay)
{
	g_StartPlay();
}

if(play_n_frames==0){
  mpctx->eof=PT_NEXT_ENTRY; goto goto_next_file;
}
F("a1\n");
if (seek_to_sec) {
    seek(mpctx, seek_to_sec, 0);
    end_at.pos += seek_to_sec;
}

if (end_at.type == END_AT_SIZE) {
    mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_MPEndposNoSizeBased);
    end_at.type = END_AT_NONE;
}

if(videoshow)
{	
	if(!get_display_is_direct())
		lcd_enter_frame();
}
GetRelativeTime();
while(!mpctx->eof){
    float aq_sleep_time=0;
  
if(!mpctx->sh_audio && mpctx->d_audio->sh) {
  mpctx->sh_audio = mpctx->d_audio->sh;
  mpctx->sh_audio->ds = mpctx->d_audio;
  F("Init audio!\n");
  reinit_audio_chain();
  
  
  }
  
  
  
/*========================== PLAY AUDIO ============================*/
if (mpctx->sh_audio)
    if (!fill_audio_out_buffers())
	// at eof, all audio at least written to ao
	if (!mpctx->sh_video)
	    mpctx->eof = PT_NEXT_ENTRY;


if(!mpctx->sh_video) {
  // handle audio-only case:

  double a_pos=0;
  if(!quiet || end_at.type == END_AT_TIME )
    a_pos = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
  if(!quiet)
    print_status(a_pos, 0, 0);

  if(end_at.type == END_AT_TIME && end_at.pos < a_pos)
    mpctx->eof = PT_NEXT_ENTRY;

} else {

/*========================== PLAY VIDEO ============================*/

  vo_pts=mpctx->sh_video->timer*90000.0;
  vo_fps=mpctx->sh_video->fps;

  if (!mpctx->num_buffered_frames) {
      double frame_time = update_video(&blit_frame);
      
    mp_dbg(MSGT_AVSYNC,MSGL_DBG2,"*** ftime=%5.3f ***\n",frame_time);
      if (mpctx->sh_video->vf_inited < 0) {
	mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_NotInitializeVOPorVO);
	  mpctx->eof = 1; goto goto_next_file;
    }
      if (frame_time < 0)
	  mpctx->eof = 1;
      else {
	  // might return with !eof && !blit_frame if !correct_pts
	  mpctx->num_buffered_frames += blit_frame;
	  time_frame += frame_time / playback_speed;  // for nosound
	  }  
      }

// ==========================================================================

    //current_module="vo_check_events";
    //if (vo_config_count) mpctx->video_out->check_events();
    frame_time_remaining = sleep_until_update(&time_frame, &aq_sleep_time);
	
//====================== FLIP PAGE (VIDEO BLT): =========================

        current_module="flip_page";
        if (!frame_time_remaining && blit_frame) {
	   unsigned int t2=GetTimer();

	   if(vo_config_count) mpctx->video_out->flip_page();
	   mpctx->num_buffered_frames--;

	   vout_time_usage += (GetTimer() - t2);
        }
//====================== A-V TIMESTAMP CORRECTION: =========================

  adjust_sync_and_print_status(frame_time_remaining, time_frame);

//============================ Auto QUALITY ============================

/*Output quality adjustments:*/
if(auto_quality>0){
  current_module="autoq";
//  float total=0.000001f * (GetTimer()-aq_total_time);
//  if(output_quality<auto_quality && aq_sleep_time>0.05f*total)
  if(output_quality<auto_quality && aq_sleep_time>0)
      ++output_quality;
  else
//  if(output_quality>0 && aq_sleep_time<-0.05f*total)
  if(output_quality>1 && aq_sleep_time<0)
      --output_quality;
  else
  if(output_quality>0 && aq_sleep_time<-0.050f) // 50ms
      output_quality=0;
//  printf("total: %8.6f  sleep: %8.6f  q: %d\n",(0.000001f*aq_total_time),aq_sleep_time,output_quality);
  set_video_quality(mpctx->sh_video,output_quality);
}

 if (play_n_frames >= 0 && !frame_time_remaining && blit_frame) {
     --play_n_frames;
     if (play_n_frames <= 0) mpctx->eof = PT_NEXT_ENTRY;
}


// FIXME: add size based support for -endpos
 if (end_at.type == END_AT_TIME &&
         !frame_time_remaining && end_at.pos <= mpctx->sh_video->pts)
     mpctx->eof = PT_NEXT_ENTRY;

} // end if(mpctx->sh_video)
//============================ Handle KEY ===============================

if(mp_mode)
{
  if(mpctx->sh_video)	
  	 Flush_OSDScrean();
  else if(mpctx->sh_audio)	
  	 usec_sleep(20000);
	
   	switch(mp_mode & 0xffff)
   	{   
   		case 1:
		{
			playback_speed = 2.0;	
			if (mpctx->sh_audio) {
				mpctx->audio_out->pause(); // stop audio, throwing away buffered data
				mpctx->sh_audio->a_buffer_len = 0;
				mpctx->sh_audio->a_out_buffer_len = 0;
				
		   }
			MP_MODE(0);
		}
	    break;
	    case 2:
		{
			playback_speed = 1.0;	
	
			if (mpctx->sh_audio) {
	    		F("---------resume \n");
	    		mpctx->audio_out->resume();		
	    		//mpctx->audio_out->reset(); // stop audio, throwing away buffered data
				wait_audio_empty();
				mpctx->sh_audio->a_buffer_len = 0;
				mpctx->sh_audio->a_out_buffer_len = 0;
				mpctx->audio_out->control(AOCONTROL_RESET_GRADED,0);		
	   		}
	   	if(mpctx->sh_video)
			{
				seek(mpctx,0.0,0);
				BUFF_SetPriority(1);
				usec_sleep(1000 * 300);
	   
			}
		MP_MODE(5);
	    }
	    break;
	    case 5:
	    	mp_mode++;
	    	break;
	    case 6:
	    {
	    	if (mpctx->sh_audio) {
	    		F("---------resume \n");
	    		mpctx->audio_out->control(AOCONTROL_RESET_GRADED,0);
	    		//mpctx->audio_out->resume();		
	    		//mpctx->audio_out->reset(); // stop audio, throwing away buffered data
						
	   		}
	   		MP_MODE(0);
	    	break;
	    }
        case 3:
		{
			//file read error;
			static unsigned int oldseektime = 0;
			float f = demuxer_get_current_time(mpctx->demuxer);
			unsigned int dseek = (unsigned int)(f * 1000.0);
			F("seek = %d \n",dseek);
			if(oldseektime != dseek)
				oldseektime = dseek;
			else
				oldseektime += 3000;	
			seek(mpctx,(float)(oldseektime + 3000)/1000.0f,1);
			MP_MODE(0);
		}
		break;		
		case 4:
		{
			seek(mpctx,curseektime,1);
			MP_MODE(2);
		}
		break;
		// vol+++++++++++++++++++++++
		case 0x10:
		{
			if(mpctx->audio_out && (mpctx->audio_out->control(AOCONTROL_GET_VOL_GRADED,0) == 2))
			{
				unsigned char err;
				os_SemaphoreSet(mp_sem_pause,0,&err);
				mpctx->osd_function = OSD_PAUSE;	
				MP_MODE(0);	
			}
		}
		break;
		case 0x11:
		{
			if(mpctx->audio_out && (mpctx->audio_out->control(AOCONTROL_GET_VOL_GRADED,0) == 2))
			{
				MP_MODE(0);	
			}
		}
		break;
		case 0x20:
		{
			if(mpctx->audio_out && (mpctx->audio_out->control(AOCONTROL_GET_VOL_GRADED,0) == 2))
			{
				mpctx->audio_out->control(AOCONTROL_RESET_GRADED,0);
				MP_MODE(0);
			}
		}
		break;
		//vol------------------------
	}
	static int a_time,b_time;
	switch (mp_mode & 0xffff0000)
	{
		case 0x10000:
			a_time = get_cur_play_time();
			mp_mode &= 0xffff;
			F("A--> %d\n",a_time);
			break;
		case 0x20000:
			b_time = get_cur_play_time();
			mp_mode &= 0xffff;
			mp_mode |= 0x30000;
			F("B--> %d\n",b_time);
		case 0x30000:
			if(get_cur_play_time() >= b_time)
			{
				seek(mpctx,(float)(a_time)/1000.0f,1);
				F(" Loop A %d-->B %d\n",a_time,b_time);
			}
			break;
		case 0x40000:
			mp_mode &= 0xffff;
			break;
	}
	//F("mp_mode = %d\n",mp_mode);
							
}	
//============================ Handle PAUSE ===============================

  current_module="pause";

  if (mpctx->osd_function == OSD_PAUSE) {
      pause_loop();
      mpctx->was_paused = 1;
      //printf(" = ======= mpctx->was_paused\n");
  }
#if 0
// handle -sstep
if(step_sec>0) {
	mpctx->osd_function=OSD_FFW;
	rel_seek_secs+=step_sec;
}

 edl_update(mpctx);
#endif
mpctx->was_paused = 0;
if(rel_seek_secs || abs_seek_pos){

   F(" =1=========== rel_seek_secs: %d\n", (unsigned int )rel_seek_secs);
  if (seek(mpctx, rel_seek_secs, abs_seek_pos) >= 0) {
        
	F(" =2=========== rel_seek_secs: %d\n", (unsigned int )rel_seek_secs);
 
#if 0 
        // Set OSD:
        
      if(!loop_seek){     	
	if( !edl_decision )
          set_osd_bar(0,"Position",0,100,demuxer_get_percent_pos(mpctx->demuxer));
      }
#endif
  }
  rel_seek_secs=0;
  abs_seek_pos=0;
  loop_seek=0;
  edl_decision = 0;
  os_SemaphorePost(mp_sem_seek);
}
  
} // while(!mpctx->eof)
 
SetMplayerAudioModeFun(0);
mp_mode = 0;
if(videoshow)
{
		 if(!get_display_is_direct())
				 lcd_reset_frame();
}
//else  if(mpctx->audio_out)
//	mpctx->audio_out->control(AOCONTROL_RESET_GRADED,0);
//mpctx->audio_out->resume();
if(!benchmark){
wait_audio_empty();	  
if(mpctx->audio_out) 
	mpctx->audio_out->pause();	
}

if(g_ExitPlay)
{
	g_ExitPlay();
	g_ExitPlay = NULL;
}  
mp_msg(MSGT_GLOBAL,MSGL_V,"EOF code: %d  \n",mpctx->eof);

#ifdef HAS_DVBIN_SUPPORT
if(mpctx->dvbin_reopen)
{
  mpctx->eof = 0;
  uninit_player(INITED_ALL-(INITED_GUI|INITED_STREAM|INITED_INPUT|INITED_GETCH2|(fixed_vo?INITED_VO:0)));
  cache_uninit(mpctx->stream);
  mpctx->dvbin_reopen = 0;
  goto goto_enable_cache;
}
#endif
}

goto_next_file:  // don't jump here after ao/vo/getch initialization!

mp_msg(MSGT_CPLAYER,MSGL_INFO,"\n");

#ifdef VERIFY_CRC32
#undef printf
   printf ("\n\n+++==+++ %s: filename: %s image_crc32 = 0x%08x +++==++\n\n\n", argv[2], argv[1], image_crc32);
#endif

#define mp_msg(mod,lev, fmt, args... )  printf( fmt, ## args )
if(benchmark){
  int64_t tot=video_time_usage+vout_time_usage+audio_time_usage;
  int64_t total_time_usage;
  total_time_usage_start=GetTimer()-total_time_usage_start;
  total_time_usage = total_time_usage_start;
  
  
  
  mp_msg(MSGT_CPLAYER,MSGL_INFO,"\nBENCHMARKs: VC:%d.%03ds VO:%d.%03ds A:%d.%03ds Sys:%d.%03ds = %d.%03ds\n",
	 (int)(video_time_usage/1000000), (int)(video_time_usage/1000 % 1000), 
	  (int)(vout_time_usage/1000000), (int)(vout_time_usage /1000 % 1000),
	  (int)(audio_time_usage/1000000), (int)(audio_time_usage/1000 % 1000),
	  (int)((total_time_usage-tot)/1000000), (int)((total_time_usage-tot)/1000 % 1000),
	 (int)(total_time_usage/1000000), (int)(total_time_usage/1000 % 1000));
  if(total_time_usage>0)
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARK%%: VC:%d.%03d%% VO:%d.%03d%% A:%d.%03d%% Sys:%d.%03d%% = %d.%03d%%\n",
	   (int)((100*video_time_usage)/total_time_usage), (int)(((100000*video_time_usage)/total_time_usage) % 1000),
	   (int)((100*vout_time_usage)/total_time_usage),  (int)(((100000*vout_time_usage)/total_time_usage) % 1000),
	   (int)((100*audio_time_usage/total_time_usage)), (int)(((100000*audio_time_usage)/total_time_usage) % 1000),
	   (int)((100 * (total_time_usage-tot))/total_time_usage),
	   (int)(((100000 * (total_time_usage-tot))/total_time_usage) % 1000),
	   100, 100);
#ifdef JZ47_OPT
  if(total_frame_cnt)
#else
  if(total_frame_cnt && frame_dropping)
#endif
		F("total_time_usage = %d\n",(int)total_time_usage);
if(mpctx->sh_video) {
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"BENCHMARKn: disp: %d (%d.%03d fps)  drop: %d (%d%%)  total: %d (%d.%03d fps)\n",
							total_frame_cnt-drop_frame_cnt,
							(int)((1000ll) * (total_frame_cnt-drop_frame_cnt)/(total_time_usage/1000ll)),
							(int)((1000ll * (1000ll) * (total_frame_cnt-drop_frame_cnt)/(total_time_usage/1000ll)) % 1000),
							drop_frame_cnt,
							(int)(100*drop_frame_cnt/total_frame_cnt),
							total_frame_cnt,
							(int)((1000000)* (total_frame_cnt-0)/total_time_usage),
							(int)((1000 * (1000)* (total_frame_cnt-0)/(total_time_usage/1000)) % 1000));
}
#ifdef JZ47_ANL_TIME
   {
     int i;
     for (i = 0; i < ANL_CNT; i++)
     {
        unsigned int sec,msec;
        msec = anltime[i] / 1000;
        sec = msec / 1000;
        msec = msec % 1000;
        printf ("+++ anl-%02d: time=%d.%03ds ++++\n", i, sec, msec);
     }
   }
#endif
  
}
#undef mp_msg


// time to uninit all, except global stuff:
uninit_player(INITED_ALL-(INITED_GUI+INITED_INPUT+(fixed_vo?INITED_VO:0)));

  if ( mpctx->set_of_sub_size > 0 ) 
   {
    current_module="sub_free";
    for (i = 0; i < mpctx->set_of_sub_size; ++i) {
        sub_free( mpctx->set_of_subtitles[i] );
#ifdef USE_ASS
        if ( mpctx->set_of_ass_tracks[i] )
            ass_free_track( mpctx->set_of_ass_tracks[i] );
#endif
    }
    mpctx->set_of_sub_size = 0;
   }
    vo_sub_last = vo_sub=NULL;
    subdata=NULL;
#ifdef USE_ASS
    ass_track = NULL;
#endif

#ifdef USE_16M_SDRAM
mpctx->eof = 1;
#else
if(mpctx->eof == PT_NEXT_ENTRY || mpctx->eof == PT_PREV_ENTRY) {
  mpctx->eof = mpctx->eof == PT_NEXT_ENTRY ? 1 : -1;
  if(play_tree_iter_step(mpctx->playtree_iter,mpctx->play_tree_step,0) == PLAY_TREE_ITER_ENTRY) {
    mpctx->eof = 1;
  } else {
    play_tree_iter_free(mpctx->playtree_iter);
    mpctx->playtree_iter = NULL;
  }
  mpctx->play_tree_step = 1;
} else if (mpctx->eof == PT_UP_NEXT || mpctx->eof == PT_UP_PREV) {
  mpctx->eof = mpctx->eof == PT_UP_NEXT ? 1 : -1;
  if ( mpctx->playtree_iter ) {
    if(play_tree_iter_up_step(mpctx->playtree_iter,mpctx->eof,0) == PLAY_TREE_ITER_ENTRY) {
     mpctx->eof = 1;
    } else {
      play_tree_iter_free(mpctx->playtree_iter);
      mpctx->playtree_iter = NULL;
    }
   }
} else { // NEXT PREV SRC
     mpctx->eof = mpctx->eof == PT_PREV_SRC ? -1 : 1;
}

if(mpctx->eof == 0) mpctx->eof = 1;
  
while(mpctx->playtree_iter != NULL) {
  filename = play_tree_iter_get_file(mpctx->playtree_iter,mpctx->eof);
  if(filename == NULL) {
    if( play_tree_iter_step(mpctx->playtree_iter,mpctx->eof,0) != PLAY_TREE_ITER_ENTRY) {
      play_tree_iter_free(mpctx->playtree_iter);
      mpctx->playtree_iter = NULL;
    };
  } else
    break;
}
#endif

#ifdef HAVE_NEW_GUI
 if( use_gui && !mpctx->playtree_iter ) 
  {
#ifdef USE_DVDREAD
   if ( !guiIntfStruct.DiskChanged ) 
#endif
   mplEnd();
  }	
#endif

if(use_gui || mpctx->playtree_iter != NULL || player_idle_mode){
  if (!mpctx->playtree_iter) filename = NULL;
  mpctx->eof = 0;
  goto play_next_file;
}
exit_player_with_rc(MSGTR_Exit_eof, 0);
return 1;
}

#ifdef USE_16M_SDRAM


struct AV_PLUGIN_TBL *ffmpeg_plugin_search_table = NULL;

struct AV_PLUGIN_TBL 
*search_ffmpeg_plugin (char *dll_name)
{
  struct AV_PLUGIN_TBL *p;
  p = ffmpeg_plugin_search_table;
  if (!p)
    return NULL;
  while (p->name)
  {
    if (!strcmp(dll_name, p->name))
      break;
    p++;
  }
  return p;
}

void *load_plugin (char *drv_name, char *dll_name, char video_plg, unsigned int arg0)
{
  struct AV_PLUGIN_TBL *p; FILE *fp;
  int buf_len, size;
  char *buf, *progaddr, *filename, *prefix;
  char *vd_prefix="mpcodecs_vd_", *ad_prefix="mpcodecs_ad_";
  void* (*func)(unsigned int);
  void *ret;

  printf ("++++++ drv_name = %s, dll_name = %s +++++++++\n", drv_name, dll_name);

#if 0
  if (video_plg)   // video plugin
    progaddr = (void *)(VD_PLUGIN_ADDR);
  else
    progaddr = (void *)(AD_PLUGIN_ADDR);
#endif

  if (!strcmp("ffmpeg", drv_name))
  {
    p = search_ffmpeg_plugin (dll_name);
    if (!p || !p->plugin_name)
    {
      printf ("+++++++ Not found plugin for %s(%s) ++++++++\n", drv_name, dll_name);
      return NULL;
    }
    filename = p->plugin_name;
    prefix = "";
  }
  else if (!strcmp("table", drv_name))
  {
    filename = dll_name;
    prefix = "";
  }
  else
  {
      filename = drv_name;
      prefix = (video_plg) ? vd_prefix : ad_prefix;
  }

// open plugin mpcodes_vd/ad_...
#if 1
    buf_len = strlen (prefix) + strlen(filename) + 8;
#else
    buf_len = strlen("nfl:\\SYSTEM\\") + strlen (prefix) + strlen(filename) + 8;
#endif
    buf = malloc(buf_len);
    if (!buf)
    {
       printf ("++++++++ ERR: alloc memory failed(%s): size = %d  +++++++++\n", filename, buf_len);
       return NULL; //while (1);
    }
#if 1
    snprintf(buf, buf_len, "%s%s.bin", prefix, filename);
#else
    snprintf(buf, buf_len, "nfl:\\SYSTEM\\%s%s", prefix, filename);
#endif

#if 1
    ret = (void *)Load_Program(buf, (void *)arg0, NULL);
    free(buf);
    return ret;
#else
    fp = fopen (buf, "rb");
    if (fp == NULL)
    {
      printf ("++++++++ ERR: can not open file: %s  +++++++++\n", buf);
      free(buf);
      return; // while (1);
    }
    free(buf);
// load the plugin
    size = fread (progaddr, 1, fp->size, fp);
    __dcache_writeback_all();
    __icache_invalidate_all();

// execute the plugin start function
    func = (void *)progaddr;
    return func (arg0);   // init
#endif
}
//    printf("ptr1 = %s, ptr2 = %s\n", ptr1, ptr2); \

#define STR_DUP(ptr1, ptr2)               \
do {                                      \
  failed = (failed << 1);                 \
  if (ptr2 != NULL)                       \
  {                                       \
    ptr1 = malloc (strlen (ptr2) + 4);    \
    if (ptr1) strcpy (ptr1, ptr2);        \
    else failed = (failed | 1);           \
  }                                       \
  else                                    \
    ptr1 = NULL;                          \
} while (0)

codecs_t *copy_codecs_script (codecs_t *avcodec)
{
  codecs_t *p;
  char *buf, failed = 0;
  
  p = malloc (sizeof(codecs_t));
  if (!p)
  {
    printf("+++ Not enough memory for alloc when find codecs !! ++\n");
    return NULL;
  }

  *p = *(avcodec);
  STR_DUP (p->name,  avcodec->name);
  STR_DUP (p->info,  avcodec->info);
  STR_DUP (p->comment,  avcodec->comment);
  STR_DUP (p->dll,  avcodec->dll);
  STR_DUP (p->drv,  avcodec->drv);
  if (failed)
    printf ("+++ Not enough memory for alloc when find codecs (0x%x) !! ++\n", failed);

  return p;
}

fakes_run ()
{
  Load_ProgramBin ();
}


char * mixer_device=NULL;
char * mixer_channel=NULL;
int soft_vol = 0;
float soft_vol_max = 110.0;

#endif

