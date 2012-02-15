/*
 * Demultiplexer for MPEG2 Transport Streams.
 *
 * Written by Nico <nsabbi@libero.it>
 * Kind feedback is appreciated; 'sucks' and alike is not.
 * Originally based on demux_pva.c written by Matteo Giani and FFmpeg (libavformat) sources
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <mplaylib.h>

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

#define TS_PH_PACKET_SIZE 192
#define TS_FEC_PACKET_SIZE 204
#define TS_PACKET_SIZE 188
#define NB_PID_MAX 8192

#define MAX_HEADER_SIZE 6			/* enough for PES header + length */
#define MAX_CHECK_SIZE	65535
//#define TS_MAX_PROBE_SIZE 2000000 /* dont forget to change this in cfg-common.h too */
#define TS_MAX_PROBE_SIZE 0x6000 /* dont forget to change this in cfg-common.h too */

#define NUM_CONSECUTIVE_TS_PACKETS 32
#define NUM_CONSECUTIVE_AUDIO_PACKETS 348
#define MAX_A52_FRAME_SIZE 3840
#define MAX_RETRY_TIME 100
#define MAX_SYNC_DEGREE  3.0               //add by yliu for sunc!!
#define MIN_SYNC_DEGREE  0.8               //add by yliu for sunc!!
#define CLOCK_SYNC_DEGREE ( 11.0845f * 1024.0f * 1024.0f )

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif
#define TYPE_AUDIO 1
#define TYPE_VIDEO 2

int ismtv;
static int ts_prog;
static int ts_keep_broken=0;
static off_t ts_probe = 0;
extern char *dvdsub_lang, *audio_lang;	//for -alang

#if 0
#undef malloc(x)
#undef memalign(x,y)

#define malloc(x) ({void * mem;printf("malloc:%s %d",__FILE__,__LINE__);mem = malloc(x);printf(" addr = %08x len = %d\n",mem,x);mem;})
#define memalign(x,y) ({void *mem;printf("memalign:%s %d",__FILE__,__LINE__); mem = memalign(x,y);printf(" addr = %08x %d\n",mem,y);mem;})
#endif

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

typedef struct {
	uint8_t *buffer;
	uint16_t buffer_len;
} ts_section_t;

typedef struct {
	int size;
	unsigned char *start;
	uint16_t payload_size;
	es_stream_type_t type, subtype;
	float pts, last_pts;
	int pid;
	char lang[4];
	int last_cc;				// last cc code (-1 if first packet)
	int is_synced;
	ts_section_t section;
	uint8_t *extradata;
	int extradata_alloc, extradata_len;
	struct {
		uint8_t au_start, au_end, last_au_end;
	} sl;
	double ocr;
} ES_stream_t;

typedef struct {
	void *sh;
	int id;
	int type;
} sh_av_t;

typedef struct MpegTSContext {
	int packet_size; 		// raw packet size, including FEC if present e.g. 188 bytes
	ES_stream_t *pids[NB_PID_MAX];
	sh_av_t streams[NB_PID_MAX];
} MpegTSContext;


typedef struct {
	demux_stream_t *ds;
	demux_packet_t *pack;
	int offset, buffer_size;
} av_fifo_t;

#define MAX_EXTRADATA_SIZE 64*1024
typedef struct {
	int32_t object_type;	//aka codec used
	int32_t stream_type;	//video, audio etc.
	uint8_t buf[MAX_EXTRADATA_SIZE];
	uint16_t buf_size;
	uint8_t szm1;
} mp4_decoder_config_t;

typedef struct {
	//flags
	uint8_t flags;
	uint8_t au_start;
	uint8_t au_end;
	uint8_t random_accesspoint;
	uint8_t random_accesspoint_only;
	uint8_t padding;
	uint8_t use_ts;
	uint8_t idle;
	uint8_t duration;
	
	uint32_t ts_resolution, ocr_resolution;
	uint8_t ts_len, ocr_len, au_len, instant_bitrate_len, degr_len, au_seqnum_len, packet_seqnum_len;
	uint32_t timescale;
	uint16_t au_duration, cts_duration;
	uint64_t ocr, dts, cts;
} mp4_sl_config_t;

typedef struct {
	uint16_t id;
	uint8_t flags;
	mp4_decoder_config_t decoder;
	mp4_sl_config_t sl;
} mp4_es_descr_t;

typedef struct {
	uint16_t id;
	uint8_t flags;
	mp4_es_descr_t *es;
	uint16_t es_cnt;
} mp4_od_t;

typedef struct {
	uint8_t skip;
	uint8_t table_id;
	uint8_t ssi;
	uint16_t section_length;
	uint16_t ts_id;
	uint8_t version_number;
	uint8_t curr_next;
	uint8_t section_number;
	uint8_t last_section_number;
	struct pat_progs_t {
		uint16_t id;
		uint16_t pmt_pid;
	} *progs;
	uint16_t progs_cnt;
	ts_section_t section;
} pat_t;

typedef struct {
	uint16_t progid;
	uint8_t skip;
	uint8_t table_id;
	uint8_t ssi;
	uint16_t section_length;
	uint8_t version_number;
	uint8_t curr_next;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t PCR_PID;
	uint16_t prog_descr_length;
	ts_section_t section;
	uint16_t es_cnt;
	struct pmt_es_t {
		uint16_t pid;
		uint32_t type;	//it's 8 bit long, but cast to the right type as FOURCC
		uint16_t descr_length;
		uint8_t format_descriptor[5];
		uint8_t lang[4];
		uint16_t mp4_es_id;
	} *es;
	mp4_od_t iod, *od;
	mp4_es_descr_t *mp4es;
	int od_cnt, mp4es_cnt;
} pmt_t;

typedef struct {
	uint64_t size;
	float duration;
	float first_pts;
	float now_pts;
	float last_pts;
} TS_stream_info;

typedef struct {
	MpegTSContext ts;
	int last_pid;
	av_fifo_t fifo[3];	//0 for audio, 1 for video, 2 for subs
	pat_t pat;
	pmt_t *pmt;
	uint16_t pmt_cnt;
	uint32_t prog;
	uint32_t vbitrate;
	int keep_broken;
	int last_aid;
	int last_vid;
	char packet[TS_FEC_PACKET_SIZE];
	TS_stream_info vstr, astr;
	sh_audio_t *ash;
	sh_video_t *vsh;
	double diff_t;
} ts_priv_t;


typedef struct {
	es_stream_type_t type;
	ts_section_t section;
} TS_pids_t;

#define PTS_FIFO_SIZE   0x10
#define MAX_DIFF        10

typedef struct _pts_packet_t
{
	double     ocr;
	demuxer_t *demuxer;
	demux_stream_t *ds;
	demux_packet_t *dp;
	struct _pts_packet_t *next;

}pts_packet_t;
 
typedef struct __correct_pts
{
	int p_num;
	double locked_ocr;
	pts_packet_t * out_packet,* in_packet;
}correct_pts_t;

typedef struct __ts_stream_t
{
	unsigned char *buf;
	int size,pos;
}ts_stream_t;

#include "demux_dab.h"

#define MAX_DATA_ERROR  10

static correct_pts_t  fifo;
static double globe_ocr,locked_ocr;
static double err_degree,ocr_base,ocr_new;
static int ocr_cnt, err_cnt;
static demux_ts_fifo_t v_fifo,a_fifo;
static int packet_cnt,abs_err,last_degree;
mts_func_t ts_mts;
static int a_pes_len,v_pes_len,v_buf_pos,a_buf_pos,pack_err = 0;
static double a_pts,v_pts;

#define IS_AUDIO(x) (((x) == AUDIO_MP2) || ((x) == AUDIO_A52) || ((x) == AUDIO_LPCM_BE) || ((x) == AUDIO_AAC) || ((x) == AUDIO_DTS))
#define IS_VIDEO(x) (((x) == VIDEO_MPEG1) || ((x) == VIDEO_MPEG2) || ((x) == VIDEO_MPEG4) || ((x) == VIDEO_H264) || ((x) == VIDEO_AVC)  || ((x) == VIDEO_VC1))

static int ts_parse(demuxer_t *demuxer, ES_stream_t *es, unsigned char *packet, int probe);
static void reset_fifos(ts_priv_t* priv, int a, int v, int s);
static void ts_init_func(mts_func_t *mts);

#define STREAM_DAB_BUFFER_SIZE   ( 188 * 70 )
//#define STREAM_DAB_BUFFER_SIZE   ( 2048 * 6 )
#define MAX_RETRY_TIME_OFDM       400
#define MAX_RETRY_TIME_TUNER      10

static int ts_stream_fill_buff(stream_t *s)
{
	int len =0 ,ofdm_reset = 0, tuner_reset = 0;

	len=read(s->fd,s->buffer,STREAM_DAB_BUFFER_SIZE);
	while( len <= 0 ) {
//		printf("read 0000 \n");
#if 0
		usleep(10000);
		ofdm_reset ++ ;
		if ( ofdm_reset > MAX_RETRY_TIME_OFDM )          //reset ofdm
		{
			tuner_reset ++;
			if ( tuner_reset > MAX_RETRY_TIME_TUNER ) //signal stop,program end!
			{
				printf("Signal stop,program end! \n");
				return 0;
			}
			ioctl(s->fd, 0x2, 0);
			usleep(100 * 1000);
		}
#endif
		len=read(s->fd,s->buffer,STREAM_DAB_BUFFER_SIZE);
//		printf("Read len %d \n",len);
	}
	s->buf_len = len ;
	s->buf_pos = 0;
	return len;
}

inline static int ts_stream_skip(stream_t *s, int len)
{
//	printf("skip %d %d %d \n",len,s->buf_pos,s->buf_len);
	if ( s->buf_len - s->buf_pos < len )
	{
		len -= s->buf_len - s->buf_pos;
		ts_stream_fill_buff(s);		
	}
	s->buf_pos += len;
	return 1;
}

inline static int ts_stream_read(stream_t *s,unsigned char * buf,int len)
{
//	printf("read %d %d %d \n",len,s->buf_pos,s->buf_len);
	if ( s->buf_len - s->buf_pos < len )
	{
		uc_memcpy(buf,&s->buffer[s->buf_pos],s->buf_len - s->buf_pos);
		len -= s->buf_len - s->buf_pos;
		buf += s->buf_len - s->buf_pos;
		ts_stream_fill_buff(s);		
	}
	uc_memcpy(buf,&s->buffer[s->buf_pos],len);
	s->buf_pos += len;
	return 1;
}

inline static int ts_stream_read_char(stream_t *s)
{
//	printf("read char %d %x \n",s->buf_pos,s->buffer[s->buf_pos]);
	if ( s->buf_len - s->buf_pos < 1 )
		ts_stream_fill_buff(s);		
	return s->buffer[s->buf_pos++];
}

static int ts_stream_reset(stream_t * s)
{		
	s->buf_pos = 0;
	ts_stream_fill_buff(s);		
	return 1;
}

static int ts_check_file(demuxer_t * demuxer)
{
	const int buf_size = (TS_FEC_PACKET_SIZE * NUM_CONSECUTIVE_TS_PACKETS);
	unsigned char buf[TS_FEC_PACKET_SIZE * NUM_CONSECUTIVE_TS_PACKETS], done = 0, *ptr;
	uint32_t _read, i, count = 0, is_ts;
	int cc[NB_PID_MAX], last_cc[NB_PID_MAX], pid, cc_ok, c, good, bad;
	uint8_t size = 0;
	off_t pos = 0;
	off_t init_pos;

	mp_msg(MSGT_DEMUX, MSGL_V, "Checking for T-DMB/DAB TS\n");

	return 188;
}


