/*
    Copyright (C) 2004 Michael Niedermayer <michaelni@gmx.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifdef __LINUX__
// #include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <limits.h>
#define F(...) 
#else
// #include <mplaylib.h>
#include <mplaylib.h>
// #include <mplaylib.h>
#endif


#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "m_option.h"
#include "libvo/sub.h"

#ifdef USE_LIBAVFORMAT_SO
#include <ffmpeg/avformat.h>
#include <ffmpeg/avutil.h>
#include <ffmpeg/opt.h>
#else
#include "avformat.h"
#include "avutil.h"
#include "avi.h"
#include "opt.h"
#endif
#include "riff.h"

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

#define PROBE_BUF_SIZE 2048

extern char *audio_lang;
static unsigned int opt_probesize = 0;
static unsigned int opt_analyzeduration = 0;
static char *opt_format;

m_option_t lavfdopts_conf[] = {
	{"probesize", &(opt_probesize), CONF_TYPE_INT, CONF_RANGE, 32, INT_MAX, NULL},
	{"format",    &(opt_format),    CONF_TYPE_STRING,       0,  0,       0, NULL},
	{"analyzeduration",    &(opt_analyzeduration),    CONF_TYPE_INT,       CONF_RANGE,  0,       INT_MAX, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};


typedef struct lavf_priv_t{
    AVInputFormat *avif;
    AVFormatContext *avfc;
    ByteIOContext pb;
    int audio_streams;
    int video_streams;
    int sub_streams;
    int64_t last_pts;
    int astreams[MAX_A_STREAMS];
    int vstreams[MAX_V_STREAMS];
    int sstreams[MAX_S_STREAMS];
}lavf_priv_t;

extern void print_wave_header(WAVEFORMATEX *h, int verbose_level);
extern void print_video_header(BITMAPINFOHEADER *h, int verbose_level);

static const AVCodecTag mp_wav_tags[] = {
    { CODEC_ID_ADPCM_4XM,         MKTAG('4', 'X', 'M', 'A')},
    { CODEC_ID_ADPCM_EA,          MKTAG('A', 'D', 'E', 'A')},
    { CODEC_ID_ADPCM_IMA_WS,      MKTAG('A', 'I', 'W', 'S')},
    { CODEC_ID_ADPCM_THP,         MKTAG('T', 'H', 'P', 'A')},
    { CODEC_ID_AMR_NB,            MKTAG('n', 'b',   0,   0)},
    { CODEC_ID_COOK,              MKTAG('c', 'o', 'o', 'k')},
    { CODEC_ID_DSICINAUDIO,       MKTAG('D', 'C', 'I', 'A')},
    { CODEC_ID_INTERPLAY_DPCM,    MKTAG('I', 'N', 'P', 'A')},
    { CODEC_ID_MUSEPACK7,         MKTAG('M', 'P', 'C', ' ')},
    { CODEC_ID_PCM_S24BE,         MKTAG('i', 'n', '2', '4')},
    { CODEC_ID_PCM_S16BE,         MKTAG('t', 'w', 'o', 's')},
    { CODEC_ID_PCM_S8,            MKTAG('t', 'w', 'o', 's')},
    { CODEC_ID_ROQ_DPCM,          MKTAG('R', 'o', 'Q', 'A')},
    { CODEC_ID_SHORTEN,           MKTAG('s', 'h', 'r', 'n')},
    { CODEC_ID_TTA,               MKTAG('T', 'T', 'A', '1')},
    { CODEC_ID_WAVPACK,           MKTAG('W', 'V', 'P', 'K')},
    { CODEC_ID_WESTWOOD_SND1,     MKTAG('S', 'N', 'D', '1')},
    { CODEC_ID_XAN_DPCM,          MKTAG('A', 'x', 'a', 'n')},
    { 0, 0 },
};

const struct AVCodecTag *mp_wav_taglists[] = {codec_wav_tags, mp_wav_tags, 0};

static const AVCodecTag mp_bmp_tags[] = {
    { CODEC_ID_AMV,               MKTAG('A', 'M', 'V', 'V')},
    { CODEC_ID_BETHSOFTVID,       MKTAG('B', 'E', 'T', 'H')},
    { CODEC_ID_C93,               MKTAG('C', '9', '3', 'V')},
    { CODEC_ID_DSICINVIDEO,       MKTAG('D', 'C', 'I', 'V')},
    { CODEC_ID_DXA,               MKTAG('D', 'X', 'A', '1')},    
    { CODEC_ID_FLIC,              MKTAG('F', 'L', 'I', 'C')},
    { CODEC_ID_IDCIN,             MKTAG('I', 'D', 'C', 'I')},
    { CODEC_ID_INTERPLAY_VIDEO,   MKTAG('I', 'N', 'P', 'V')},
    { CODEC_ID_ROQ,               MKTAG('R', 'o', 'Q', 'V')},
    { CODEC_ID_THP,               MKTAG('T', 'H', 'P', 'V')},
    { CODEC_ID_TIERTEXSEQVIDEO,   MKTAG('T', 'S', 'E', 'Q')},
    { CODEC_ID_TXD,               MKTAG('T', 'X', 'D', 'V')},
    { CODEC_ID_VMDVIDEO,          MKTAG('V', 'M', 'D', 'V')},
    { CODEC_ID_WS_VQA,            MKTAG('V', 'Q', 'A', 'V')},
    { CODEC_ID_XAN_WC3,           MKTAG('W', 'C', '3', 'V')},
    { CODEC_ID_NUV,               MKTAG('N', 'U', 'V', '1')},
    { 0, 0 },
};

const struct AVCodecTag *mp_bmp_taglists[] = {codec_bmp_tags, mp_bmp_tags, 0};

static int mp_open(URLContext *h, const char *filename, int flags){
    return 0;
}

static int mp_read(URLContext *h, unsigned char *buf, int size){
    stream_t *stream = (stream_t*)h->priv_data;
    int ret;

    if(stream_eof(stream)) //needed?
        return -1;
    ret=stream_read(stream, buf, size);

    mp_msg(MSGT_HEADER,MSGL_DBG2,"%d=mp_read(%p, %p, %d), eof:%d\n", ret, h, buf, size, stream->eof);
    return ret;
}

static int mp_write(URLContext *h, unsigned char *buf, int size){
    return -1;
}

static offset_t mp_seek(URLContext *h, offset_t pos, int whence){
    stream_t *stream = (stream_t*)h->priv_data;
    
    mp_msg(MSGT_HEADER,MSGL_DBG2,"mp_seek(%p, %d, %d)\n", h, (int)pos, whence);
    if(whence == SEEK_CUR)
        pos +=stream_tell(stream);
    else if(whence == SEEK_END && stream->end_pos > 0)
        pos += stream->end_pos;
    else if(whence == SEEK_SET)
        pos += stream->start_pos;
    else if(whence == AVSEEK_SIZE && stream->end_pos > 0)
        return stream->end_pos - stream->start_pos;
    else
        return -1;

    if(pos<0)
        return -1;
    if(pos<stream->end_pos && stream->eof)
        stream_reset(stream);
    if(stream_seek(stream, pos)==0)
        return -1;

    return pos - stream->start_pos;
}

static int mp_close(URLContext *h){
    return 0;
}

static URLProtocol mp_protocol = {
    "mp",
    mp_open,
    mp_read,
    mp_write,
    mp_seek,
    mp_close,
};

static void list_formats(void) {
    AVInputFormat *fmt;
    mp_msg(MSGT_DEMUX, MSGL_INFO, "Available lavf input formats:\n");
    for (fmt = first_iformat; fmt; fmt = fmt->next)
        mp_msg(MSGT_DEMUX, MSGL_INFO, "%15s : %s\n", fmt->name, fmt->long_name);
}

static int lavf_check_file(demuxer_t *demuxer){
    AVProbeData avpd;
    uint8_t buf[PROBE_BUF_SIZE];
    lavf_priv_t *priv;
    F("1\n");
    if(!demuxer->priv) 
        demuxer->priv=calloc(sizeof(lavf_priv_t),1);
    F("2\n");
    priv= demuxer->priv;

    av_register_all();
		F("3\n");
    if(stream_read(demuxer->stream, buf, PROBE_BUF_SIZE)!=PROBE_BUF_SIZE)
        return 0;
    F("4\n");
    avpd.filename= demuxer->stream->url;
    avpd.buf= buf;
    avpd.buf_size= PROBE_BUF_SIZE;
		F("5\n");
    if (opt_format) {
        if (strcmp(opt_format, "help") == 0) {
           list_formats();
           return 0;
        }
        priv->avif= av_find_input_format(opt_format);
        if (!priv->avif) {
            mp_msg(MSGT_DEMUX,MSGL_FATAL,"Unknown lavf format %s\n", opt_format);
            return 0;
        }
        mp_msg(MSGT_DEMUX,MSGL_INFO,"Forced lavf %s demuxer\n", priv->avif->long_name);
        return DEMUXER_TYPE_LAVF;
    }
    F("6\n");
    priv->avif= av_probe_input_format(&avpd, 1);
    F("7\n");
    
    if(!priv->avif){
        mp_msg(MSGT_HEADER,MSGL_V,"LAVF_check: no clue about this gibberish!\n");
        return 0;
    }else
        mp_msg(MSGT_HEADER,MSGL_V,"LAVF_check: %s\n", priv->avif->long_name);
		return DEMUXER_TYPE_LAVF;
}

static const char *preferred_list[] = {
    "dxa",
    "wv",
    "nuv",
    "nut",
    "gxf",
    "mxf",
    "flv",
    "swf",
    NULL
};

static int lavf_check_preferred_file(demuxer_t *demuxer){
    if (lavf_check_file(demuxer)) {
        char **p = preferred_list;
        lavf_priv_t *priv = demuxer->priv;
        while (*p) {
            if (strcmp(*p, priv->avif->name) == 0)
                return DEMUXER_TYPE_LAVF_PREFERRED;
            p++;
        }
    }
    return 0;
}
    
static demuxer_t* demux_open_lavf(demuxer_t *demuxer){
    AVFormatContext *avfc;
    AVFormatParameters ap;
    const AVOption *opt;
    lavf_priv_t *priv= demuxer->priv;
    int i,g;
    char mp_filename[256]="mp:";

    memset(&ap, 0, sizeof(AVFormatParameters));

    stream_seek(demuxer->stream, 0);

    register_protocol(&mp_protocol);

    avfc = av_alloc_format_context();

    if (correct_pts)
        avfc->flags |= AVFMT_FLAG_GENPTS;
    if (index_mode == 0)
        avfc->flags |= AVFMT_FLAG_IGNIDX;

    ap.prealloced_context = 1;
    if(opt_probesize) {
        opt = av_set_int(avfc, "probesize", opt_probesize);
        if(!opt) mp_msg(MSGT_HEADER,MSGL_ERR, "demux_lavf, couldn't set option probesize to %u\n", opt_probesize);
    }
    if(opt_analyzeduration) {
        opt = av_set_int(avfc, "analyzeduration", opt_analyzeduration * AV_TIME_BASE);
        if(!opt) mp_msg(MSGT_HEADER,MSGL_ERR, "demux_lavf, couldn't set option analyzeduration to %u\n", opt_analyzeduration);
    }

    if(demuxer->stream->url)
        strncpy(mp_filename + 3, demuxer->stream->url, sizeof(mp_filename)-3);
    else
        strncpy(mp_filename + 3, "foobar.dummy", sizeof(mp_filename)-3);
    
    url_fopen(&priv->pb, mp_filename, URL_RDONLY);
    
    ((URLContext*)(priv->pb.opaque))->priv_data= demuxer->stream;
        
    if(av_open_input_stream(&avfc, &priv->pb, mp_filename, priv->avif, &ap)<0){
        mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF_header: av_open_input_stream() failed\n");
        return NULL;
    }

    priv->avfc= avfc;

    if(av_find_stream_info(avfc) < 0){
        mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF_header: av_find_stream_info() failed\n");
        return NULL;
    }

    if(avfc->title    [0]) demux_info_add(demuxer, "name"     , avfc->title    );
    if(avfc->author   [0]) demux_info_add(demuxer, "author"   , avfc->author   );
    if(avfc->copyright[0]) demux_info_add(demuxer, "copyright", avfc->copyright);
    if(avfc->comment  [0]) demux_info_add(demuxer, "comments" , avfc->comment  );
    if(avfc->album    [0]) demux_info_add(demuxer, "album"    , avfc->album    );
//    if(avfc->year        ) demux_info_add(demuxer, "year"     , avfc->year     );
//    if(avfc->track       ) demux_info_add(demuxer, "track"    , avfc->track    );
    if(avfc->genre    [0]) demux_info_add(demuxer, "genre"    , avfc->genre    );

    for(i=0; i<avfc->nb_streams; i++){
        AVStream *st= avfc->streams[i];
        AVCodecContext *codec= st->codec;

        switch(codec->codec_type){
        case CODEC_TYPE_AUDIO:{
            WAVEFORMATEX *wf= calloc(sizeof(WAVEFORMATEX) + codec->extradata_size, 1);
            sh_audio_t* sh_audio;
            if(priv->audio_streams >= MAX_A_STREAMS)
                break;
            sh_audio=new_sh_audio(demuxer, i);
            mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_AudioID, "lavf", i);
            if(!sh_audio)
                break;
            priv->astreams[priv->audio_streams] = i;
            priv->audio_streams++;
            // mp4a tag is used for all mp4 files no matter what they actually contain
            if(codec->codec_tag == MKTAG('m', 'p', '4', 'a'))
                codec->codec_tag= 0;
            if(codec->codec_id == CODEC_ID_ADPCM_IMA_AMV)
                codec->codec_tag= MKTAG('A','M','V','A');
            if(!codec->codec_tag)
                codec->codec_tag= av_codec_get_tag(mp_wav_taglists, codec->codec_id);
            wf->wFormatTag= codec->codec_tag;
            wf->nChannels= codec->channels;
            wf->nSamplesPerSec= codec->sample_rate;
            wf->nAvgBytesPerSec= codec->bit_rate/8;
            wf->nBlockAlign= codec->block_align ? codec->block_align : 1;
            wf->wBitsPerSample= codec->bits_per_sample;
            wf->cbSize= codec->extradata_size;
            if(codec->extradata_size){
                memcpy(
                    wf + 1, 
                    codec->extradata,
                    codec->extradata_size);
            }
            sh_audio->wf= wf;
            sh_audio->audio.dwSampleSize= codec->block_align;
            if(codec->frame_size && codec->sample_rate){
                sh_audio->audio.dwScale=codec->frame_size;
                sh_audio->audio.dwRate= codec->sample_rate;
            }else{
                sh_audio->audio.dwScale= codec->block_align ? codec->block_align*8 : 8;
                sh_audio->audio.dwRate = codec->bit_rate;
            }
            g= ff_gcd(sh_audio->audio.dwScale, sh_audio->audio.dwRate);
            sh_audio->audio.dwScale /= g;
            sh_audio->audio.dwRate  /= g;
//            printf("sca:%d rat:%d fs:%d sr:%d ba:%d\n", sh_audio->audio.dwScale, sh_audio->audio.dwRate, codec->frame_size, codec->sample_rate, codec->block_align);
            sh_audio->ds= demuxer->audio;
            sh_audio->format= codec->codec_tag;
            sh_audio->channels= codec->channels;
            sh_audio->samplerate= codec->sample_rate;
            sh_audio->i_bps= codec->bit_rate/8;
            switch (codec->codec_id) {
              case CODEC_ID_PCM_S8:
              case CODEC_ID_PCM_U8:
                sh_audio->samplesize = 1;
                break;
              case CODEC_ID_PCM_S16LE:
              case CODEC_ID_PCM_S16BE:
              case CODEC_ID_PCM_U16LE:
              case CODEC_ID_PCM_U16BE:
                sh_audio->samplesize = 2;
                break;
              case CODEC_ID_PCM_ALAW:
                sh_audio->format = 0x6;
                break;
              case CODEC_ID_PCM_MULAW:
                sh_audio->format = 0x7;
                break;
            }
            if( mp_msg_test(MSGT_HEADER,MSGL_V) ) print_wave_header(sh_audio->wf, MSGL_V);
	    if((audio_lang && st->language[0] && !strncmp(audio_lang, st->language, 3))
	        || (demuxer->audio->id == i || demuxer->audio->id == -1)
	    ) {
	        demuxer->audio->id = i;
                demuxer->audio->sh= demuxer->a_streams[i];
	    }
            else
                st->discard= AVDISCARD_ALL;
            break;}
        case CODEC_TYPE_VIDEO:{
            sh_video_t* sh_video;
            BITMAPINFOHEADER *bih;
            if(priv->video_streams >= MAX_V_STREAMS)
                break;
            sh_video=new_sh_video(demuxer, i);
            mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_VideoID, "lavf", i);
            if(!sh_video) break;
            priv->vstreams[priv->video_streams] = i;
            priv->video_streams++;
            bih=calloc(sizeof(BITMAPINFOHEADER) + codec->extradata_size,1);

            if(!codec->codec_tag)
                codec->codec_tag= av_codec_get_tag(mp_bmp_taglists, codec->codec_id);
            bih->biSize= sizeof(BITMAPINFOHEADER) + codec->extradata_size;
            bih->biWidth= codec->width;
            bih->biHeight= codec->height;
            bih->biBitCount= codec->bits_per_sample;
            bih->biSizeImage = bih->biWidth * bih->biHeight * bih->biBitCount/8;
            bih->biCompression= codec->codec_tag;
            sh_video->bih= bih;
            sh_video->disp_w= codec->width;
            sh_video->disp_h= codec->height;
            kprintf(">>>>>>>>> (%d, %d)\n", sh_video->disp_w, sh_video->disp_h);
            if (st->time_base.den) { /* if container has time_base, use that */
                sh_video->video.dwRate= st->time_base.den;
                sh_video->video.dwScale= st->time_base.num;
            } else {
            sh_video->video.dwRate= codec->time_base.den;
            sh_video->video.dwScale= codec->time_base.num;
            }
            sh_video->fps=av_q2d(st->r_frame_rate);
            sh_video->frametime=1/av_q2d(st->r_frame_rate);
            sh_video->format = bih->biCompression;
            sh_video->aspect=   codec->width * codec->sample_aspect_ratio.num 
                              / (float)(codec->height * codec->sample_aspect_ratio.den);
            sh_video->i_bps= codec->bit_rate/8;
            mp_msg(MSGT_DEMUX,MSGL_DBG2,"aspect= %d*%d/(%d*%d)\n", 
                codec->width, codec->sample_aspect_ratio.num,
                codec->height, codec->sample_aspect_ratio.den);

            sh_video->ds= demuxer->video;
            if(codec->extradata_size)
                memcpy(sh_video->bih + 1, codec->extradata, codec->extradata_size);
            if( mp_msg_test(MSGT_HEADER,MSGL_V) ) print_video_header(sh_video->bih, MSGL_V);
