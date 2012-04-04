#include "mplayer_noahos.h"

extern Jz47_AV_Decoder *jz47_av_decp;
extern void  IpuOutmodeChanged(int x,int y,int w, int h);
extern int ipu_image_completed;
extern char* noah_get_file_name();
extern unsigned int mp_memory_empty;

static MEDIALIB_STATUS PlayerStatus;
static int parameter_num;
static char* parameter_buf[2];
static float time_frame=0; // Timer
static int frame_time_remaining=0; // flag
static int blit_frame=0;
static char **options = 0;
static int bOpenForInfo = 0;
extern DWORD TimerCount(void);

////////////////////////////////////////////////////
// 功能: 初始化MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int InitMplayerLib()
{
	parameter_num = 2;
	parameter_buf[0] = "";
	parameter_buf[1] = noah_get_file_name();
	kprintf("mplayer file = %s\n",parameter_buf[1]);
	PlayerStatus = MEDIALIB_INIT;

#if EXCEPTION_MPLAYER
	mp_excptid = AddExcept(mplayer_excpt,saveregister);
#endif
#ifdef JZ4740_MXU_OPT
	mxucr = enable_jz4740_mxu ();
#endif

#ifdef JZ4755_OPT
	REG32(CPM_CLKGR_VADDR) &= ~(IDCT_CLOCK | DBLK_CLOCK | MC_CLOCK | IPU_CLOCK | AHB1_CLOCK | AUX_CLOCK);
#else
#ifdef JZ4750_OPT
	REG32(CPM_CLKGR_VADDR) &= ~(IDCT_CLOCK);
	REG32(CPM_CLKGR_VADDR) &= ~(DBLK_CLOCK);
	REG32(CPM_CLKGR_VADDR) &= ~(ME_CLOCK);
	REG32(CPM_CLKGR_VADDR) &= ~(MC_CLOCK);
	REG32(CPM_CLKGR_VADDR) &= ~(IPU_CLOCK);
    {
        /* dblk init */
        volatile unsigned * dblk_bug_fmt=0xb30b0004;
        volatile unsigned * dblk_bug_ctrl=0xb30b0000;
        *dblk_bug_fmt=0x0;
        *dblk_bug_ctrl=0x0;
    } 
#endif
#endif

    // movie info:
    //float a_frame=0;    // Audio

	int i;

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
  
	for(i=1; i<parameter_num; i++)
		if(!strcmp(parameter_buf[i], "-really-quiet"))
			verbose= -10;

#ifdef USE_TV
	stream_tv_defaults.immediate = 1;
#endif

	ipu_image_start();
	mplayer_showmode(1);

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
	/// Catch signals
#ifndef __MINGW32__
    //  signal(SIGCHLD,child_sighandler);
#endif

#ifdef CRASH_DEBUG
	prog_path = parameter_buf[0];
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
	if(videoshow == 0)
	{
  	
		options = &audiofile;
	}	
	ao_data.buffersize = -1;

	return OPEN_SUCCESS_ERR;
}


////////////////////////////////////////////////////
// 功能: 打开MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int IsOpenForInfo(void)
{
	return bOpenForInfo;
}


////////////////////////////////////////////////////
// 功能: 打开MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int OpenMplayerLib(int info)
{
	mpctx->eof = 0;

//	kprintf("open mplayer lib start: %d\n",TimerCount());

	bOpenForInfo = info;
	// init global sub numbers
	mpctx->global_sub_size = 0;
	{ int i; for (i = 0; i < SUB_SOURCES; i++) mpctx->global_sub_indices[i] = -1; }
    //============ Open & Sync STREAM --- fork cache2 ====================
	filename = parameter_buf[1];
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
    if(!mpctx->stream) 
	{ // error...
		mpctx->eof = 1;
		return STREAM_OPEN_ERR;
	}
//	kprintf("open mplayer lib open stream: %d\n",TimerCount());	
	
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
		return STREAM_OPEN_ERR;
#endif
	}
	mpctx->stream->start_pos+=seek_to_byte;

    // CACHE2: initial prefill: 20%  later: 5%  (should be set by -cacheopts)
