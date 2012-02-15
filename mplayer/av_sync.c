
#ifdef __LINUX__
#include "stdio.h"
#include "stdlib.h"
#else
#include "uclib.h"
#endif

#include "config.h"
#include "version.h"
#include "mp_msg.h"
#include "../stream/stream.h"
#include "../libmpdemux/demuxer.h"
#include "../libmpdemux/stheader.h"
#include "../libao2/audio_out.h"
#include "../libvo/video_out.h"
#include "../libvo/sub.h"

#include "playtree.h"
#include "playtreeparser.h"

#include "../libmpcodecs/dec_audio.h"
#include "../libmpcodecs/dec_video.h"
#include "../libmpcodecs/mp_image.h"
#include "../libmpcodecs/vf.h"
#include "../libmpcodecs/vd.h"

#include "mixer.h"

#include "mp_core.h"
#define av_delay_thred 0.10
extern int drop_frame_cnt;
static int dropframemode = 0;
extern int dropped_frames;
float demux_keyframe_time(demuxer_t *demuxer,demux_stream_t *ds);	    
int demux_skip_keyframe_time(demuxer_t *demuxer,demux_stream_t *ds,float time);
extern MPContext *mpctx;


//#undef printf
#ifdef __LINUX__
int AV_presync(float frame_time)
#else
int AV_presync_dym_length (float frame_time)
#endif
{
		if(mpctx->eof) return;
		sh_video_t * const sh_video = mpctx->sh_video;
		demux_stream_t *d_video=sh_video->ds;
		//if(d_video->packs == 0)
		//	ds_fill_buffer(d_video);
	  
	  float delay = playback_speed * mpctx->audio_out->get_delay();
	  float av_delay = mpctx->delay - delay;// - dropped_frames * frame_time;

    static int skip_mode = 0;
    static int skip_file = 0;
    if(d_video->current == 0) return 0;
    unsigned int result = (unsigned int)demux_current_is_keyframe(mpctx->demuxer,d_video);
    //printf("result = %d\n",result);
    if(result == -1)
			 return 0;
	if(result) // is keyframe
    {
    	skip_mode = 0;
    	dropframemode = 0;
    	skip_file = 0;
    	
    	//F("keyframe = true\n");
    }
    float f;
    if((av_delay > av_delay_thred) && (skip_mode == 0))
    {
			
	    f = demux_keyframe_time(mpctx->demuxer,d_video);
	   //	F("f = %f av_delay = %f %d\n",f,av_delay,rv9_skip_dblk);
	   	if(f < av_delay)
		{
				if(dropframemode == 0)
				{
					demux_skip_keyframe_time(mpctx->demuxer,d_video,av_delay);
				}else
				{	
    				//	//printf("f = %f av_delay = %f\n",f,av_delay);
    				//demux_skip_to_keyframe(mpctx->demuxer);
    				skip_mode = 1;
    			}
    	}
    	if((skip_file == 0) && (av_delay > 2.0))
    	{	
	    	float demux_buf_time(demuxer_t *demuxer,demux_stream_t *ds);
			float buftime = demux_buf_time(mpctx->demuxer,d_video);    	
	    	if(buftime > 0.0)
	    	{	
		    	f = f - buftime;
		    	if(f < av_delay - 0.4)
		    	{
		    		//printf("f = %f av_delay = %f\n",f,av_delay);
		    		demux_skip_to_keyframe(mpctx->demuxer);
		    		skip_file = 1;
		    	}
	    	}
	    	
    	}
    	if(d_video->bytes > 0x480000)
    	{
    		//kprintf("too many bytes = %d\n",d_video->bytes);
    		demux_skip_to_keyframe(mpctx->demuxer);	
    		//skip_mode = 1; 
    	}	
    		
    	dropframemode = 1;
    	
    	
    	
    }
    if (skip_mode == 1)
    {
    	dropped_frames++;
    	++drop_frame_cnt;
    }	
		return skip_mode;	
     
}

#ifndef __LINUX__
int (*AV_presync)(float frame_time) = AV_presync_dym_length;
#endif

