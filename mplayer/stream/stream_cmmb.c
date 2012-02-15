
#include <mplaylib.h>
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"
#include "libmpdemux/demuxer.h"

#define EACH_SIZE 0x6000

#define MTV_CMD_HARD_RESET       0x01
#define MTV_CMD_SOFT_RESET       0x02
#define MTV_CMD_FLUSH_FIFO       0x03
#define MTV_CMD_QUERY_FIFO       0x04
#define MTV_CMD_QUERY_SIGN       0x05
#define MTV_CMD_POWER_DOWN       0x06

#define MTV_COMMOM_CMD_NUM       0x06

#define MTV_CMD_SET_FREQUENCY       (0x01 + MTV_COMMOM_CMD_NUM)
#define MTV_CMD_SET_CHANNEL         (0x02 + MTV_COMMOM_CMD_NUM)

typedef struct 
{
	unsigned char select_ch;
	unsigned char enable_0,ts_start_0,ts_count_0,modu_type_0,rs_mode_0,outer_mode_0,ldpc_mode_0;
	unsigned char enable_1,ts_start_1,ts_count_1,modu_type_1,rs_mode_1,outer_mode_1,ldpc_mode_1;
}channel_config;

#if 1
static struct stream_priv_s {
  char* filename;
  char *filename2;
} stream_priv_dflts = {
  NULL, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static m_option_t stream_opts_fields[] = {
  {"string", ST_OFF(filename), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  {"filename", ST_OFF(filename2), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  { NULL, NULL, 0, 0, 0, 0,  NULL }
};

static struct m_struct_st stream_opts = {
  "CMMB",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};  
FILE *ff;
#endif

static unsigned int cmmb_id = 0;

static int cmmb_control(struct stream_st *s,int cmd,void* arg)
{
	printf("Reset mobile TV CMMB ! %d \n",cmd);
	switch (cmd)
	{
	case 0x47:
//		close(s->fd);
//		usleep(10);
//		s->fd = open("nodmtv",O_RDWR|O_SYNC);
//		usleep(500);
		break;
	default: ;
	}
	return 1;
}

#define STREAM_CMMB_BUFFER_SIZE   ( 0xffff )
#define MAX_RETRY_TIME_OFDM       400
#define MAX_RETRY_TIME_TUNER      10

static int fill_buffer(stream_t *s, char* buffer, int max_len){
	int len =0 ,ofdm_reset = 0, tuner_reset = 0;

	len=read(s->fd,s->buffer,STREAM_CMMB_BUFFER_SIZE);
#if 0
	while( len <= 0 ) {
		usleep(10 * 1000);
		ofdm_reset ++ ;
		if ( ofdm_reset > MAX_RETRY_TIME_OFDM )          //reset ofdm
		{
			tuner_reset ++;
			if ( tuner_reset > MAX_RETRY_TIME_TUNER ) //signal stop,program end!
			{
				printf("Signal stop,program end! \n");
				return 0;
			}
			ioctl(s->fd, 0x5, 0);
			usleep(100 * 1000);
		}
		len=read(s->fd,s->buffer,STREAM_CMMB_BUFFER_SIZE);
	}
#endif
//	printf("Read len %d \n",len);
	return len;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
	int r;
	return r;
}

static int seek_forward(stream_t *s,off_t newpos) {
	return 1;
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {
	int f,frq;
  mode_t m = 0;
  off_t len;
  unsigned char *filename;
  struct stream_priv_s* p = (struct stream_priv_s*)opts;
  channel_config cc;

  if(p->filename)
	  filename = p->filename;
  
  printf("CMMB stream opening! \n");
#if 0  //daniel
  f=open(filename,O_RDWR|O_SYNC);
#endif
  
    if(f<0) {
	    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
	    m_struct_free(&stream_opts,opts);
	    return STREAM_ERROR;
    }
	frq = 20;
	ioctl( f , MTV_CMD_SET_FREQUENCY, &frq);
	cc.select_ch = 0;
	cc.ts_start_0 = 19;
	cc.ts_count_0 = 4;
	cc.modu_type_0 = 0x01;
	cc.rs_mode_0 = 0x01;
	cc.outer_mode_0 = 0x01;
	cc.ldpc_mode_0 = 0x00;
	ioctl( f , MTV_CMD_SET_CHANNEL , &cc);

//    usleep(200 * 1000);
    cmmb_id = 0x434d4d42;
    stream->seek = seek_forward;
    stream->flags |= STREAM_SEEK_FW;
    stream->type = STREAMTYPE_CMMB;
    stream->fd = f;
    stream->fill_buffer = fill_buffer;
    stream->write_buffer = write_buffer;
    stream->control = cmmb_control;
    stream->priv = (unsigned int *) & cmmb_id;
    m_struct_free(&stream_opts,opts);
    return STREAM_OK;
}

stream_info_t stream_info_cmmb = {
	"CMMB Stream input",
	"CMMB",
	"Yliu",
	"Stream input support CMMB!",
	open_f,
	{ "cmmb", NULL },
	&stream_opts,
	1 // Urls are an option string
};