#ifdef HAS_DVBIN_SUPPORT
goto_enable_cache:
#endif
    //============ Open DEMUXERS --- DETECT file type =======================
  if(options) cur_demuxermode = 1;  
	current_module="demux_open";
	F("demux_open = %d",TimerCount());
	mpctx->demuxer=demux_open(mpctx->stream,mpctx->file_format,audio_id,video_id,dvdsub_id,filename);
	F("demux_open end = %d",TimerCount());

//	kprintf("open mplayer lib demux open: %d\n",TimerCount());
	if(!mpctx->demuxer) 
		return DEMUXER_OPEN_ERR;
	inited_flags|=INITED_DEMUXER;
#ifndef NOAH_OS
	if((demux_control(mpctx->demuxer,DEMUXER_CTRL_IS_SINGLE_SUPPORT,NULL) == DEMUXER_CTRL_OK) && (options == 0))
	{
		printf("single file support!\n");
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
		return DEMUXER_CONTROL_ERR;
  
	}	
#endif	
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
		if(mpctx->sh_video)
		{
			if(mpctx->sh_video->disp_w * mpctx->sh_video->disp_h > 1024 * 576)
				rv9_skip_dblk = 1;
			jz47_auto_select(mpctx->sh_video->disp_w,mpctx->sh_video->disp_h);
			
		}else if(videoshow) jz47_auto_select(0,0);
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
	if(!mpctx->sh_video && !mpctx->sh_audio)
	{
		mp_msg(MSGT_CPLAYER,MSGL_FATAL, MSGTR_NoStreamFound);
		return AUDIO_VIDEO_OPEN_ERR; // exit_player(MSGTR_Exit_error);
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
	if(!mpctx->sh_video) 
		goto OPEN_MEDIALIB_END;
//		return OPEN_SUCCESS_ERR; // audio-only

	if(!reinit_video_chain()) 
	{
		if(!mpctx->sh_video)
		{
			if(!mpctx->sh_audio) 
				return AUDIO_VIDEO_OPEN_ERR;
			goto OPEN_MEDIALIB_END;
//			return OPEN_SUCCESS_ERR; // exit_player(MSGTR_Exit_error);
		}
	}

	if(vo_flags & 0x08 && vo_spudec)
		spudec_set_hw_spu(vo_spudec,mpctx->video_out);

#ifdef HAVE_FREETYPE
	force_load_font = 1;
#endif
OPEN_MEDIALIB_END:
	current_module="main";
	F("44\n");

	// Disable the term OSD in verbose mode
	if(verbose) 
		term_osd = 0;

	mpctx->num_buffered_frames=0;

	// Make sure old OSD does not stay around,
	// e.g. with -fixed-vo and same-resolution files
	//clear_osd_msgs();
	//update_osd_msg();

	time_frame=0; // Timer
	frame_time_remaining=0; // flag
	blit_frame=0;
	//================ SETUP AUDIO ==========================

	if(mpctx->sh_audio)
	{
		reinit_audio_chain();
		if (mpctx->sh_audio && mpctx->sh_audio->codec)
			mp_msg(MSGT_IDENTIFY,MSGL_INFO, "ID_AUDIO_CODEC=%s\n", mpctx->sh_audio->codec->name);
	}
//	kprintf("open mplayer lib end: %d\n",TimerCount());	
	return OPEN_SUCCESS_ERR;
}

