//  ASF file parser for DEMUXER v0.3  by A'rpi/ESP-team

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream/stream.h"
#include "asf.h"
#include "demuxer.h"

#include "libvo/fastmemcpy.h"

/*
 * Load 16/32-bit values in little endian byte order
 * from an unaligned address
 */
#ifdef ARCH_X86
#define	LOAD_LE32(p)	(*(unsigned int*)(p))
#define	LOAD_LE16(p)	(*(unsigned short*)(p))
#define	LOAD_BE32(p)	(((unsigned char*)(p))[3]     | \
 			 ((unsigned char*)(p))[2]<< 8 | \
 			 ((unsigned char*)(p))[1]<<16 | \
 			 ((unsigned char*)(p))[0]<<24 )
#else
#define	LOAD_LE32(p)	(((unsigned char*)(p))[0]     | \
 			 ((unsigned char*)(p))[1]<< 8 | \
 			 ((unsigned char*)(p))[2]<<16 | \
 			 ((unsigned char*)(p))[3]<<24 )
#define	LOAD_LE16(p)	(((unsigned char*)(p))[0]     | \
			 ((unsigned char*)(p))[1]<<8)
#define	LOAD_BE32(p)	(*(unsigned int*)(p))
#endif

// defined at asfheader.c:
extern int frame_dropping;
extern int asf_check_header(demuxer_t *demuxer);
extern int read_asf_header(demuxer_t *demuxer,struct asf_priv* asf);

// based on asf file-format doc by Eugene [http://divx.euro.ru]

static void asf_descrambling(unsigned char **src,unsigned len, struct asf_priv* asf){
  unsigned char *dst=malloc(len);
  unsigned char *s2=*src;
  unsigned i=0,x,y;
  while(len>=asf->scrambling_h*asf->scrambling_w*asf->scrambling_b+i){
//    mp_msg(MSGT_DEMUX,MSGL_DBG4,"descrambling! (w=%d  b=%d)\n",w,asf_scrambling_b);
	//i+=asf_scrambling_h*asf_scrambling_w;
	for(x=0;x<asf->scrambling_w;x++)
	  for(y=0;y<asf->scrambling_h;y++){
	    fast_memcpy(dst+i,s2+(y*asf->scrambling_w+x)*asf->scrambling_b,asf->scrambling_b);
		i+=asf->scrambling_b;
	  }
	s2+=asf->scrambling_h*asf->scrambling_w*asf->scrambling_b;
  }
  //if(i<len) fast_memcpy(dst+i,src+i,len-i);
  free(*src);
  *src = dst;
}

/*****************************************************************
 * \brief initializes asf private data
 *
 */
static void init_priv (struct asf_priv* asf){
  asf->last_vid_seq=-1;
  asf->vid_ext_timing_index=-1;
  asf->aud_ext_timing_index=-1;
  asf->vid_ext_frame_index=-1;
}

#ifdef USE_LIBAVCODEC_SO
#include <ffmpeg/avcodec.h>
#elif defined(USE_LIBAVCODEC)
#include "libavcodec/avcodec.h"
#else
#define FF_INPUT_BUFFER_PADDING_SIZE 8
#endif


static void demux_asf_append_to_packet(demux_packet_t* dp,unsigned char *data,int len,int offs)
{
  if(dp->len!=offs && offs!=-1) mp_msg(MSGT_DEMUX,MSGL_V,"warning! fragment.len=%d BUT next fragment offset=%d  \n",dp->len,offs);
  dp->buffer=realloc(dp->buffer,dp->len+len+FF_INPUT_BUFFER_PADDING_SIZE);
  fast_memcpy(dp->buffer+dp->len,data,len);
  memset(dp->buffer+dp->len+len, 0, FF_INPUT_BUFFER_PADDING_SIZE);
  mp_dbg(MSGT_DEMUX,MSGL_DBG4,"data appended! %d+%d\n",dp->len,len);
  dp->len+=len;
}