/*    short 	biPlanes;
    int  	biXPelsPerMeter;
    int  	biYPelsPerMeter;
    int 	biClrUsed;
    int 	biClrImportant;*/
            if(demuxer->video->id != i && demuxer->video->id != -1)
                st->discard= AVDISCARD_ALL;
            else{
                demuxer->video->id = i;
                demuxer->video->sh= demuxer->v_streams[i];
            }
            break;}
        case CODEC_TYPE_SUBTITLE:{
            sh_sub_t* sh_sub;
            if(priv->sub_streams >= MAX_S_STREAMS)
                break;
            /* only support text subtitles for now */
            if(codec->codec_id != CODEC_ID_TEXT)
                break;
            sh_sub = new_sh_sub_sid(demuxer, i, priv->sub_streams);
            mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_SubtitleID, "lavf", priv->sub_streams);
            if(!sh_sub) break;
            priv->sstreams[priv->sub_streams] = i;
            sh_sub->type = 't';
            demuxer->sub->sh = demuxer->s_streams[priv->sub_streams++];
            break;}
        default:
            st->discard= AVDISCARD_ALL;
        }
    }
    
    mp_msg(MSGT_HEADER,MSGL_V,"LAVF: %d audio and %d video streams found\n",priv->audio_streams,priv->video_streams);
    mp_msg(MSGT_HEADER,MSGL_V,"LAVF: build %d\n", LIBAVFORMAT_BUILD);
    if(!priv->audio_streams) demuxer->audio->id=-2;  // nosound