static inline int32_t progid_idx_in_pmt(ts_priv_t *priv, uint16_t progid)
{
	int x;

	if(priv->pmt == NULL)
		return -1;

	for(x = 0; x < priv->pmt_cnt; x++)
	{
		if(priv->pmt[x].progid == progid)
			return x;
	}

	return -1;
}


static inline int32_t progid_for_pid(ts_priv_t *priv, int pid, int32_t req)		//finds the first program listing a pid
{
	int i, j;
	pmt_t *pmt;


	if(priv->pmt == NULL)
		return -1;


	for(i=0; i < priv->pmt_cnt; i++)
	{
		pmt = &(priv->pmt[i]);

		if(pmt->es == NULL)
			return -1;

		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].pid == pid)
			{
				if((req == 0) || (req == pmt->progid))
					return pmt->progid;
			}
		}

	}
	return -1;
}


static inline int pid_match_lang(ts_priv_t *priv, uint16_t pid, char *lang)
{
	uint16_t i, j;
	pmt_t *pmt;

	if(priv->pmt == NULL)
		return -1;

	for(i=0; i < priv->pmt_cnt; i++)
	{
		pmt = &(priv->pmt[i]);

		if(pmt->es == NULL)
			return -1;

		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].pid != pid)
				continue;

			mp_msg(MSGT_DEMUXER, MSGL_V, "CMP LANG %s AND %s, pids: %d %d\n",pmt->es[j].lang, lang, pmt->es[j].pid, pid);
			if(strncmp(pmt->es[j].lang, lang, 3) == 0)
			{
				return 1;
			}
		}

	}

	return -1;
}

typedef struct {
	int32_t atype, vtype, stype;	//types
	int32_t apid, vpid, spid;	//stream ids
	char slang[4], alang[4];	//languages
	uint16_t prog;
	off_t probe;
} tsdemux_init_t;



struct {
	char *buf;
	int pos;
} pes_priv1[8192], *pptr;

static off_t ts_detect_streams(demuxer_t *demuxer, tsdemux_init_t *param)
{
	int video_found = 0, audio_found = 0, sub_found = 0, i, num_packets = 0, req_apid, req_vpid, req_spid;
	int is_audio, is_video, is_sub, has_tables;
	int32_t p, chosen_pid = 0;
	off_t pos=0, ret = 0, init_pos, end_pos;
	ES_stream_t es;
	unsigned char tmp[TS_FEC_PACKET_SIZE];
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;
	char *tmpbuf;

	printf("Stream detect !\n");
	
	priv->last_pid = 8192;		//invalid pid

	req_apid = param->apid;
	req_vpid = param->vpid;
	req_spid = param->spid;

	has_tables = 0;
	memset(pes_priv1, 0, sizeof(pes_priv1));
	init_pos = stream_tell(demuxer->stream);
	mp_msg(MSGT_DEMUXER, MSGL_V, "PROBING UP TO %"PRIu64", PROG: %d\n", (uint64_t) param->probe, param->prog);
	end_pos = init_pos + (param->probe ? param->probe : TS_MAX_PROBE_SIZE);
#if 0

	while(1)
	{
		pos = stream_tell(demuxer->stream);
		if(pos > end_pos || demuxer->stream->eof)
			break;

		if(ts_parse(demuxer, &es, tmp, 1))
		{
			//Non PES-aligned A52 audio may escape detection if PMT is not present;
			//in this case we try to find at least 3 A52 syncwords
			if((es.type == PES_PRIVATE1) && (! audio_found) && req_apid > -2)
			{
				pptr = &pes_priv1[es.pid];
				if(pptr->pos < 64*1024)
				{
				tmpbuf = (char*) realloc(pptr->buf, pptr->pos + es.size);
				if(tmpbuf != NULL)
				{
					pptr->buf = tmpbuf;
					memcpy(&(pptr->buf[ pptr->pos ]), es.start, es.size);
					pptr->pos += es.size;
					if(a52_check(pptr->buf, pptr->pos) > 2)
					{
						param->atype = AUDIO_A52;
						param->apid = es.pid;
						es.type = AUDIO_A52;
					}
				}
				}
			}
			
			is_audio = IS_AUDIO(es.type) || ((es.type==SL_PES_STREAM) && IS_AUDIO(es.subtype));
			is_video = IS_VIDEO(es.type) || ((es.type==SL_PES_STREAM) && IS_VIDEO(es.subtype));
			is_sub   = ((es.type == SPU_DVD) || (es.type == SPU_DVB));


			if((! is_audio) && (! is_video) && (! is_sub))
				continue;
			if(is_audio && req_apid==-2)
				continue;

			if(is_video)
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_VIDEO_ID=%d\n", es.pid);
    				chosen_pid = (req_vpid == es.pid);
				if((! chosen_pid) && (req_vpid > 0))
					continue;
			}
			else if(is_audio)
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_AUDIO_ID=%d\n", es.pid);
				if (es.lang[0] > 0)
					mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_AID_%d_LANG=%s\n", es.pid, es.lang);
				if(req_apid > 0)
				{
					chosen_pid = (req_apid == es.pid);
					if(! chosen_pid)
						continue;
				}
				else if(param->alang[0] > 0 && es.lang[0] > 0)
				{
					if(pid_match_lang(priv, es.pid, param->alang) == -1)
						continue;

					chosen_pid = 1;
					param->apid = req_apid = es.pid;
				}
			}
			else if(is_sub)
			{
				mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_SUBTITLE_ID=%d\n", es.pid);
				if (es.lang[0] > 0)
					mp_msg(MSGT_IDENTIFY, MSGL_V, "ID_SID_%d_LANG=%s\n", es.pid, es.lang);
				chosen_pid = (req_spid == es.pid);
				if((! chosen_pid) && (req_spid > 0))
					continue;
			}

			if(req_apid < 0 && (param->alang[0] == 0) && req_vpid < 0 && req_spid < 0)
				chosen_pid = 1;

			if((ret == 0) && chosen_pid)
			{
				ret = stream_tell(demuxer->stream);
			}

			p = progid_for_pid(priv, es.pid, param->prog);
			if(p != -1)
				has_tables++;

			if((param->prog == 0) && (p != -1))
			{
				if(chosen_pid)
					param->prog = p;
			}

			if((param->prog > 0) && (param->prog != p))
			{
				if(audio_found)
				{
					if(is_video && (req_vpid == es.pid))
					{
						param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
						param->vpid = es.pid;
						video_found = 1;
						break;
					}
				}

				if(video_found)
				{
					if(is_audio && (req_apid == es.pid))
					{
						param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
						param->apid = es.pid;
						audio_found = 1;
						break;
					}
				}


				continue;
			}


			mp_msg(MSGT_DEMUXER, MSGL_DBG2, "TYPE: %x, PID: %d, PROG FOUND: %d\n", es.type, es.pid, param->prog);


#endif
	param->vtype = IS_VIDEO(es.type) ? es.type : es.subtype;
	param->vpid = es.pid;
	video_found = 1;

	param->atype = IS_AUDIO(es.type) ? es.type : es.subtype;
	param->apid = es.pid;
	audio_found = 1;

	for(i=0; i<8192; i++)
	{
		if(pes_priv1[i].buf != NULL)
		{
			free(pes_priv1[i].buf);
			pes_priv1[i].buf = NULL;
			pes_priv1[i].pos = 0;
		}
	}

	for(i=0; i<8192; i++)
	{
		if(priv->ts.pids[i] != NULL)
		{
			priv->ts.pids[i]->payload_size = 0;
			priv->ts.pids[i]->pts = priv->ts.pids[i]->last_pts = 0;
			priv->ts.pids[i]->last_cc = -1;
			priv->ts.pids[i]->is_synced = 0;
		}
	}

	return ret;
}

static int parse_avc_sps(uint8_t *buf, int len, int *w, int *h)
{
	int sps, sps_len;
	unsigned char *ptr; 
	mp_mpeg_header_t picture;
	if(len < 6)
		return 0;
	sps = buf[5] & 0x1f;
	if(!sps)
		return 0;
	sps_len = (buf[6] << 8) | buf[7];
	if(!sps_len || (sps_len > len - 8))
		return 0;
	ptr = &(buf[8]);
	picture.display_picture_width = picture.display_picture_height = 0;
	h264_parse_sps(&picture, ptr, len - 8);
	if(!picture.display_picture_width || !picture.display_picture_height)
		return 0;
	*w = picture.display_picture_width;
	*h = picture.display_picture_height;
	return 1;
}

static demuxer_t *demux_open_ts(demuxer_t * demuxer)
{
	int i,retry;
	uint8_t packet_size;
	sh_video_t *sh_video,* shv;
	sh_audio_t *sh_audio,* sha;
	off_t start_pos;
	tsdemux_init_t params;
	ts_priv_t * priv = (ts_priv_t*) demuxer->priv;

	char audio_context[20] =
		{0x2b,0x90,0x88,0x00};
	char video_context[30] = 
		{0x01,0x42,0x80,0x0d,0xff,
		 0xe1,0x00,0x09,0x27,0x42,
		 0x80,0x0d,0x95,0xa0,0x50,
		 0x7c,0x40,0x01,0x00,0x05,
		 0x28,0xce,0x02,0x58,0x80};

	mp_msg(MSGT_DEMUX, MSGL_V, "DEMUX OPEN, AUDIO_ID: %d, VIDEO_ID: %d, SUBTITLE_ID: %d,\n",
		demuxer->audio->id, demuxer->video->id, demuxer->sub->id);
	demuxer->type= DEMUXER_TYPE_DAB;
	ts_stream_reset(demuxer->stream);

	ts_packet_fifo_init(&a_fifo,MAX_AUDIO_FIFO_BUFSIZE);
	ts_packet_fifo_init(&v_fifo,MAX_VIDEO_FIFO_BUFSIZE);
	locked_ocr = 0;
	ocr_base = ocr_new = 0.0;
	ocr_cnt = 0;
	err_cnt = 0;
	packet_cnt = 0;
	abs_err = 0;
	last_degree = 0;
	a_pes_len = v_pes_len = v_buf_pos = a_buf_pos = 0;
	a_pts = v_pts = 0.0;

	ts_init_func( &ts_mts );
	ismtv = 0x4d5456;

	packet_size = 188;
	priv = calloc(1, sizeof(ts_priv_t));
	if(priv == NULL)
	{
		mp_msg(MSGT_DEMUX, MSGL_FATAL, "DEMUX_OPEN_TS, couldn't allocate enough memory for ts->priv, exit\n");
		return NULL;
	}

	for(i=0; i < 8192; i++)
	{
	    priv->ts.pids[i] = NULL;
	    priv->ts.streams[i].id = -3;
	}
	priv->pat.progs = NULL;
	priv->pat.progs_cnt = 0;
	priv->pat.section.buffer = NULL;
	priv->pat.section.buffer_len = 0;

	priv->pmt = NULL;
	priv->pmt_cnt = 0;

	priv->keep_broken = ts_keep_broken;
	priv->ts.packet_size = packet_size;


	demuxer->priv = priv;
	demuxer->seekable = 0;

	params.atype = params.vtype = params.stype = UNKNOWN;
	params.apid = demuxer->audio->id;
	params.vpid = demuxer->video->id;
	params.spid = demuxer->sub->id;
	params.prog = ts_prog;
	params.probe = ts_probe;

	sha = new_sh_audio_aid(demuxer, 0, 64);
	if(sha)
	{
		sha->format = AUDIO_AAC;
		sha->ds = demuxer->audio;
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
		shv->bih->biSize= sizeof(BITMAPINFOHEADER) + 25;
		shv->bih->biCompression = shv->format;
		memcpy(shv->bih + 1, video_context, 25);
		shv->bih->biWidth = 320;
		shv->bih->biHeight = 240;

		demuxer->video->id = 80;
		shv->ds = demuxer->video;
		demuxer->video->sh = shv;
	}

	mp_msg(MSGT_DEMUXER,MSGL_V, "Opened TS demuxer, audio: %x(pid %d), video: %x(pid %d)...POS=%"PRIu64", PROBE=%"PRIu64"\n", params.atype, demuxer->audio->id, params.vtype, demuxer->video->id, (uint64_t) start_pos, ts_probe);

	start_pos = (start_pos <= priv->ts.packet_size ? 0 : start_pos - priv->ts.packet_size);
	demuxer->movi_start = start_pos;
	ts_stream_reset(demuxer->stream);
//	stream_seek(demuxer->stream, start_pos);	//IF IT'S FROM A PIPE IT WILL FAIL, BUT WHO CARES?

	demuxer->filepos = stream_tell(demuxer->stream);
	return demuxer;
}