static int demux_asf_read_packet(demuxer_t *demux,unsigned char *data,int len,int id,int seq, unsigned int time,unsigned short dur,int offs,int keyframe){
  struct asf_priv* asf = demux->priv;
  demux_stream_t *ds=NULL;
  int close_seg=0;
  
  mp_dbg(MSGT_DEMUX,MSGL_DBG4,"demux_asf.read_packet: id=%d seq=%d len=%d\n",id,seq,len);
  
  if(demux->video->id==-1)
    if(demux->v_streams[id])
        demux->video->id=id;

  if(demux->audio->id==-1)
    if(demux->a_streams[id])
        demux->audio->id=id;

  if(id==demux->audio->id){
      // audio
      ds=demux->audio;
      if(!ds->sh){
        ds->sh=demux->a_streams[id];
        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected ASF audio ID = %d\n",ds->id);
      }
  } else 
  if(id==demux->video->id){
      // video
      ds=demux->video;
      if(!ds->sh){
        ds->sh=demux->v_streams[id];
        mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected ASF video ID = %d\n",ds->id);
      }
  }
  
  if(ds){
    if(ds->asf_packet){
      demux_packet_t* dp=ds->asf_packet;

      if (ds==demux->video && asf->asf_is_dvr_ms) {
        if (asf->new_vid_frame_seg) {
          dp->pos=demux->filepos;
          close_seg = 1;
        } else seq = ds->asf_seq;
      } else close_seg = ds->asf_seq!=seq;

      if(close_seg){
        // closed segment, finalize packet:
		if(ds==demux->audio)
		  if(asf->scrambling_h>1 && asf->scrambling_w>1 && asf->scrambling_b>0)
		    asf_descrambling(&ds->asf_packet->buffer,ds->asf_packet->len,asf);
        ds_add_packet(ds,ds->asf_packet);
        ds->asf_packet=NULL;
      } else {
        // append data to it!
        demux_asf_append_to_packet(dp,data,len,offs);
        // we are ready now.
        return 1;
      }
    }
    // create new packet:
    { demux_packet_t* dp;
      if(offs>0){
        mp_msg(MSGT_DEMUX,MSGL_V,"warning!  broken fragment, %d bytes missing  \n",offs);
        return 0;
      }
      dp=new_demux_packet(len);
      fast_memcpy(dp->buffer,data,len);
      if (asf->asf_is_dvr_ms)
        dp->pts=time*0.0000001;
      else
        dp->pts=time*0.001;
      dp->flags=keyframe;
//      if(ds==demux->video) printf("ASF time: %8d  dur: %5d  \n",time,dur);
      dp->pos=demux->filepos;
      ds->asf_packet=dp;
      ds->asf_seq=seq;
      // we are ready now.
      return 1;
    }
  }

  return 0;
}

/*****************************************************************
 * \brief read the replicated data associated with each segment
 * \parameter pp reference to replicated data
 * \parameter id stream number
 * \parameter seq media object number
 * \parameter keyframe key frame indicator - set to zero if keyframe, non-zero otherwise
 * \parameter seg_time set to payload time when valid, if audio or new video frame payload, zero otherwise
 *
 */