//    else if(best_audio > 0 && demuxer->audio->id == -1) demuxer->audio->id=best_audio;
    if(!priv->video_streams){
        if(!priv->audio_streams){
	    mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF: no audio or video headers found - broken file?\n");
            return NULL; 
        }
        demuxer->video->id=-2; // audio-only
    } //else if (best_video > 0 && demuxer->video->id == -1) demuxer->video->id = best_video;
#ifndef __LINUX__
	if(demuxer->audio->id != -2)
	{

		unsigned int audio_buf_count = 4;
		#include "libao2/audio_out.h"
		extern ao_data_t ao_data;
		sh_audio_t *sh_audio=demuxer->audio->sh;
		int audio_byte = sh_audio->channels * sh_audio->samplesize * sh_audio->samplerate;
		
		audio_byte = (audio_byte * 4 / 50 + 31) / 32 * 32;  //~0.08s
		
		if(audio_byte < 2048) audio_byte = 2048;

		audio_buf_count = 8;
		
		ao_data.outburst = audio_byte;
		ao_data.buffersize = ao_data.outburst * audio_buf_count;
		
		printf("ao_data.outburst = %d ao_data.buffersize = %d\n",ao_data.outburst,ao_data.buffersize);
	}
#endif
    return demuxer;
}
    
