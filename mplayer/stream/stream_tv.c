/*
 *  Copyright (C) 2006 Benjamin Zores
 *   Stream layer for TV Input, based on previous work from Albeu
 *
 *   This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"

#ifdef __LINUX__
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#include "stream.h"
#include "libmpdemux/demuxer.h"
#include "m_option.h"
#include "m_struct.h"
#include "tv.h"

#ifdef __LINUX__
#include <stdio.h>
#else
#include <mplaylib.h>
#endif

tv_param_t stream_tv_defaults = {
    NULL,          //freq
    NULL,          //channel
    "europe-east", //chanlist
    "pal",         //norm
    0,             //automute
#ifdef HAVE_TV_V4L2
    -1,            //normid
#endif
    NULL,          //device
    NULL,          //driver
    -1,            //width
    -1,            //height
    0,             //input, used in v4l and bttv
    -1,            //outfmt
    -1.0,          //fps
    NULL,          //channels
    0,             //noaudio;
    0,             //immediate;
    44100,         //audiorate;
    0,             //audio_id
#if defined(HAVE_TV_V4L)
    -1,            //amode
    -1,            //volume
    -1,            //bass
    -1,            //treble
    -1,            //balance
    -1,            //forcechan
    0,             //force_audio
    -1,            //buffer_size
    0,             //mjpeg
    2,             //decimation
    90,            //quality
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    0,             //alsa
#endif
#endif
    NULL,          //adevice
    0,             //brightness
    0,             //contrast
    0,             //hue
    0,             //saturation
    -1,            //gain
    NULL,          //tdevice
    0,             //tformat
    100,           //tpage
    0,             //tlang

    0,             //scan_autostart
    50,            //scan_threshold
    0.5            //scan_period
};

#define ST_OFF(f) M_ST_OFF(tv_param_t,f)
static m_option_t stream_opts_fields[] = {
    {"hostname", ST_OFF(channel), CONF_TYPE_STRING, 0, 0 ,0, NULL},
    {"filename", ST_OFF(input), CONF_TYPE_INT, 0, 0 ,0, NULL},
    { NULL, NULL, 0, 0, 0, 0,  NULL }
};

static struct m_struct_st stream_opts = {
    "tv",
    sizeof(tv_param_t),
    &stream_tv_defaults,
    stream_opts_fields
};

static void
tv_stream_close (stream_t *stream)
{
  if(stream->priv)
    m_struct_free(&stream_opts,stream->priv);
  stream->priv=NULL;
}
static int
tv_stream_open (stream_t *stream, int mode, void *opts, int *file_format)
{

  stream->type = STREAMTYPE_TV;
  stream->priv = opts;
  stream->close=tv_stream_close;
  *file_format =  DEMUXER_TYPE_TV;

  return STREAM_OK;
}

stream_info_t stream_info_tv = {
  "TV Input",
  "tv",
  "Benjamin Zores, Albeu",
  "",
  tv_stream_open, 			
  { "tv", NULL },
  &stream_opts,
  1
};