static void get_payload_extension_data(demuxer_t *demux, unsigned char** pp, unsigned char id, unsigned int seq, int *keyframe, uint64_t *seg_time){
    struct asf_priv* asf = demux->priv;
    uint64_t payload_time; //100ns units
    int i, ext_max, ext_timing_index;
    uint8_t *pi = *pp+4;

    if(demux->video->id==-1)
        if(demux->v_streams[id])
            demux->video->id=id;

    if(demux->audio->id==-1)
        if(demux->a_streams[id])
            demux->audio->id=id;

    if (id!=demux->video->id && id!=demux->audio->id) return;

    if (id==demux->video->id) {
      ext_max = asf->vid_repdata_count;
      ext_timing_index = asf->vid_ext_timing_index;
    } else {
      ext_max = asf->aud_repdata_count;
      ext_timing_index = asf->aud_ext_timing_index;
    }

    *seg_time=0.0;
    asf->new_vid_frame_seg = 0;

    for (i=0; i<ext_max; i++) {    
        uint16_t payextsize;
        uint8_t segment_marker;

        if (id==demux->video->id)
            payextsize = asf->vid_repdata_sizes[i];
        else
            payextsize = asf->aud_repdata_sizes[i];

        if (payextsize == 65535) {
            payextsize = LOAD_LE16(pi);
            pi+=2;
        }
       
        // if this is the timing info extension then read the payload time
        if (i == ext_timing_index)
            payload_time =  (uint64_t) LOAD_LE32(pi+8) | (uint64_t)LOAD_LE32(pi+8 + 4) << 32;
        
        // if this is the video frame info extension then 
        // set the keyframe indicator, the 'new frame segment' indicator
        // and (initially) the 'frame time'
        if (i == asf->vid_ext_frame_index && id==demux->video->id) {
            segment_marker = pi[0];
            // Known video stream segment_marker values that
            // contain useful information:
            //
            // NTSC/ATSC (29.97fps):        0X4A 01001010
            //                              0X4B 01001011
            //                              0X49 01001001
            //
            // PAL/ATSC (25fps):            0X3A 00111010
            //                              0X3B 00111011
            //                              0X39 00111001
            //
            // ATSC progressive (29.97fps): 0X7A 01111010
            //                              0X7B 01111011
            //                              0X79 01111001
            //   11111111
            //       ^    this is new video frame marker
            //
            //   ^^^^     these bits indicate the framerate
            //            0X4 is 29.97i, 0X3 is 25i, 0X7 is 29.97p, ???=25p 
            //
            //        ^^^ these bits indicate the frame type:
            //              001 means I-frame
            //              010 and 011 probably mean P and B

            asf->new_vid_frame_seg = (0X08 & segment_marker) && seq != asf->last_vid_seq;

            if (asf->new_vid_frame_seg) asf->last_vid_seq = seq;

            if (asf->avg_vid_frame_time == 0) {
                // set the average frame time initially (in 100ns units). 
                // This is based on what works for known samples.
                // It can be extended if more samples of different types can be obtained.
                if (((segment_marker & 0XF0) >> 4) == 4) {
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 30.0) * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 3) {
                    asf->avg_vid_frame_time = (uint64_t)(0.04 * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 6) {
                    asf->avg_vid_frame_time = (uint64_t)(0.02 * 10000000.0);
                    asf->know_frame_time=1;
                } else if (((segment_marker & 0XF0) >> 4) == 7) {
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 60.0) * 10000000.0);
                    asf->know_frame_time=1;
                } else {
                    // we dont know the frame time initially so 
                    // make a guess and then recalculate as we go.
                    asf->avg_vid_frame_time = (uint64_t)((1.001 / 60.0) * 10000000.0); 
                    asf->know_frame_time=0;
                }
            }
            *keyframe = (asf->new_vid_frame_seg && (segment_marker & 0X07) == 1);
        }
        pi +=payextsize;
    }

    if (id==demux->video->id && asf->new_vid_frame_seg) {    
        asf->vid_frame_ct++;
        // Some samples only have timings on key frames and 
        // the rest contain non-cronological timestamps. Interpolating
        // the values between key frames works for all samples.
        if (*keyframe) {
            asf->found_first_key_frame=1;
            if (!asf->know_frame_time && asf->last_key_payload_time > 0) {
                // We dont know average frametime so recalculate.
                // Giving precedence to the 'weight' of the existing
                // average limits damage done to new value when there is
                // a sudden time jump which happens occasionally.
                asf->avg_vid_frame_time = 
                   (0.9 * asf->avg_vid_frame_time) +
                   (0.1 * ((payload_time - asf->last_key_payload_time) / asf->vid_frame_ct));
            }
            asf->last_key_payload_time = payload_time;
            asf->vid_frame_ct = 1;
            *seg_time = payload_time;
        } else
            *seg_time = (asf->last_key_payload_time  + (asf->avg_vid_frame_time * (asf->vid_frame_ct-1)));
    }

    if (id==demux->audio->id) {
        if (payload_time != -1)
            asf->last_aud_diff = payload_time - asf->last_aud_pts;
        asf->last_aud_pts += asf->last_aud_diff;
        *seg_time = asf->last_aud_pts;
   }
}
//static int num_elementary_packets100=0;
//static int num_elementary_packets101=0;

