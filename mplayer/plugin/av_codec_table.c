
#include "../av_plugin.h"
#define NULL ((void *)0)

#include "../codec-cfg.h"
#include "../codecs.conf.h"
#include "../uclib.h"
extern codecs_t *video_codecs;
extern codecs_t *audio_codecs;
extern int nr_vcodecs;
extern int nr_acodecs;

extern struct AV_PLUGIN_TBL *ffmpeg_plugin_search_table;
extern struct AV_PLUGIN_TBL ffmpeg_plugin_table [];

int install_init_plugin_complete = 0;
extern unsigned int _fbss_plugin, _end_plugin;

#undef printf
void _start_avcodec_table (int tableno)
{
  unsigned int *p = &_fbss_plugin;

//  kprintf ("+++ table init start %d, %d +++\n", tableno, FFMPEG_PLUGIN_TABLE);

  /* BSS init */
  while (p != &_end_plugin)
    *p++ = 0;

  switch (tableno)
  {
    case FFMPEG_PLUGIN_TABLE:
      ffmpeg_plugin_search_table = ffmpeg_plugin_table;
      break;
  }

// init codecs tables
  video_codecs = builtin_video_codecs;
  audio_codecs = builtin_audio_codecs;
  nr_vcodecs = sizeof(builtin_video_codecs)/sizeof(codecs_t);
  nr_acodecs = sizeof(builtin_audio_codecs)/sizeof(codecs_t);

// install init plugin
//  kprintf ("+++ install_init_plugin_complete = %d  +++\n", install_init_plugin_complete);
  if (!install_init_plugin_complete)
  {
    install_mpcodecs_vorbis ();
    install_demuxer_ogg ();
    install_init_plugin_complete = 1;
  }
  return;
}


