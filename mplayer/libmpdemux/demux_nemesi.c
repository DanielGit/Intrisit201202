/*
 *  Copyright (C) 2007 Alessandro Molina <amol.wrk@gmail.com>
 *
 *  MPlayer is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  MPlayer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MPlayer; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "demux_nemesi.h"
#include "stheader.h"
#define HAVE_STRUCT_SOCKADDR_STORAGE
#include "nemesi/rtsp.h"
#include "nemesi/rtp.h"

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

int rtsp_transport_tcp = 0;
int rtsp_transport_sctp = 0;
// extern int rtsp_port;

typedef enum { NEMESI_SESSION_VIDEO,
               NEMESI_SESSION_AUDIO } Nemesi_SessionType;

typedef struct {
    rtsp_ctrl * rtsp;
    rtp_session * session[2];
    rtp_frame first_pkt[2];
    double time[2];
    double seek;
} Nemesi_DemuxerStreamData;

static void link_session_and_fetch_conf(Nemesi_DemuxerStreamData * ndsd,
                                        Nemesi_SessionType stype,
                                        rtp_session * sess,
                                        rtp_buff * buff, unsigned int * fps)
{
    extern float force_fps;
    rtp_ssrc *ssrc;
    rtsp_ctrl * ctl = ndsd->rtsp;
    rtp_frame * fr = &ndsd->first_pkt[stype];
    rtp_buff trash_buff;

    ndsd->session[stype] = sess;

    if (buff == NULL)
        buff = &trash_buff;

    if ( (buff != NULL) || (fps != NULL) ) {
        rtp_fill_buffers(rtsp_get_rtp_th(ctl));
        for (ssrc = rtp_active_ssrc_queue(rtsp_get_rtp_queue(ctl));
             ssrc;
             ssrc = rtp_next_active_ssrc(ssrc)) {
            if (ssrc->rtp_sess == sess) {
                rtp_fill_buffer(ssrc, fr, buff);
                break;
            }
        }

        if ( (force_fps == 0.0) && (fps != NULL) ) {
            rtp_fill_buffers(rtsp_get_rtp_th(ctl));
            *fps = rtp_get_fps(ssrc);
        }
    }
}

demuxer_t* demux_open_rtp(demuxer_t* demuxer)
{
    nms_rtsp_hints hints;
    char * url = demuxer->stream->streaming_ctrl->url->url;
    rtsp_ctrl * ctl;
    RTSP_Error reply;
    rtsp_medium * media;
    Nemesi_DemuxerStreamData * ndsd = calloc(1, sizeof(Nemesi_DemuxerStreamData));

    memset(&hints,0,sizeof(hints));
//  if (rtsp_port) hints.first_rtp_port = rtsp_port;
    if (rtsp_transport_tcp) {
        hints.pref_rtsp_proto = TCP;
        hints.pref_rtp_proto = TCP;
    }
    if (rtsp_transport_sctp) {
        hints.pref_rtsp_proto = SCTP;
        hints.pref_rtp_proto = SCTP;
    }

    mp_msg(MSGT_DEMUX, MSGL_INFO, "Initializing libNemesi\n");
    if ((ctl = rtsp_init(&hints)) == NULL) {
        free(ndsd);
        return STREAM_ERROR;
    }

    ndsd->rtsp = ctl;
    demuxer->priv = ndsd;
    //nms_verbosity_set(1);

    mp_msg(MSGT_DEMUX, MSGL_INFO, "Opening: %s\n", url);
    if (rtsp_open(ctl, url)) {
        mp_msg(MSGT_DEMUX, MSGL_ERR, "rtsp_open failed.\n");
        return demuxer;
    }

    reply = rtsp_wait(ctl);
    if (reply.got_error) {
        mp_msg(MSGT_DEMUX, MSGL_ERR,
               "OPEN Error from the server: %s\n",
               reply.message.reply_str);
        return demuxer;
    }

    rtsp_play(ctl, 0, 0);
    reply = rtsp_wait(ctl);
    if (reply.got_error) {
        mp_msg(MSGT_DEMUX, MSGL_ERR,
               "PLAY Error from the server: %s\n",
               reply.message.reply_str);
        return demuxer;
    }

    media = ctl->rtsp_queue->media_queue;
    for (; media; media=media->next) {
        sdp_medium_info * info = media->medium_info;
        rtp_session * sess = media->rtp_sess;

        int media_format = atoi(info->fmts);
        rtp_pt * ptinfo = rtp_get_pt_info(sess, media_format);
        char const * format_name = ptinfo ? ptinfo->name : NULL;

        if (sess->parsers[media_format] == NULL) {
            mp_msg(MSGT_DEMUX, MSGL_ERR,
                   "libNemesi unsupported media format: %s\n",
                   format_name ? format_name : info->fmts);
            continue;
        }
        else {
            mp_msg(MSGT_DEMUX, MSGL_INFO,
                   "libNemesi supported media: %s\n",
                   format_name);
        }

        if (ptinfo->type == AU) {
            if (ndsd->session[NEMESI_SESSION_AUDIO] == NULL) {
                sh_audio_t* sh_audio = new_sh_audio(demuxer,0);
                WAVEFORMATEX* wf = calloc(1,sizeof(WAVEFORMATEX));
                demux_stream_t* d_audio = demuxer->audio;

                mp_msg(MSGT_DEMUX, MSGL_INFO, "Detected as AUDIO stream...\n");

                link_session_and_fetch_conf(ndsd, NEMESI_SESSION_AUDIO,
                                            sess, NULL, NULL);

                sh_audio->wf = wf;
                d_audio->sh = sh_audio;
                sh_audio->ds = d_audio;
                wf->nSamplesPerSec = 0;

                //List of known audio formats
                if (!strcmp(format_name, "MPA"))
                    wf->wFormatTag =
                        sh_audio->format = 0x55;
                else if (!strcmp(format_name, "vorbis"))
                    wf->wFormatTag =
                        sh_audio->format = mmioFOURCC('v','r','b','s');
                else
                    mp_msg(MSGT_DEMUX, MSGL_WARN,
                           "Unknown MPlayer format code for MIME"
                           " type \"audio/%s\"\n", format_name);
            } else {
                mp_msg(MSGT_DEMUX, MSGL_ERR,
                       "There is already an audio session registered,"
                       " ignoring...\n");
            }
        } else if (ptinfo->type == VI) {
            if (ndsd->session[NEMESI_SESSION_AUDIO] == NULL) {
                sh_video_t* sh_video;
                BITMAPINFOHEADER* bih;
                demux_stream_t* d_video;
                int fps = 0;
                rtp_buff buff;

                mp_msg(MSGT_DEMUX, MSGL_INFO, "Detected as VIDEO stream...\n");

                link_session_and_fetch_conf(ndsd, NEMESI_SESSION_VIDEO,
                                            sess, &buff, &fps);

                if (buff.len) {
                    bih = calloc(1,sizeof(BITMAPINFOHEADER)+buff.len);
                    bih->biSize = sizeof(BITMAPINFOHEADER)+buff.len;
                    memcpy(bih+1, buff.data, buff.len);
                } else {
                    bih = calloc(1,sizeof(BITMAPINFOHEADER));
                    bih->biSize = sizeof(BITMAPINFOHEADER);
                }

                sh_video = new_sh_video(demuxer,0);
                sh_video->bih = bih;
                d_video = demuxer->video;
                d_video->sh = sh_video;
                sh_video->ds = d_video;

                if (fps)
                    sh_video->fps = fps;

                //List of known video formats
                if (!strcmp(format_name, "MPV")) {
                    bih->biCompression =
                        sh_video->format = mmioFOURCC('M','P','E','G');
                } else if (!strcmp(format_name, "H264")) {
                    bih->biCompression =
                        sh_video->format = mmioFOURCC('H','2','6','4');
                } else if (!strcmp(format_name, "H263-1998")) {
                    bih->biCompression =
                        sh_video->format = mmioFOURCC('H','2','6','3');
                } else {
                    mp_msg(MSGT_DEMUX, MSGL_WARN,
                        "Unknown MPlayer format code for MIME"
                        " type \"video/%s\"\n", format_name);
                }
            } else {
                mp_msg(MSGT_DEMUX, MSGL_ERR,
                       "There is already a video session registered,"
                       " ignoring...\n");
            }
        } else {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "Unsupported media type\n");
        }
    }

    demuxer->stream->eof = 0;

    return demuxer;
}

static int get_data_for_session(Nemesi_DemuxerStreamData * ndsd,
                                Nemesi_SessionType stype, rtp_frame * fr)
{
    rtsp_ctrl * ctl = ndsd->rtsp;
    rtp_ssrc *ssrc = NULL;

    for (ssrc = rtp_active_ssrc_queue(rtsp_get_rtp_queue(ctl));
         ssrc;
         ssrc = rtp_next_active_ssrc(ssrc)) {
        if (ssrc->rtp_sess == ndsd->session[stype]) {
            if (ndsd->first_pkt[stype].len != 0) {
                fr->data = ndsd->first_pkt[stype].data;
                fr->time_sec = ndsd->first_pkt[stype].time_sec;
                fr->len = ndsd->first_pkt[stype].len;
                ndsd->first_pkt[stype].len = 0;
                return RTP_FILL_OK;
            } else {
                rtp_buff buff;
                return rtp_fill_buffer(ssrc, fr, &buff);
            }
        }
    }

    return RTP_SSRC_NOTVALID;
}

int demux_rtp_fill_buffer(demuxer_t* demuxer, demux_stream_t* ds)
{
    Nemesi_DemuxerStreamData * ndsd = demuxer->priv;
    Nemesi_SessionType stype;
    rtsp_ctrl * ctl = ndsd->rtsp;
    rtp_thread * rtp_th = rtsp_get_rtp_th(ctl);
    rtp_frame fr;

    demux_packet_t* dp;

    if ( (!ctl->rtsp_queue) || (demuxer->stream->eof) || (rtp_fill_buffers(rtp_th)) ) {
        mp_msg(MSGT_DEMUX, MSGL_INFO, "End of Stream...\n");
        demuxer->stream->eof = 1;
        return 0;
    }

    if (ds == demuxer->video)
        stype = NEMESI_SESSION_VIDEO;
    else if (ds == demuxer->audio)
        stype = NEMESI_SESSION_AUDIO;
    else
        return 0;

    if(!get_data_for_session(ndsd, stype, &fr)) {
        dp = new_demux_packet(fr.len);
        memcpy(dp->buffer, fr.data, fr.len);
        fr.time_sec += ndsd->seek;
        ndsd->time[stype] = dp->pts = fr.time_sec;
        ds_add_packet(ds, dp);
    }
    else {
        stype = (stype + 1) % 2;
        if (stype == NEMESI_SESSION_VIDEO)
            ds = demuxer->video;
        else
            ds = demuxer->audio;

        if(!get_data_for_session(ndsd, stype, &fr)) {
            dp = new_demux_packet(fr.len);
            memcpy(dp->buffer, fr.data, fr.len);
            fr.time_sec += ndsd->seek;
            ndsd->time[stype] = dp->pts = fr.time_sec;
            ds_add_packet(ds, dp);
        }
    }

    return 1;
}


void demux_close_rtp(demuxer_t* demuxer)
{
    Nemesi_DemuxerStreamData * ndsd = demuxer->priv;
    rtsp_ctrl * ctl = ndsd->rtsp;
    RTSP_Error err;

    mp_msg(MSGT_DEMUX, MSGL_INFO, "Closing libNemesi RTSP Stream...\n");

    if (ndsd == NULL)
        return;

    free(ndsd);

    if (rtsp_close(ctl)) {
        err = rtsp_wait(ctl);
        if (err.got_error)
            mp_msg(MSGT_DEMUX, MSGL_ERR,
                   "Error Closing Stream: %s\n",
                   err.message.reply_str);
    }

    rtsp_uninit(ctl);
}

static void demux_seek_rtp(demuxer_t *demuxer, float rel_seek_secs,
                           float audio_delay, int flags)
{
    Nemesi_DemuxerStreamData * ndsd = demuxer->priv;
    rtsp_ctrl * ctl = ndsd->rtsp;
    sdp_attr * r_attr = NULL;
    sdp_range r = {0, 0};
    double time = ndsd->time[NEMESI_SESSION_VIDEO] ?
                  ndsd->time[NEMESI_SESSION_VIDEO] :
                  ndsd->time[NEMESI_SESSION_AUDIO];

    if (!ctl->rtsp_queue)
        return;

    r_attr = sdp_get_attr(ctl->rtsp_queue->info->attr_list, "range");
    if (r_attr)
        r = sdp_parse_range(r_attr->value);

    //flags & 1 -> absolute seek
    //flags & 2 -> percent seek
    if (flags == 0) {
        time += rel_seek_secs;
        if (time < r.begin)
            time = r.begin;
        else if (time > r.end)
            time = r.end;
        ndsd->seek = time;

        mp_msg(MSGT_DEMUX,MSGL_WARN,"libNemesi SEEK %f on %f - %f)\n",
           time, r.begin, r.end);

        if (!rtsp_seek(ctl, time, 0)) {
            RTSP_Error err = rtsp_wait(ctl);
            if (err.got_error) {
                mp_msg(MSGT_DEMUX, MSGL_ERR,
                       "Error Performing Seek: %s\n",
                       err.message.reply_str);
                demuxer->stream->eof = 1;
            }
            else
                mp_msg(MSGT_DEMUX, MSGL_INFO, "Seek, performed\n");
        }
        else {
            mp_msg(MSGT_DEMUX, MSGL_ERR, "Unable to pause stream to perform seek\n");
            demuxer->stream->eof = 1;
        }
    }
    else
        mp_msg(MSGT_DEMUX, MSGL_ERR, "Unsupported seek type\n");
}

static int demux_rtp_control(struct demuxer_st *demuxer, int cmd, void *arg)
{
    Nemesi_DemuxerStreamData * ndsd = demuxer->priv;
    rtsp_ctrl * ctl = ndsd->rtsp;
    sdp_attr * r_attr = NULL;
    sdp_range r = {0, 0};
    double time = ndsd->time[NEMESI_SESSION_VIDEO] ?
                  ndsd->time[NEMESI_SESSION_VIDEO] :
                  ndsd->time[NEMESI_SESSION_AUDIO];

    if (!ctl->rtsp_queue)
        return DEMUXER_CTRL_DONTKNOW;

    r_attr = sdp_get_attr(ctl->rtsp_queue->info->attr_list, "range");
    if (r_attr)
        r = sdp_parse_range(r_attr->value);

    switch (cmd) {
        case DEMUXER_CTRL_GET_TIME_LENGTH:
            if (r.end == 0)
                return DEMUXER_CTRL_DONTKNOW;

            *((double *)arg) = ((double)r.end) - ((double)r.begin);
            return DEMUXER_CTRL_OK;

        case DEMUXER_CTRL_GET_PERCENT_POS:
            if (r.end == 0)
                return DEMUXER_CTRL_DONTKNOW;

            *((int *)arg) = (int)( time * 100 / (r.end - r.begin) );
            return DEMUXER_CTRL_OK;
        default:
           return DEMUXER_CTRL_DONTKNOW;
    }
}

demuxer_desc_t demuxer_desc_rtp = {
  "libNemesi demuxer",
  "rtp",
  "",
  "Alessandro Molina",
  "requires libNemesi",
  DEMUXER_TYPE_RTP,
  0, // no autodetect
  NULL,
  demux_rtp_fill_buffer,
  demux_open_rtp,
  demux_close_rtp,
  demux_seek_rtp,
  demux_rtp_control
};