static int demux_lavf_fill_buffer(demuxer_t *demux, demux_stream_t *dsds){
		AVPacket pkt;
    lavf_priv_t *priv= demux->priv;
    demux_packet_t *dp;
    demux_stream_t *ds;
    int id;
    mp_msg(MSGT_DEMUX,MSGL_DBG2,"demux_lavf_fill_buffer()\n");

//	kprintf("demux_lavf_fill_buffer\n");
    demux->filepos=stream_tell(demux->stream);

    if(av_read_frame(priv->avfc, &pkt) < 0)
        return 0;
        
    id= pkt.stream_index;

    if(id==demux->audio->id){
        // audio
        ds=demux->audio;
        if(!ds->sh){
            ds->sh=demux->a_streams[id];
            mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected LAVF audio ID = %d\n",ds->id);
        }
    } else if(id==demux->video->id){
        // video
        ds=demux->video;
        if(!ds->sh){
            ds->sh=demux->v_streams[id];
            mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected LAVF video ID = %d\n",ds->id);
        }
    } else if(id==demux->sub->id){
        // subtitle
        ds=demux->sub;
        sub_utf8=1;
    } else {
        av_free_packet(&pkt);
        return 1;
    }
        
    if(0/*pkt.destruct == av_destruct_packet*/){
        //ok kids, dont try this at home :)
        dp=malloc(sizeof(demux_packet_t));
        dp->len=pkt.size;
        dp->next=NULL;
        dp->refcount=1;
        dp->master=NULL;
        dp->buffer=pkt.data;
        pkt.destruct= NULL;
    }else{
#if 1
			if(pkt.destruct == av_destruct_packet_nofree)
			{
				
        dp=new_demux_packet(pkt.size);
        memcpy(dp->buffer, pkt.data, pkt.size);
					
			}else
			{		
				unsigned int pp;
	        dp=new_demux_packet(16);
	        if (dp->len != 0)
	        {
	           pp = dp->buffer;
#ifdef __LINUX__
	           free(pp);
#else
	           uc_free(pp);
#endif
	        }
	        dp->len = pkt.size;
	        dp->buffer = pkt.data;
	        memset(dp->buffer + pkt.size, 0, 8);
	        pkt.data = NULL;
      }
#else
{
        unsigned int pp;
        dp=new_demux_packet(16);
        if (dp->len != 0)
        {
           pp = dp->buffer;
           uc_free(pp);
        }
        dp->len = pkt.size;
        dp->buffer = pkt.data;
        memset(dp->buffer + pkt.size, 0, 8);
        pkt.data = NULL;
}
#endif
        av_free_packet(&pkt);
    }

    if(pkt.pts != AV_NOPTS_VALUE){
        dp->pts=pkt.pts * av_q2d(priv->avfc->streams[id]->time_base);
        priv->last_pts= dp->pts * AV_TIME_BASE;
        if(pkt.duration)
            dp->endpts = dp->pts + pkt.duration * av_q2d(priv->avfc->streams[id]->time_base);
    }
    dp->pos=demux->filepos;
    dp->flags= !!(pkt.flags&PKT_FLAG_KEY);
    // append packet to DS stream:
    ds_add_packet(ds,dp);
    return 1;
}

