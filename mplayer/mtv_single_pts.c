

#include "mtv_player.h"

#include "mplaylib.h"

#if defined(SING_THREAD_PTS)
//#include <stdio.h> 
//#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>

#include "subreader.h"
#include "libao2/audio_out.h"
#include "libvo/video_out.h"

#include "playtree.h"

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#include "libmpcodecs/dec_audio.h"
#include "libmpcodecs/dec_video.h"
#include "libmpcodecs/mp_image.h"
#include "libmpcodecs/vf.h"
#include "libmpcodecs/vd.h"
#include "mixer.h"

#include "mp_core.h"

#include "libfaad2/faad.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/h264.h"
#include "libmpdemux/mtv_struct.h"

#define MAX_MPI_BUF 5
#define MAX_PCM_BUF 5
#define MAX_PTS_BUF 5
#define TYPE_AUDIO  1
#define TYPE_VIDEO  2
#define MAX_DELAY_TIME 1000000
#define PCM_BUF_SIZE  22050
#define MAX_AUDIO_ERROR 6
#define MAX_VIDEO_ERROR 10

//#undef printf

static MPContext *mpctx;
extern char* current_module;
struct pts_buf_t
{
	double cur_pts,pts_delay;
	int type,index,playsize,ready;
};

typedef struct __media_packet_t
{
	int type,index,playsize;
	double pts,delay;
	mp_image_t * mpi;
	unsigned char * pcm;
	struct __media_packet_t * next,* priv;
}media_packet_t;

typedef struct __media_fifo_t
{
	int num;
	media_packet_t * in, * out;
}media_fifo_t;

pthread_t at_id1,at_id2;
pthread_attr_t at_attr1,at_attr2;
sem_t a_sem,v_sem,p_sem,f_sem;
int tmp = 0;
mp_image_t *mpi = NULL;
mp_image_t *mpi_buf[MAX_MPI_BUF];
//double pts_buf[MAX_PCM_BUF];
unsigned char *pcm_buf[MAX_PCM_BUF],pcm_mute[4096];
int playsize[MAX_PCM_BUF];
extern vd_functions_t* mpvdec;
struct pts_buf_t pts_buf[MAX_PTS_BUF];
extern int err_degree;
void *faac_sample_buffer;
//extern faacDecDecode;
FILE *fp_pcm;
extern mts_func_t ts_mts;
extern mts_func_t cmmb_mts;

mts_func_t *mts_p;
media_fifo_t * mf;

media_fifo_t * media_fifo_init( media_fifo_t * mf)
{
	int i;
	media_packet_t *mp, *mpp, *mppp;
	mf = malloc(sizeof(media_fifo_t));
	mppp = mpp = malloc(sizeof(media_packet_t));
	for ( i = 0; i < MAX_PTS_BUF; i ++ )
	{
		mp = mpp;
		mp->mpi = new_mp_image(320,240);
		mp->pcm = malloc(sizeof(unsigned char)*PCM_BUF_SIZE);
		if ( !mp->pcm )
		{
			printf("PCM can not malloc buffer! \n");
			return 0;
		} 
		mpp = malloc(sizeof(media_packet_t));
		mp->next = mpp;
		mpp->priv = mp;
	}
	mp->next = mppp;
	mppp->priv = mp;
	mf->in = mf->out = mppp;
	mf->num = 0;
	free(mpp);
	return mf;
}

typedef struct {
    AVCodecContext *avctx;
    AVFrame *pic;
    enum PixelFormat pix_fmt;
    int do_slices;
    int do_dr1;
    int vo_inited;
    int best_csp;
    int b_age;
    int ip_age[2];
    int qp_stat[32];
    double qp_sum;
    double inv_qp_sum;
    int ip_count;
    int b_count;
    AVRational last_sample_aspect_ratio;
} vd_ffmpeg_ctx;

extern double locked_ocr;

void stop_decode()
{
#if 0
	sh_video_t *sh_video = mpctx->sh_video;
//	sh_audio_t *sh_audio = mpctx->sh_audio;
	vd_ffmpeg_ctx *ctx = sh_video->context;
	AVCodecContext *avctx = ctx->avctx;
	H264Context *h = avctx->priv_data;
//	MpegEncContext *s = &h->s;
//	void *p;

	h264_reset(avctx);
#endif
}

