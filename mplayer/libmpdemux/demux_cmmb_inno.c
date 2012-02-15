#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "stream/stream.h"
#include "demuxer.h"
#include "parse_es.h"
#include "stheader.h"
#include "mtv_struct.h"
#include "unrarlib.h"
#include "ms_hdr.h"
#include "mpeg_hdr.h"
#include "../libavcodec/jzmedia.h"
//#include "demux_cmmb.h"
#include "demux_dab.h"
#include <mplaylib.h>
extern void init_cmmb_func_inno();
extern int demux_cmmb_inno(unsigned char *,int);

  
#define MAX_DEMUXER_FIFO_SIZE    40
#define MAX_AUDIO_FIFO_BUFSIZE   1024 
#define MAX_VIDEO_FIFO_BUFSIZE   ( 1024 * 50 )

//#undef printf
static unsigned int mtv_pos=0;
static unsigned int mtv_packet_len=0;
static double cur_ts=0;

typedef enum
{
	UNKNOWN		= -1,
	VIDEO_MPEG1 	= 0x10000001,
	VIDEO_MPEG2 	= 0x10000002,
	VIDEO_MPEG4 	= 0x10000004,
	VIDEO_H264 	= 0x10000005,
	VIDEO_AVC	= mmioFOURCC('a', 'v', 'c', '1'),
	VIDEO_VC1	= mmioFOURCC('W', 'V', 'C', '1'),
	AUDIO_MP2   	= 0x50,
	AUDIO_A52   	= 0x2000,
	AUDIO_DTS	= 0x2001,
	AUDIO_LPCM_BE  	= 0x10001,
	AUDIO_AAC	= mmioFOURCC('M', 'P', '4', 'A'),
	SPU_DVD		= 0x3000000,
	SPU_DVB		= 0x3000001,
	PES_PRIVATE1	= 0xBD00000,
	SL_PES_STREAM	= 0xD000000,
	SL_SECTION	= 0xD100000,
	MP4_OD		= 0xD200000,
} es_stream_type_t;

static double ocr_base,ocr_new,locked_ocr;
static int ocr_cnt;
static double err_degree;
static demux_ts_fifo_t v_fifo,a_fifo;
//static TP_DTV_STREAM_T dtv_stream;
mts_func_t cmmb_mts;
static int packet_cnt,abs_err,last_degree,pack_err = 0;
#define MAX_DIFF_PTS 7.0


unsigned char *audio_buf;
unsigned char *video_buf;
unsigned int  audio_len=0,video_len=0;
double ts_a,ts_v;

demuxer_t *dsdemuxer;
static int last_type; 
void add_video_packet(double ts,unsigned char *p,int len,int in)
{
	static demux_packet_t* dp = 0;
//	F("%d pts %f len:%d in:%d %x\n",MAX_DEMUXER_FIFO_SIZE - 1,ts,len,in,p);

	if ( in == 2 )     //a new frame!
	{
		if (!dp)
	    {
			dp = new_demux_packet(20 * 1024);
	    }
		if(dp)
		{
			memcpy(dp->buffer, p, len);
			mtv_pos +=len;
		  mtv_packet_len =len;
			dp->pts = ts;
			dp->pos = mtv_pos;
			dp->flags = 0;
		}
	}
	else if ( in == 1 )     //last pic
	{
		if(dp)
		{
			memcpy(dp->buffer + dp->pos, p, len);
			mtv_pos +=len;
			dp->pos = mtv_pos;
			mtv_packet_len +=len;
			dp->len = mtv_packet_len; 
	        ds_add_packet(dsdemuxer->video, dp);
	        dp = 0;
	    }else
	    {
   			dp = new_demux_packet(len);
			if(dp)
			{
				memcpy(dp->buffer, p, len);
        mtv_pos +=len;
        mtv_packet_len +=len;
				dp->pts = ts;
				dp->pos = mtv_pos;
				dp->flags = 0;
				dp->len = mtv_packet_len;
				ds_add_packet(dsdemuxer->video, dp);
				dp = 0;
			}
	  }
	}
	else if ( in == 3 )       //single frame
	{
		dp = new_demux_packet(len);
		if(dp)
		{
			memcpy(dp->buffer, p, len);
      mtv_pos +=len;
			dp->pts = ts;
			dp->pos = mtv_pos;
			dp->flags = 0;
			dp->len = len;
			ds_add_packet(dsdemuxer->video, dp);
			dp = 0;
		}
	}
	else                  //middle frame
	{
		if(dp)
		{
			memcpy(dp->buffer + dp->pos, p, len);
			mtv_pos +=len;
			mtv_packet_len +=len;
	    }else
	    {
	    	dp = new_demux_packet(20 * 1024);
			if(dp)
			{
				memcpy(dp->buffer, p, len);
				mtv_pos +=len;
				mtv_packet_len =len;
				dp->pts = ts;
				dp->pos = mtv_pos;
				dp->flags = 0;
			}
	    }	
	}

}

