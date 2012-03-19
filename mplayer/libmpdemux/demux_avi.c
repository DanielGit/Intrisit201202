//  AVI file parser for DEMUXER v2.9  by A'rpi/ESP-team

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define F(...) 
#else
#include <mplaylib.h>
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"

#include "aviheader.h"
#include "libao2/audio_out.h"
//#undef printf
#ifndef __LINUX__
demuxer_t* (*init_avi_with_ogg_funcp)(demuxer_t* demuxer) = NULL;
int (*demux_ogg_open_funcp)(demuxer_t* demuxer) = NULL;
#else
extern demuxer_t* init_avi_with_ogg(demuxer_t* demuxer);
extern int demux_ogg_open(demuxer_t* demuxer);
#endif

extern demuxer_desc_t demuxer_desc_avi_ni;
extern demuxer_desc_t demuxer_desc_avi_nini;
AVIINDEXENTRY *GetIndex(demuxer_t *demuxer,int pos);
static int generate_index(demuxer_t *demux);
// PTS:  0=interleaved  1=BPS-based
int pts_from_bps=1;

// Select ds from ID
demux_stream_t* demux_avi_select_stream(demuxer_t *demux,unsigned int id){
	int stream_id=avi_stream_id(id);


	if(demux->video->id==-1)
		if(demux->v_streams[stream_id])
			demux->video->id=stream_id;

	if(demux->audio->id==-1)
		if(demux->a_streams[stream_id])
			demux->audio->id=stream_id;

	if(stream_id==demux->audio->id){
		if(!demux->audio->sh){
			sh_audio_t* sh;
			avi_priv_t *priv=demux->priv;
			sh=demux->audio->sh=demux->a_streams[stream_id];
			mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected AVI audio ID = %d\n",demux->audio->id);
			if(sh->wf){
				priv->audio_block_size=sh->wf->nBlockAlign;
				if(!priv->audio_block_size){
					// for PCM audio we can calculate the blocksize:
					if(sh->format==1)
						priv->audio_block_size=sh->wf->nChannels*(sh->wf->wBitsPerSample/8);
					else
						priv->audio_block_size=1; // hope the best...
				} else {
					// workaround old mencoder's bug:
					if(sh->audio.dwSampleSize==1 && sh->audio.dwScale==1 &&
					   (sh->wf->nBlockAlign==1152 || sh->wf->nBlockAlign==576)){
						mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_WorkAroundBlockAlignHeaderBug);
						priv->audio_block_size=1;
					}
				}
			} else {
				priv->audio_block_size=sh->audio.dwSampleSize;
			}
		}
		return demux->audio;
	}
	if(stream_id==demux->video->id){
		if(!demux->video->sh){
			demux->video->sh=demux->v_streams[stream_id];
			mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected AVI video ID = %d\n",demux->video->id);
		}
		return demux->video;
	}
	if(id!=mmioFOURCC('J','U','N','K')){
		// unknown
		mp_msg(MSGT_DEMUX,MSGL_DBG2,"Unknown chunk: %.4s (%X)\n",(char *) &id,id);
		//abort();
	}
	return NULL;
}

static int valid_fourcc(unsigned int id){
    static const char valid[] = "0123456789abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    unsigned char* fcc=(unsigned char*)(&id);
    return strchr(valid, fcc[0]) && strchr(valid, fcc[1]) &&
		strchr(valid, fcc[2]) && strchr(valid, fcc[3]);
}

static int choose_chunk_len(unsigned int len1,unsigned int len2){
    // len1 has a bit more priority than len2. len1!=len2
    // Note: this is a first-idea-logic, may be wrong. comments welcomed.

    // prefer small frames rather than 0
    if(!len1) return (len2>0x80000) ? len1 : len2;
    if(!len2) return (len1>0x100000) ? len2 : len1;

    // choose the smaller value:
    return (len1<len2)? len1 : len2;
}
static int skip_keyframe_count = 0;
static int cur_skip_keyframe = 0;
static float per_frametime = 0;
static int FindNextKeyFrameIdx(demuxer_t * demux)
{
	avi_priv_t *priv=demux->priv;
	int idxpos = priv->idx_pos;
	int idx_size = priv->idx_size;
	int id = demux->video->id;
	int m_cur_skip_keyframe = cur_skip_keyframe;
	AVIINDEXENTRY *idx=NULL;
	int count = 0;
	//F("++++\n");
	while(idxpos < idx_size)
	{		
		idx = GetIndex(demux,idxpos);
		idxpos++;
		if(avi_stream_id(idx->ckid) == id)
		{
			count++;
			if(idx->dwFlags & AVIIF_KEYFRAME)
			{
	  			m_cur_skip_keyframe++;
	  			if(m_cur_skip_keyframe > skip_keyframe_count)
	  			{	
	  				break;
	  			}
			}
	  	
		}
	}
	//F("-----\n");
	return count;
}
static int FindSkipKeyFrameTime(demuxer_t * demux,int time)
{
	avi_priv_t *priv=demux->priv;
	AVIINDEXENTRY *idx=NULL;
	int idxpos = priv->idx_pos;
	int idx_size = priv->idx_size;
	int id = demux->video->id;
	int frametime = 0;
	//F("+++++\n");
	while(idxpos < idx_size){
		idx = GetIndex(demux,idxpos);
		idxpos++;
		if(avi_stream_id(idx->ckid) == id)
		{
	  		frametime += (int)(per_frametime * 1000.0);
	  		if(idx->dwFlags & AVIIF_KEYFRAME)
	  		{
	  			skip_keyframe_count++;
	  			if((frametime > time) && (time < 3000))
	  				break;
	  		}	
		}
	}
	cur_skip_keyframe = 0;
	//F("-----\n");
	return skip_keyframe_count;
}

static unsigned int skip_to_keyframe = 0;