// return value:
//     0 = EOF or no stream found
//     1 = successfully read a packet
static int demux_asf_fill_buffer(demuxer_t *demux, demux_stream_t *ds){
  struct asf_priv* asf = demux->priv;
  
  demux->filepos=stream_tell(demux->stream);
  // Brodcast stream have movi_start==movi_end
  // Better test ?
  if((demux->movi_start < demux->movi_end) && (demux->filepos>=demux->movi_end)){
          demux->stream->eof=1;
          return 0;
  }

    stream_read(demux->stream,asf->packet,asf->packetsize);
    if(demux->stream->eof) return 0; // EOF
    
    {
	    unsigned char* p=asf->packet;
            unsigned char* p_end=asf->packet+asf->packetsize;
            unsigned char flags=p[0];
            unsigned char segtype=p[1];
            unsigned padding;
            unsigned plen;
	    unsigned sequence;
            unsigned long time=0;
            unsigned short duration=0;

            int segs=1;
            unsigned char segsizetype=0x80;
            int seg=-1;
            
            if( mp_msg_test(MSGT_DEMUX,MSGL_DBG2) ){
                int i;
                for(i=0;i<16;i++) printf(" %02X",asf->packet[i]);
                printf("\n");
            }
            
	    // skip ECC data if present by testing bit 7 of flags
	    // 1xxxbbbb -> ecc data present, skip bbbb byte(s)
	    // 0xxxxxxx -> payload parsing info starts
	    if (flags & 0x80)
	    {
		p += (flags & 0x0f)+1;
		flags = p[0];
		segtype = p[1];
	    }
	    
            //if(segtype!=0x5d) printf("Warning! packet[4] != 0x5d  \n");

	    p+=2; // skip flags & segtype

            // Read packet size (plen):
	    switch((flags>>5)&3){
	    case 3: plen=LOAD_LE32(p);p+=4;break;	// dword
	    case 2: plen=LOAD_LE16(p);p+=2;break;	// word
	    case 1: plen=p[0];p++;break;		// byte
	    default: plen=0;
		//plen==0 is handled later
		//mp_msg(MSGT_DEMUX,MSGL_V,"Invalid plen type! assuming plen=0\n");
	    }

            // Read sequence:
	    switch((flags>>1)&3){
	    case 3: sequence=LOAD_LE32(p);p+=4;break;	// dword
	    case 2: sequence=LOAD_LE16(p);p+=2;break;	// word
	    case 1: sequence=p[0];p++;break;		// byte
	    default: sequence=0;
	    }

            // Read padding size (padding):
	    switch((flags>>3)&3){
	    case 3: padding=LOAD_LE32(p);p+=4;break;	// dword
	    case 2: padding=LOAD_LE16(p);p+=2;break;	// word
	    case 1: padding=p[0];p++;break;		// byte
	    default: padding=0;
	    }
	    
	    if(((flags>>5)&3)!=0){
              // Explicit (absoulte) packet size
              mp_dbg(MSGT_DEMUX,MSGL_DBG2,"Explicit packet size specified: %d  \n",plen);
              if(plen>asf->packetsize) mp_msg(MSGT_DEMUX,MSGL_V,"Warning! plen>packetsize! (%d>%d)  \n",plen,asf->packetsize);
	    } else {
              // Padding (relative) size
              plen=asf->packetsize-padding;
	    }

	    // Read time & duration:
	    time = LOAD_LE32(p); p+=4;
	    duration = LOAD_LE16(p); p+=2;

	    // Read payload flags:
            if(flags&1){
	      // multiple sub-packets
              segsizetype=p[0]>>6;
              segs=p[0] & 0x3F;
              ++p;
            }
            mp_dbg(MSGT_DEMUX,MSGL_DBG4,"%08"PRIu64":  flag=%02X  segs=%d  seq=%u  plen=%u  pad=%u  time=%ld  dur=%d\n",
              (uint64_t)demux->filepos,flags,segs,sequence,plen,padding,time,duration);

            for(seg=0;seg<segs;seg++){
              //ASF_segmhdr_t* sh;
              unsigned char streamno;
              unsigned int seq;
              unsigned int x;	// offset or timestamp
	      unsigned int rlen;
	      //
              int len;
              unsigned int time2=0;
	      int keyframe=0;

#ifdef __LINUX__
              if(p>=p_end)
#else
              if(p>=p_end || p < 0x80000000)
#endif
	      {
                mp_msg(MSGT_DEMUX,MSGL_V,"Warning! invalid packet 1, aborting parsing...\n");
                break;
              }

              if( mp_msg_test(MSGT_DEMUX,MSGL_DBG2) ){
                int i;
                printf("seg %d:",seg);
                for(i=0;i<16;i++) printf(" %02X",p[i]);
                printf("\n");
              }

              streamno=p[0]&0x7F;
	      if(p[0]&0x80) keyframe=1;
	      p++;

              // Read media object number (seq):
	      switch((segtype>>4)&3){
	      case 3: seq=LOAD_LE32(p);p+=4;break;	// dword
	      case 2: seq=LOAD_LE16(p);p+=2;break;	// word
	      case 1: seq=p[0];p++;break;		// byte
	      default: seq=0;
	      }
	      
              // Read offset or timestamp:
	      switch((segtype>>2)&3){
	      case 3: x=LOAD_LE32(p);p+=4;break;	// dword
	      case 2: x=LOAD_LE16(p);p+=2;break;	// word
	      case 1: x=p[0];p++;break;		// byte
	      default: x=0;
	      }

              // Read replic.data len:
	      switch((segtype)&3){
	      case 3: rlen=LOAD_LE32(p);p+=4;break;	// dword
	      case 2: rlen=LOAD_LE16(p);p+=2;break;	// word
	      case 1: rlen=p[0];p++;break;		// byte
	      default: rlen=0;
	      }

//	      printf("### rlen=%d   \n",rlen);
      
              switch(rlen){
              case 0x01: // 1 = special, means grouping
	        //printf("grouping: %02X  \n",p[0]);
                ++p; // skip PTS delta
                break;
              default:
	        if(rlen>=8){
            	    p+=4;	// skip object size
            	    time2=LOAD_LE32(p); // read PTS
            	    if (asf->asf_is_dvr_ms)
            	        get_payload_extension_data(demux, &p, streamno, seq, &keyframe, &time2);
#ifdef JZ47_OPT
              if (rlen <= 0x20)
#endif
		    p+=rlen-4;
		} else {
            	    mp_msg(MSGT_DEMUX,MSGL_V,"unknown segment type (rlen): 0x%02X  \n",rlen);
		    time2=0; // unknown
		    p+=rlen;
		}
              }

              if(flags&1){
                // multiple segments
		switch(segsizetype){
	          case 3: len=LOAD_LE32(p);p+=4;break;	// dword
	          case 2: len=LOAD_LE16(p);p+=2;break;	// word
	          case 1: len=p[0];p++;break;		// byte
	          default: len=plen-(p-asf->packet); // ???
		}
              } else {
                // single segment
                len=plen-(p-asf->packet);
              }
#ifdef JZ47_OPT
              if (rlen > 0x20)
	        len = 1536;
#endif
              if(len<0 || (p+len)>p_end){
                mp_msg(MSGT_DEMUX,MSGL_V,"ASF_parser: warning! segment len=%d\n",len);
              }
              mp_dbg(MSGT_DEMUX,MSGL_DBG4,"  seg #%d: streamno=%d  seq=%d  type=%02X  len=%d\n",seg,streamno,seq,rlen,len);

              switch(rlen){
              case 0x01:
                // GROUPING:
                //printf("ASF_parser: warning! grouping (flag=1) not yet supported!\n",len);
                //printf("  total: %d  \n",len);
		while(len>0){
		  int len2=p[0];
		  p++;
                  //printf("  group part: %d bytes\n",len2);
                  demux_asf_read_packet(demux,p,len2,streamno,seq,x,duration,-1,keyframe);
                  p+=len2;
		  len-=len2+1;
		  ++seq;
		}
                if(len!=0){
                  mp_msg(MSGT_DEMUX,MSGL_V,"ASF_parser: warning! groups total != len\n");
                }
                break;
              default:
                // NO GROUPING:
                //printf("fragment offset: %d  \n",sh->x);
                if (!asf->asf_is_dvr_ms || asf->found_first_key_frame)
                    demux_asf_read_packet(demux,p,len,streamno,seq,time2,duration,x,keyframe);
                p+=len;
                break;
	      }
              
            } // for segs
            return 1; // success
    }
    
    mp_msg(MSGT_DEMUX,MSGL_V,"%08"PRIX64":  UNKNOWN TYPE  %02X %02X %02X %02X %02X...\n",(int64_t)demux->filepos,asf->packet[0],asf->packet[1],asf->packet[2],asf->packet[3],asf->packet[4]);
    return 0;
}