struct AV_PLUGIN_TBL ffmpeg_plugin_table [] = 
{
  /* ffmpeg video codec plugin */
//
  {"h264", "ffmpeg_vd_h264",  0},
  {"svq3", "ffmpeg_vd_svq3",  0},
  {"flv", "ffmpeg_vd_mpeg4", 0},
  {"h263i", "ffmpeg_vd_mpeg4", 0},
  {"wmv1", "ffmpeg_vd_mpeg4", 0},
  {"msmpeg4", "ffmpeg_vd_mpeg4", 0},
  {"msmpeg4v2", "ffmpeg_vd_mpeg4", 0},
  {"msmpeg4v1", "ffmpeg_vd_mpeg4", 0},
  {"h263", "ffmpeg_vd_mpeg4", 0},
  {"mpeg4", "ffmpeg_vd_mpeg4", 0},
  {"wmv2", "ffmpeg_vd_mpeg4", 0},
  {"rv10", "ffmpeg_vd_mpeg4", 0},
  {"rv20", "ffmpeg_vd_mpeg4", 0},
  {"vc1", "ffmpeg_vd_mpeg4", 0},
  {"wmv3", "ffmpeg_vd_mpeg4", 0},
  {"theora", "ffmpeg_vd_mpeg4", 0},
  {"vp3", "ffmpeg_vd_mpeg4", 0},
  {"vp5", "ffmpeg_vd_mpeg4", 0},
  {"vp6", "ffmpeg_vd_mpeg4", 0},
  {"vp6f", "ffmpeg_vd_mpeg4", 0},
  {"vp6a", "ffmpeg_vd_mpeg4", 0},
//
  {"mpeg1video", "ffmpeg_vd_mpegvideo", 0},
  {"mpeg2video", "ffmpeg_vd_mpegvideo", 0},
  {"mpegvideo", "ffmpeg_vd_mpegvideo", 0},
  {"mdec", "ffmpeg_vd_mpegvideo", 0},
  {"asv1", "ffmpeg_vd_mpegvideo", 0},
  {"asv2", "ffmpeg_vd_mpegvideo", 0},
  {"avs", "ffmpeg_vd_mpegvideo", 0},
  {"cavs", "ffmpeg_vd_mpegvideo", 0},
  {"sp5x", "ffmpeg_vd_mpegvideo", 0},
  {"amv", "ffmpeg_vd_mpegvideo", 0},
  {"nuv", "ffmpeg_vd_mpegvideo", 0},
  {"h261", "ffmpeg_vd_mpegvideo", 0},
  {"mjpeg", "ffmpeg_vd_mpegvideo", 0},
  {"thp", "ffmpeg_vd_mpegvideo", 0},
  {"jpegls", "ffmpeg_vd_mpegvideo", 0},
  {"mjpegb", "ffmpeg_vd_mpegvideo", 0},
  {"sgi", "ffmpeg_vd_mpegvideo", 0},
  {"smackvid", "ffmpeg_vd_mpegvideo", 0},
  {"tiertexseqvideo", "ffmpeg_vd_mpegvideo", 0},
  {"tiff", "ffmpeg_vd_mpegvideo", 0},
  {"truemotion1", "ffmpeg_vd_mpegvideo", 0},
  {"truemotion2", "ffmpeg_vd_mpegvideo", 0},
  {"camtasia", "ffmpeg_vd_mpegvideo", 0},
  {"txd", "ffmpeg_vd_mpegvideo", 0},
  {"ultimotion", "ffmpeg_vd_mpegvideo", 0},
  {"vcr1", "ffmpeg_vd_mpegvideo", 0},
  {"vmdvideo", "ffmpeg_vd_mpegvideo", 0},
  {"VMware video", "ffmpeg_vd_mpegvideo", 0},
  {"vqavideo", "ffmpeg_vd_mpegvideo", 0},
  {"wnv1", "ffmpeg_vd_mpegvideo", 0},
  {"xan_wc3", "ffmpeg_vd_mpegvideo", 0},
  {"xl", "ffmpeg_vd_mpegvideo", 0},
  {"xsub", "ffmpeg_vd_mpegvideo", 0},
//
  {"bethsoftvid", "ffmpeg_vd_mpegmisc", 0},
  {"aasc", "ffmpeg_vd_mpegmisc", 0},
  {"cinepak", "ffmpeg_vd_mpegmisc", 0},
  {"cljr", "ffmpeg_vd_mpegmisc", 0},
  {"camstudio", "ffmpeg_vd_mpegmisc", 0},
  {"cyuv", "ffmpeg_vd_mpegmisc", 0},
  {"dnxhd", "ffmpeg_vd_mpegmisc", 0},
  {"ffv1", "ffmpeg_vd_mpegmisc", 0},
  {"huffyuv", "ffmpeg_vd_mpegmisc", 0},
  {"ffvhuff", "ffmpeg_vd_mpegmisc", 0},
  {"flic", "ffmpeg_vd_mpegmisc", 0},
  {"4xm", "ffmpeg_vd_mpegmisc", 0},
  {"fraps", "ffmpeg_vd_mpegmisc", 0},
  {"dsicinvideo", "ffmpeg_vd_mpegmisc", 0},
  {"dvvideo", "ffmpeg_vd_mpegmisc", 0},
  {"8bps", "ffmpeg_vd_mpegmisc", 0},
  {"gif", "ffmpeg_vd_mpegmisc", 0},
  {"bmp", "ffmpeg_vd_mpegmisc", 0},
  {"c93", "ffmpeg_vd_mpegmisc", 0},
  {"idcinvideo", "ffmpeg_vd_mpegmisc", 0},
  {"indeo2", "ffmpeg_vd_mpegmisc", 0},
  {"indeo3", "ffmpeg_vd_mpegmisc", 0},
  {"interplayvideo", "ffmpeg_vd_mpegmisc", 0},
  {"kmvc", "ffmpeg_vd_mpegmisc", 0},
  {"loco", "ffmpeg_vd_mpegmisc", 0},
//
  {"svq1", "ffmpeg_vd_mpegmisc2", 0},
  {"mmvideo", "ffmpeg_vd_mpegmisc2", 0},
  {"msrle", "ffmpeg_vd_mpegmisc2", 0},
  {"msvideo1", "ffmpeg_vd_mpegmisc2", 0},
  {"mszh", "ffmpeg_vd_mpegmisc2", 0},
  {"zlib", "ffmpeg_vd_mpegmisc2", 0},
  {"smc", "ffmpeg_vd_mpegmisc2", 0},
  {"targa", "ffmpeg_vd_mpegmisc2", 0},
  {"ptx", "ffmpeg_vd_mpegmisc2", 0},
  {"qdraw", "ffmpeg_vd_mpegmisc2", 0},
  {"qpeg", "ffmpeg_vd_mpegmisc2", 0},
  {"qtrle", "ffmpeg_vd_mpegmisc2", 0},
  {"rawvideo", "ffmpeg_vd_mpegmisc2", 0},
  {"roqvideo", "ffmpeg_vd_mpegmisc2", 0},
  {"rpza", "ffmpeg_vd_mpegmisc2", 0},

  /* ffmpeg audio codec plugin */
  {"smackaud", "", 0},
  {"vmdaudio", "", 0},
  {"dsicinaudio", "", 0},
  {"wmav1", "ffmpeg_ad_one", 0},
  {"wmav2", "ffmpeg_ad_one", 0},
  {"mace3", "ffmpeg_ad_one", 0},
  {"mace6", "ffmpeg_ad_one", 0},
  {"shorten", "ffmpeg_ad_one", 0},
  {"sonic", "ffmpeg_ad_one", 0},
  {"ws_snd1", "ffmpeg_ad_one", 0},

