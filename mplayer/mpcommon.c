
#ifdef __LINUX__
#include <stdlib.h>
#else
#include <mplaylib.h>
#endif
#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
#include "mplayer.h"
#include "libvo/sub.h"
#include "libvo/video_out.h"
#include "spudec.h"
#include "vobsub.h"
#ifdef HAVE_TV_TELETEXT
#include "stream/tv.h"
#endif

double sub_last_pts = -303;

#ifdef USE_ASS
#include "libass/ass.h"
#include "libass/ass_mp.h"
ass_track_t* ass_track = 0; // current track to render
#endif

sub_data* subdata = NULL;
subtitle* vo_sub_last = NULL;

#ifndef __LINUX__
// This is for  SubTittle callback
typedef void (*PFUN_SUBTITLEOSD) (unsigned char *SubString, unsigned int StringLen, unsigned int StartTimer, unsigned int EndTimer);
static PFUN_SUBTITLEOSD SubTitleOsdFun = NULL;
void SetSubTitleOsd(PFUN_SUBTITLEOSD pfun)
{
	SubTitleOsdFun  = pfun;
}

#endif

void update_subtitles(sh_video_t *sh_video, demux_stream_t *d_dvdsub, int reset)
{
#ifndef __LINUX__

  unsigned char *packet=NULL;
  int len = ds_get_packet_sub(d_dvdsub, (unsigned char**)&packet);
  if (len <= 0 || len >= 128 || !SubTitleOsdFun) return;
  	   
  int  StartTimer =  d_dvdsub->current->pts    *  1000;
  int  EndTimer  =   d_dvdsub->current->endpts *  1000;
  char type = d_dvdsub->sh ? ((sh_sub_t *)d_dvdsub->sh)->type : 'v';
  if ( 't' == type )
  {	
	   SubTitleOsdFun((unsigned char *)packet, len, StartTimer, EndTimer);
  }
  
#else

    unsigned char *packet=NULL;
    int len;
    char type = d_dvdsub->sh ? ((sh_sub_t *)d_dvdsub->sh)->type : 'v';
    static subtitle subs;
    if (type == 'a')
#ifdef USE_ASS
      if (!ass_enabled)
#endif
      type = 't';
    if (reset) {
	sub_clear_text(&subs, MP_NOPTS_VALUE);
	if (vo_sub) {
	    vo_sub = NULL;
	    vo_osd_changed(OSDTYPE_SUBTITLE);
	}
	if (vo_spudec) {
	    spudec_reset(vo_spudec);
	    vo_osd_changed(OSDTYPE_SPU);
	}
    }
    // find sub
    if (subdata) {
	double pts = sh_video->pts;
	if (sub_fps==0) sub_fps = sh_video->fps;
	current_module = "find_sub";
	if (pts > sub_last_pts || pts < sub_last_pts-1.0) {
	    find_sub(subdata, (pts+sub_delay) *
		     (subdata->sub_uses_time ? 100. : sub_fps)); 
	    if (vo_sub) vo_sub_last = vo_sub;
	    // FIXME! frame counter...
	    sub_last_pts = pts;
	}
    }

    // DVD sub:
    if (vo_config_count && vo_spudec && type == 'v') {
	int timestamp;
	current_module = "spudec";
	spudec_heartbeat(vo_spudec, 90000*sh_video->timer);
	/* Get a sub packet from the DVD or a vobsub and make a timestamp
	 * relative to sh_video->timer */
	while(1) {
	    // Vobsub
	    len = 0;
	    if (vo_vobsub) {
		if (sh_video->pts+sub_delay >= 0) {
		    len = vobsub_get_packet(vo_vobsub, sh_video->pts+sub_delay,
					    (void**)&packet, &timestamp);
		    if (len > 0) {
			timestamp -= (sh_video->pts + sub_delay - sh_video->timer)*90000;
			mp_dbg(MSGT_CPLAYER,MSGL_V,"\rVOB sub: len=%d v_pts=%5.3f v_timer=%5.3f sub=%5.3f ts=%d \n",len,sh_video->pts,sh_video->timer,timestamp / 90000.0,timestamp);
		    }
		}
	    } else {
		// DVD sub
		len = ds_get_packet_sub(d_dvdsub, (unsigned char**)&packet);
		if (len > 0) {
		    // XXX This is wrong, sh_video->pts can be arbitrarily
		    // much behind demuxing position. Unfortunately using
		    // d_video->pts which would have been the simplest
		    // improvement doesn't work because mpeg specific hacks
		    // in video.c set d_video->pts to 0.
		    float x = d_dvdsub->pts - sh_video->pts;
		    if (x > -20 && x < 20) // prevent missing subs on pts reset
			timestamp = 90000*(sh_video->timer + d_dvdsub->pts
					   + sub_delay - sh_video->pts);
		    else timestamp = 90000*(sh_video->timer + sub_delay);
		    mp_dbg(MSGT_CPLAYER, MSGL_V, "\rDVD sub: len=%d  "
			   "v_pts=%5.3f  s_pts=%5.3f  ts=%d \n", len,
			   sh_video->pts, d_dvdsub->pts, timestamp);
		}
	    }
	    if (len<=0 || !packet) break;
	    if (timestamp >= 0)
		spudec_assemble(vo_spudec, packet, len, timestamp);
	}

	if (spudec_changed(vo_spudec))
	    vo_osd_changed(OSDTYPE_SPU);
    } else if (dvdsub_id >= 0 && type == 't') {
	double curpts = sh_video->pts + sub_delay;
	double endpts;
	vo_sub = &subs;
	while (d_dvdsub->first) {
	    double pts = ds_get_next_pts(d_dvdsub);
	    if (pts > curpts)
		break;
	    endpts = d_dvdsub->first->endpts;
	    len = ds_get_packet_sub(d_dvdsub, &packet);
#ifdef USE_ASS
	    if (ass_enabled) {
		static ass_track_t *global_ass_track = NULL;
		if (!global_ass_track) global_ass_track = ass_default_track(ass_library);
		ass_track = global_ass_track;
		vo_sub = NULL;
		if (pts != MP_NOPTS_VALUE) {
		    if (endpts == MP_NOPTS_VALUE) endpts = pts + 3;
		    sub_clear_text(&subs, MP_NOPTS_VALUE);
		    sub_add_text(&subs, packet, len, endpts);
		    subs.start = pts * 100;
		    subs.end = endpts * 100;
		    ass_process_subtitle(ass_track, &subs);
		}
	    } else
#endif
		if (pts != MP_NOPTS_VALUE) {
		    if (endpts == MP_NOPTS_VALUE)
			sub_clear_text(&subs, MP_NOPTS_VALUE);
		    sub_add_text(&subs, packet, len, endpts);
		    vo_osd_changed(OSDTYPE_SUBTITLE);
		}
	}
	if (sub_clear_text(&subs, curpts))
	    vo_osd_changed(OSDTYPE_SUBTITLE);
    }
    current_module=NULL;
#endif    
}