#include "stheader.h"

extern void skip_audio_frame(sh_audio_t *sh_audio);

static void demux_seek_asf(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags){
    struct asf_priv* asf = demuxer->priv;
    demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=d_audio->sh;
//    sh_video_t *sh_video=d_video->sh;

  //FIXME: OFF_T - didn't test ASF case yet (don't have a large asf...)
  //FIXME: reports good or bad to steve@daviesfam.org please

  //================= seek in ASF ==========================
    float p_rate=asf->packetrate; // packets / sec
#ifdef JZ47_OPT
    p_rate = (demuxer->movi_end-demuxer->movi_start)/(float)asf->packetsize/(float)asf->movielength;
    if (p_rate > asf->packetrate - 2 && p_rate < asf->packetrate + 2)
      p_rate = asf->packetrate;
#endif
    off_t rel_seek_packs=(flags&2)?	 // FIXME: int may be enough?
	(rel_seek_secs*(demuxer->movi_end-demuxer->movi_start)/asf->packetsize):
	(rel_seek_secs*p_rate);
    off_t rel_seek_bytes=rel_seek_packs*asf->packetsize;
    off_t newpos;
    //printf("ASF: packs: %d  duration: %d  \n",(int)fileh.packets,*((int*)&fileh.duration));
//    printf("ASF_seek: %d secs -> %d packs -> %d bytes  \n",
//       rel_seek_secs,rel_seek_packs,rel_seek_bytes);
    newpos=((flags&1)?demuxer->movi_start:demuxer->filepos)+rel_seek_bytes;
    if(newpos<0 || newpos<demuxer->movi_start) newpos=demuxer->movi_start;
//    printf("\r -- asf: newpos=%d -- \n",newpos);
    stream_seek(demuxer->stream,newpos);

    if (asf->asf_is_dvr_ms) asf->dvr_last_vid_pts = 0.0f;

    if (d_video->id >= 0)
    ds_fill_buffer(d_video);
    if(sh_audio){
      ds_fill_buffer(d_audio);
    }
    
    if (d_video->id >= 0)
    while(1){
	if(sh_audio && !d_audio->eof){
	  float a_pts=d_audio->pts;
          a_pts+=(ds_tell_pts(d_audio)-sh_audio->a_in_buffer_len)/(float)sh_audio->i_bps;
	  // sync audio:
          if (d_video->pts > a_pts){
	      skip_audio_frame(sh_audio);
//	      if(!ds_fill_buffer(d_audio)) sh_audio=NULL; // skip audio. EOF?
	      continue;
	  }
	}
	if(d_video->flags&1) break; // found a keyframe!
	if(!ds_fill_buffer(d_video)) break; // skip frame.  EOF?
    }


}