static void demux_close_ts(demuxer_t * demuxer)
{
	uint16_t i;
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;
	
	if(priv)
	{
		if(priv->pat.section.buffer)
			free(priv->pat.section.buffer);
		if(priv->pat.progs)
			free(priv->pat.progs);
	
		if(priv->pmt)
		{	
			for(i = 0; i < priv->pmt_cnt; i++)
			{
				if(priv->pmt[i].section.buffer)
					free(priv->pmt[i].section.buffer);
				if(priv->pmt[i].es)
					free(priv->pmt[i].es);
			}
			free(priv->pmt);
		}
		free(priv);
	}
	demuxer->priv=NULL;
}


extern unsigned char mp_getbits(unsigned char*, unsigned int, unsigned char);
#define getbits mp_getbits

static int mp4_parse_sl_packet(pmt_t *pmt, uint8_t *buf, uint16_t packet_len, int pid, ES_stream_t *pes_es)
{
	int i, n, m, mp4_es_id = -1;
	uint64_t v = 0;
	uint32_t pl_size = 0; 
	int deg_flag = 0;
	mp4_es_descr_t *es = NULL;
	mp4_sl_config_t *sl = NULL;
	uint8_t au_start = 0, au_end = 0, rap_flag = 0, ocr_flag = 0, padding = 0,  padding_bits = 0, idle = 0;
	double de;
		
	pes_es->is_synced = 0;
	mp_msg(MSGT_DEMUXER,MSGL_V, "mp4_parse_sl_packet, pid: %d, pmt: %pm, packet_len: %d\n", pid, pmt, packet_len);	
	if(! pmt || !packet_len)
		return 0;
	
	for(i = 0; i < pmt->es_cnt; i++)
	{
		if(pmt->es[i].pid == pid)
			mp4_es_id = pmt->es[i].mp4_es_id;
	}
	if(mp4_es_id < 0)
		return -1;
	
	for(i = 0; i < pmt->mp4es_cnt; i++)
 	{
		if(pmt->mp4es[i].id == mp4_es_id)
			es = &(pmt->mp4es[i]);
	}
	if(! es)
		return -1;
	
	pes_es->subtype = es->decoder.object_type;
	
	sl = &(es->sl);
	if(!sl)
		return -1;
		
	//now es is the complete es_descriptor of out mp4 ES stream
	mp_msg(MSGT_DEMUXER,MSGL_DBG2, "ID: %d, FLAGS: 0x%x, subtype: %x\n", es->id, sl->flags, pes_es->subtype);
	
	n = 0;
	if(sl->au_start)
		pes_es->sl.au_start = au_start = getbits(buf, n++, 1);
	else
		pes_es->sl.au_start = (pes_es->sl.last_au_end ? 1 : 0);
	if(sl->au_end)
		pes_es->sl.au_end = au_end = getbits(buf, n++, 1);
	
	if(!sl->au_start && !sl->au_end)
	{
		pes_es->sl.au_start = pes_es->sl.au_end = au_start = au_end = 1;
	}
	pes_es->sl.last_au_end = pes_es->sl.au_end;
	
	
	if(sl->ocr_len > 0)
		ocr_flag = getbits(buf, n++, 1);
	if(sl->idle)
		idle = getbits(buf, n++, 1);
	if(sl->padding)
		padding = getbits(buf, n++, 1);
	if(padding)
	{
		padding_bits = getbits(buf, n, 3);
		n += 3;
	}
	
	if(idle || (padding && !padding_bits))
	{
		pes_es->payload_size = 0;
		return -1;
	}
	
	//(! idle && (!padding || padding_bits != 0)) is true
	n += sl->packet_seqnum_len;
	if(sl->degr_len)
		deg_flag = getbits(buf, n++, 1);
	if(deg_flag)
		n += sl->degr_len;
	v = 0;
	if(ocr_flag)
	{
		int i = 0, m;
		while(i < sl->ocr_len)
		{
			m = FFMIN(8, sl->ocr_len - i);
			v <<= m;
			v |= getbits(buf, n, m);
			i += m;
			n += m;
			if(packet_len * 8 <= n+8)
				return -1;
		}

		if ( ocr_base != 0 )
		{
			ocr_new = (float)v/90000.0f;
			if ( abs(ocr_new - ocr_base) < 1 ) 
				ocr_cnt ++;
			else ocr_cnt = 0;
			ocr_base = ocr_new;
		}
		else
			ocr_base = (float)v/90000.0f;

		if ( ocr_cnt > 5 )
		{
			locked_ocr = (double)GetTimer()/1000000.0 - (double)ocr_base;
//			printf("Locked ocr as %f\n",locked_ocr );
		}

	}
	v = 0;
	if(packet_len * 8 <= n)
		return -1;
 
	mp_msg(MSGT_DEMUXER,MSGL_DBG2, "\nAU_START: %d, AU_END: %d\n", au_start, au_end);
	if(au_start)
	{
		int dts_flag = 0, cts_flag = 0, ib_flag = 0;
		
		if(sl->random_accesspoint)
			rap_flag = getbits(buf, n++, 1);

		//check commented because it seems it's rarely used, and we need this flag set in case of au_start
		//the decoder will eventually discard the payload if it can't decode it
		//if(rap_flag || sl->random_accesspoint_only)
			pes_es->is_synced = 1;
		
		n += sl->au_seqnum_len;
		if(packet_len * 8 <= n+8)
			return -1;
		if(sl->use_ts)
		{
			dts_flag = getbits(buf, n++, 1);
			cts_flag = getbits(buf, n++, 1);
		}
		if(sl->instant_bitrate_len)
			ib_flag = getbits(buf, n++, 1);
		if(packet_len * 8 <= n+8)
			return -1;
		if(dts_flag && (sl->ts_len > 0))
		{
			int i = 0, m;
			n += sl->ts_len;
		}
		if(packet_len * 8 <= n+8)
			return -1;
	      
		v = 0;
		if(cts_flag && (sl->ts_len > 0))
		{
			int i = 0, m;
			unsigned int t;

			while(i < sl->ts_len)
			{
				m = FFMIN(8, sl->ts_len - i);
				v <<= m;
				v |= getbits(buf, n, m);
				i += m;
				n += m;
				if(packet_len * 8 <= n+8)
					return -1;
			}
			
			pes_es->pts = (float) v/ (float) sl->ts_resolution;
			mp_msg(MSGT_DEMUXER,MSGL_DBG2, "CTS: %d bits, value: %"PRIu64"/%d = %.3f\n", sl->ts_len, v, sl->ts_resolution, pes_es->pts);

		}
		
		i = 0;
		pl_size = 0;
		while(i < sl->au_len)
		{
			m = FFMIN(8, sl->au_len - i);
			pl_size |= getbits(buf, n, m);
			if(sl->au_len - i > 8)
				pl_size <<= 8;
			i += m;
			n += m;
			if(packet_len * 8 <= n+8)
				return -1;
		}
		mp_msg(MSGT_DEMUXER,MSGL_DBG2, "AU_LEN: %u (%d bits)\n", pl_size, sl->au_len);
		if(ib_flag)
			n += sl->instant_bitrate_len;
	}
	
	m = (n+7)/8;
	if(0 < pl_size && pl_size < pes_es->payload_size)
		pes_es->payload_size = pl_size;
	
	mp_msg(MSGT_DEMUXER,MSGL_V, "mp4_parse_sl_packet, n=%d, m=%d, size from pes hdr: %u, sl hdr size: %u, RAP FLAGS: %d/%d\n", 
		n, m, pes_es->payload_size, pl_size, (int) rap_flag, (int) sl->random_accesspoint_only);
	
	return m;
}

static int pes_parse2(unsigned char *buf, uint16_t packet_len, ES_stream_t *es, int32_t type_from_pmt, pmt_t *pmt, int pid)
{
	unsigned char  *p;
	uint32_t       header_len;
	int64_t        pts;
	uint32_t       stream_id;
	uint32_t       pkt_len, pes_is_aligned;
	unsigned int tmp;

	//Here we are always at the start of a PES packet
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2(%p, %d): \n", buf, (uint32_t) packet_len);

	if(packet_len == 0 || packet_len > 184)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2, BUFFER LEN IS TOO SMALL OR TOO BIG: %d EXIT\n", packet_len);
		return 0;
	}

	p = buf;
	pkt_len = packet_len;

	mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: HEADER %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);
	if (p[0] || p[1] || (p[2] != 1))
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: error HEADER %02x %02x %02x (should be 0x000001) \n", p[0], p[1], p[2]);
		return 0 ;
	}

	packet_len -= 6;
	if(packet_len==0)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: packet too short: %d, exit\n", packet_len);
		return 0;
	}

	es->payload_size = (p[4] << 8 | p[5]);
	pes_is_aligned = (p[6] & 4);

	stream_id  = p[3];

	header_len = p[8];

	if (header_len + 9 > pkt_len) //9 are the bytes read up to the header_length field
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "demux_ts: illegal value for PES_header_data_length (0x%02x)\n", header_len);
		return 0;
	}

	p += header_len + 9;
	packet_len -= header_len + 3;

	if(es->payload_size)
		es->payload_size -= header_len + 3;


	es->is_synced = 1;	//only for SL streams we have to make sure it's really true, see below
//	if ((stream_id == 0xfa))
	if (1)
	{
		int l;
		
		es->is_synced = 0;
		if(type_from_pmt != UNKNOWN)	//MP4 A/V or SL
		{
			es->start   = p;
			es->size    = packet_len;
			es->type    = type_from_pmt;
				
//			if(type_from_pmt == SL_PES_STREAM)
//			{
			l = mp4_parse_sl_packet(pmt, p, packet_len, pid, es);
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "L=%d, TYPE=%x\n", l, type_from_pmt);
			if(l < 0)
			{
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "pes_parse2: couldn't parse SL header, passing along full PES payload\n");
				l = 0;
			}
			es->start   += l;
			es->size    -= l;
//		}

			if(es->payload_size)
				es->payload_size -= packet_len;
			return 1;
		}
	}
	else
	{
		printf("error pes id! \n");
	}

	es->is_synced = 0;
	return 0;
}




static int ts_sync(stream_t *stream)
{
	int c=0,n = 1;

	while((c=ts_stream_read_char(stream)) != 0x47)
	{
		n ++;
//		if ( n % 30 == 0) printf("\n");
//		printf("%2x ",c);
	}
	if ( n > 1 )
		printf("sync skip %d \n",n);

	return n;
}

static inline int32_t prog_idx_in_pat(ts_priv_t *priv, uint16_t progid)
{
	int x;

	if(priv->pat.progs == NULL)
			return -1;

	for(x = 0; x < priv->pat.progs_cnt; x++)
	{
		if(priv->pat.progs[x].id == progid)
			return x;
	}

	return -1;
}


static inline int32_t prog_id_in_pat(ts_priv_t *priv, uint16_t pid)
{
	int x;

	if(priv->pat.progs == NULL)
		return -1;

	for(x = 0; x < priv->pat.progs_cnt; x++)
	{
		if(priv->pat.progs[x].pmt_pid == pid)
			return priv->pat.progs[x].id;
	}

	return -1;
}