////////////////////////////////////////////////////
// 功能: 开始播放MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int PLayMplayerLib()
{
//	kprintf("play mplayer lib start: %d\n",TimerCount());

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
		return AUDIO_VIDEO_OPEN_ERR;

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

    struct codecs_info_x cur_codec;
    
    if(g_GetCodecInfo){
        memset(&cur_codec,0,sizeof(struct codecs_info_x));
        
        if(mpctx->sh_audio)
        {
            if(mpctx->sh_audio->codec){
                cur_codec.ad_name = mpctx->sh_audio->codec->name;
                if((mpctx->sh_audio->codec->drv) && strcmp(mpctx->sh_audio->codec->drv,"ffmpeg") != 0)
                    cur_codec.ad_drv = mpctx->sh_audio->codec->drv;
                else if(mpctx->sh_audio->codec->dll)
                    cur_codec.ad_drv = mpctx->sh_audio->codec->dll;
                
            }   
        }
         if(mpctx->sh_video)
        {
            if(mpctx->sh_video->codec){
                cur_codec.vd_name = mpctx->sh_video->codec->name;
                if((mpctx->sh_video->codec->drv) && strcmp(mpctx->sh_video->codec->drv,"ffmpeg") != 0)
                    cur_codec.vd_drv = mpctx->sh_video->codec->drv;
                else if(mpctx->sh_video->codec->dll)
                    cur_codec.vd_drv = mpctx->sh_video->codec->dll;
                
            }
            cur_codec.width = mpctx->sh_video->disp_w;
            cur_codec.height = mpctx->sh_video->disp_h;
        }
        exit_player_mode = g_GetCodecInfo(&cur_codec);
        if(exit_player_mode == -1)
		{
            exit_player_mode = 0;
            return CODEC_INFO_ERR;   
        }
        exit_player_mode = 1;
        
        //#undef printf
        //F("displayer = %d\n",displayer);
        
    }
// 	if(g_StartPlay)
// 	{
// 		g_StartPlay();
// 	}

	if(play_n_frames==0)
	{
		mpctx->eof=PT_NEXT_ENTRY; 
		return VIDEO_FRAMES_ERR;
	}
    //#undef printf
	F("a1\n");
    F("seek2pos = %d\n",seek2pos);
    
	if (end_at.type == END_AT_SIZE) {
		mp_msg(MSGT_CPLAYER, MSGL_WARN, MSGTR_MPEndposNoSizeBased);
		end_at.type = END_AT_NONE;
	}
#ifndef NOAH_OS	
	if(videoshow)
	{	
		if(!get_display_is_direct())
			lcd_enter_frame();
	}
	if(IS_AUDIOLEVELCTRL())
	{
			int d = 1;
			kernel_ioctl(&d,KERNEL_PM_SETAUDIOCONTROL);
	}
#endif		
	GetRelativeTime();
    #if USE_CLOSEKERNEL
    if(videoshow)
        kernel_ioctl(NULL,KERNEL_STOP_SCHEDULE);
    #endif
        
#ifndef NOAH_OS        
    if(videoshow && mpctx->sh_video && mpctx->sh_audio)
    	audiostk = CreateAudioThread(1024 * 512);
#endif    
	if(mp_memory_empty )
	{
		mpctx->eof = 1;
		PlayerStatus = MEDIALIB_ERR;
		return VIDEO_FRAMES_ERR;
	}
	
	PlayerStatus = MEDIALIB_PLAYING;
//	kprintf("play mplayer lib end: %d\n",TimerCount());
	return OPEN_SUCCESS_ERR;
}

