
#include "config.h"

#include <mplaylib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

#ifndef NOAH_OS
#include "../uc_inc/mp_cache.h"
#endif

//#undef printf
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
  "file",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};  
static char *read_buf = 0;
#define DIRECT_MODE    0
#define NO_DIRECT_MODE 1

static char cur_mode = DIRECT_MODE;
static char sync_mode = 0;
static char init_mode = DIRECT_MODE;

extern void *audio_sem_filemode;

static int cur_filepos = 0,oldreadsize = 0,old_filepos = -1;



static int AUDIO_Seek(int f,unsigned int pos,int mode)
{
//	printf("mode = %d %d\n",mode,pos);
	
	if(mode == 0)
	{	
		cur_filepos = pos;
		if(pos == old_filepos)
			return old_filepos;	
	}
	cur_filepos = BUFF_Seek((JZFS_FILE *)f, pos, mode);
//	printf("cur_filepos = %d\n",cur_filepos);
	return cur_filepos;
}


void set_change_mode(char mode)
{
	if(init_mode != NO_DIRECT_MODE)
		cur_mode = mode;
}

#ifndef NOAH_OS
void stream_change_mode(int mode)
{
	char err;
	if((init_mode != NO_DIRECT_MODE) && (mode == 1))
	{
		
		os_ScheduleLock();
		sync_mode = 1;
		os_ScheduleUnlock();
		cur_mode = mode;
		os_SemaphoreSet(audio_sem_filemode,0,&err);
		os_SemaphorePend(audio_sem_filemode,200,&err);
	}
	
}
#endif

static int fill_buffer(stream_t *s, char** buffer, int max_len){
  int r;
  
  if(cur_mode == DIRECT_MODE)
  {	
  	  if(sync_mode)
  	  {
  	  	unsigned int cur_offset;
  	  	cur_offset = jzfs_Tell((JZFS_FILE *)s->fd);
  	  	//printf("cur_offset = %d %d\n",cur_offset,cur_filepos);
  	  	BUFF_ConsumeBuff(cur_offset);
#ifndef NOAH_OS  	  	
  	  	os_SemaphorePost(audio_sem_filemode);
#endif  	  	
  	  	sync_mode = 0;
  	  }
  	  r = BUFF_DirectRead((JZFS_FILE *)s->fd,buffer,max_len);
	  if(r)
	  	memcpy(read_buf,*buffer,r);
	  *buffer = read_buf;	
  }else
  {
  	if(sync_mode)
  	{
  		
  	  	unsigned int cur_offset = BUFF_PauseBuff();
  	  	//printf("cur_offset = %d\n",cur_offset);
  		if(BUFF_Seek((JZFS_FILE *)s->fd,cur_offset,SEEK_SET)<0) {
		    s->eof=1;
		    return 0;
		}
		old_filepos = -1;
		cur_filepos = cur_offset;
#ifndef NOAH_OS		
  	  	os_SemaphorePost(audio_sem_filemode);
#endif  	  	
  	  	sync_mode = 0;
  	}
	
  	if(old_filepos != cur_filepos)
  	{	
  		old_filepos = cur_filepos;
  		r = BUFF_DirectRead((JZFS_FILE *)s->fd,&read_buf,max_len);
  		oldreadsize = r;
  		
  	}
	*buffer = read_buf;
	r = oldreadsize;
	
	cur_filepos += oldreadsize;
  }	
  
  return (r <= 0) ? -1 : r;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
  int r = write(s->fd,buffer,len);
  return (r <= 0) ? -1 : r;
}

static int seek(stream_t *s,off_t newpos) {
  s->pos = newpos;
  
  if(AUDIO_Seek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
  return 1;
}

static int seek_forward(stream_t *s,off_t newpos) {
  if(newpos<s->pos){
    mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
    return 0;
  }
  while(s->pos<newpos){
#ifdef NOAH_OS
    int len=s->fill_buffer(s, s->buffer, STREAM_BUFFER_SIZE);
#else  	
    int len=s->fill_buffer(s,&(s->pBuffer),STREAM_BUFFER_SIZE);
#endif    
    if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; break; } // EOF
    s->buf_pos=0;
    s->buf_len=len;
    s->pos+=len;
  }
  return 1;
}
static void close_f(stream_t *s)
{
	F("\n");
	if(read_buf) 
	{
		free(read_buf);
		read_buf = 0;
	}
	BUFF_Close(s->fd);
	//close(s->fd);
	F("\n");
}

static int control(stream_t *s, int cmd, void *arg) {
  switch(cmd) {
    case STREAM_CTRL_GET_SIZE: {
      off_t size;

      size = AUDIO_Seek(s->fd, 0, SEEK_END);
      AUDIO_Seek(s->fd, s->pos, SEEK_SET);
      if(size != (off_t)-1) {
        *((off_t*)arg) = size;
        return 1;
      }
     case STREAM_CTRL_RESET:
     	F("reset;\n");
     	s->pos = 0;
     	AUDIO_Seek(s->fd, 0, SEEK_SET);
      return 1; 
    }
  }
  return STREAM_UNSUPPORTED;
}
static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {
  int f;
  mode_t m = 0;
  off_t len;
  unsigned char *filename;
  struct stream_priv_s* p = (struct stream_priv_s*)opts;
	char rmode[12];
  if(mode == STREAM_READ)
    strcpy(rmode,"rb");
  else if(mode == STREAM_WRITE)
    strcpy(rmode,"wb");
  else {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] Unknown open mode %d\n",mode);
    m_struct_free(&stream_opts,opts);
    return STREAM_UNSUPPORTED;
  }
  if(p->filename)
    filename = p->filename;
  else if(p->filename2)
    filename = p->filename2;
  else
    filename = NULL;
  if(!filename) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] No filename\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

   f=BUFF_Open(filename,rmode);

   read_buf = (char *)malloc(AUDIO_STREAM_BUFFER_SIZE);
   
   if(f==0) {
    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }
   if(BUFF_GetMode() == 1)
   {
	   init_mode = NO_DIRECT_MODE;
	   cur_mode = init_mode;
   }
//   printf("curmode = %d\n",cur_mode);
   
   
   len = BUFF_GetFileSize(f);
  //len=lseek(f,0,SEEK_END); lseek(f,0,SEEK_SET);
  if(len == -1) {
    if(mode == STREAM_READ) stream->seek = seek_forward;
    stream->type = STREAMTYPE_STREAM; // Must be move to STREAMTYPE_FILE
    stream->flags |= STREAM_SEEK_FW;
  } 
  else if(len >= 0) {
    stream->seek = seek;
    stream->end_pos = len;
    stream->type = STREAMTYPE_FILE;
  }

  mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %"PRId64" bytes\n", (int64_t)len);
  stream->sector_size = AUDIO_STREAM_BUFFER_SIZE;
  stream->fd = f;
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = NULL;
  stream->control = control;
  stream->close = close_f;
  
  cur_filepos = 0;
  old_filepos = -1;
  
  m_struct_free(&stream_opts,opts);
  return STREAM_OK;
}


stream_info_t stream_info_audiofile = {
  "AudioFile",
  "file",
  "Albeu",
  "based on the code from ??? (probably Arpi)",
  open_f,
  { "file", "", NULL },
  &stream_opts,
  1 // Urls are an option string
};