static int parse_pat(ts_priv_t * priv, int is_start, unsigned char *buff, int size)
{
	int skip;
	unsigned char *ptr;
	unsigned char *base;
	int entries, i;
	uint16_t progid;
	struct pat_progs_t *tmp;
	ts_section_t *section;

	section = &(priv->pat.section);
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;
	
	ptr = &(section->buffer[skip]);
	//PARSING
	priv->pat.table_id = ptr[0];
	if(priv->pat.table_id != 0)
		return 0;
	priv->pat.ssi = (ptr[1] >> 7) & 0x1;
	priv->pat.curr_next = ptr[5] & 0x01;
	priv->pat.ts_id = (ptr[3]  << 8 ) | ptr[4];
	priv->pat.version_number = (ptr[5] >> 1) & 0x1F;
	priv->pat.section_length = ((ptr[1] & 0x03) << 8 ) | ptr[2];
	priv->pat.section_number = ptr[6];
	priv->pat.last_section_number = ptr[7];

	//check_crc32(0xFFFFFFFFL, ptr, priv->pat.buffer_len - 4, &ptr[priv->pat.buffer_len - 4]);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PAT: section_len: %d, section %d/%d\n", priv->pat.section_length, priv->pat.section_number, priv->pat.last_section_number);

	entries = (int) (priv->pat.section_length - 9) / 4;	//entries per section

	for(i=0; i < entries; i++)
	{
		int32_t idx;
		base = &ptr[8 + i*4];
		progid = (base[0] << 8) | base[1];

		if((idx = prog_idx_in_pat(priv, progid)) == -1)
		{
			int sz = sizeof(struct pat_progs_t) * (priv->pat.progs_cnt+1);
			tmp = realloc_struct(priv->pat.progs, priv->pat.progs_cnt+1, sizeof(struct pat_progs_t));
			if(tmp == NULL)
			{
				mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PAT: COULDN'T REALLOC %d bytes, NEXT\n", sz);
				break;
			}
			priv->pat.progs = tmp;
			idx = priv->pat.progs_cnt;
			priv->pat.progs_cnt++;
		}

		priv->pat.progs[idx].id = progid;
		priv->pat.progs[idx].pmt_pid = ((base[2]  & 0x1F) << 8) | base[3];
		mp_msg(MSGT_DEMUX, MSGL_V, "PROG: %d (%d-th of %d), PMT: %d\n", priv->pat.progs[idx].id, i+1, entries, priv->pat.progs[idx].pmt_pid);
	}

	return 1;
}


static inline int32_t es_pid_in_pmt(pmt_t * pmt, uint16_t pid)
{
	uint16_t i;

	if(pmt == NULL)
		return -1;

	if(pmt->es == NULL)
		return -1;

	for(i = 0; i < pmt->es_cnt; i++)
	{
		if(pmt->es[i].pid == pid)
			return (int32_t) i;
	}

	return -1;
}


static uint16_t get_mp4_desc_len(uint8_t *buf, int *len)
{
	//uint16_t i = 0, size = 0;
	int i = 0, j, size = 0;
	
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "PARSE_MP4_DESC_LEN(%d), bytes: ", *len);
	j = FFMIN(*len, 4);
	while(i < j)
	{
		mp_msg(MSGT_DEMUX, MSGL_DBG2, " %x ", buf[i]);
		size |= (buf[i] & 0x7f);
		if(!(buf[i] & 0x80))
			break;
		size <<= 7;
		i++;
	}
	mp_msg(MSGT_DEMUX, MSGL_DBG2, ", SIZE=%d\n", size);
	
	*len = i+1;
	return size;
}


uint16_t parse_mp4_slconfig_descriptor(uint8_t *buf, int len, void *elem)
{
	int i = 0;
	mp4_es_descr_t *es;
	mp4_sl_config_t *sl;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_SLCONFIG_DESCRIPTOR(%d)\n", len);
	es = (mp4_es_descr_t *) elem;
	if(!es)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}
	sl = &(es->sl);

	sl->ts_len = sl->ocr_len = sl->au_len = sl->instant_bitrate_len = sl->degr_len = sl->au_seqnum_len = sl->packet_seqnum_len = 0;
	sl->ocr = sl->dts = sl->cts = 0;
	
	if(buf[0] == 0)
	{
		i++;
		sl->flags = buf[i];
		i++;
		sl->ts_resolution = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->ocr_resolution = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->ts_len = buf[i];
		i++;
		sl->ocr_len = buf[i];
		i++;
		sl->au_len = buf[i];
		i++;
		sl->instant_bitrate_len = buf[i];
		i++;
		sl->degr_len = (buf[i] >> 4) & 0x0f;
		sl->au_seqnum_len = ((buf[i] & 0x0f) << 1) | ((buf[i+1] >> 7) & 0x01);
		i++;
		sl->packet_seqnum_len = ((buf[i] >> 2) & 0x1f);
		i++;
		
	}
	else if(buf[0] == 1)
	{
		sl->flags = 0;
		sl->ts_resolution = 1000;
		sl->ts_len = 32;
		i++;
	}
	else if(buf[0] == 2)
	{
		sl->flags = 4;
		i++;
	}
	else 
	{
		sl->flags = 0;
		i++;
	}
	
	sl->au_start = (sl->flags >> 7) & 0x1;
	sl->au_end = (sl->flags >> 6) & 0x1;
	sl->random_accesspoint = (sl->flags >> 5) & 0x1;
	sl->random_accesspoint_only = (sl->flags >> 4) & 0x1;
	sl->padding = (sl->flags >> 3) & 0x1;
	sl->use_ts = (sl->flags >> 2) & 0x1;
	sl->idle = (sl->flags >> 1) & 0x1;
	sl->duration = sl->flags & 0x1;
	
	if(sl->duration)
	{
		sl->timescale = (buf[i] << 24) | (buf[i+1] << 16) | (buf[i+2] << 8) | buf[i+3];
		i += 4;
		sl->au_duration = (buf[i] << 8) | buf[i+1];
		i += 2;
		sl->cts_duration = (buf[i] << 8) | buf[i+1];
		i += 2; 
	}
	else	//no support for fixed durations atm
		sl->timescale = sl->au_duration = sl->cts_duration = 0;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "MP4SLCONFIG(len=0x%x), predef: %d, flags: %x, use_ts: %d, tslen: %d, timescale: %d, dts: %"PRIu64", cts: %"PRIu64"\n", 
		len, buf[0], sl->flags, sl->use_ts, sl->ts_len, sl->timescale, (uint64_t) sl->dts, (uint64_t) sl->cts);
	
	return len;
}

int parse_mp4_descriptors(pmt_t *pmt, uint8_t *buf, int len, void *elem);

uint16_t parse_mp4_decoder_config_descriptor(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i = 0, j;
	mp4_es_descr_t *es;
	mp4_decoder_config_t *dec;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DECODER_CONFIG_DESCRIPTOR(%d)\n", len);
	es = (mp4_es_descr_t *) elem;
	if(!es)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}
	dec = (mp4_decoder_config_t*) &(es->decoder);
	
	dec->object_type = buf[i];
	dec->stream_type =  (buf[i+1]>>2) & 0x3f;
	
	if(dec->object_type == 1 && dec->stream_type == 1)
	{
		 dec->object_type = MP4_OD;
		 dec->stream_type = MP4_OD;
	}
	else if(dec->stream_type == 4)
	{
		if(dec->object_type == 0x6a)
			dec->object_type = VIDEO_MPEG1;
		if(dec->object_type >= 0x60 && dec->object_type <= 0x65)
			dec->object_type = VIDEO_MPEG2;
		else if(dec->object_type == 0x20)
			dec->object_type = VIDEO_MPEG4;
		else if(dec->object_type == 0x21)
			dec->object_type = VIDEO_AVC;
		else dec->object_type = UNKNOWN;
	}
	else if(dec->stream_type == 5)
	{
		if(dec->object_type == 0x40)
			dec->object_type = AUDIO_AAC;
		else if(dec->object_type == 0x6b)
			dec->object_type = AUDIO_MP2;
		else if(dec->object_type >= 0x66 && dec->object_type <= 0x69)
			dec->object_type = AUDIO_MP2;
		else
			dec->object_type = UNKNOWN;
	}
//	else
//	{
//		 dec->object_type = MP4_OD;
//		 dec->stream_type = MP4_OD;
//	}

//		dec->object_type = dec->stream_type = UNKNOWN;
	
	if(dec->object_type != UNKNOWN)
	{
		//update the type of the current stream
		for(j = 0; j < pmt->es_cnt; j++)
		{
			if(pmt->es[j].mp4_es_id == es->id)
			{
				pmt->es[j].type = SL_PES_STREAM;
			}
		}
	}
	
	if(len > 13)
		parse_mp4_descriptors(pmt, &buf[13], len-13, dec);
	
	mp_msg(MSGT_DEMUX, MSGL_V, "MP4DECODER(0x%x), object_type: 0x%x, stream_type: 0x%x\n", len, dec->object_type, dec->stream_type);
	
	return len;
}

uint16_t parse_mp4_decoder_specific_descriptor(uint8_t *buf, int len, void *elem)
{
	int i;
	mp4_decoder_config_t *dec;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DECODER_SPECIFIC_DESCRIPTOR(%d)\n", len);
	dec = (mp4_decoder_config_t *) elem;
	if(!dec)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "argh! NULL elem passed, skip\n");
		return len;
	}
	
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "MP4 SPECIFIC INFO BYTES: \n");
	for(i=0; i<len; i++)
		mp_msg(MSGT_DEMUX, MSGL_DBG2, "%02x ", buf[i]);
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "\n");

	if(len > MAX_EXTRADATA_SIZE)
	{
		mp_msg(MSGT_DEMUX, MSGL_ERR, "DEMUX_TS, EXTRADATA SUSPICIOUSLY BIG: %d, REFUSED\r\n", len);
		return len;
	}
	memcpy(dec->buf, buf, len);
	dec->buf_size = len;
	
	return len;
}

uint16_t parse_mp4_es_descriptor(pmt_t *pmt, uint8_t *buf, int len)
{
	int i = 0, j = 0, k, found;
	uint8_t flag;
	mp4_es_descr_t es, *target_es = NULL, *tmp;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4ES: len=%d\n", len);
	memset(&es, 0, sizeof(mp4_es_descr_t));
	while(i < len)
	{
		es.id = (buf[i] << 8) | buf[i+1];
		mp_msg(MSGT_DEMUX, MSGL_V, "MP4ES_ID: %d\n", es.id);
		i += 2;
		flag = buf[i];
		i++;
		if(flag & 0x80)
			i += 2;
		if(flag & 0x40)
			i += buf[i]+1;
		if(flag & 0x20)		//OCR, maybe we need it
			i += 2;
		
		j = parse_mp4_descriptors(pmt, &buf[i], len-i, &es);
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4ES, types after parse_mp4_descriptors: 0x%x, 0x%x\n", es.decoder.object_type, es.decoder.stream_type);
		if(es.decoder.object_type != UNKNOWN && es.decoder.stream_type != UNKNOWN)
		{
			found = 0;
			//search this ES_ID if we already have it
			for(k=0; k < pmt->mp4es_cnt; k++)
			{
				if(pmt->mp4es[k].id == es.id)
				{
					target_es = &(pmt->mp4es[k]);
					found = 1;
				}
			}
			
			if(! found)
			{
				tmp = realloc_struct(pmt->mp4es, pmt->mp4es_cnt+1, sizeof(mp4_es_descr_t));
				if(tmp == NULL)
				{
					fprintf(stderr, "CAN'T REALLOC MP4_ES_DESCR\n");
					continue;
				}
				pmt->mp4es = tmp;
				target_es = &(pmt->mp4es[pmt->mp4es_cnt]);
				pmt->mp4es_cnt++;
			}
			memcpy(target_es, &es, sizeof(mp4_es_descr_t));
			mp_msg(MSGT_DEMUX, MSGL_V, "MP4ES_CNT: %d, ID=%d\n", pmt->mp4es_cnt, target_es->id);
		}

		i += j;
	}
	
	return len;
}