//---------------------------------------------------------------------------

static int FindNextKeyFrameIdx(demuxer_t * demux)
{  demux_stream_t *vs =demux->video;
  int i,count = 0;
  demux_packet_t *dp = vs->first;
 
  while(1){
    if(dp == NULL) return count+300;
    count++;
    if(dp->flags & 0x1){
      
      return count;
    }
    dp = dp->next;
    
  }
}
//---------------------------------------------------------------------------------------------------
static int demux_asf_control(demuxer_t *demuxer,int cmd, void *arg){
    struct asf_priv* asf = demuxer->priv;
/*  demux_stream_t *d_audio=demuxer->audio;
    demux_stream_t *d_video=demuxer->video;
    sh_audio_t *sh_audio=d_audio->sh;
    sh_video_t *sh_video=d_video->sh;
*/
    switch(cmd) {
     
      //add for skip to frame start
	case DEMUXER_CTRL_SKIP_TOKEYFRAME:
	  // skip_to_keyframe = 1;
		return 1;
	case DEMUXER_CTRL_IS_KEYFRAME:
		if(*(int *)arg & 0x1000)
			return 0;
		return (*(int *)arg & 0x1);
	case DEMUXER_CTRL_SET_SKIP:
		*(int *)arg |= 0x1000;
		return	DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_SKIPFRAME_TIME:
	{
		return DEMUXER_CTRL_OK;
	}
	case DEMUXER_CTRL_GET_KEYFRAME_TIME:
	{
	  int count = FindNextKeyFrameIdx(demuxer);
	  
	   *(float *)arg = ((float)(count) * 0.041708);				
		return DEMUXER_CTRL_OK;
	}
      //add for skip to frame end
    
	case DEMUXER_CTRL_GET_TIME_LENGTH:
	    *((double *)arg)=(double)(asf->movielength);
	    return DEMUXER_CTRL_OK;

	case DEMUXER_CTRL_GET_PERCENT_POS:
		return DEMUXER_CTRL_DONTKNOW;

	default:
	    return DEMUXER_CTRL_NOTIMPL;
    }
}