static int demux_avi_read_packet(demuxer_t *demux,demux_stream_t *ds,unsigned int id,unsigned int len,int idxpos,int flags){
	avi_priv_t *priv=demux->priv;
	int skip;
	float pts=0;
	mp_dbg(MSGT_DEMUX,MSGL_DBG3,"demux_avi.read_packet: %X\n",id);
//	kprintf("demux_avi.read_packet: %X\n",id);
	if(ds==demux->audio){
		if(priv->pts_corrected==0){
			if(priv->pts_has_video){
				// we have video pts now
				float delay=0;
				if(((sh_audio_t*)(ds->sh))->wf->nAvgBytesPerSec)
					delay=(float)priv->pts_corr_bytes/((sh_audio_t*)(ds->sh))->wf->nAvgBytesPerSec;
				mp_msg(MSGT_DEMUX,MSGL_V,"XXX initial  v_pts=%5.3f  a_pos=%d (%5.3f) \n",priv->avi_audio_pts,priv->pts_corr_bytes,delay);
				//priv->pts_correction=-priv->avi_audio_pts+delay;
				priv->pts_correction=delay-priv->avi_audio_pts;
				priv->avi_audio_pts+=priv->pts_correction;
				priv->pts_corrected=1;
			} else
				priv->pts_corr_bytes+=len;
		}
		if(pts_from_bps){
			pts = priv->audio_block_no *
				(float)((sh_audio_t*)demux->audio->sh)->audio.dwScale /
				(float)((sh_audio_t*)demux->audio->sh)->audio.dwRate;
		} else
			pts=priv->avi_audio_pts; //+priv->pts_correction;
		priv->avi_audio_pts=0;
		// update blockcount:
		priv->audio_block_no+=priv->audio_block_size ?
			((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
	} else 
		if(ds==demux->video){
     // video
			if(priv->skip_video_frames>0){
				// drop frame (seeking)
				--priv->skip_video_frames;
				ds=NULL;
			}
			if(ds)
                ++priv->video_pack_no;
			pts = priv->avi_video_pts = (float)(priv->video_pack_no) * per_frametime;
			
		//	printf("pts = %f %f %f\n",pts,per_frametime);
			if(ds)
			{	
				sh_video_t *sh_video = ds->sh;
				ds->pts = pts;
			}
			priv->avi_audio_pts=priv->avi_video_pts+priv->pts_correction;
			priv->pts_has_video=1;
		 
			if(flags)
			{
		 		skip_to_keyframe = 0;
				cur_skip_keyframe++;
				if(cur_skip_keyframe < skip_keyframe_count)
				{
					flags |= 0x1000;
				}	
			}		 
			if(skip_to_keyframe) ds = NULL; 	
		}
  
	skip=(len+1)&(~1); // total bytes in this chunk
  
	if(ds)
	{
//		kprintf("ds != 0 len = %d \n",len);
		mp_dbg(MSGT_DEMUX,MSGL_DBG2,"DEMUX_AVI: Read %d data bytes from packet %04X\n",len,id);
		ds_read_packet(ds,demux->stream,len,pts,idxpos,flags);
		skip-=len;
	}
	if(skip){
		mp_dbg(MSGT_DEMUX,MSGL_DBG2,"DEMUX_AVI: Skipping %d bytes from packet %04X\n",skip,id);
		stream_skip(demux->stream,skip);
	}
	return ds?1:0;
}
// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_avi_fill_buffer(demuxer_t *demux, demux_stream_t *dsds){
	avi_priv_t *priv=demux->priv;
	unsigned int id=0;
	unsigned int len;
	int ret=0;
	demux_stream_t *ds;
//F("1\n");
	do{
		int flags=1;
		AVIINDEXENTRY *idx=NULL;
		if(priv->idx_size>0 && priv->idx_pos<priv->idx_size){
			off_t pos;
    
			//idx= &((AVIINDEXENTRY *)priv->idx)[priv->idx_pos++];
			idx = GetIndex(demux,priv->idx_pos++);
			//printf("idx = %x\n",idx);
			if(idx->dwFlags&AVIIF_LIST){
				// LIST
				continue;
			}
			if(!demux_avi_select_stream(demux,idx->ckid)){
				mp_dbg(MSGT_DEMUX,MSGL_DBG3,"Skip chunk %.4s (0x%X)  \n",(char *)&idx->ckid,(unsigned int)idx->ckid);
				continue; // skip this chunk
			}

			pos = (off_t)priv->idx_offset+AVI_IDX_OFFSET(idx);
			if((pos<demux->movi_start || pos>=demux->movi_end) && (demux->movi_end>demux->movi_start) && (demux->stream->flags & STREAM_SEEK)){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkOffset out of range!   idx=0x%"PRIX64"  \n",(int64_t)pos);
				continue;
			}
			//F("pos = %x\n",pos);
			stream_seek(demux->stream,pos);
//			kprintf("demux_avi_fill_buffer: pos = %d\n",pos);
   
			demux->filepos=stream_tell(demux->stream);
//			kprintf("demux_avi_fill_buffer: file pos = %d",demux->filepos);
			
			//F("demux->filepos = %x\n",demux->filepos);
			id=stream_read_dword_le(demux->stream);
			if(stream_eof(demux->stream)) return 0; // EOF!
    
			if(id!=idx->ckid){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkID mismatch! raw=%.4s idx=%.4s  \n",(char *)&id,(char *)&idx->ckid);
				if(valid_fourcc(idx->ckid))
					id=idx->ckid;	// use index if valid
				else
					if(!valid_fourcc(id)) continue; // drop chunk if both id and idx bad
			}
			len=stream_read_dword_le(demux->stream);
//			kprintf("demux_avi_fill_buffer: len = %d\n",len);
			if((len!=idx->dwChunkLength)&&((len+1)!=idx->dwChunkLength)){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkSize mismatch! raw=%d idx=%d  \n",len,idx->dwChunkLength);
				if(len>0x200000 && idx->dwChunkLength>0x200000) continue; // both values bad :(
				len=choose_chunk_len(idx->dwChunkLength,len);
			}
			if(!(idx->dwFlags&AVIIF_KEYFRAME)) flags=0;
		} else {
			demux->filepos=stream_tell(demux->stream);
			if(demux->filepos>=demux->movi_end && demux->movi_end>demux->movi_start && (demux->stream->flags & STREAM_SEEK)){
				demux->stream->eof=1;
				return 0;
			}
			id=stream_read_dword_le(demux->stream);
			len=stream_read_dword_le(demux->stream);
			if(stream_eof(demux->stream)) return 0; // EOF!
    
			if(id==mmioFOURCC('L','I','S','T') || id==mmioFOURCC('R', 'I', 'F', 'F')){
				id=stream_read_dword_le(demux->stream); // list or RIFF type
				continue;
			}
		}
	
		ds=demux_avi_select_stream(demux,id);
		if(ds)
			if(ds->packs+1>=MAX_PACKS || ds->bytes+len>=MAX_PACK_BYTES){
				// this packet will cause a buffer overflow, switch to -ni mode!!!
				mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_SwitchToNi);
				if(priv->idx_size>0){
					// has index
					demux->type=DEMUXER_TYPE_AVI_NI;
					demux->desc=&demuxer_desc_avi_ni;
					--priv->idx_pos; // hack
				} else {
					// no index
					demux->type=DEMUXER_TYPE_AVI_NINI;
					demux->desc=&demuxer_desc_avi_nini;
					priv->idx_pos=demux->filepos; // hack
	    
				}
				priv->idx_pos_v=priv->idx_pos_a=priv->idx_pos;
				// quit now, we can't even (no enough buffer memory) read this packet :(
				return -1;
			}
  
		ret=demux_avi_read_packet(demux,ds,id,len,priv->idx_pos-1,flags);
 
  	
	} while(ret!=1);
	return 1;
}


// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
int demux_avi_fill_buffer_ni(demuxer_t *demux,demux_stream_t* ds){
	avi_priv_t *priv=demux->priv;
	unsigned int id=0;
	unsigned int len;
	int ret=0;
//F("1\n");
	do{
		int flags=1;
		AVIINDEXENTRY *idx=NULL;
		int idx_pos=0;
		demux->filepos=stream_tell(demux->stream);
  
		if(ds==demux->video) idx_pos=priv->idx_pos_v++; else
			if(ds==demux->audio) idx_pos=priv->idx_pos_a++; else
				idx_pos=priv->idx_pos++;
  
		if(priv->idx_size>0 && idx_pos<priv->idx_size){
			off_t pos;
			//idx=&((AVIINDEXENTRY *)priv->idx)[idx_pos];
			idx = GetIndex(demux,idx_pos);
			if(idx->dwFlags&AVIIF_LIST){
				// LIST
				continue;
			}
			if(ds && demux_avi_select_stream(demux,idx->ckid)!=ds){
				mp_dbg(MSGT_DEMUX,MSGL_DBG3,"Skip chunk %.4s (0x%X)  \n",(char *)&idx->ckid,(unsigned int)idx->ckid);
				continue; // skip this chunk
			}

			pos = priv->idx_offset+AVI_IDX_OFFSET(idx);
			if((pos<demux->movi_start || pos>=demux->movi_end) && (demux->movi_end>demux->movi_start)){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkOffset out of range!  current=0x%"PRIX64"  idx=0x%"PRIX64"  \n",(int64_t)demux->filepos,(int64_t)pos);
				continue;
			}
			stream_seek(demux->stream,pos);

			id=stream_read_dword_le(demux->stream);

			if(stream_eof(demux->stream)) return 0;

			if(id!=idx->ckid){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkID mismatch! raw=%.4s idx=%.4s  \n",(char *)&id,(char *)&idx->ckid);
				if(valid_fourcc(idx->ckid))
					id=idx->ckid;	// use index if valid
				else
					if(!valid_fourcc(id)) continue; // drop chunk if both id and idx bad
			}
			len=stream_read_dword_le(demux->stream);
			if((len!=idx->dwChunkLength)&&((len+1)!=idx->dwChunkLength)){
				mp_msg(MSGT_DEMUX,MSGL_V,"ChunkSize mismatch! raw=%d idx=%d  \n",len,idx->dwChunkLength);
				if(len>0x200000 && idx->dwChunkLength>0x200000) continue; // both values bad :(
				len=choose_chunk_len(idx->dwChunkLength,len);
			}
			if(!(idx->dwFlags&AVIIF_KEYFRAME)) flags=0;
		} else return 0;
		ret=demux_avi_read_packet(demux,demux_avi_select_stream(demux,id),id,len,idx_pos,flags);
	} while(ret!=1);
	return 1;
}

#define MIN_INDEX_SIZE (4 * 1024)
#define MAX_INDEX_SIZE (4 * 1024)

static int generate_index(demuxer_t *demux)
{
	avi_priv_t *priv=demux->priv;
	unsigned int id=0;
	unsigned int len;
	int ret=0;
	off_t fpos=stream_tell(demux->stream);
	off_t fpos1=fpos;
	AVIINDEXENTRY * idx;
	if(priv->idx) free(priv->idx);
	priv->idx = (AVIINDEXENTRY *)malloc(MIN_INDEX_SIZE);
	int idxcount = 0;
	int idxtotal = MIN_INDEX_SIZE / sizeof(AVIINDEXENTRY);
	do{
		//F("1\n");
		fpos1 = stream_tell(demux->stream);
		if(fpos1>=demux->movi_end && (demux->movi_end>demux->movi_start)){
			break;
		}
		//F("2\n");
		fpos1 = stream_tell(demux->stream);
		id=stream_read_dword_le(demux->stream);
		len=stream_read_dword_le(demux->stream);
		//F("3\n");
		if(stream_eof(demux->stream) || !len) break;
  
		if(id==mmioFOURCC('L','I','S','T')){
			id=stream_read_dword_le(demux->stream);      // list type
      
			continue;
		}
  
		if(id==mmioFOURCC('R','I','F','F')){
			mp_msg(MSGT_DEMUX,MSGL_V,"additional RIFF header...\n");
			id=stream_read_dword_le(demux->stream);      // "AVIX"
			continue;
		}
		if(idxcount >= idxtotal) 
		{
			if (idxcount >= MAX_INDEX_SIZE / sizeof(AVIINDEXENTRY))
			{
			  idxcount = 0;
			  break;
			}
	    else
		  {
			  idxtotal *= 2;
			  priv->idx = (AVIINDEXENTRY *)realloc(priv->idx,idxtotal * sizeof(AVIINDEXENTRY));
			}
		}
		if(idxcount < idxtotal)
		{
  		
			unsigned int flags = 0;
			if(avi_stream_id(id) == demux->video->id)
			{	
				idx = (AVIINDEXENTRY *)priv->idx;
				idx += idxcount;
				idxcount++;
				idx->ckid = id;
				idx->dwChunkLength = len;
				stream_read(demux->stream,&flags,4);
				//printf("flags = 0x%x\n",flags);
				idx->dwFlags = (flags == 0xb0010000)? AVIIF_KEYFRAME:0;
				len -= 4;
				idx->dwChunkOffset = fpos1 - fpos;
			}
			if(avi_stream_id(id) == demux->audio->id)
			{
				idx = (AVIINDEXENTRY *)priv->idx;
				idx += idxcount;
				idxcount++;
				idx->ckid = id;
				idx->dwChunkLength = len;
				idx->dwFlags =  AVIIF_KEYFRAME;
				idx->dwChunkOffset = fpos1 - fpos;
			}
  		
			//printf("idx->ckid = %d fpos1 = %d\n",idx->ckid,fpos1);
		}
		//F("1\n");
		int skip=(len+1)&(~1); // total bytes in this chunk
		stream_skip(demux->stream,skip);
		//F("1\n");
	} while(1);
	printf("idxcount = %x\n",idxcount);
	if(idxcount == 0) 
	{
		free(priv->idx);
		priv->idx = NULL;
	}
	stream_seek(demux->stream,fpos);
	priv->idx_offset = fpos;
	demux->stream->eof = 0;
	return idxcount;
}

// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
int demux_avi_fill_buffer_nini(demuxer_t *demux,demux_stream_t* ds){
	avi_priv_t *priv=demux->priv;
	unsigned int id=0;
	unsigned int len;
	int ret=0;
	off_t *fpos=NULL;
	//F("1\n");
	if(ds==demux->video) fpos=&priv->idx_pos_v; else
		if(ds==demux->audio) fpos=&priv->idx_pos_a; else
			return 0;

	stream_seek(demux->stream,fpos[0]);

	do{

		demux->filepos=stream_tell(demux->stream);
		if(demux->filepos>=demux->movi_end && (demux->movi_end>demux->movi_start)){
			ds->eof=1;
			return 0;
		}

		id=stream_read_dword_le(demux->stream);
		len=stream_read_dword_le(demux->stream);

		if(stream_eof(demux->stream)) return 0;
  
		if(id==mmioFOURCC('L','I','S','T')){
			id=stream_read_dword_le(demux->stream);      // list type
      
			continue;
		}
  
		if(id==mmioFOURCC('R','I','F','F')){
			mp_msg(MSGT_DEMUX,MSGL_V,"additional RIFF header...\n");
			id=stream_read_dword_le(demux->stream);      // "AVIX"
			continue;
		}
  
		if(ds==demux_avi_select_stream(demux,id)){
			// read it!
			ret=demux_avi_read_packet(demux,ds,id,len,priv->idx_pos-1,0);
		} else {
			// skip it!
			int skip=(len+1)&(~1); // total bytes in this chunk
			stream_skip(demux->stream,skip);
		}
  
	} while(ret!=1);
	fpos[0]=stream_tell(demux->stream);
	return 1;
}

// AVI demuxer parameters:
int index_mode=-1;  // -1=untouched  0=don't use index  1=use (geneate) index
//int index_mode=1;  // -1=untouched  0=don't use index  1=use (geneate) index
char *index_file_save = NULL, *index_file_load = NULL;
int force_ni=0;     // force non-interleaved AVI parsing

