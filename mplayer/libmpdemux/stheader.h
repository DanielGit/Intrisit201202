#ifndef STHEADER_H
#define STHEADER_H

#include "aviheader.h"
#include "ms_hdr.h"

// Stream headers:

typedef struct {
  int aid;
  demux_stream_t *ds;
  struct codecs_st *codec;
  unsigned int format;
  int inited;
  float stream_delay; // number of seconds stream should be delayed (according to dwStart or similar)
  // output format:
  int sample_format;
  int samplerate;
  int samplesize;
  int channels;
  int o_bps; // == samplerate*samplesize*channels   (uncompr. bytes/sec)
  int i_bps; // == bitrate  (compressed bytes/sec)
  // in buffers:
  int audio_in_minsize;	// max. compressed packet size (== min. in buffer size)
  char* a_in_buffer;
  int a_in_buffer_len;
  int a_in_buffer_size;
  // decoder buffers:
  int audio_out_minsize; // max. uncompressed packet size (==min. out buffsize)
  char* a_buffer;
  int a_buffer_len;
  int a_buffer_size;
  // output buffers:
  char* a_out_buffer;
  int a_out_buffer_len;
  int a_out_buffer_size;
//  void* audio_out;        // the audio_out handle, used for this audio stream
  void* afilter;          // the audio filter stream
  struct ad_functions_s* ad_driver;
#ifdef DYNAMIC_PLUGINS
  void *dec_handle;
#endif
  // win32-compatible codec parameters:
  AVIStreamHeader audio;
  WAVEFORMATEX* wf;
  // codec-specific:
  void* context; // codec-specific stuff (usually HANDLE or struct pointer)
  unsigned char* codecdata; // extra header data passed from demuxer to codec
  int codecdata_len;
  int codecdatatype; //1 = malloc 0 no alloc
  double pts;  // last known pts value in output from decoder
  int pts_bytes; // bytes output by decoder after last known pts
} sh_audio_t;

typedef struct {
  int vid;
  demux_stream_t *ds;
  struct codecs_st *codec;
  unsigned int format;
  int inited;
  float timer;		  // absolute time in video stream, since last start/seek
  float stream_delay; // number of seconds stream should be delayed (according to dwStart or similar)
  // frame counters:
  float num_frames;       // number of frames played
  int num_frames_decoded; // number of frames decoded
  int pic_flag;
  // timing (mostly for mpeg):
  double pts;     // predicted/interpolated PTS of the current frame
  double i_pts;   // PTS for the _next_ I/P frame
  float next_frame_time;
  double last_pts;
  double buffered_pts[20];
  int num_buffered_pts;
  // output format: (set by demuxer)
  float fps;              // frames per second (set only if constant fps)
  float frametime;        // 1/fps
  float aspect;           // aspect ratio stored in the file (for prescaling)
  float stream_aspect;  // aspect ratio stored in the media headers (e.g. in DVD IFO files)
  int i_bps;              // == bitrate  (compressed bytes/sec)
  int disp_w,disp_h;      // display size (filled by fileformat parser)
  // output driver/filters: (set by libmpcodecs core)
  unsigned int outfmtidx;
  void* video_out;        // the video_out handle, used for this video stream
  void* vfilter;          // the video filter chain, used for this video stream
  int vf_inited;
#ifdef DYNAMIC_PLUGINS
  void *dec_handle;
#endif
  // win32-compatible codec parameters:
  AVIStreamHeader video;
  BITMAPINFOHEADER* bih;
  void* ImageDesc; // for quicktime codecs
  // codec-specific:
  void* context;   // codec-specific stuff (usually HANDLE or struct pointer)
} sh_video_t;

typedef struct {
  int sid;
  char type;                    // t = text, v = VobSub, a = SSA/ASS
  int has_palette;              // If we have a valid palette
  unsigned int palette[16];     // for VobSubs
  int width, height;            // for VobSubs
  int custom_colors;
  unsigned int colors[4];
  int forced_subs_only;
#ifdef USE_ASS
  ass_track_t* ass_track;  // for SSA/ASS streams (type == 'a')
#endif
} sh_sub_t;

// demuxer.c:
#define new_sh_audio(d, i) new_sh_audio_aid(d, i, i)
sh_audio_t* new_sh_audio_aid(demuxer_t *demuxer,int id,int aid);
#define new_sh_video(d, i) new_sh_video_vid(d, i, i)
sh_video_t* new_sh_video_vid(demuxer_t *demuxer,int id,int vid);
#define new_sh_sub(d, i) new_sh_sub_sid(d, i, i)
sh_sub_t *new_sh_sub_sid(demuxer_t *demuxer, int id, int sid);
void free_sh_audio(demuxer_t *demuxer, int id);
void free_sh_video(sh_video_t *sh);

// video.c:
int video_read_properties(sh_video_t *sh_video);
int video_read_frame(sh_video_t* sh_video,float* frame_time_ptr,unsigned char** start,int force_fps);

#endif