void parse_mp4_object_descriptor(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i, j = 0, id;
	
	i=0;
	id = (buf[0] << 2) | ((buf[1] & 0xc0) >> 6);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_OBJECT_DESCRIPTOR: len=%d, OD_ID=%d\n", len, id);
	if(buf[1] & 0x20)
	{
		i += buf[2] + 1;	//url
		mp_msg(MSGT_DEMUX, MSGL_V, "URL\n");
	}
	else
	{
		i = 2;
		
		while(i < len)
		{
			j = parse_mp4_descriptors(pmt, &(buf[i]), len-i, elem);
			mp_msg(MSGT_DEMUX, MSGL_V, "OBJD, NOW i = %d, j=%d, LEN=%d\n", i, j, len);
			i += j;
		}
	}
}


void parse_mp4_iod(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int i, j = 0;
	mp4_od_t *iod = &(pmt->iod);
	
	iod->id = (buf[0] << 2) | ((buf[1] & 0xc0) >> 6);
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_IOD: len=%d, IOD_ID=%d\n", len, iod->id);
	i = 2;
	if(buf[1] & 0x20)
	{
		i += buf[2] + 1;	//url
		mp_msg(MSGT_DEMUX, MSGL_V, "URL\n");
	}
	else
	{
		i = 7;
		while(i < len)
		{
			j = parse_mp4_descriptors(pmt, &(buf[i]), len-i, elem);
			mp_msg(MSGT_DEMUX, MSGL_V, "IOD, NOW i = %d, j=%d, LEN=%d\n", i, j, len);
			i += j;
		}
	}
}

int parse_mp4_descriptors(pmt_t *pmt, uint8_t *buf, int len, void *elem)
{
	int tag, descr_len, i = 0, j = 0;
	
	mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_MP4_DESCRIPTORS, len=%d\n", len);
	if(! len)
		return len;
	
	while(i < len)
	{
		tag = buf[i];
		j = len - i -1;
		descr_len = get_mp4_desc_len(&(buf[i+1]), &j);
		mp_msg(MSGT_DEMUX, MSGL_V, "TAG=%d (0x%x), DESCR_len=%d, len=%d, j=%d\n", tag, tag, descr_len, len, j);
		if(descr_len > len - j+1)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "descriptor is too long, exit\n");
			return len;
		}
		i += j+1;
		
		switch(tag)
		{
			case 0x1:
 				parse_mp4_object_descriptor(pmt, &(buf[i]), descr_len, elem);
				break;
			case 0x2:
 				parse_mp4_iod(pmt, &(buf[i]), descr_len, elem);
				break;
			case 0x3:
				parse_mp4_es_descriptor(pmt, &(buf[i]), descr_len);
				break;
			case 0x4:
				parse_mp4_decoder_config_descriptor(pmt, &buf[i], descr_len, elem);
				break;
			case 0x05:
 				parse_mp4_decoder_specific_descriptor(&buf[i], descr_len, elem);
				break;
			case 0x6:
 				parse_mp4_slconfig_descriptor(&buf[i], descr_len, elem);
				break;
			default:
				mp_msg(MSGT_DEMUX, MSGL_V, "Unsupported mp4 descriptor 0x%x\n", tag);
		}
		i += descr_len;
	}
	
	return len;
}

static ES_stream_t *new_pid(ts_priv_t *priv, int pid)
{
	ES_stream_t *tss;

	printf("new_pid %d \n",pid);
	tss = malloc(sizeof(ES_stream_t));
	if(! tss)
		return NULL;
	memset(tss, 0, sizeof(ES_stream_t));
	tss->pid = pid;
	tss->last_cc = -1;
	tss->type = UNKNOWN;
	tss->subtype = UNKNOWN;
	tss->is_synced = 0;
	tss->extradata = NULL;
	tss->extradata_alloc = tss->extradata_len = 0;
	priv->ts.pids[pid] = tss;
	
	return tss;
}


static int parse_program_descriptors(pmt_t *pmt, uint8_t *buf, uint16_t len)
{
	uint16_t i = 0, k, olen = len;

	while(len > 0)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "PROG DESCR, TAG=%x, LEN=%d(%x)\n", buf[i], buf[i+1], buf[i+1]);
		if(buf[i+1] > len-2)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "ERROR, descriptor len is too long, skipping\n");
			return olen;
		}

		if(buf[i] == 0x1d)
		{
			if(buf[i+3] == 2)	//buggy versions of vlc muxer make this non-standard mess (missing iod_scope)
				k = 3;
			else
				k = 4;		//this is standard compliant
			parse_mp4_descriptors(pmt, &buf[i+k], (int) buf[i+1]-(k-2), NULL);
		}

		len -= 2 + buf[i+1];
	}
	
	return olen;
}

static int parse_descriptors(struct pmt_es_t *es, uint8_t *ptr)
{
	int j, descr_len, len;

	j = 0;
	len = es->descr_length;
	while(len > 2)
	{
		descr_len = ptr[j+1];
		mp_msg(MSGT_DEMUX, MSGL_V, "...descr id: 0x%x, len=%d\n", ptr[j], descr_len);
		if(descr_len > len)
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "INVALID DESCR LEN for tag %02x: %d vs %d max, EXIT LOOP\n", ptr[j], descr_len, len);
			return -1;
		}


		if(ptr[j] == 0x6a || ptr[j] == 0x7a)	//A52 Descriptor
		{
			if(es->type == 0x6)
			{
				es->type = AUDIO_A52;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "DVB A52 Descriptor\n");
			}
		}
		else if(ptr[j] == 0x7b)	//DVB DTS Descriptor
		{
			if(es->type == 0x6)
			{
				es->type = AUDIO_DTS;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "DVB DTS Descriptor\n");
			}
		}
		else if(ptr[j] == 0x59)	//Subtitling Descriptor
		{
			uint8_t subtype;

			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Subtitling Descriptor\n");
			if(descr_len < 8)
			{
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "Descriptor length too short for DVB Subtitle Descriptor: %d, SKIPPING\n", descr_len);
			}
			else
			{
				memcpy(es->lang, &ptr[j+2], 3);
				es->lang[3] = 0;
				subtype = ptr[j+5];
				if(
					(subtype >= 0x10 && subtype <= 0x13) ||
					(subtype >= 0x20 && subtype <= 0x23)
				)
				{
					es->type = SPU_DVB;
					//page parameters: compo page 2 bytes, ancillary page 2 bytes
				}
				else
					es->type = UNKNOWN;
			}
		}
		else if(ptr[j] == 0x50)	//Component Descriptor
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Component Descriptor\n");
			memcpy(es->lang, &ptr[j+5], 3);
			es->lang[3] = 0;
		}
		else if(ptr[j] == 0xa)	//Language Descriptor
		{
			memcpy(es->lang, &ptr[j+2], 3);
			es->lang[3] = 0;
			mp_msg(MSGT_DEMUX, MSGL_V, "Language Descriptor: %s\n", es->lang);
		}
		else if(ptr[j] == 0x5)	//Registration Descriptor (looks like e fourCC :) )
		{
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Registration Descriptor\n");
			if(descr_len < 4)
			{
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "Registration Descriptor length too short: %d, SKIPPING\n", descr_len);
			}
			else
			{
				char *d;
				memcpy(es->format_descriptor, &ptr[j+2], 4);
				es->format_descriptor[4] = 0;

				d = &ptr[j+2];
				if(d[0] == 'A' && d[1] == 'C' && d[2] == '-' && d[3] == '3')
				{
					es->type = AUDIO_A52;
				}
				else if(d[0] == 'D' && d[1] == 'T' && d[2] == 'S' && d[3] == '1')
				{
					es->type = AUDIO_DTS;
				}
				else if(d[0] == 'D' && d[1] == 'T' && d[2] == 'S' && d[3] == '2')
				{
					es->type = AUDIO_DTS;
				}
				else if(d[0] == 'V' && d[1] == 'C' && d[2] == '-' && d[3] == '1')
				{
					es->type = VIDEO_VC1;
				}
				else
					es->type = UNKNOWN;
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "FORMAT %s\n", es->format_descriptor);
			}
		}
		else if(ptr[j] == 0x1e)
		{
			es->mp4_es_id = (ptr[j+2] << 8) | ptr[j+3];
			mp_msg(MSGT_DEMUX, MSGL_V, "SL Descriptor: ES_ID: %d(%x), pid: %d\n", es->mp4_es_id, es->mp4_es_id, es->pid);
		}
		else
			mp_msg(MSGT_DEMUX, MSGL_DBG2, "Unknown descriptor 0x%x, SKIPPING\n", ptr[j]);

		len -= 2 + descr_len;
		j += 2 + descr_len;
	}

	return 1;
}

static int parse_sl_section(pmt_t *pmt, ts_section_t *section, int is_start, unsigned char *buff, int size)
{
	int tid, len, skip;
	uint8_t *ptr;
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;
		
	ptr = &(section->buffer[skip]);
	tid = ptr[0];
	len = ((ptr[1] & 0x0f) << 8) | ptr[2];
	mp_msg(MSGT_DEMUX, MSGL_V, "TABLEID: %d (av. %d), skip=%d, LEN: %d\n", tid, section->buffer_len, skip, len);
	if(len > 4093 || section->buffer_len < len || tid != 5)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "SECTION TOO LARGE or wrong section type, EXIT\n");
		return 0;
	}
	
	if(! (ptr[5] & 1))
		return 0;
	
	//8 is the current position, len - 9 is the amount of data available
	parse_mp4_descriptors(pmt, &ptr[8], len - 9, NULL);
	
	return 1;
}

