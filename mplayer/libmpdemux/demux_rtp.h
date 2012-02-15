#ifndef DEMUX_RTP_H
#define DEMUX_RTP_H

#ifdef __LINUX__
#include <stdlib.h>
#include <stdio.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#ifndef STREAM_H
#include "stream/stream.h"
#endif
#ifndef DEMUXER_H
#include "demuxer.h"
#endif

// Open a RTP demuxer (which was initiated either from a SDP file,
// or from a RTSP URL):
demuxer_t* demux_open_rtp(demuxer_t* demuxer);

// Test whether a RTP demuxer is for a MPEG stream:
int demux_is_mpeg_rtp_stream(demuxer_t* demuxer);

// Test whether a RTP demuxer contains combined (multiplexed)
// audio+video (and so needs to be demuxed by higher-level code):
int demux_is_multiplexed_rtp_stream(demuxer_t* demuxer);

// Read from a RTP demuxer:
int demux_rtp_fill_buffer(demuxer_t *demux, demux_stream_t* ds);

// Close a RTP demuxer
void demux_close_rtp(demuxer_t* demuxer);

#endif