void read_avi_header(demuxer_t *demuxer,int index_mode);
extern int frame_dropping;
static demuxer_t* demux_open_avi(demuxer_t* demuxer){
    demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=NULL;
    sh_video_t *sh_video=NULL;
    avi_priv_t* priv=malloc(sizeof(avi_priv_t));

	// priv struct:
	priv->avi_audio_pts=priv->avi_video_pts=0.0f;
	priv->pts_correction=0.0f;
	priv->skip_video_frames=0;
	priv->pts_corr_bytes=0;
	priv->pts_has_video=priv->pts_corrected=0;
	priv->video_pack_no=0;
	priv->audio_block_no=0;
	priv->audio_block_size=0;
	priv->isodml = 0;
	priv->suidx_size = 0;
	priv->suidx = NULL;
	priv->idx = NULL;

	demuxer->priv=(void*)priv;

	//---- AVI header:
	read_avi_header(demuxer,(demuxer->stream->flags & STREAM_SEEK_BW)?index_mode:-2);
  
	if(demuxer->audio->id>=0 && !demuxer->a_streams[demuxer->audio->id]){
		mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_InvalidAudioStreamNosound,demuxer->audio->id);
		demuxer->audio->id=-2; // disabled
	}
	if(demuxer->video->id>=0 && !demuxer->v_streams[demuxer->video->id]){
		mp_msg(MSGT_DEMUX,MSGL_WARN,MSGTR_InvalidAudioStreamUsingDefault,demuxer->video->id);
		demuxer->video->id=-1; // autodetect
	}
  
	stream_reset(demuxer->stream);
	stream_seek(demuxer->stream,demuxer->movi_start);
  

	priv->idx_pos=0;
	priv->idx_pos_a=0;
	priv->idx_pos_v=0;
	AVIINDEXENTRY * idx;
	if(priv->idx_size>1){
		// decide index format:
#if 1
		idx = GetIndex(demuxer,0);
		if((AVI_IDX_OFFSET(&idx[0])<demuxer->movi_start ||
			AVI_IDX_OFFSET(&idx[1])<demuxer->movi_start )&& !priv->isodml)
			priv->idx_offset=demuxer->movi_start-4;
		else
			priv->idx_offset=0;
#else
		if(AVI_IDX_OFFSET(&idx[0])<demuxer->movi_start)
			priv->idx_offset=demuxer->movi_start-4;
		else
			priv->idx_offset=0;
#endif
		mp_msg(MSGT_DEMUX,MSGL_V,"AVI index offset: 0x%X (movi=0x%X idx0=0x%X idx1=0x%X)\n",
			   (int)priv->idx_offset,(int)demuxer->movi_start,
			   (int)idx[0].dwChunkOffset,
			   (int)idx[1].dwChunkOffset);
	}
  
	if(priv->idx_size>0){
		// check that file is non-interleaved:
		int i;
		off_t a_pos=-1;
		off_t v_pos=-1;
		for(i=0;i<priv->idx_size;i++){
			AVIINDEXENTRY* idx= GetIndex(demuxer,i);
			demux_stream_t* ds=demux_avi_select_stream(demuxer,idx->ckid);
			off_t pos = priv->idx_offset + AVI_IDX_OFFSET(idx);
			if(a_pos==-1 && ds==demuxer->audio){
				a_pos=pos;
				if(v_pos!=-1) break;
			}
			if(v_pos==-1 && ds==demuxer->video){
				v_pos=pos;
				if(a_pos!=-1) break;
			}
		}
      
		if(v_pos==-1){
			mp_msg(MSGT_DEMUX,MSGL_ERR,"AVI_NI: " MSGTR_MissingVideoStream);
			return NULL;
		}
		if(a_pos==-1){
			d_audio->sh=sh_audio=NULL;
		} else {
			if(force_ni || abs(a_pos-v_pos)>0x100000){  // distance > 1MB
				mp_msg(MSGT_DEMUX,MSGL_INFO,MSGTR_NI_Message,force_ni?MSGTR_NI_Forced:MSGTR_NI_Detected);
				demuxer->type=DEMUXER_TYPE_AVI_NI; // HACK!!!!
				demuxer->desc=&demuxer_desc_avi_ni; // HACK!!!!
				pts_from_bps=1; // force BPS sync!
			}
		}
	} else {
		// no index
		if(force_ni){
			mp_msg(MSGT_DEMUX,MSGL_INFO,MSGTR_UsingNINI);
			demuxer->type=DEMUXER_TYPE_AVI_NINI; // HACK!!!!
			demuxer->desc=&demuxer_desc_avi_nini; // HACK!!!!
			priv->idx_pos_a=
				priv->idx_pos_v=demuxer->movi_start;
			pts_from_bps=1; // force BPS sync!
		}
		demuxer->seekable=0;
	}
	if(!ds_fill_buffer(d_video)){
		mp_msg(MSGT_DEMUX,MSGL_ERR,"AVI: " MSGTR_MissingVideoStreamBug);
		return NULL;
	}
	sh_video=d_video->sh;sh_video->ds=d_video;
	if(d_audio->id!=-2){
		mp_msg(MSGT_DEMUX,MSGL_V,"AVI: Searching for audio stream (id:%d)\n",d_audio->id);
		if(!priv->audio_streams || !ds_fill_buffer(d_audio)){
			mp_msg(MSGT_DEMUX,MSGL_INFO,"AVI: " MSGTR_MissingAudioStream);
			d_audio->sh=sh_audio=NULL;
		} else {
			sh_audio=d_audio->sh;sh_audio->ds=d_audio;
		}
	}
	if(priv->idx_size == 0)
	{
		priv->idx_size = generate_index(demuxer);
		priv->idx_pos = 0;
		//priv->idx_offset = 0;
		if(priv->idx_size > 10) demuxer->seekable=1;
	}
	// calculating audio/video bitrate:
	if(priv->idx_size>0){
		// we have index, let's count 'em!
		int64_t vsize=0;
		int64_t asize=0;
		size_t vsamples=0;
		size_t asamples=0;
#if 0
		int average_iframe_gas = 0;
		int iframe_count = 0;
		int frame_count = 0;
#define MAX_DELTA_COUNT 1024
		int *iframe_gas = (int *)malloc(MAX_DELTA_COUNT * 4);
#endif
		int i;
    
		for(i=0;i<priv->idx_size;i++){ 
			AVIINDEXENTRY *idx = GetIndex(demuxer,i);
			int id=avi_stream_id(idx->ckid);
      
			int len=idx->dwChunkLength;
			if(sh_video->ds->id == id) {
				vsize+=len;
				++vsamples;
#if 0
				if(idx->dwFlags == AVIIF_KEYFRAME)
				{
					if(iframe_count < MAX_DELTA_COUNT)
					{	
						iframe_gas[iframe_count] = i;
					}
					iframe_count++;
				}
				frame_count++;
#endif
			}
			else if(sh_audio && sh_audio->ds->id == id) {
				asize+=len;
				asamples+=(len+priv->audio_block_size-1)/priv->audio_block_size;
			}
		}
#if 0
		average_iframe_gas = frame_count / iframe_count;
		int64_t iframe_delt = 0;
		iframe_count = (iframe_count>MAX_DELTA_COUNT?MAX_DELTA_COUNT:iframe_count);
		int d = 0,d1 = 0;
		for(i = 0; i < iframe_count ;i++)
		{
			d1 =  iframe_gas[i] - d;
			d = iframe_gas[i];
			d1 = d1 - average_iframe_gas;
			iframe_delt += d1 * d1; 
   
		}
		iframe_delt = iframe_delt / iframe_count;
		free(iframe_gas);
		if((average_iframe_gas > 30) && (iframe_delt < 15000))
			frame_dropping = 1;
		else
			frame_dropping = 1;
#endif
		mp_msg(MSGT_DEMUX,MSGL_V,"AVI video size=%"PRId64" (%u) audio size=%"PRId64" (%u)\n",vsize,vsamples,asize,asamples);
		priv->numberofframes=vsamples;
		sh_video->i_bps=((float)vsize/(float)vsamples)*(float)sh_video->video.dwRate/(float)sh_video->video.dwScale;
		if(sh_audio) sh_audio->i_bps=((float)asize/(float)asamples)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
	} else {
		// guessing, results may be inaccurate:
		int64_t vsize;
		int64_t asize=0;

		if((priv->numberofframes=sh_video->video.dwLength)<=1)
			// bad video header, try to get number of frames from audio
			if(sh_audio && sh_audio->wf->nAvgBytesPerSec) priv->numberofframes=sh_video->fps*sh_audio->audio.dwLength/sh_audio->audio.dwRate*sh_audio->audio.dwScale;
		if(priv->numberofframes<=1){
			mp_msg(MSGT_SEEK,MSGL_WARN,MSGTR_CouldntDetFNo);
			priv->numberofframes=0;
		}          

		if(sh_audio){
			if(sh_audio->wf->nAvgBytesPerSec && sh_audio->audio.dwSampleSize!=1){
				asize=(float)sh_audio->wf->nAvgBytesPerSec*sh_audio->audio.dwLength*sh_audio->audio.dwScale/sh_audio->audio.dwRate;
			} else {
				asize=sh_audio->audio.dwLength;
				sh_audio->i_bps=(float)asize/(sh_video->frametime*priv->numberofframes);
			}
		}
		vsize=demuxer->movi_end-demuxer->movi_start-asize-8*priv->numberofframes;
		mp_msg(MSGT_DEMUX,MSGL_V,"AVI video size=%"PRId64" (%u)  audio size=%"PRId64"\n",vsize,priv->numberofframes,asize);
		sh_video->i_bps=(float)vsize/(sh_video->frametime*priv->numberofframes);
	}
	
	per_frametime = (float)sh_video->video.dwScale / (float)sh_video->video.dwRate;
	//F("priv->idx_size = %d\n",priv->idx_size);
	if(priv->idx_size > 1)
	{	
  	
		frame_dropping = 0;	
 	}
	if(demuxer->audio->id != -2)
	{
		unsigned int audio_buf_size = (128 * 1024);
		unsigned int audio_buf_count = 8;
		extern ao_data_t ao_data;
		
		int file_bps = sh_video->i_bps * 8;
		
		//printf("file_bps = %d\n",file_bps);
		sh_audio_t *sh_audio=demuxer->audio->sh;
		int audio_byte;
		if (sh_audio == NULL)
			audio_byte = 2 * 48000 * 2 / 8;
		else
			audio_byte = sh_audio->channels * sh_audio->samplesize * sh_audio->samplerate / 8;
		
		if(file_bps > 1000 * 1000 * 4)
		{
			audio_buf_size = (128 * 1024);
			audio_buf_count = 32;
		}else if(file_bps > 1000 * 1000)
		{
			audio_buf_count = 8;
			audio_buf_size = (audio_byte / audio_buf_count + 31) / 32 * 32 * 2; 
			audio_buf_size = audio_buf_size * audio_buf_count;
		}else
		{
			audio_buf_count = 4;
			audio_buf_size = (audio_byte / audio_buf_count + 31) / 32 * 32 * 2; 
			audio_buf_size = audio_buf_size * audio_buf_count;
			audio_buf_size = audio_buf_size;
		}
		ao_data.buffersize = audio_buf_size;
		ao_data.outburst = audio_buf_size / audio_buf_count;
	}	
  