void restart_decode()
{
#if 0
	sh_video_t *sh_video = mpctx->sh_video;
	sh_audio_t *sh_audio = mpctx->sh_audio;
	vd_ffmpeg_ctx *ctx = sh_video->context;
	AVCodecContext *avctx = ctx->avctx;
#endif
}
void initmtv(MPContext *mpc)
{
	sh_video_t *sh_video;
	vd_ffmpeg_ctx *ctx;
	AVCodecContext *avctx;
	sh_video = mpctx->sh_video;
	ctx = sh_video->context;
	avctx = ctx->avctx;
	avctx->thread_count = 1;	
}
void* (*mtv_faacDecDecode_funcp)(faacDecFrameInfo *hInfo,
                                uint8_t *buffer, uint32_t buffer_size) = NULL;

void decode_thread( void )
{
	int in_size,v_off,flags = 0,playsize;
	sh_video_t *sh_video;
	sh_audio_t *sh_audio;
	demux_stream_t *d_video,*d_audio;
	demuxer_t *demuxer;
	unsigned int * start;
	double pts,delay,speed;
	unsigned char * pcm_o;
	vd_ffmpeg_ctx *ctx;
	AVCodecContext *avctx;

	demuxer = mpctx->demuxer;
	sh_video = mpctx->sh_video;
	sh_audio = mpctx->sh_audio;
	d_video = mpctx->d_video;
	d_audio = mpctx->d_audio;
	v_off = 0;
	*(mts_p->err_degree) = 1;
	speed = 1.0;
	flags |= AOPLAY_FINAL_CHUNK;
	ctx = sh_video->context;
	avctx = ctx->avctx;
	avctx->thread_count = 1;
//	avctx->unused = 0x4d5456;

	while(1)
	{
		F("1\n");
		if ( mts_p->get_first_pts(demuxer,&delay) == TYPE_AUDIO )
		{
                  //faacDecHandle faac_hdec;
   F("2\n");
                    faacDecFrameInfo faac_finfo;
  			in_size = mts_p->get_packet(demuxer,d_audio, &start, &pts);               //get a audio packet
			pcm_o = mtv_faacDecDecode_funcp(&faac_finfo, start, in_size);            //decode a audio packet
//			pcm_o = faacDecDecode(faac_hdec, &faac_finfo, start, in_size);            //decode a audio packet
			if ( faac_finfo.error > 0 )
			{
				F("Fe!\n");
				*(mts_p->packet_err) = 1;
				continue;
			}
			else if (faac_finfo.samples > 0 ) //&& speed < 0.6 )
			{
				F("3\n");
				playsize = sh_audio->samplesize*faac_finfo.samples;
#if 0   //for external sync,do not delet!
				at -= GetTimer();
				if ( at > 10000 & at < 40000 )
				{
					delay_int = (int)at /20000;
					delay_int *= 10000;
					usleep(delay_int);
				}
#endif
				while(mpctx->audio_out->get_space() < playsize) ;
				mpctx->audio_out->play(pcm_o,playsize,flags);               //play a audio packet
//				at = GetTimer() + delay * 1000000;
			}
			else 
			{
				continue;
			}
		}
		else         //decode video
		{
			F("4\n");
			in_size = mts_p->get_packet(demuxer,d_video, &start, &pts);              //get a video packet
			mpi = mpvdec->decode(sh_video, start, in_size, 0);                     //decode a video packet
			if ( mpi )
			{
#ifdef GAOTEST
				at -= GetTimer();
				if ( at > 10000 & at < 40000 )
				{
					delay_int = (int)at /10000;
					delay_int *= 10000;
					usleep(delay_int);
				}
#endif
				filter_video(sh_video, mpi, 0);                                      //play a video packet
//				at = GetTimer() + delay * 1000000;
			}
			else 
			{
				printf("He!\n");
//				*(mts_p->packet_err) = 1;
				continue;
			}
		}
	}
}

void create_muti_thread(MPContext *mpc)
{ 
	mpctx = mpc;

	if ( mpctx->demuxer->type == DEMUXER_TYPE_DAB )
	{
		mts_p = & ts_mts;
	}
	else
	{
		mts_p = & cmmb_mts;
	}
//	mf = media_fifo_init( mf );
//	if ( !(mf = media_fifo_init( mf )) )
//		exit_player_with_rc(NULL, 0);
}

#endif