////////////////////////////////////////////////////
// 功能: 得到PCM数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetMplayerDataLib()
{
	if(mpctx->eof)
	{
		PlayerStatus = MEDIALIB_END;
		return PLAY_FINISH_ERR;
	}
//	ipu_image_completed = 1;
	
#if MP_STATISTIC
		static int start_frame_time = 0;
		int stat_tmp;
		stat_tmp = GetTimer();
		if(start_frame_time)
		{
			//printf("stat_tmp = %d start_frame_time = %d\n",stat_tmp,start_frame_time);
			if(stat_tmp - start_frame_time > 0)
			{
				av_total_time += (stat_tmp - start_frame_time); 
				av_total_count++;
			}
		}
		start_frame_time = stat_tmp;
		total_dec_count++;
#endif
	if(!mpctx->sh_audio && mpctx->d_audio->sh && (seekmode == 0)) 
	{
		mpctx->sh_audio = mpctx->d_audio->sh;
		mpctx->sh_audio->ds = mpctx->d_audio;
		F("Init audio!\n");
		reinit_audio_chain();
        if(seek2pos)
            seekmode = 1;
	}
//			printf("videoshow = %d\n",videoshow);
/*========================== PLAY AUDIO ============================*/
	//printf("mpctx->sh_audio = %x\n",mpctx->sh_audio);
	if (mpctx->sh_audio && (seekmode == 0))
	{	
		//if (!fill_audio_out_buffers())
		int fillcount = 0;
#ifndef NOAH_OS				
		if(audiostk)
			fillcount = fill_audiobuffers(0);
		else
			fillcount = fill_audio_out_buffers_foraudio();
#else
			fillcount = fill_audiobuffers(0);
#endif
			
		// at eof, all audio at least written to ao
		if (!fillcount)
		{
			mpctx->eof = 1;
			PlayerStatus = MEDIALIB_END;
			return PLAY_FINISH_ERR;
		}
	}
	if(!mpctx->sh_video) 
	{
		// handle audio-only case:
		double a_pos=0;
		if(!quiet || end_at.type == END_AT_TIME )
			a_pos = playing_audio_pts(mpctx->sh_audio, mpctx->d_audio, mpctx->audio_out);
		if(!quiet)
			print_status(a_pos, 0, 0);

		if(end_at.type == END_AT_TIME && end_at.pos < a_pos)
			mpctx->eof = PT_NEXT_ENTRY;
		if(videoshow) Flush_OSDScrean();

	} 
	else 
	{
	} // end if(mpctx->sh_video)
    //============================ Handle KEY ===============================
    if(seek_start)
    {
        if(seek_start > 2)
        {
            seek_start = 0;
            unsigned char err;
#ifndef NOAH_OS                    
            os_SemaphoreSet(mp_sem_seek,0,&err);
            os_SemaphorePost(mp_sem_seek);
#endif                    
            usec_sleep(20000);    
        }else
            seek_start++;
        
    }
    
//	F("seekmode = %d\n",seekmode);
	if(mp_mode)
	{

		if(mpctx->sh_audio)	
			usec_sleep(10000);
		F("mp_mode = %d\n",mp_mode);
					
	}	
    //============================ Handle PAUSE ===============================

	current_module="pause";

	if (mpctx->osd_function == OSD_PAUSE) 
	{
		pause_loop();
		mpctx->was_paused = 1;
		//printf(" = ======= mpctx->was_paused\n");
	}

	mpctx->was_paused = 0;
	int *m_abs_seek_pos = 0;
	m_abs_seek_pos = &abs_seek_pos;
	if((*m_abs_seek_pos) && seek_start)
	{                 
    	rel_seek_secs = rel_seek_secs_ms / 1000.0;
		if (seek(mpctx, rel_seek_secs, abs_seek_pos) == -1) 
		{
			seek_error = 1;
			seek_start = 0;
			unsigned char err;
#ifndef NOAH_OS            
			os_SemaphoreSet(mp_sem_seek,0,&err);
			os_SemaphorePost(mp_sem_seek);
#endif       
		}else
			seek_error = 0;
		old_rel_seek_secs = rel_seek_secs;
		old_abs_seek_pos = abs_seek_pos;
		rel_seek_secs=0;
		abs_seek_pos=0;
		rel_seek_secs_ms = 0;
		edl_decision = 0;
	}

	if(mp_memory_empty )
	{
		mpctx->eof = 1;
		PlayerStatus = MEDIALIB_ERR;
		return MALLOC_ERROR;
	}

	return OPEN_SUCCESS_ERR;
}