void update_teletext(sh_video_t *sh_video, demuxer_t *demuxer, int reset)
{
#ifdef HAVE_TV_TELETEXT
    tvi_handle_t* tvh=demuxer->priv;
    int page_changed;

    if (demuxer->type != DEMUXER_TYPE_TV || !tvh) return;

    //Also forcing page update when such ioctl is not supported or call error occured
    if(tvh->functions->control(tvh->priv,TV_VBI_CONTROL_IS_CHANGED,&page_changed)!=TVI_CONTROL_TRUE)
        page_changed=1;

    if(!page_changed)
        return;

    if(tvh->functions->control(tvh->priv,TV_VBI_CONTROL_GET_VBIPAGE,&vo_osd_teletext_page)!=TVI_CONTROL_TRUE)
        vo_osd_teletext_page=NULL;
    if(tvh->functions->control(tvh->priv,TV_VBI_CONTROL_GET_HALF_PAGE,&vo_osd_teletext_half)!=TVI_CONTROL_TRUE)
        vo_osd_teletext_half=0;
    if(tvh->functions->control(tvh->priv,TV_VBI_CONTROL_GET_MODE,&vo_osd_teletext_mode)!=TVI_CONTROL_TRUE)
        vo_osd_teletext_mode=0;
    if(tvh->functions->control(tvh->priv,TV_VBI_CONTROL_GET_FORMAT,&vo_osd_teletext_format)!=TVI_CONTROL_TRUE)
        vo_osd_teletext_format=0;
    vo_osd_changed(OSDTYPE_TELETEXT);

    tvh->functions->control(tvh->priv,TV_VBI_CONTROL_MARK_UNCHANGED,NULL);
#endif
}
