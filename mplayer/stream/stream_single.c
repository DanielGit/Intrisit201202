
#include "config.h"

#include <mplaylib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

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
//#undef printf

//////////////////////////////////////////////////////////////////////
#undef fseek
#undef fopen
#undef fclose
#undef mplayread
#undef GetFileSize

#define fseek(f,pos,mode) SBUF_Seek(f,pos,mode)
#define fopen(x,y) SBUF_Open(x ,y)
#define fclose(f) SBUF_Close(f)

#define mplayread(f,buf,size) SBUF_Read(f,buf,size)
#define GetFileSize(f) SBUF_GetFileSize(f)
extern unsigned int FS_FReadEx(void *pData, unsigned int Size, unsigned int N, JZFS_FILE *pFile);

//static char *read_buf = 0;
//static int old_offset = -1,cur_offset = 0;
typedef struct __Read_Buff_Type_Tag{
    char *read_buf;
    int offset;
    int len;
}Read_Buff_Type;
static Read_Buff_Type pFileBuf[2];
static int idCurBuf = -1;
static int cur_offset = 0;
//#define READ_BUFF_SIZE 4096
#define READ_BUFF_SIZE		(32 * 1024)

static int SBUF_Open(char *s,char *mode)
{
    int i;
    
    JZFS_FILE *fp = NULL;
	mode = mode;
    for(i = 0;i < 2;i++)
    {
        pFileBuf[i].read_buf = (char *)memalign(32,READ_BUFF_SIZE);
        if(pFileBuf[i].read_buf == NULL) goto openbuferror;
        pFileBuf[i].offset = -1;
        pFileBuf[i].len = 0;
        
    }
    idCurBuf = -1;
    cur_offset = 0;
    fp = jzfs_Open(s,"rb");
	return (int)fp;
 openbuferror:
    for(i = 0;i < 2;i++)
    {
        if(pFileBuf[i].read_buf)
            free(pFileBuf[i].read_buf);
        pFileBuf[i].read_buf = 0;
        
    }
    return 0;
}
static void SBUF_Close(int f)
{
	JZFS_FILE *fp = (JZFS_FILE *)f;
    int i;
     for(i = 0;i < 2;i++)
     {
         if(pFileBuf[i].read_buf)
             free(pFileBuf[i].read_buf);
         pFileBuf[i].read_buf = 0;
         
     }
     if(fp)
         jzfs_Close(fp);
}
static unsigned int SBUF_GetFileSize(int f)
{
#ifndef NOAH_OS	
	JZFS_FILE *fp = (JZFS_FILE *)f;
	return fp->nSize;
#else
  return BUFF_GetFileSize (f);
#endif	
}
static int SBUF_Seek(int f, int Offset, int Whence)
{
	JZFS_FILE *fp = (JZFS_FILE *)f;
    int i;
    F("Offset %d Whence = %d\n",Offset,Whence);
    if(Whence == 0)
    {
        for(i = 0;i < 2;i++)
        {
            
            if(Offset == pFileBuf[i].offset)
            {
                idCurBuf = -1; 
                cur_offset = pFileBuf[i].offset;
                F("cur_offset = %d\n",cur_offset);
                return cur_offset;
            }
            
        }
    }
	cur_offset = jzfs_Seek(fp,Offset,Whence);
    idCurBuf = -1;
    F("cur_offset = %d\n",cur_offset);
    return cur_offset;
}
static int SBUF_Read(int f,char **ppData,int len)
{
	JZFS_FILE *fp = (JZFS_FILE *)f;
	int len1;
    int i;
    F("idCurBuf = %d cur_offset = %d\n",idCurBuf,cur_offset);
    if(idCurBuf == -1)
        for(i = 0;i < 2;i++){
            if(cur_offset == pFileBuf[i].offset){
                *ppData = pFileBuf[i].read_buf;
                cur_offset = cur_offset + pFileBuf[i].len;
                jzfs_Seek(fp,cur_offset,0);
                return pFileBuf[i].len;
            }
        }
    idCurBuf = (cur_offset / READ_BUFF_SIZE) & 1;
    F("idCurBuf = %d\n",idCurBuf);
    F("cur_offset = %d,pFileBuf[idCurBuf].offset = %d\n",cur_offset,pFileBuf[idCurBuf].offset);
    
    if(cur_offset == pFileBuf[idCurBuf].offset){
        *ppData = pFileBuf[idCurBuf].read_buf;
        cur_offset = cur_offset + pFileBuf[idCurBuf].len;
        jzfs_Seek(fp,cur_offset,0);
        return pFileBuf[idCurBuf].len;
    }
    pFileBuf[idCurBuf].offset = cur_offset;
    __dcache_hitinv(pFileBuf[idCurBuf].read_buf,len);
    char *readbuf = (char *)((unsigned int)pFileBuf[idCurBuf].read_buf | 0xa0000000);
    
    len1 = FS_FReadEx(readbuf,1,len ,fp);
    pFileBuf[idCurBuf].len = len1;
    F("len = %d %d\n",len,len1);
    cur_offset += len1;
    
    *ppData = pFileBuf[idCurBuf].read_buf;
    return len1;
}

//////////////////////////////////////////////////////////////////////
static int fill_buffer(stream_t *s, char** buffer, int max_len){
  //F("read %d\n",max_len);
  int r = mplayread(s->fd,buffer,max_len);
  return (r <= 0) ? -1 : r;
}

static int write_buffer(stream_t *s, char* buffer, int len) {
  return -1;
}

static int seek(stream_t *s,off_t newpos) {
  s->pos = newpos;
  if(fseek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
  return 1;
}

static int seek_forward(stream_t *s,off_t newpos) {
#if 0
  if(newpos<s->pos){
    mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
    return 0;
  }
  while(s->pos<newpos){
    int len=s->fill_buffer(s,(char**)&(s->pBuffer),s->sector_size);
    if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; break; } // EOF
    s->buf_pos=0;
    s->buf_len=len;
    s->pos+=len;
  }
#endif
  F("error!\n");
  return 0;
}
static void close_f(stream_t *s)
{
	fclose(s->fd);
}

static int control(stream_t *s, int cmd, void *arg) {
  switch(cmd) {
    case STREAM_CTRL_GET_SIZE: {
      off_t size;

      size = fseek(s->fd, 0, SEEK_END);
      fseek(s->fd, s->pos, SEEK_SET);
      if(size != (off_t)-1) {
        *((off_t*)arg) = size;
        return 1;
      }
     case STREAM_CTRL_RESET:
     	s->pos = 0;
     	fseek(s->fd, 0, SEEK_SET);
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
	 f = (int) fopen(filename,rmode);
   if(f==0) {
    mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }
	len = GetFileSize(f);
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
  stream->sector_size = READ_BUFF_SIZE;
  stream->fd = f;
  stream->fill_buffer = fill_buffer;
  stream->write_buffer = NULL;
  stream->control = control;
  stream->close = close_f; 
  m_struct_free(&stream_opts,opts);
  return STREAM_OK;
}


stream_info_t stream_info_singlefile = {
  "SingleFile",
  "file",
  "Albeu",
  "based on the code from ??? (probably Arpi)",
  open_f,
  { "file", "", NULL },
  &stream_opts,
  1 // Urls are an option string
};