  {"mp2", "ffmpeg_ad_two", 0},
  {"mp3", "ffmpeg_ad_two", 0},
  {"mp3adu", "ffmpeg_ad_two", 0},
  {"mp3on4", "ffmpeg_ad_two", 0},

  {"cook", "ffmpeg_ad_three", 0},
  {"real_288", "ffmpeg_ad_three", 0},
  {"real_144", "ffmpeg_ad_three", 0},
  {"flac", "ffmpeg_ad_three", 0},
  {"dca", "ffmpeg_ad_three", 0},
  {"alac", "ffmpeg_ad_three", 0},
  {"ape", "ffmpeg_ad_three", 0},
  /* pcm */
  {"pcm_s32le", "ffmpeg_ad_three", 0},
  {"pcm_s32be", "ffmpeg_ad_three", 0},
  {"pcm_u32le", "ffmpeg_ad_three", 0},
  {"pcm_u32be", "ffmpeg_ad_three", 0},
  {"pcm_s24le", "ffmpeg_ad_three", 0},
  {"pcm_s24be", "ffmpeg_ad_three", 0},
  {"pcm_u24le", "ffmpeg_ad_three", 0},
  {"pcm_u24be", "ffmpeg_ad_three", 0},
  {"pcm_s24daud", "ffmpeg_ad_three", 0},
  {"pcm_s16le", "ffmpeg_ad_three", 0},
  {"pcm_s16be", "ffmpeg_ad_three", 0},
  {"pcm_u16le", "ffmpeg_ad_three", 0},
  {"pcm_u16be", "ffmpeg_ad_three", 0},
  {"pcm_s8", "ffmpeg_ad_three", 0},
  {"pcm_u8", "ffmpeg_ad_three", 0},
  {"pcm_alaw", "ffmpeg_ad_three", 0},
  {"pcm_mulaw", "ffmpeg_ad_three", 0},
  {"pcm_zork", "ffmpeg_ad_three", 0},
  /* dpcm */
  {"roq_dpcm", "ffmpeg_ad_three", 0},
  {"interplay_dpcm", "ffmpeg_ad_three", 0},
  {"xan_dpcm", "ffmpeg_ad_three", 0},
  {"sol_dpcm", "ffmpeg_ad_three", 0},
  /* adpcm */
  {"g726", "ffmpeg_ad_three", 0},
  {"adpcm_adx", "ffmpeg_ad_three", 0},
  {"adpcm_4xm", "ffmpeg_ad_three", 0},
  {"adpcm_ct", "ffmpeg_ad_three", 0},
  {"adpcm_ea", "ffmpeg_ad_three", 0},
  {"adpcm_ima_amv", "ffmpeg_ad_three", 0},
  {"adpcm_ima_dk3", "ffmpeg_ad_three", 0},
  {"adpcm_ima_dk4", "ffmpeg_ad_three", 0},
  {"adpcm_ima_qt", "ffmpeg_ad_three", 0},
  {"adpcm_ima_smjpeg", "ffmpeg_ad_three", 0},
  {"adpcm_ima_wav", "ffmpeg_ad_three", 0},
  {"adpcm_ima_ws", "ffmpeg_ad_three", 0},
  {"adpcm_ms", "ffmpeg_ad_three", 0},
  {"adpcm_sbpro_2", "ffmpeg_ad_three", 0},
  {"adpcm_sbpro_3", "ffmpeg_ad_three", 0},
  {"adpcm_sbpro_4", "ffmpeg_ad_three", 0},
  {"adpcm_swf", "ffmpeg_ad_three", 0},
  {"adpcm_thp", "ffmpeg_ad_three", 0},
  {"adpcm_xa", "ffmpeg_ad_three", 0},
  {"adpcm_yamaha", "ffmpeg_ad_three", 0},

  {"mpc sv7", "ffmpeg_ad_four", 0},
  {"qdm2", "ffmpeg_ad_four", 0},
  {"truespeech", "ffmpeg_ad_four", 0},
  {"tta", "ffmpeg_ad_four", 0},
  {"wavpack", "ffmpeg_ad_four", 0},
  {"imc", "ffmpeg_ad_four", 0},

  {"ac3", "ffmpeg_ad_five", 0},
  {"vorbis", "ffmpeg_ad_five", 0},

  {"libamr_nb", "ffmpeg_ad_six", 0},
  {"atrac 3", "ffmpeg_ad_six", 0},

  /* end */
  {"", "", 0},
};