//  printf("sh_video->fps = %f\n",sh_video->fps);
//	force_fps = sh_video->fps;
	return demuxer;
  
}


void demux_seek_avi(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags){
    avi_priv_t *priv=demuxer->priv;
    demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=d_audio->sh;
    sh_video_t *sh_video=d_video->sh;
    float skip_audio_secs=0;
    //F("1\n");`
	//FIXME: OFF_T - Didn't check AVI case yet (avi files can't be >2G anyway?)
	//================= seek in AVI ==========================
    int rel_seek_frames=rel_seek_secs*sh_video->fps;
    int video_chunk_pos=d_video->pos;
    int i;

	if(flags&1){
		// seek absolute
		video_chunk_pos = 0;
		//rel_seek_frames -= priv->video_pack_no; 
	}
      
	if(flags&2){
		rel_seek_frames=rel_seek_secs*priv->numberofframes;
	}
    F("rel_seek_frames = %d\n",rel_seek_frames);
	priv->skip_video_frames=0;
	priv->avi_audio_pts=0;

// ------------ STEP 1: find nearest video keyframe chunk ------------
	// find nearest video keyframe chunk pos:
	if(rel_seek_frames>0){
        // seek forward
        while(video_chunk_pos<priv->idx_size-1){
			AVIINDEXENTRY *idx = GetIndex(demuxer,video_chunk_pos);
			int id=idx->ckid;
			if(avi_stream_id(id)==d_video->id){  // video frame
				if((--rel_seek_frames)<0 && idx->dwFlags&AVIIF_KEYFRAME) break;
			}
			++video_chunk_pos;
        }
	} else {
        // seek backward
        while(video_chunk_pos>0){
			AVIINDEXENTRY *idx = GetIndex(demuxer,video_chunk_pos);
          
			int id=idx->ckid;
			if(avi_stream_id(id)==d_video->id){  // video frame
				if((++rel_seek_frames)>0 && idx->dwFlags&AVIIF_KEYFRAME) break;
			}
			--video_chunk_pos;
        }
	}
	priv->idx_pos_a=priv->idx_pos_v=priv->idx_pos=video_chunk_pos;

	// re-calc video pts:
	d_video->pack_no=0;
	for(i=0;i<video_chunk_pos;i++){
		AVIINDEXENTRY *idx = GetIndex(demuxer,i);
		int id=idx->ckid;
		if(avi_stream_id(id)==d_video->id) ++d_video->pack_no;
	}
     
	priv->video_pack_no=
		sh_video->num_frames=sh_video->num_frames_decoded=d_video->pack_no;
	priv->avi_video_pts=d_video->pack_no*(float)sh_video->video.dwScale/(float)sh_video->video.dwRate;
	d_video->pos=video_chunk_pos;
      
	F("V_SEEK:  pack=%d  pts=%d  chunk=%d  \n",d_video->pack_no,(int)(priv->avi_video_pts * 100 ),video_chunk_pos);

// ------------ STEP 2: seek audio, find the right chunk & pos ------------

	d_audio->pack_no=0;
	priv->audio_block_no=0;
	d_audio->dpos=0;

	if(sh_audio)
	{
        int i;
        int len=0;
		int skip_audio_bytes=0;
		int curr_audio_pos=-1;
		int audio_chunk_pos=-1;
		int chunk_max=(demuxer->type==DEMUXER_TYPE_AVI)?video_chunk_pos:priv->idx_size;
	
		F("V_SEEK:  sh_audio->audio.dwSampleSize= %d ",sh_audio->audio.dwSampleSize);
		if(sh_audio->audio.dwSampleSize){
			// constant rate audio stream
			/* immediate seeking to audio position, including when streams are delayed */
			curr_audio_pos=(priv->avi_video_pts + audio_delay)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
			curr_audio_pos*=sh_audio->audio.dwSampleSize;

			// find audio chunk pos:
			F("V_SEEK:  chunk_max = %d",chunk_max);
			for(i=0;i<chunk_max;i++){
				AVIINDEXENTRY *idx = GetIndex(demuxer,i);
				int id=idx->ckid;
				if(avi_stream_id(id)==d_audio->id){
					len=idx->dwChunkLength;
					if(d_audio->dpos<=curr_audio_pos && curr_audio_pos<(d_audio->dpos+len)){
						break;
					}
					++d_audio->pack_no;
					priv->audio_block_no+=priv->audio_block_size ?
						((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
					d_audio->dpos+=len;
				}
			}
			audio_chunk_pos=i;
			skip_audio_bytes=curr_audio_pos-d_audio->dpos;

			F("SEEK: i=%d (max:%d) dpos=%d (wanted:%d)",i,chunk_max,(int)d_audio->dpos,curr_audio_pos);
	      
		} else {
			// VBR audio
			/* immediate seeking to audio position, including when streams are delayed */
			int chunks=(priv->avi_video_pts + audio_delay)*(float)sh_audio->audio.dwRate/(float)sh_audio->audio.dwScale;
			audio_chunk_pos=0;
	    
			// find audio chunk pos:
			F("V_SEEK:  vbr  priv->idx_size = %d, chunks = %d",priv->idx_size,chunks);
			for(i=0;i<priv->idx_size && chunks>0;i++)
			{
				AVIINDEXENTRY *idx = GetIndex(demuxer,i);
            
				int id=idx->ckid;
				if(avi_stream_id(id)==d_audio->id){
					len=idx->dwChunkLength;
					if(i>chunk_max){
						skip_audio_bytes+=len;
					} else {
						++d_audio->pack_no;
						priv->audio_block_no+=priv->audio_block_size ?
							((len+priv->audio_block_size-1)/priv->audio_block_size) : 1;
						d_audio->dpos+=len;
						audio_chunk_pos=i;
					}
					if(priv->audio_block_size)
						chunks-=(len+priv->audio_block_size-1)/priv->audio_block_size;
				}
			}
		}
	
		// Now we have:
		//      audio_chunk_pos = chunk no in index table (it's <=chunk_max)
		//      skip_audio_bytes = bytes to be skipped after chunk seek
		//      d-audio->pack_no = chunk_no in stream at audio_chunk_pos
		//      d_audio->dpos = bytepos in stream at audio_chunk_pos
		// let's seek!
	
		// update stream position:
		d_audio->pos=audio_chunk_pos;
	
		F("V_SEEK:	demuxer->type = %d",demuxer->type);
		if(demuxer->type==DEMUXER_TYPE_AVI)
		{
			// interleaved stream:
			if(audio_chunk_pos<video_chunk_pos){
				// calc priv->skip_video_frames & adjust video pts counter:
				for(i=audio_chunk_pos;i<video_chunk_pos;i++){
					AVIINDEXENTRY *idx = GetIndex(demuxer,i);
					int id=idx->ckid;
					if(avi_stream_id(id)==d_video->id) ++priv->skip_video_frames;
				}
				// requires for correct audio pts calculation (demuxer):
				priv->avi_video_pts-=priv->skip_video_frames*(float)sh_video->video.dwScale/(float)sh_video->video.dwRate;
				priv->avi_audio_pts=priv->avi_video_pts;
				// set index position:
				priv->idx_pos_a=priv->idx_pos_v=priv->idx_pos=audio_chunk_pos;
			}
		} else {
			// non-interleaved stream:
			priv->idx_pos_a=audio_chunk_pos;
			priv->idx_pos_v=video_chunk_pos;
			priv->idx_pos=(audio_chunk_pos<video_chunk_pos)?audio_chunk_pos:video_chunk_pos;
		}

		F("SEEK: idx=%d  (a:%d v:%d)  v.skip=%d  a.skip=%d/%d",
			   (int)priv->idx_pos,audio_chunk_pos,video_chunk_pos,
			   (int)priv->skip_video_frames,skip_audio_bytes,(int)(skip_audio_secs*1000));

		if(skip_audio_bytes)
		{
			F("V_SEEK:	skip_audio_bytes = %d",skip_audio_bytes);
            demux_read_data(d_audio,NULL,skip_audio_bytes);
		}

	}
	d_video->pts=priv->avi_video_pts; // OSD
	skip_keyframe_count = 0;
	cur_skip_keyframe = 0;
	skip_to_keyframe = 0;

}


void demux_close_avi(demuxer_t *demuxer) {
	avi_priv_t* priv=demuxer->priv;

	if(!priv)
		return;

	if(priv->idx_size > 0)
		if(priv->idx)
			free(priv->idx);
	free(priv);
}


static int demux_avi_control(demuxer_t *demuxer,int cmd, void *arg){
    avi_priv_t *priv=demuxer->priv;
    demux_stream_t *d_video=demuxer->video;
    sh_video_t *sh_video=d_video->sh;

    switch(cmd) {
//	case DEMUXER_CTRL_CORRECT_PTS:
//		return DEMUXER_CTRL_OK;
   
	case DEMUXER_CTRL_SKIP_TOKEYFRAME:
		skip_to_keyframe = 1;
		return 1;
	case DEMUXER_CTRL_IS_KEYFRAME:
		if(*(int *)arg & 0x1000)
			return 0;
		return (*(int *)arg & 1);
	case DEMUXER_CTRL_SET_SKIP:
		*(int *)arg |= 0x1000;
		return	DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_SKIPFRAME_TIME:
	{
		FindSkipKeyFrameTime(demuxer,(int)(*(float *)arg * 1000.0));
		return DEMUXER_CTRL_OK;
	}
	case DEMUXER_CTRL_GET_KEYFRAME_TIME:
	{
		int count = FindNextKeyFrameIdx(demuxer);
		*(float *)arg = ((float)(count) * per_frametime);				
		return DEMUXER_CTRL_OK;
	}
	case DEMUXER_CTRL_GET_TIME_LENGTH:
		if (!priv->numberofframes || !sh_video) return DEMUXER_CTRL_DONTKNOW;
	    *((double *)arg)=(double)priv->numberofframes/sh_video->fps;
	    if (sh_video->video.dwLength<=1) return DEMUXER_CTRL_GUESS;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_GET_PERCENT_POS:
		if (!priv->numberofframes || !sh_video) {
			return DEMUXER_CTRL_DONTKNOW;
	    }
	    *((int *)arg)=(int)(priv->video_pack_no*100/priv->numberofframes);
	    if (sh_video->video.dwLength<=1) return DEMUXER_CTRL_GUESS;
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_SWITCH_AUDIO:
	case DEMUXER_CTRL_SWITCH_VIDEO: {
	    int audio = (cmd == DEMUXER_CTRL_SWITCH_AUDIO);
	    demux_stream_t *ds = audio ? demuxer->audio : demuxer->video;
	    void **streams = audio ? demuxer->a_streams : demuxer->v_streams;
	    int maxid = FFMIN(100, audio ? MAX_A_STREAMS : MAX_V_STREAMS);
	    int chunkid;
	    if (ds->id < -1)
			return DEMUXER_CTRL_NOTIMPL;

	    if (*(int *)arg >= 0)
			ds->id = *(int *)arg;
	    else {
			int i;
			for (i = 0; i < maxid; i++) {
				if (++ds->id >= maxid) ds->id = 0;
				if (streams[ds->id]) break;
			}
	    }

	    chunkid = (ds->id / 10 + '0') | (ds->id % 10 + '0') << 8;
	    ds->sh = NULL;
	    if (!streams[ds->id]) // stream not available
			ds->id = -1;
	    else
			demux_avi_select_stream(demuxer, chunkid);
	    *(int *)arg = ds->id;
	    return DEMUXER_CTRL_OK;
	}

	default:
	    return DEMUXER_CTRL_NOTIMPL;
    }
}


static int avi_check_file(demuxer_t *demuxer)
{
	//F("1\n");
	int id=stream_read_dword_le(demuxer->stream); // "RIFF"
  
    
	//F("2\n");
	if((id==mmioFOURCC('R','I','F','F')) || (id==mmioFOURCC('O','N','2',' '))) {
		stream_read_dword_le(demuxer->stream); //filesize
		id=stream_read_dword_le(demuxer->stream); // "AVI "
		if(id==formtypeAVI)
			return DEMUXER_TYPE_AVI;
		// "Samsung Digimax i6 PMP" crap according to bug 742
		if(id==mmioFOURCC('A','V','I',0x19))
			return DEMUXER_TYPE_AVI;
		if(id==mmioFOURCC('O','N','2','f')){
			mp_msg(MSGT_DEMUXER,MSGL_INFO,MSGTR_ON2AviFormat);
			return DEMUXER_TYPE_AVI;
		}
	}
 
	return 0;
}


static demuxer_t* demux_open_hack_avi(demuxer_t *demuxer)
{
	sh_audio_t* sh_a;

	demuxer = (demuxer_t*) demux_open_avi(demuxer);
	if(!demuxer) return NULL; // failed to open
	sh_a = (sh_audio_t*)demuxer->audio->sh;
	if(demuxer->audio->id != -2 && sh_a) {
#ifdef HAVE_OGGVORBIS
		// support for Ogg-in-AVI:
		if(sh_a->format == 0xFFFE)
#ifndef __LINUX__
			demuxer = init_avi_with_ogg_funcp(demuxer);
#else
		demuxer = init_avi_with_ogg(demuxer);
#endif
		else if(sh_a->format == 0x674F) {
			stream_t* s;
			demuxer_t  *od;
			s = new_ds_stream(demuxer->audio);
			od = new_demuxer(s,DEMUXER_TYPE_OGG,-1,-2,-2,NULL);
#ifndef __LINUX__
			if(!demux_ogg_open_funcp(od)) {
#else
				if(!demux_ogg_open(od)) {
#endif
					mp_msg( MSGT_DEMUXER,MSGL_ERR,MSGTR_ErrorOpeningOGGDemuxer);
					free_stream(s);
					demuxer->audio->id = -2;
				} else
					demuxer = new_demuxers_demuxer(demuxer,od,demuxer);
			}
#endif
		}

		return demuxer;
	}


	demuxer_desc_t demuxer_desc_avi = {
		"AVI demuxer",
		"avi",
		"AVI",
		"Arpi?",
		"AVI files, including non interleaved files",
		DEMUXER_TYPE_AVI,
		1, // safe autodetect
		avi_check_file,
		demux_avi_fill_buffer,
		demux_open_hack_avi,
		demux_close_avi,
		demux_seek_avi,
		demux_avi_control
	};

	demuxer_desc_t demuxer_desc_avi_ni = {
		"AVI demuxer, non-interleaved",
		"avini",
		"AVI",
		"Arpi?",
		"AVI files, including non interleaved files",
		DEMUXER_TYPE_AVI,
		1, // safe autodetect
		avi_check_file,
		demux_avi_fill_buffer_ni,
		demux_open_hack_avi,
		demux_close_avi,
		demux_seek_avi,
		demux_avi_control
	};

	demuxer_desc_t demuxer_desc_avi_nini = {
		"AVI demuxer, non-interleaved and no index",
		"avinini",
		"AVI",
		"Arpi?",
		"AVI files, including non interleaved files",
		DEMUXER_TYPE_AVI,
		1, // safe autodetect
		avi_check_file,
		demux_avi_fill_buffer_nini,
		demux_open_hack_avi,
		demux_close_avi,
		demux_seek_avi,
		demux_avi_control
	};