static int parse_pmt(ts_priv_t * priv, uint16_t progid, uint16_t pid, int is_start, unsigned char *buff, int size)
{
	unsigned char *base, *es_base;
	pmt_t *pmt;
	int32_t idx, es_count, section_bytes;
	uint8_t m=0;
	int skip;
	pmt_t *tmp;
	struct pmt_es_t *tmp_es;
	ts_section_t *section;
	ES_stream_t *tss;
	
	idx = progid_idx_in_pmt(priv, progid);

	if(idx == -1)
	{
		int sz = (priv->pmt_cnt + 1) * sizeof(pmt_t);
		tmp = realloc_struct(priv->pmt, priv->pmt_cnt + 1, sizeof(pmt_t));
		if(tmp == NULL)
		{
			mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PMT: COULDN'T REALLOC %d bytes, NEXT\n", sz);
			return 0;
		}
		priv->pmt = tmp;
		idx = priv->pmt_cnt;
		memset(&(priv->pmt[idx]), 0, sizeof(pmt_t));
		priv->pmt_cnt++;
		priv->pmt[idx].progid = progid;
	}

	pmt = &(priv->pmt[idx]);

	section = &(pmt->section);
	skip = collect_section(section, is_start, buff, size);
	if(! skip)
		return 0;
		
	base = &(section->buffer[skip]);

	mp_msg(MSGT_DEMUX, MSGL_V, "FILL_PMT(prog=%d), PMT_len: %d, IS_START: %d, TS_PID: %d, SIZE=%d, M=%d, ES_CNT=%d, IDX=%d, PMT_PTR=%p\n",
		progid, pmt->section.buffer_len, is_start, pid, size, m, pmt->es_cnt, idx, pmt);

	pmt->table_id = base[0];
	if(pmt->table_id != 2)
		return -1;
	pmt->ssi = base[1] & 0x80;
	pmt->section_length = (((base[1] & 0xf) << 8 ) | base[2]);
	pmt->version_number = (base[5] >> 1) & 0x1f;
	pmt->curr_next = (base[5] & 1);
	pmt->section_number = base[6];
	pmt->last_section_number = base[7];
	pmt->PCR_PID = ((base[8] & 0x1f) << 8 ) | base[9];
	pmt->prog_descr_length = ((base[10] & 0xf) << 8 ) | base[11];
	if(pmt->prog_descr_length > pmt->section_length - 9)
	{
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT, INVALID PROG_DESCR LENGTH (%d vs %d)\n", pmt->prog_descr_length, pmt->section_length - 9);
		return -1;
	}

	if(pmt->prog_descr_length)
		parse_program_descriptors(pmt, &base[12], pmt->prog_descr_length);

	es_base = &base[12 + pmt->prog_descr_length];	//the beginning of th ES loop

	section_bytes= pmt->section_length - 13 - pmt->prog_descr_length;
	es_count  = 0;

	while(section_bytes >= 5)
	{
		int es_pid, es_type;

		es_type = es_base[0];
		es_pid = ((es_base[1] & 0x1f) << 8) | es_base[2];

		idx = es_pid_in_pmt(pmt, es_pid);
		if(idx == -1)
		{
			int sz = sizeof(struct pmt_es_t) * (pmt->es_cnt + 1);
			tmp_es = realloc_struct(pmt->es, pmt->es_cnt + 1, sizeof(struct pmt_es_t));
			if(tmp_es == NULL)
			{
				mp_msg(MSGT_DEMUX, MSGL_ERR, "PARSE_PMT, COULDN'T ALLOCATE %d bytes for PMT_ES\n", sz);
				continue;
			}
			pmt->es = tmp_es;
			idx = pmt->es_cnt;
			memset(&(pmt->es[idx]), 0, sizeof(struct pmt_es_t));
			pmt->es_cnt++;
		}

		pmt->es[idx].descr_length = ((es_base[3] & 0xf) << 8) | es_base[4];


		if(pmt->es[idx].descr_length > section_bytes - 5)
		{
			mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT, ES_DESCR_LENGTH TOO LARGE %d > %d, EXIT\n",
				pmt->es[idx].descr_length, section_bytes - 5);
			return -1;
		}


		pmt->es[idx].pid = es_pid;
		if(es_type != 0x6)
			pmt->es[idx].type = UNKNOWN;
		else
			pmt->es[idx].type = es_type;
		
		if ( parse_descriptors(&pmt->es[idx], &es_base[5]) < 0 ) return -1;

		switch(es_type)
		{
			case 1:
				pmt->es[idx].type = VIDEO_MPEG1;
				break;
			case 2:
				pmt->es[idx].type = VIDEO_MPEG2;
				break;
			case 3:
			case 4:
				pmt->es[idx].type = AUDIO_MP2;
				break;
			case 6:
				if(pmt->es[idx].type == 0x6)	//this could have been ovrwritten by parse_descriptors
					pmt->es[idx].type = UNKNOWN;
				break;
			case 0x10:
				pmt->es[idx].type = VIDEO_MPEG4;
				break;
			case 0x0f:
			case 0x11:
				pmt->es[idx].type = AUDIO_AAC;
				break;
			case 0x1b:
				pmt->es[idx].type = VIDEO_H264;
				break;
			case 0x12:
				pmt->es[idx].type = SL_PES_STREAM;
				break;
			case 0x13:
				pmt->es[idx].type = SL_SECTION;
				break;
			case 0x81:
				pmt->es[idx].type = AUDIO_A52;
				break;
			case 0x8A:
				pmt->es[idx].type = AUDIO_DTS;
				break;
			case 0xEA:
				pmt->es[idx].type = VIDEO_VC1;
				break;
			default:
				mp_msg(MSGT_DEMUX, MSGL_DBG2, "UNKNOWN ES TYPE=0x%x\n", es_type);
				pmt->es[idx].type = UNKNOWN;
				return -1;
		}
		
		tss = priv->ts.pids[es_pid];			//an ES stream
//		printf("22222222222 %d \n",es_pid);
		if ( es_pid != 0x00 &&
		     es_pid != 0x10 &&
		     es_pid != 0x20 &&
		     es_pid != 0x30 &&
		     es_pid != 0x40 &&
		     es_pid != 0x50)
			return -1;

		if(tss == NULL)
		{
			tss = new_pid(priv, es_pid);
			if(tss)
				tss->type = pmt->es[idx].type;
		}

		section_bytes -= 5 + pmt->es[idx].descr_length;
		mp_msg(MSGT_DEMUX, MSGL_V, "PARSE_PMT(%d INDEX %d), STREAM: %d, FOUND pid=0x%x (%d), type=0x%x, ES_DESCR_LENGTH: %d, bytes left: %d\n",
			progid, idx, es_count, pmt->es[idx].pid, pmt->es[idx].pid, pmt->es[idx].type, pmt->es[idx].descr_length, section_bytes);


		es_base += 5 + pmt->es[idx].descr_length;

		es_count++;
	}

	mp_msg(MSGT_DEMUX, MSGL_V, "----------------------------\n");
	return 1;
}

static pmt_t* pmt_of_pid(ts_priv_t *priv, int pid, mp4_decoder_config_t **mp4_dec)
{
	int32_t i, j, k;

	if(priv->pmt)
	{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			if(priv->pmt[i].es && priv->pmt[i].es_cnt)
			{
				for(j = 0; j < priv->pmt[i].es_cnt; j++)
				{
					if(priv->pmt[i].es[j].pid == pid)
					{
						//search mp4_es_id
						if(priv->pmt[i].es[j].mp4_es_id)
						{
							for(k = 0; k < priv->pmt[i].mp4es_cnt; k++)
							{
								if(priv->pmt[i].mp4es[k].id == priv->pmt[i].es[j].mp4_es_id)
								{
									*mp4_dec = &(priv->pmt[i].mp4es[k].decoder);
									break;
								}
							}
						}
						
						return &(priv->pmt[i]);
					}
				}
			}
		}
	}
	
	return NULL;
}


static inline int32_t pid_type_from_pmt(ts_priv_t *priv, int pid)
{
	int32_t pmt_idx, pid_idx, i, j;

	pmt_idx = progid_idx_in_pmt(priv, priv->prog);

	if(pmt_idx != -1)
	{
		pid_idx = es_pid_in_pmt(&(priv->pmt[pmt_idx]), pid);
		if(pid_idx != -1)
			return priv->pmt[pmt_idx].es[pid_idx].type;
	}
	//else
	//{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			pmt_t *pmt = &(priv->pmt[i]);
			for(j = 0; j < pmt->es_cnt; j++)
				if(pmt->es[j].pid == pid)
					return pmt->es[j].type;
		}
	//}

	return UNKNOWN;
}


static inline uint8_t *pid_lang_from_pmt(ts_priv_t *priv, int pid)
{
	int32_t pmt_idx, pid_idx, i, j;

	pmt_idx = progid_idx_in_pmt(priv, priv->prog);

	if(pmt_idx != -1)
	{
		pid_idx = es_pid_in_pmt(&(priv->pmt[pmt_idx]), pid);
		if(pid_idx != -1)
			return priv->pmt[pmt_idx].es[pid_idx].lang;
	}
	else
	{
		for(i = 0; i < priv->pmt_cnt; i++)
		{
			pmt_t *pmt = &(priv->pmt[i]);
			for(j = 0; j < pmt->es_cnt; j++)
				if(pmt->es[j].pid == pid)
					return pmt->es[j].lang;
		}
	}

	return NULL;
}

//double a_max = 0.06,a_avg = 0.06,a_last = 0;
//double v_max = 0.033,v_avg = 0.033,v_last = 0;

#define MAX_DIFF_PTS 3.0
#undef DO_CORRECT_PTS

void ts_flush_fifo( demux_ts_fifo_t * f )
{
	demux_ts_fifo_t *af,*vf;
	af = &a_fifo;
	vf = &v_fifo;
	af->first = af->last;
	af->num = 0;
	vf->first = vf->last;
	vf->num = 0;
}

static double get_locked_ocr()
{
	return locked_ocr;
}

static int filte_packet_old(demux_packet_t *dp,int media_type)
{
	double de;
	demux_ts_fifo_t *f;

	if (locked_ocr == 0 ) return 0;
	if ( media_type == 2 ) f = &a_fifo;
	else f = &v_fifo;

	de = (double)GetTimer()/1000000.0 - locked_ocr - dp->pts;
//	printf("de %f %f \n",de,dp->pts);
	if ( abs(de) > MAX_DIFF_PTS )              //check diff > max
	{
#ifndef DO_CORRECT_PTS
		printf("diff > max \n");
		return 0;
#else
		printf("correct a error pts %f \n",dp->pts);
		new_pts = ds->last->pts;
		if ( ds == demuxer->video)
			dp->pts = new_pts + 0.0335;
		else
			dp->pts = new_pts + 0.055;
		printf("correct a error pts to %f \n",dp->pts);		
		return 1;
#endif
	}

	if ( f->num < 1 ) return 1;
	if ( f->last->pts == dp->pts )             // check diff = 0
	{
#ifndef DO_CORRECT_PTS
		printf("diff == 0 \n");
		return 0;
#else
		if ( ds->last->len == dp->len )     //an old packet !
		{
			printf("Throw a old packet! \n");
			return 0;
		}
		else              //an pts error paket!
 		{
			printf("correct an pts 0000 packet! \n");
			dp->pts = ds->last->pts +0.0335;
			return 1;
		}
#endif
	}
	de = (double)GetTimer()/1000000.0 - locked_ocr - dp->pts;
	if ( de > 1 )           //check late packet
	{
		printf("Demuxer drop a late packet! \n");
		return 0;
	}

	if ( f->last->pts > dp->pts )             // check diff < 0
	{
#ifndef DO_CORRECT_PTS
		printf("diff < 0 \n");
		return 0;
#else
		// how? 
		;
#endif
	}
	return 1;
}

static int filte_packet(demux_packet_t *dp,int media_type)
{
	double de;
	demux_ts_fifo_t *f;

	if (locked_ocr == 0 ) return 0;
	if ( media_type == 2 ) f = &a_fifo;
	else f = &v_fifo;

	if ( f->num < 1 ) return 1;

	de = (double)GetTimer()/1000000.0 - locked_ocr - dp->pts;
//	printf("de %f \n",de);
	if ( abs(de) > MAX_DIFF_PTS )              //check diff > max
	{
//		printf("diff > max \n");
//		dp->pts = f->last->pts;
		return 0;
	}
	else if ( f->last->pts > dp->pts )             // check diff < 0
	{
//		printf("diff < 0 \n");
//		dp->pts = f->last->pts;
		return 0;
	}
	return 1;
}

int ts_packet_fifo_init(demux_ts_fifo_t * f,int size)
{
	int i;
	demux_ts_packet_t *mp, *mpp, *mppp;
	mppp = mpp = malloc(sizeof(demux_ts_packet_t));
	for ( i = 0; i < MAX_DEMUXER_FIFO_SIZE; i ++ )
	{
		printf(" fifo %x \n",mpp);
		mp = mpp;
		mp->buf = malloc(sizeof(unsigned char)*size);
		if ( !mp->buf )
		{
			printf("Demuxer can not malloc buffer! \n");
			return 0;
		}
//		else printf(" mp->buf %x \n",mp->buf);
		mpp = malloc(sizeof(demux_ts_packet_t));
		mp->next = mpp;
		mpp->priv = mp;
	}
	mp->next = mppp;
	mppp->priv = mp;
	f->first = f->last = mppp;
	f->num = 0;
	printf(" fifo init %x \n",f);
	free(mpp);
	return 1;
}
static int demux_ts_fill_buffer(demuxer_t * demuxer, demux_stream_t *ds);