////////////////////////////////////////////////////
// 功能: 关闭MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetMplayerVideoDataLib()
{
	float aq_sleep_time=0;
	double frame_time;

	if(mpctx->eof)
	{
		PlayerStatus = MEDIALIB_END;
		return PLAY_FINISH_ERR;
	}
/*	
	if (mpctx->sh_video->next_frame_time > 0.001 && mpctx->audio_out->get_delay() <= mpctx->sh_video->next_frame_time)
		return (int)(mpctx->sh_video->pts*1000);
*/
    /*========================== PLAY VIDEO ============================*/
	vo_pts=mpctx->sh_video->timer*90000.0;
	vo_fps=mpctx->sh_video->fps;

//	kprintf("mpctx->num_buffered_frames = %d\n",mpctx->num_buffered_frames);
	if (!mpctx->num_buffered_frames) 
	{
		if(seekmode)
        {
#ifndef NOAH_OS                    	
            sync_audio_thread();
            if(audiostk)
              OP_AU_LOCK();                          
            frame_time = seek_update_video(&blit_frame);
            if(audiostk)
              OP_AU_UNLOCK();
#else
            frame_time = seek_update_video(&blit_frame);
#endif                          
        }
		else	
			frame_time = update_video(&blit_frame);
		
		#if MP_STATISTIC
		if(av_v_dec_count > stat_count)
		{
			mpctx->eof = 1;
		}	
		#endif
		mp_dbg(MSGT_AVSYNC,MSGL_DBG2,"*** ftime=%5.3f ***\n",frame_time);
		if (mpctx->sh_video->vf_inited < 0) 
		{
			mp_msg(MSGT_CPLAYER,MSGL_FATAL,MSGTR_NotInitializeVOPorVO);
			mpctx->eof = 1; 
			PlayerStatus = MEDIALIB_ERR;
			return PLAY_FINISH_ERR;
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
	if(seekmode == 0)
	{
	//current_module="vo_check_events";
	//if (vo_config_count) mpctx->video_out->check_events();
//	kprintf("time_frame = %f aq_sleep_time = %f\n",time_frame,aq_sleep_time);
	frame_time_remaining = sleep_until_update(&time_frame, &aq_sleep_time);
	
    //====================== FLIP PAGE (VIDEO BLT): =========================
#if 1
	current_module="flip_page";
//	kprintf("frame_time_remaining = %d blit_frame = %d\n",frame_time_remaining,blit_frame);
	if (!frame_time_remaining && blit_frame) {
		unsigned int t2=GetTimer();

		if(vo_config_count) 
		{
			mpctx->video_out->flip_page();
		}
		mpctx->num_buffered_frames--;

		vout_time_usage += (GetTimer() - t2);
	}
#endif
    //====================== A-V TIMESTAMP CORRECTION: =========================

	adjust_sync_and_print_status(frame_time_remaining, time_frame);

	//============================ Auto QUALITY ============================

    //Output quality adjustments:
	if(auto_quality>0)
	{
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

	if (play_n_frames >= 0 && !frame_time_remaining && blit_frame)
	{
		--play_n_frames;
		if (play_n_frames <= 0)
			mpctx->eof = PT_NEXT_ENTRY;
	}


    // FIXME: add size based support for -endpos
	if (end_at.type == END_AT_TIME && !frame_time_remaining && end_at.pos <= mpctx->sh_video->pts)
		mpctx->eof = PT_NEXT_ENTRY;
	}

	return (int)(mpctx->sh_video->pts*1000);
}

////////////////////////////////////////////////////
// 功能: 关闭MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MplayerCloseLib()
{

#if DEBUG_FRAME_DECODE_TIME
	if(decode_total_frame != 0)
		kprintf("average_decode_time: %d\n", (decode_audio_time + decode_video_time) / decode_total_frame);
#endif

	mp_mode = 0;

	// time to uninit all, except global stuff:
	uninit_player(INITED_ALL-(INITED_GUI+INITED_INPUT+(fixed_vo?INITED_VO:0)));

	vo_sub_last = vo_sub=NULL;
	subdata=NULL;
 	mpctx->eof = 1;
 	exit_player_with_rc(MSGTR_Exit_eof, 0);
	return OPEN_SUCCESS_ERR;
}
////////////////////////////////////////////////////
// 功能: 关闭MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MplayerGetInfo()
{
	if( !mpctx->sh_video && !mpctx->sh_audio )
		return AUDIO_VIDEO_OPEN_ERR;

	//得到AUDIO信息
	if( mpctx->sh_audio )
	{
		jz47_av_decp->MediaInfo.bHasAudio = 1;
		jz47_av_decp->MediaInfo.bAllowSeek = mpctx->demuxer->seekable;
		jz47_av_decp->MediaInfo.TotalTime = demuxer_get_time_length(mpctx->demuxer) * 1000;
		jz47_av_decp->MediaInfo.AudioBitrate = mpctx->sh_audio->i_bps;
		jz47_av_decp->MediaInfo.AudioSamplerate = mpctx->sh_audio->samplerate;
		jz47_av_decp->MediaInfo.AudioChannels = mpctx->sh_audio->channels;
		strcpy(jz47_av_decp->AudioCodec,mpctx->sh_audio->codec->name);
	}
	else
	{	//没有音频
		jz47_av_decp->MediaInfo.bHasAudio = 0;
		jz47_av_decp->MediaInfo.bAllowSeek = 0;
		jz47_av_decp->MediaInfo.TotalTime = 0;
		jz47_av_decp->MediaInfo.AudioBitrate = 0;
		jz47_av_decp->MediaInfo.AudioSamplerate = 0;
		jz47_av_decp->MediaInfo.AudioChannels = 0;
		strcpy(jz47_av_decp->AudioCodec,"NULL");
	}

	//得到VIDEO信息
	if( mpctx->sh_video )
	{
		jz47_av_decp->MediaInfo.bHasVideo = 1;
		jz47_av_decp->MediaInfo.VideoWidth = mpctx->sh_video->disp_w;
		jz47_av_decp->MediaInfo.VideoHeight = mpctx->sh_video->disp_h;
		jz47_av_decp->MediaInfo.VideoFps = mpctx->sh_video->fps;
		jz47_av_decp->MediaInfo.VideoBitrate = mpctx->sh_video->i_bps;
		strcpy(jz47_av_decp->VideoCodec,mpctx->sh_video->codec->name);
	}
	else
	{	//没有视频
		jz47_av_decp->MediaInfo.bHasVideo = 0;
		jz47_av_decp->MediaInfo.VideoWidth = 0;
		jz47_av_decp->MediaInfo.VideoHeight = 0;
		jz47_av_decp->MediaInfo.VideoFps = 0;
		jz47_av_decp->MediaInfo.VideoBitrate = 0;
		strcpy(jz47_av_decp->VideoCodec,"NULL");
	}

	return 0;
}

////////////////////////////////////////////////////
// 功能: 关闭MPLAYER库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void noah_set_lcd_size(int x, int y, int w, int h)
{
	jz47_av_decp->UsrLcdPosX = x;
	jz47_av_decp->UsrLcdPosY = y;
	jz47_av_decp->UsrLcdWidth = w;
	jz47_av_decp->UsrLcdHeight = h;
	IpuOutmodeChanged(jz47_av_decp->UsrLcdPosX,jz47_av_decp->UsrLcdPosY,
		jz47_av_decp->UsrLcdWidth,jz47_av_decp->UsrLcdHeight);
}
////////////////////////////////////////////////////
// 功能: 得到当前的播放状态
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
MEDIALIB_STATUS GetMplayerStatus()
{
	return PlayerStatus;
}

////////////////////////////////////////////////////
// 功能: 暂停播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerError()
{
	PlayerStatus = MEDIALIB_ERR;
}


////////////////////////////////////////////////////
// 功能: 暂停播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerPause()
{
	PlayerStatus = MEDIALIB_PAUSE;
}

////////////////////////////////////////////////////
// 功能: 停止播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerStop()
{
	PlayerStatus = MEDIALIB_STOP;
}

////////////////////////////////////////////////////
// 功能: 重新播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerResume()
{
	GetRelativeTime();	// ignore time that passed during pause
	PlayerStatus = MEDIALIB_PLAYING;
}

////////////////////////////////////////////////////
// 功能: 设置播放完毕
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerEnd()
{
	PlayerStatus = MEDIALIB_END;
}

////////////////////////////////////////////////////
// 功能: seek播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMplayerSeek(int f)
{
// 	MEDIALIB_STATUS bak_status;
// 	bak_status   = PlayerStatus;

//	PlayerStatus = MEDIALIB_SEEK;
//	if( f < 0  || f > jz47_av_decp->MediaInfo.TotalTime )
//	kprintf("seek time = %d , total time = %d , current time = %d\n",f,jz47_av_decp->MediaInfo.TotalTime,get_cur_play_time());

	seek(mpctx, f / 1000.0f ,1);

	seek_start = 0;
	rel_seek_secs=0;
	abs_seek_pos=0;
	rel_seek_secs_ms = 0;
	GetRelativeTime();	// ignore time that passed during pause

//	kprintf("seek time end\n");
//	PlayerStatus = bak_status;
}

////////////////////////////////////////////////////
// 功能: seek播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetMplayerCurTime()
{
	return GetCurPlayingTime();
//	return get_cur_play_time();
}