static void demux_seek_lavf(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags){
    lavf_priv_t *priv = demuxer->priv;
    int avsflags = 0;
    mp_msg(MSGT_DEMUX,MSGL_DBG2,"demux_seek_lavf(%p, %f, %f, %d)\n", demuxer, rel_seek_secs, audio_delay, flags);

    if (flags & 1) // absolute seek
      priv->last_pts = priv->avfc->start_time;
    if (flags & 2) { // percent seek
      if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
        return;
      priv->last_pts += rel_seek_secs * priv->avfc->duration;
    } else {
      priv->last_pts += rel_seek_secs * AV_TIME_BASE;
      if (rel_seek_secs < 0) avsflags = AVSEEK_FLAG_BACKWARD;
    }
    av_seek_frame(priv->avfc, -1, priv->last_pts, avsflags);
}

static int demux_lavf_control(demuxer_t *demuxer, int cmd, void *arg)
{
    lavf_priv_t *priv = demuxer->priv;
    
    switch (cmd) {
	case DEMUXER_CTRL_CORRECT_PTS:
		return DEMUXER_CTRL_OK;
		
	case DEMUXER_CTRL_GET_TIME_LENGTH:
	    if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
	        return DEMUXER_CTRL_DONTKNOW;
	    
	    *((double *)arg) = (double)priv->avfc->duration / AV_TIME_BASE;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_GET_PERCENT_POS:
	    if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
	        return DEMUXER_CTRL_DONTKNOW;
	    
	    *((int *)arg) = (int)((priv->last_pts - priv->avfc->start_time)*100 / priv->avfc->duration);
	    return DEMUXER_CTRL_OK;
	case DEMUXER_CTRL_SWITCH_AUDIO:
	case DEMUXER_CTRL_SWITCH_VIDEO:
	{
	    int id = *((int*)arg);
	    int newid = -2;
	    int i, curridx = -2;
	    int nstreams, *pstreams;
	    demux_stream_t *ds;

	    if(cmd == DEMUXER_CTRL_SWITCH_VIDEO)
	    {
	        ds = demuxer->video;
	        nstreams = priv->video_streams;
	        pstreams = priv->vstreams;
	    }
	    else
	    {
	        ds = demuxer->audio;
	        nstreams = priv->audio_streams;
	        pstreams = priv->astreams;
	    }
	    if(ds->id == -2)
	        return DEMUXER_CTRL_NOTIMPL;
	    for(i = 0; i < nstreams; i++)
	    {
	        if(pstreams[i] == ds->id) //current stream id
	        {
	            curridx = i;
	            break;
	        }
	    }

	    if(id < 0)
	    {
	        i = (curridx + 1) % nstreams;
	        newid = pstreams[i];
	    }
	    else
	    {
	        for(i = 0; i < nstreams; i++)
	        {
		    if(pstreams[i] == id)
		    {
		        newid = id;
		        break;
		    }
	        }
	    }
	    if(newid == -2 || i == curridx)
	        return DEMUXER_CTRL_NOTIMPL;
	    else
	    {
	        ds_free_packs(ds);
	        priv->avfc->streams[ds->id]->discard = AVDISCARD_ALL;
	        *((int*)arg) = ds->id = newid;
	        priv->avfc->streams[newid]->discard = AVDISCARD_NONE;
	        return DEMUXER_CTRL_OK;
	    }
        }
	default:
	    return DEMUXER_CTRL_NOTIMPL;
    }
}

