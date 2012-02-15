

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
  "DAB/T-DMB",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};  
int tt = 0;
FILE *ff;
#endif

static unsigned int dab_id = 0;

void reset_tuner(struct stream_st *s)
{
	ioctl(s->fd, 0x2, 0);
	usleep(100 * 1000);
}

static int dab_control(struct stream_st *s,int cmd,void* arg)
{
	printf("Reset mobile TV ! %d \n",cmd);
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

#define STREAM_DAB_BUFFER_SIZE   ( 2048 * 6 )
//#define STREAM_DAB_BUFFER_SIZE   18800
#define MAX_RETRY_TIME_OFDM       400
#define MAX_RETRY_TIME_TUNER      10

static int fill_buffer(stream_t *s, char* buffer, int max_len){
	int len =0 ,ofdm_reset = 0, tuner_reset = 0;

	len=read(s->fd,s->buffer,STREAM_DAB_BUFFER_SIZE);
	while( len <= 0 ) {
//		printf("read 0000 \n");
#if 0
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
			ioctl(s->fd, 0x2, 0);
			usleep(100 * 1000);
		}
#endif
		len=read(s->fd,s->buffer,STREAM_DAB_BUFFER_SIZE);
//		printf("Read len %d \n",len);
	}
	return len;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
	int r;
	return r;
}

static int seek_forward(stream_t *s,off_t newpos) {
	return 0;

	if ( newpos<s->pos ) {
		mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
		return 0;
	}
	while ( s->pos < newpos ) {
		int len=s->fill_buffer(s,s->buffer,STREAM_BUFFER_SIZE);
		if ( len <= 0 ) { 
			s->eof=1; 
			s->buf_pos=s->buf_len=0; 
			break; 
		} // EOF
		s->buf_pos=0;
		s->buf_len=len;
		s->pos+=len;
	}
	return 1;
}

static int seek(stream_t *s,off_t newpos) {
  s->pos = newpos;
  return 1;  //always ok!

  if(lseek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
  return 1;
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {
  int f;
  mode_t m = 0;
  off_t len;
  unsigned char *filename;
  struct stream_priv_s* p = (struct stream_priv_s*)opts;

  if(p->filename)
    filename = p->filename;

//  filename = &(stream->url)[3];
  printf("Dab stream opening! %s \n",filename);
  if ( !filename )
#if 0    // daneil
	 f=open("nodmtv",O_RDWR|O_SYNC);     
#else
   ;
#endif
  else
#if 0     // daniel
	f=open(filename,O_RDWR|O_SYNC);
#else
    ;
#endif

    if(f<0) {
	    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
	    m_struct_free(&stream_opts,opts);
	    return STREAM_ERROR;
    }

       usleep(200 * 1000);
       dab_id = 0x00444142;
//    stream->seek = seek;
    stream->seek = seek_forward;
    stream->flags |= STREAM_SEEK_FW;
    stream->type = STREAMTYPE_DAB;
    stream->fd = f;
    stream->fill_buffer = fill_buffer;
    stream->write_buffer = write_buffer;
    stream->control = dab_control;
    stream->priv = (unsigned int *) &dab_id;
//    ff = fopen("mdump.dab","w+");
    ioctl(f , 0x8 , 0);
    m_struct_free(&stream_opts,opts);
    return STREAM_OK;
}

stream_info_t stream_info_dab = {
	"DAB/T-DMB Stream input",
	"DAB/T-DMB",
	"Yliu",
	"Stream input support DAB/T-DMB!",
	open_f,
	{ "dab", NULL },
	&stream_opts,
	1 // Urls are an option string
};