static demuxer_t* demux_open_asf(demuxer_t* demuxer)
{
    struct asf_priv* asf = demuxer->priv;
    sh_audio_t *sh_audio=NULL;
    sh_video_t *sh_video=NULL;

    //---- ASF header:
    if(!asf) return NULL;
    init_priv(asf);
    if (!read_asf_header(demuxer,asf))
        return NULL;
    stream_reset(demuxer->stream);
    stream_seek(demuxer->stream,demuxer->movi_start);
//    demuxer->idx_pos=0;
//    demuxer->endpos=avi_header.movi_end;
    if(demuxer->video->id != -2) {
        if(!ds_fill_buffer(demuxer->video)){
            mp_msg(MSGT_DEMUXER,MSGL_WARN,"ASF: " MSGTR_MissingVideoStream);
            demuxer->video->sh=NULL;
            //printf("ASF: missing video stream!? contact the author, it may be a bug :(\n");
        } else {
            sh_video=demuxer->video->sh;sh_video->ds=demuxer->video;
            sh_video->fps=1000.0f; sh_video->frametime=0.001f;

            if (asf->asf_is_dvr_ms) {
                sh_video->bih->biWidth = 0;
                sh_video->bih->biHeight = 0;
            }
        }
    }

    if(demuxer->audio->id!=-2){
        mp_msg(MSGT_DEMUXER,MSGL_V,MSGTR_ASFSearchingForAudioStream,demuxer->audio->id);
        if(!ds_fill_buffer(demuxer->audio)){
            mp_msg(MSGT_DEMUXER,MSGL_INFO,"ASF: " MSGTR_MissingAudioStream);
            demuxer->audio->sh=NULL;
        } else {
            sh_audio=demuxer->audio->sh;sh_audio->ds=demuxer->audio;
            sh_audio->format=sh_audio->wf->wFormatTag;
        }
    }
    if(!demuxer->stream->seek)
        demuxer->seekable=0;
    frame_dropping = 0;
    return demuxer;
}


static void demux_close_asf(demuxer_t *demuxer) {
    struct asf_priv* asf = demuxer->priv;

    if (!asf) return;

    if (asf->aud_repdata_sizes)
      free(asf->aud_repdata_sizes);
      
    if (asf->vid_repdata_sizes)
      free(asf->vid_repdata_sizes);
      
    free(asf);
}

demuxer_desc_t demuxer_desc_asf = {
  "ASF demuxer",
  "asf",
  "ASF",
  "A'rpi",
  "ASF, WMV, WMA",
  DEMUXER_TYPE_ASF,
  1, // safe autodetect
  asf_check_header,
  demux_asf_fill_buffer,
  demux_open_asf,
  demux_close_asf,
  demux_seek_asf,
  demux_asf_control
};