/** \brief Get the language code for a subtitle track.

  Retrieves the language code for a subtitle track.

  \param demuxer The demuxer to work on
  \param track_num The subtitle track number to get the language from
*/
char *demux_lavf_sub_lang(demuxer_t *demuxer, int track_num)
{
    lavf_priv_t *priv = demuxer->priv;
    return priv->avfc->streams[priv->sstreams[track_num]]->language;
}

static void demux_close_lavf(demuxer_t *demuxer)
{
    lavf_priv_t* priv = demuxer->priv;
    if (priv){
        if(priv->avfc)
       {
         av_close_input_file(priv->avfc); priv->avfc= NULL;
        }
        free(priv); demuxer->priv= NULL;
    }
}


demuxer_desc_t demuxer_desc_lavf = {
  "libavformat demuxer",
  "lavf",
  "libavformat",
  "Michael Niedermayer",
  "supports many formats, requires libavformat",
  DEMUXER_TYPE_LAVF,
  0, // Check after other demuxer
  lavf_check_file,
  demux_lavf_fill_buffer,
  demux_open_lavf,
  demux_close_lavf,
  demux_seek_lavf,
  demux_lavf_control
};

demuxer_desc_t demuxer_desc_lavf_preferred = {
  "libavformat preferred demuxer",
  "lavfpref",
  "libavformat",
  "Michael Niedermayer",
  "supports many formats, requires libavformat",
  DEMUXER_TYPE_LAVF_PREFERRED,
  1,
  lavf_check_preferred_file,
  demux_lavf_fill_buffer,
  demux_open_lavf,
  demux_close_lavf,
  demux_seek_lavf,
  demux_lavf_control
};