void add_audio_packet(double ts,unsigned char *p,int len,int in)
{
//	F("a pts %f len:%d in:%d %x\n",ts,len,in,p);
	 demux_packet_t* dp;
   dp = new_demux_packet(len);
   cur_ts=ts;
   
	if(dp)
	{
		memcpy(dp->buffer, p, len);
		mtv_pos +=len;
		dp->pts = ts;
		dp->pos = mtv_pos;
		dp->len = len;
		dp->flags = 0;
		ds_add_packet(dsdemuxer->audio, dp);
	}
}

static int demux_cmmb_fill_buffer_data(demuxer_t * demuxer, demux_stream_t *ds);


//***************************************************//
//
// demuxer functions:
//
//***************************************************//

static demuxer_t *demux_cmmb_open(demuxer_t * demuxer)
{
	sh_video_t *shv;
	sh_audio_t *sha;

	char audio_context[20] = {0x2b,0x10,0x88,0x00};

	mp_msg(MSGT_DEMUX, MSGL_V, "DEMUX OPEN, AUDIO_ID: %d, VIDEO_ID: %d, SUBTITLE_ID: %d,\n",
		   demuxer->audio->id, demuxer->video->id, demuxer->sub->id);
	demuxer->type= DEMUXER_TYPE_CMMB;
	stream_reset(demuxer->stream);

	locked_ocr = 0;
	ocr_base = ocr_new = 0.0;
	ocr_cnt = 0;
	err_degree = 1.0;

	demuxer->seekable = 0;

	sha = new_sh_audio_aid(demuxer, 0, 64);
	if(sha)
	{
		sha->format = AUDIO_AAC;
		sha->ds = demuxer->audio;
		sha->samplerate = 24000;
		sha->samplesize = 2;
		sha->wf = (WAVEFORMATEX *) malloc(sizeof (WAVEFORMATEX) + 4);
		sha->wf->cbSize = 4;
		memcpy(sha->wf + 1, audio_context, 4);

		demuxer->audio->id = 64;
		sha->ds = demuxer->audio;
		demuxer->audio->sh = sha;
	}

	shv = new_sh_video_vid(demuxer, 1, 80);
	if(shv)
	{

		shv->format = VIDEO_AVC;
		shv->ds = demuxer->video;
		shv->bih = (BITMAPINFOHEADER *) calloc(1, sizeof(BITMAPINFOHEADER) + 25);
		shv->bih->biSize= sizeof(BITMAPINFOHEADER);

		shv->bih->biWidth = 320;
		shv->bih->biHeight = 240;

		demuxer->video->id = 80;
		shv->ds = demuxer->video;
		demuxer->video->sh = shv;
	}

	demuxer->movi_start = 0;
	stream_reset(demuxer->stream);
	init_cmmb_func_inno(add_video_packet,add_audio_packet);
	
	return demuxer;
}

static void demux_cmmb_close(demuxer_t * demuxer)
{
	uint16_t i;
	
	if(demuxer->priv)
	{
		free(demuxer->priv);
	}
	demuxer->priv=NULL;
}
 
