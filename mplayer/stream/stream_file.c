
#include "config.h"

#ifdef __LINUX__
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <mplaylib.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

#ifndef __LINUX__
#ifndef NOAH_OS
#include "../uc_inc/mp_cache.h"
#endif
#endif

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

extern int noah_long_jump(int err);
static int fill_buffer(stream_t *s, char** buffer, int max_len){
#ifdef __LINUX__
  int r = read(s->fd,buffer,max_len);
#else
  int r = BUFF_DirectRead((JZFS_FILE *)s->fd,buffer,max_len);
#endif
// 	if( r <= 0 )
// 		kprintf("fill_buffer read = %d\n",r);
  noah_long_jump(r);
  return (r <= 0) ? -1 : r;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
  int r = write(s->fd,buffer,len);
  return (r <= 0) ? -1 : r;
}

static int seek(stream_t *s,off_t newpos) {
  s->pos = newpos;
  
#ifdef __LINUX__
  if(lseek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
#else
  if(BUFF_Seek((JZFS_FILE *)s->fd,s->pos,SEEK_SET)<0) 
  {
//  	kprintf("seek finish\n");
    s->eof=1;
    return 0;
  }
#endif
  return 1;
}

static int seek_forward(stream_t *s,off_t newpos) {
  if(newpos<s->pos){
    mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
    return 0;
  }
  while(s->pos<newpos){
#if  defined(__LINUX__) || defined (NOAH_OS)
    int len=s->fill_buffer(s,s->buffer,STREAM_BUFFER_SIZE);
#else
    int len=s->fill_buffer(s,&(s->pBuffer),STREAM_BUFFER_SIZE);
#endif
	if(len<=0)
	{ 
//		kprintf("seek forward finish\n");
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
#ifndef __LINUX__
static void close_f(stream_t *s)
{
	F("\n");
	BUFF_Close(s->fd);
	//close(s->fd);
	F("\n");
}
#endif

static int control(stream_t *s, int cmd, void *arg) {
  switch(cmd) {
    case STREAM_CTRL_GET_SIZE: {
      off_t size;

#ifdef __LINUX__
      size = lseek(s->fd, 0, SEEK_END);
      lseek(s->fd, s->pos, SEEK_SET);
#else
      size = BUFF_Seek(s->fd, 0, SEEK_END);
      BUFF_Seek(s->fd, s->pos, SEEK_SET);
#endif
      if(size != (off_t)-1) {
        *((off_t*)arg) = size;
        return 1;
      }
#ifndef __LINUX__
     case STREAM_CTRL_RESET:
//     	F("reset;\n");
     	s->pos = 0;
     	BUFF_Seek(s->fd, 0, SEEK_SET);
      return 1; 
#endif
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
#ifdef __LINUX__
  if(mode == STREAM_READ)
    m = O_RDONLY;
  else if(mode == STREAM_WRITE)
    m = O_RDWR|O_CREAT|O_TRUNC;
#else
	char rmode[12];
  if(mode == STREAM_READ)
    strcpy(rmode,"rb");
  else if(mode == STREAM_WRITE)
    strcpy(rmode,"wb");
#endif
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

#ifdef ANDROID    /*Cynthia,0518-2009*/
  extern int playfile_fd_opened;
  if ( playfile_fd_opened > 0 ) {
	  f = playfile_fd_opened;
	  goto opened;
  }
#endif

  if(!filename) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] No filename\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

#ifdef __LINUX__
#if defined(__CYGWIN__)|| defined(__MINGW32__)
  m |= O_BINARY;
#endif    

  if(!strcmp(filename,"-")){
    if(mode == STREAM_READ) {
      // read from stdin
      mp_msg(MSGT_OPEN,MSGL_INFO,MSGTR_ReadSTDIN);
      f=0; // 0=stdin
#ifdef __MINGW32__
	  setmode(fileno(stdin),O_BINARY);
#endif
    } else {
      mp_msg(MSGT_OPEN,MSGL_INFO,"Writing to stdout\n");
      f=1;
#ifdef __MINGW32__
	  setmode(fileno(stdout),O_BINARY);
#endif
    }
  } else {
    if(mode == STREAM_READ)
      f=open(filename,m);
    else {
      mode_t openmode = S_IRUSR|S_IWUSR;
#ifndef __MINGW32__
      openmode |= S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
#endif
      f=open(filename,m, openmode);
    }
    if(f<0) {
      mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
      m_struct_free(&stream_opts,opts);
      return STREAM_ERROR;
    }
  }

#else
   f=BUFF_Open(filename,rmode);
   if(f==0) {
    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

#endif

#ifdef ANDROID
opened:
#endif	/* ANDROID */

#ifdef __LINUX__
  len=lseek(f,0,SEEK_END); lseek(f,0,SEEK_SET);
#ifdef __MINGW32__
  if(f==0 || len == -1) {
#else
  if(len == -1) {
#endif

#else
	len = BUFF_GetFileSize(f);
  if(len == -1) {
#endif
    if(mode == STREAM_READ) stream->seek = seek_forward;
    stream->type = STREAMTYPE_STREAM; // Must be move to STREAMTYPE_FILE
    stream->flags |= STREAM_SEEK_FW;
  } else if(len >= 0) {
    stream->seek = seek;
    stream->end_pos = len;
    stream->type = STREAMTYPE_FILE;
  }

  mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %"PRId64" bytes\n", (int64_t)len);
#ifdef __LINUX__
  stream->fd = f;
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = write_buffer;
  stream->control = control;
#else
  stream->sector_size = 0;
  stream->fd = f;
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = NULL;
  stream->control = control;
  stream->close = close_f; 
#endif
  m_struct_free(&stream_opts,opts);
  return STREAM_OK;
}

stream_info_t stream_info_file = {
  "File",
  "file",
  "Albeu",
  "based on the code from ??? (probably Arpi)",
  open_f,
  { "file", "", NULL },
  &stream_opts,
  1 // Urls are an option string
};