int ts_add_packet(demuxer_t *demuxer, demux_stream_t *ds, demux_packet_t *dp, int offset)
{
//	printf("offset %d %d %d \n",offset,dp->len,a_fifo.last->buf);
	if ( ds == demuxer->audio )
	{
		if (a_fifo.num > MAX_DEMUXER_FIFO_SIZE) return 0;
		a_fifo.last->pts = dp->pts; 
		a_fifo.last->len = dp->len;
		if ( a_fifo.last->len >MAX_AUDIO_FIFO_BUFSIZE ) a_fifo.last->len = MAX_AUDIO_FIFO_BUFSIZE;
		memcpy(a_fifo.last->buf, dp->buffer, a_fifo.last->len);
		a_fifo.num ++;
		a_fifo.last = a_fifo.last->next;
	}
	else
	{
		if (v_fifo.num > MAX_DEMUXER_FIFO_SIZE) return 0;
		v_fifo.last->pts = dp->pts;
		v_fifo.last->len = dp->len;		
		if ( v_fifo.last->len >MAX_VIDEO_FIFO_BUFSIZE ) v_fifo.last->len = MAX_VIDEO_FIFO_BUFSIZE;
		memcpy(v_fifo.last->buf, dp->buffer, v_fifo.last->len);
		v_fifo.num ++;
		v_fifo.last = v_fifo.last->next;
	}
	return 1;
}

int ts_get_packet(demuxer_t *demuxer, demux_stream_t *ds,unsigned char **start,double *pts)
{
	demux_ts_fifo_t *f;
	int len;

	if ( ds == demuxer->audio ) f = &a_fifo;
	else f = &v_fifo;

//	printf("f->num %d \n",f->num);
//	while(f->num<1)
//		demux_ts_fill_buffer(demuxer,ds);

	*start = f->first->buf;
	*pts = f->first->pts;
	f->num --;
	len = f->first->len;
	f->first = f->first->next;
	return len;
}

int ts_get_first_pts( demuxer_t *demuxer ,double * delay)
{
	demux_ts_fifo_t *af,*vf;
//	double t = GetTimer();

	af = &a_fifo;
	vf = &v_fifo;

	while(af->num<2 || vf->num<2)
	{
		demux_ts_fill_buffer(demuxer,0);
	}
//	printf("dt %f \n",GetTimer() - t);
//	printf("diff av %f %f \n",af->first->pts , vf->first->pts);
//	t = (double)GetTimer()/1000000.0 - locked_ocr - mf->out->pts;

	if ( af->first->pts <= vf->first->pts )
	{
		* delay = vf->first->pts - af->first->pts;
		return 1;
	}
	else
	{
		* delay = af->first->pts - vf->first->pts;
		return 2;
	}
}

static double ts_query_speed(stream_t *s)
{
	struct fifo
	{
		int total;
		int num;
	};
	struct fifo state;

	ioctl( s->fd , 0x4, (void *)&state);
	printf("%d\n",state.num);
	return (float)state.num / (float)state.total;

}
 
static void ts_init_func(mts_func_t *mts)
{
	mts->get_first_pts = ts_get_first_pts;
	mts->get_packet    = ts_get_packet;
	mts->query_speed   = ts_query_speed;
	mts->locked_ocr    = &locked_ocr;
	mts->err_degree    = &err_degree;
	mts->packet_cnt    = &packet_cnt;
	mts->packet_err    = &pack_err;

//	printf("??????????????????????? %x \n",mts->err_degree);
}

int fill_packet(demux_packet_t *dp, int media_type)
{
	int ret = 0;
	pts_packet_t *correct_dp;
	demux_ts_fifo_t *f;

	if ( media_type == 1 )          //Video
	{
		if ( !filte_packet(dp,media_type) ) 
		{
//			printf("dv\n");
			return 0;
		}
		if (v_fifo.num > MAX_DEMUXER_FIFO_SIZE - 2 ) return 1;
		v_fifo.last->pts = dp->pts;
		v_fifo.last->len = dp->len;		
		if ( v_fifo.last->len >MAX_VIDEO_FIFO_BUFSIZE ) v_fifo.last->len = MAX_VIDEO_FIFO_BUFSIZE;
		v_fifo.num ++;
#if 0
		printf("add video %f ",v_fifo.last->pts);
		printf("%2x %2x %2x %2x \n",
		       v_fifo.last->buf[0],v_fifo.last->buf[1],v_fifo.last->buf[2],v_fifo.last->buf[3]);
#endif
		v_fifo.last = v_fifo.last->next;
	}
	else                          //Audio
	{
		if ( !filte_packet(dp,media_type) ) 
		{
//			printf("da\n");
			return 0;
		}
		if (a_fifo.num > MAX_DEMUXER_FIFO_SIZE - 2 ) return 1;
		a_fifo.last->pts = dp->pts; 
		a_fifo.last->len = dp->len;
		if ( a_fifo.last->len >MAX_AUDIO_FIFO_BUFSIZE ) a_fifo.last->len = MAX_AUDIO_FIFO_BUFSIZE;
		a_fifo.num ++;
#if 0
		printf("add audio %f ",a_fifo.last->pts);
		printf("%2x %2x %2x %2x \n",
		       a_fifo.last->buf[0],a_fifo.last->buf[1],a_fifo.last->buf[2],a_fifo.last->buf[3]);
#endif
		a_fifo.last = a_fifo.last->next;
	}
	return 1;
}

static int fill_extradata(mp4_decoder_config_t * mp4_dec, ES_stream_t *tss)
{
	uint8_t *tmp;
	
	mp_msg(MSGT_DEMUX, MSGL_DBG2, "MP4_dec: %p, pid: %d\n", mp4_dec, tss->pid);
		
	if(mp4_dec->buf_size > tss->extradata_alloc)
	{
		tmp = (uint8_t *) realloc(tss->extradata, mp4_dec->buf_size);
		if(!tmp)
			return 0;
		tss->extradata = tmp;
		tss->extradata_alloc = mp4_dec->buf_size;
	}
	memcpy(tss->extradata, mp4_dec->buf, mp4_dec->buf_size);
	tss->extradata_len = mp4_dec->buf_size;
	mp_msg(MSGT_DEMUX, MSGL_V, "EXTRADATA: %p, alloc=%d, len=%d\n", tss->extradata, tss->extradata_alloc, tss->extradata_len);
	
	return tss->extradata_len;
}

void parse_adap_field(demuxer_t *demuxer)      //add by yliu for PCR
{
	unsigned char buf[10];
	int64_t base = 0,exten = 0;
//	double pcr;
	stream_t *s = demuxer->stream;
	ts_priv_t *priv = (ts_priv_t*) demuxer->priv;
	unsigned int pcr;
//	stream_read(stream, buf, 1);
	buf[0]=s->buffer[s->buf_pos];
	if ( buf[0] == 7 ) {
//		stream_read(s, buf, 7) ;
//		buf[0] = stream_read_char(s);
		buf[1] = s->buffer[s->buf_pos+1];
		buf[2] = s->buffer[s->buf_pos+2];
		buf[3] = s->buffer[s->buf_pos+3];
		buf[4] = s->buffer[s->buf_pos+4];
		buf[5] = s->buffer[s->buf_pos+5];
		buf[6] = s->buffer[s->buf_pos+6];
		buf[7] = s->buffer[s->buf_pos+7];
		if (  buf[2] == 0xff && buf[3] == 0xff && buf[4] == 0xff && 
		      buf[5] == 0xff && buf[6] == 0xff && buf[7] == 0xff  )
			return ;
//		printf("pcr len %d \n",buf[0]);

//		printf(" PCR :%x %x %x %x %x %x \n",
//		       buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);

		base = (unsigned long long)buf[2] << 25 ;
		base |= (unsigned long long)buf[3] << 17 ;
		base |= (unsigned long long)buf[4] << 9 ;
		base |= (unsigned long long)buf[5] << 1 ;
		base |= ( buf[6] & 0x80 ) >> 7;
//		base = 0;
//		exten = ( (int64_t) buf[7] & 0x01 ) << 9;
//		exten |= buf[8] ;
//		pcr = base ;
//		priv->pcr = pcr;
//		demuxer->stream_pts = pcr ;
//		if ( priv->vsh ) { 
//			priv->vsh->timer = pcr ;
//			printf("PCR read base :%f %d\n",
//			       priv->vsh->timer,base);
//		}
		pcr = base / 90000;
//		printf("PCR read base :%x %x %x \n",pcr - 0x5e70,base,pcr);
	}
} 

extern char* current_module;

static int get_data_from_pes(stream_t *stream,int buf_size,unsigned char *raw_data,int *buf_pos,double * pts)
{
//	unsigned char buf[188],p[188],ocr_flag,cts_flag = 1;
	unsigned char *buf,p[188],ocr_flag,cts_flag = 1;
	int pes_len,head_len,n,m,len;
	uint64_t v;
//	current_module="sl_packet";

	len = ts_stream_read(stream, p, 4);      //read first 4 byte of pes packet!
	pes_len = ts_stream_read_char(stream) << 8 | ts_stream_read_char(stream);

	ts_stream_read_char(stream);
	ts_stream_read_char(stream);
	head_len = ts_stream_read_char(stream);
	pes_len -= (head_len + 3);            //real pes_len
	buf_size -= 9;                        //TS packet payload len

//	ts_stream_read(stream, buf, buf_size);    
	buf = &stream->buffer[stream->buf_pos];
	n = 2;
	ocr_flag = getbits(buf, n, 1);
	n = 4 ;
	v = 0;
//	printf("nn1 %d \n",n);
	if(ocr_flag)
	{
		int i = 0, k;
//		printf(" d %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);

		n += 40;
		while(i < 33)
		{
			k = FFMIN(8, 33 - i);
			v <<= k;
			v |= getbits(buf, n, k);
			i += k;
			n += k;
		}

		if ( ocr_base != 0 )
		{
			ocr_new = (float)v/90000.0f;
			if ( abs(ocr_new - ocr_base) < 1 ) 
				ocr_cnt ++;
			else ocr_cnt = 0;
			ocr_base = ocr_new;
		}
		else
			ocr_base = (float)v/90000.0f;

		if ( ocr_cnt > 5 )
		{
			locked_ocr = (double)GetTimer()/1000000.0 - (double)ocr_base;
//			printf("Locked ocr as %f\n",ocr_base);
		}
	}

	n += 2;
#if 0
//	printf("nn2 %d \n",n);
		int q;
		for ( q = 0; q < 8; q ++)
		{
			printf("%2x ",buf[n/8 + q]);
		}
//		printf("nnn1 %d %d \n",cts_flag,sl->ts_len);
//		printf(" cts_flag %d \n",cts_flag);
#endif

	v = 0;
	if(cts_flag)
	{
		int i = 0, k;

		while(i < 33)
		{
			k = FFMIN(8, 33 - i);
			v <<= k;
			v |= getbits(buf, n, k);
			i += k;
			n += k;
		}
		
		*pts = (float) v/ (float) 90000.0;
//		printf("Get PTS %f \n",*pts);
//		printf("pes_es->pts %f \n",*pts);
	}
	
	m = ( n + 7 ) / 8;
	buf_size -= m;
	ts_stream_skip(stream, m);     
	current_module="sl_packet3";
//	printf("raw %x \n",raw_data);
//	uc_memcpy(raw_data,&buf[m],buf_size);
	ts_stream_read(stream,raw_data,buf_size);
//	pes_len -= m ;
	*buf_pos = buf_size;
	current_module="sl_packet4";
	return pes_len;
}

