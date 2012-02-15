/*
 *  Copyright (C) 2007 Alessandro Molina <amol.wrk@gmail.com>
 *   based on previous RTSP support from Benjamin Zores.
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

#define HAVE_STRUCT_SOCKADDR_STORAGE

#ifdef __LINUX__
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#include <mplaylib.h>
#include <mplaylib.h>
#endif
#include <ctype.h>
#include "config.h"

#include "nemesi/rtsp.h"
#include "libmpdemux/demux_nemesi.h"

#include <errno.h>

#include "stream.h"
#include "tcp.h"

extern int network_bandwidth;
int rtsp_port = 0;
char *rtsp_destination = NULL;

static int rtsp_streaming_seek(int fd, off_t pos,
                               streaming_ctrl_t* streaming_ctrl) {
    return -1;
}

static int rtsp_streaming_open (stream_t *stream, int mode, void *opts,
                                int *file_format)
{
    rtsp_ctrl * ctl;
    extern int index_mode;
    URL_t *url;
    stream->fd = -1;

    mp_msg (MSGT_OPEN, MSGL_V, "STREAM_RTSP, URL: %s\n", stream->url);
    stream->streaming_ctrl = streaming_ctrl_new ();
    if (!stream->streaming_ctrl)
    return STREAM_ERROR;

    stream->streaming_ctrl->bandwidth = network_bandwidth;
    url = url_new(stream->url);
    stream->streaming_ctrl->url = check4proxies(url);
    stream->streaming_ctrl->streaming_seek = rtsp_streaming_seek;

    *file_format = DEMUXER_TYPE_RTP;
    stream->type = STREAMTYPE_STREAM;
    return STREAM_OK;
}

stream_info_t stream_info_rtsp = {
  "RTSP streaming",
  "rtsp",
  "Alessandro Molina",
  "implemented over libnemesi",
  rtsp_streaming_open,
  {"rtsp", NULL},
  NULL,
  0 /* Urls are an option string */
};

