#ifndef DEMUX_TS_H
#define DEMUX_TS_H

#define MAX_DEMUXER_FIFO_SIZE    10
#define MAX_AUDIO_FIFO_BUFSIZE   1024
#define MAX_VIDEO_FIFO_BUFSIZE   ( 8192 * 2 )

typedef struct __demux_ts_packct_t
{
	double pts;
	int len;
	unsigned char *buf;
	struct __demux_ts_packet_t *next,*priv;
}demux_ts_packet_t;

typedef struct __demux_ts_fifo_t
{
	int num,buf_size;
	demux_ts_packet_t *first, *last;
}demux_ts_fifo_t;

#endif