static void demux_cmmb_seek(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
#if 0 //from demux_ts.c

demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	demux_stream_t *d_sub=demuxer->sub;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
	ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
	int i, video_stats;
	off_t newpos;

	//================= seek in MPEG-TS ==========================

	ts_dump_streams(demuxer->priv);
	reset_fifos(priv, sh_audio != NULL, sh_video != NULL, demuxer->sub->id > 0);


	if(sh_audio != NULL)
		ds_free_packs(d_audio);
	if(sh_video != NULL)
		ds_free_packs(d_video);
	if(demuxer->sub->id > 0)
		ds_free_packs(d_sub);
		
#endif	

	demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	demux_stream_t *d_sub=demuxer->sub;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
	//================= seek in CMMB ==========================
//	ts_dump_streams(demuxer->priv);
//	reset_fifos(priv, sh_audio != NULL, sh_video != NULL, demuxer->sub->id > 0);
	demuxer->stream_pts=cur_ts;
  F("seek from demux!--------------------\n"); 
  	if(sh_audio != NULL)
		ds_free_packs(d_audio);
	if(sh_video != NULL)
		ds_free_packs(d_video);
	if(demuxer->sub->id > 0)
		ds_free_packs(d_sub);
}

////////////////////////////////////
static int demux_cmmb_fill_buffer_data(demuxer_t * demuxer, demux_stream_t *ds)
{
	int l,offset = 0;
	stream_t *s = demuxer->stream;
	int i,tmp,num,len = 0;
	unsigned int cfg;
	unsigned char fieldlen;
	unsigned char *buf;
	  demux_packet_t* dp;
	 
	 dsdemuxer =  demuxer;
	while(1)                   //read sync field
	{
		if (stream_read(demuxer->stream, &cfg, 4) !=4)
			return 0;
		
		if(cfg == 0x01000000)
		{
			break;
		}
	}

	if (stream_read(demuxer->stream, &fieldlen, 1) !=1)
		return 0;

	tmp = fieldlen - 1;

	buf = malloc(tmp + 5);
	if(!buf)
	{
		F("malloc failed!\n");
		return 0;
	}
	
	memcpy(buf,&cfg,4);
	buf[4] = fieldlen;
	
	len = stream_read(demuxer->stream, &buf[5], tmp);
	if(len < tmp){
		free(buf);
		return 0;
	}
	
	num = buf[11] & 0x0f;
	
	len =  buf[12];
	len = ( len << 8 ) + buf[13];
	len = ( len << 8 ) + buf[14];
	num --;
	l = len;
	for ( i = 1; i <= num; i ++ )
	{
		len =  buf[12 + i * 3];
		len = ( len << 8 ) + buf[13 + i * 3];
		len = ( len << 8 ) + buf[14 + i * 3];
		l += len;
	}
	int dlen = 5 + tmp + l;
	dlen = (dlen + 3) & (~3);
	buf = realloc(buf,5 + tmp + l);
	if(!buf)
	{
		F("realloc failed!\n");
		free(buf);
		return 0;
	}
	len = stream_read(demuxer->stream, &buf[5 + tmp] ,dlen - 5 -tmp);
//	F("len = %d l = %d\n",len ,l);
//	F(" %d %d %d %d %d\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
	if ( len < l )
	{
			 free(buf);
			 return 0;
	}

	demux_cmmb_inno(buf,l+tmp+5);
	free(buf);
	return 1;
}

static int check_cmmb_file(demuxer_t *demuxer)
{
	stream_t *s = demuxer->stream;
	demuxer->priv = NULL;
	unsigned int * id = s->priv;
	
	return DEMUXER_TYPE_CMMB;
}

static int demux_cmmb_control(demuxer_t *demuxer, int cmd, void *arg)
{
	switch(cmd)
	{
		case DEMUXER_CTRL_SWITCH_AUDIO:
		case DEMUXER_CTRL_SWITCH_VIDEO:
		case DEMUXER_CTRL_IDENTIFY_PROGRAM:	
			return DEMUXER_CTRL_OK;
		default:
			return DEMUXER_CTRL_NOTIMPL;
	}
}

demuxer_desc_t demuxer_desc_cmmb = {
  "CMMB  demuxer",
  "CMMB",
  "CMMB",
  "Lucifer Liu",
  "",
  DEMUXER_TYPE_CMMB,
  0, // unsafe autodetect
  check_cmmb_file,
  demux_cmmb_fill_buffer_data,
  demux_cmmb_open,
  demux_cmmb_close,
  demux_cmmb_seek,
  demux_cmmb_control
};
