#ifndef MUTI_THREAD_STRUCT_H
#define MUTI_THREAD_STRUCT_H

typedef struct __mts_func_t
{
	volatile double *locked_ocr,* err_degree;
	volatile int *packet_cnt,*packet_err ;

	int (* get_first_pts)(demuxer_t *,double *);
	int (* get_packet)(demuxer_t *,demux_stream_t *,unsigned char **,double *);
	double (* query_speed)(stream_t *);

}mts_func_t;


#endif