int a_frame_num,v_frame_num,a_play_num,v_play_num;
// 0 = EOF or no stream found
// else = [-] number of bytes written to the packet
static int ts_parse(demuxer_t *demuxer , ES_stream_t *es, unsigned char *packet1, int probe)
{
	uint8_t a_done,v_done;
	unsigned char packet[188];
	int buf_size, is_start, pid, base,media_type,ii,afc,len;
	stream_t *stream = demuxer->stream;
	char *p,*a_buf,*v_buf;
	demux_packet_t dp;
	int junk = 0, rap_flag = 0,q;

//	a_pes_len = 0;
//	v_pes_len = 0;
	v_done = 0;
	a_done = 0;
//	v_buf = 0;
//	a_buf = 0;
//	ts_stream_reset(demuxer->stream);
	while( !(a_done || v_done) )
	{
		buf_size = 188;
		packet_cnt ++;
		if ( pack_err )
			err_cnt --;
		else
			err_cnt ++;

		if ( packet_cnt % 261 == 0 )
		{
			abs_err = ( err_cnt - last_degree + 261 ) / 2 ;
			abs_err = 261 - (261 - abs_err) * 2;
			err_degree = (double)abs_err / 261.0;
//			printf("Signal %d a=%d v=%d ap=%d vp=%d\n",abs_err,a_frame_num,v_frame_num,a_play_num,v_play_num);
			last_degree = err_cnt;
		}
		
		if(! ts_sync(stream))
		{
			ts_stream_reset(demuxer->stream);
			mp_msg(MSGT_DEMUX, MSGL_INFO, "TS_PARSE: COULDN'T SYNC\n");
			return -1;
		}

		len = ts_stream_read(stream, &packet[1], 3);     
		buf_size -= 4;

		if((packet[1]  >> 7) & 0x01)	//transport error
		{
			printf("Te ! \n");
			pack_err = 1;
#if 0
			ts_stream_skip(stream, buf_size);
			continue;
#endif
		}

		is_start = packet[1] & 0x40;
		pid = ((packet[1] & 0x1f) << 8) | packet[2];

#if 0
		if ( pid != 0x00 &&
		     pid != 0x10 &&
		     pid != 0x20 &&
		     pid != 0x30 &&
		     pid != 0x40 &&
		     pid != 0x50)
#else
		if ( pid != 0x40 && pid != 0x50)
#endif
		{
			ts_stream_skip(stream, buf_size);
			continue;
		}

		afc = (packet[3] >> 4) & 3;
		if(! (afc % 2))	//no payload in this TS packet
		{
			ts_stream_skip(stream, buf_size-1);
			continue;
		}

		if(afc > 1)
		{
			int c;
			c = ts_stream_read_char(stream);
			buf_size--;
			if(c < 0 || c > 183)	//broken from the stream layer or invalid
			{
				ts_stream_skip(stream, buf_size-1+junk);
				continue;
			}
			
			if(c > 0)
			{
				ts_stream_skip(stream, c);
				buf_size -= c;
				if(buf_size == 0)
					continue;
			}
		}
		if ( is_start )              //check is really start or not!
		{
			if ( stream->buffer[stream->buf_pos + 3] != 0xfa )
				if ( stream->buffer[stream->buf_pos + 2] != 0x01 )
				{
//					printf("false start! \n");
					is_start = 0;
					pack_err = 1;
				}
		}
		else
		{
			if ( stream->buffer[stream->buf_pos + 3] == 0xfa )
				if ( stream->buffer[stream->buf_pos + 2] == 0x01 )
				{
//					printf("this packet should be PES! \n");
					is_start = 1;
					pack_err = 1;
				}
		}

//		printf("buf %d %x %x \n",pid,v_fifo.last->buf,a_fifo.last->buf);
		if(is_start)
		{

//			if ( media_type == 1 )
			if ( pid == 0x50 )
			{
				v_buf = v_fifo.last->buf;
				if ( v_pes_len > v_buf_pos )
				{
//					printf("V Miss some packet or PES len error ! \n");
					dp.len = v_buf_pos;
					dp.pts = v_pts;
					if ( fill_packet(&dp,1) ) 
						v_done =1;
					v_buf = v_fifo.last->buf;
					v_buf_pos = 0;
					v_pes_len = 0;
//					pack_err = 1;
				}
				if ( (unsigned int)&v_fifo.last > 0x1000000 )printf("v_buf %x \n",&v_fifo.last);
				v_pes_len = get_data_from_pes(stream,buf_size,v_buf,&v_buf_pos,&v_pts);
//				printf("out\n");
				if ( v_pes_len > 16384 ) 
				{
//					printf("a error video pes len %d \n",v_pes_len);
					v_pes_len = 16196 - 188;
					pack_err = 1;
				}
				if ( v_pes_len <= v_buf_pos )
				{
					dp.len = v_buf_pos;
					dp.pts = v_pts;
					if ( fill_packet(&dp,1) ) 
						v_done =1;
					v_buf = v_fifo.last->buf;
					v_buf_pos = 0;
					v_pes_len = 0;
				}
			}
			else
			{
				a_buf = a_fifo.last->buf;
				if ( a_pes_len > a_buf_pos )
				{
//					printf("A Miss some packet or PES len error ! \n");
					dp.len = a_buf_pos;
					dp.pts = a_pts;
					if ( fill_packet(&dp,2) ) 
						a_done =1;
					a_buf = a_fifo.last->buf;
					a_buf_pos = 0;
					a_pes_len = 0;
//					pack_err = 1;
				}
//				printf("a_buf %x \n",a_buf);
				if ( (unsigned int)&a_fifo.last > 0x1000000 )printf("a_buf %x \n",&a_fifo.last);
				a_pes_len = get_data_from_pes(stream,buf_size,a_buf,&a_buf_pos,&a_pts);
//				printf("out\n");
				if ( a_pes_len > 1024 ) 
				{
//					printf("a error audio pes len %d \n",a_pes_len);
					a_pes_len = 1024 - 188;
					pack_err = 1;
				}
				if ( a_pes_len <= a_buf_pos )
				{
					dp.len = a_buf_pos;
					dp.pts = a_pts;
//					printf("fill a at once! %d %d \n",a_pes_len,a_buf_pos);
					if ( fill_packet(&dp,2) ) 
						a_done =1;
					a_buf = a_fifo.last->buf;
					a_buf_pos = 0;
					a_pes_len = 0;
				}
			}
		}
		else                    //not start just fill data!
		{

//			if ( media_type == 1 )
			if ( pid == 0x50 )
			{
				v_buf = v_fifo.last->buf;
				if ( v_pes_len == 0 )
				{
					printf("Miss a v PES header! \n");
					v_pes_len = 16384 - 188;
					pack_err = 1;
				}
				ts_stream_read(stream,&v_buf[v_buf_pos],buf_size);
				v_buf_pos += buf_size;
//				printf(" %d %d %d \n",v_buf_pos,buf_size,v_pes_len);
				if ( v_pes_len <= v_buf_pos ) 
				{
					dp.len = v_buf_pos;
					dp.pts = v_pts;
//					printf("add normal %d %d\n",v_buf_pos,v_pes_len_bak);
					if ( fill_packet(&dp,1) ) 
						v_done = 1;
					v_buf = v_fifo.last->buf;
					v_buf_pos = 0;
					v_pes_len = 0;
				}
			}
			else
			{
				a_buf = a_fifo.last->buf;
				if ( a_pes_len == 0 )
				{
					printf("Miss a a PES header! \n");
					a_pes_len = 1024 - 188;
					pack_err = 1;
				}
				ts_stream_read(stream,&a_buf[a_buf_pos],buf_size);
				a_buf_pos += buf_size;
//				printf("a %d %d %d \n",a_buf_pos,buf_size,a_pes_len);
				if ( a_pes_len <= a_buf_pos ) 
				{
					dp.len = a_buf_pos;
					dp.pts = a_pts;
					if ( fill_packet(&dp,2) ) 
						a_done =1;
					a_buf = a_fifo.last->buf;
					a_buf_pos = 0;
					a_pes_len = 0;
				}

			}

		}
	}
	return 1;
}

extern void skip_audio_frame(sh_audio_t *sh_audio);
 
static void reset_fifos(ts_priv_t* priv, int a, int v, int s)
{
	if(a)
	{
		if(priv->fifo[0].pack != NULL)
		{
			free_demux_packet(priv->fifo[0].pack);
			priv->fifo[0].pack = NULL;
		}
		priv->fifo[0].offset = 0;
	}

	if(v)
	{
		if(priv->fifo[1].pack != NULL)
		{
			free_demux_packet(priv->fifo[1].pack);
			priv->fifo[1].pack = NULL;
		}
		priv->fifo[1].offset = 0;
	}

	if(s)
	{
		if(priv->fifo[2].pack != NULL)
		{
			free_demux_packet(priv->fifo[2].pack);
			priv->fifo[2].pack = NULL;
		}
		priv->fifo[2].offset = 0;
	}
}


static void demux_seek_ts(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
	demux_stream_t *d_audio=demuxer->audio;
	demux_stream_t *d_video=demuxer->video;
	demux_stream_t *d_sub=demuxer->sub;
	sh_audio_t *sh_audio=d_audio->sh;
	sh_video_t *sh_video=d_video->sh;
	ts_priv_t * priv = (ts_priv_t*) demuxer->priv;
	int i, video_stats;
	off_t newpos;

	if(sh_audio != NULL)
		ds_free_packs(d_audio);
	if(sh_video != NULL)
		ds_free_packs(d_video);
	if(demuxer->sub->id > 0)
		ds_free_packs(d_sub);
}

int dfs = 0;


static int demux_ts_fill_buffer(demuxer_t * demuxer, demux_stream_t *ds)
{
	ES_stream_t es;
//	ts_priv_t *priv = (ts_priv_t *)demuxer->priv;
	ts_parse(demuxer, 0, 0, 0);
//	printf("out ! \n");
	return -1;
}


static int ts_check_file_dmx(demuxer_t *demuxer)
{
	stream_t *s = demuxer->stream;
	unsigned int * id = s->priv;
	if ( !id ) return 0;
	if ( *id == 0x00444142 )
		return DEMUXER_TYPE_DAB;
	else
		return 0;

}

static int is_usable_program(ts_priv_t *priv, pmt_t *pmt)
{
	int j;

	for(j = 0; j < pmt->es_cnt; j++)
	{
		if(priv->ts.pids[pmt->es[j].pid] == NULL || priv->ts.streams[pmt->es[j].pid].sh == NULL)
			continue;
		if(
			priv->ts.streams[pmt->es[j].pid].type == TYPE_VIDEO ||
			priv->ts.streams[pmt->es[j].pid].type == TYPE_AUDIO
		)
			return 1;
	}

	return 0;
}

static int demux_ts_control(demuxer_t *demuxer, int cmd, void *arg)
{
	ts_priv_t* priv = (ts_priv_t *)demuxer->priv;

	switch(cmd)
	{
		case DEMUXER_CTRL_SWITCH_AUDIO:
		case DEMUXER_CTRL_SWITCH_VIDEO:
		case DEMUXER_CTRL_IDENTIFY_PROGRAM:		//returns in prog->{aid,vid} the new ids that comprise a program
		default:
			return DEMUXER_CTRL_OK;
	}
}


demuxer_desc_t demuxer_desc_dab = {
  "T-DMB/DAB demuxer",
  "T-DMB/DAB",
  "T-DMB/DAB",
  "Lucifer liu",
  "",
  DEMUXER_TYPE_DAB,
  0, // unsafe autodetect
  ts_check_file_dmx,
  demux_ts_fill_buffer,
  demux_open_ts,
  demux_close_ts,
  demux_seek_ts,
  demux_ts_control
};
