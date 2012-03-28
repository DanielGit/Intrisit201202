
#include "../av_plugin.h"
#define NULL ((void *)0)

#include "../codec-cfg.h"
#include "../codecs.conf.h"
#include "../libavcodec/avcodec.h"
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
  {"h264", "ffmpeg_vd_h264",  CODEC_ID_H264},
  {"svq3", "ffmpeg_vd_svq3",  CODEC_ID_SVQ3},
  {"flv", "ffmpeg_vd_mpeg4", CODEC_ID_FLV1},
  {"h263i", "ffmpeg_vd_mpeg4", CODEC_ID_H263I},
  {"wmv1", "ffmpeg_vd_mpeg4", CODEC_ID_WMV1},
  {"msmpeg4", "ffmpeg_vd_mpeg4", CODEC_ID_MSMPEG4V3},
  {"msmpeg4v2", "ffmpeg_vd_mpeg4", CODEC_ID_MSMPEG4V2},
  {"msmpeg4v1", "ffmpeg_vd_mpeg4", CODEC_ID_MSMPEG4V1},
  {"h263", "ffmpeg_vd_mpeg4", CODEC_ID_H263},
  {"mpeg4", "ffmpeg_vd_mpeg4", CODEC_ID_MPEG4},
  {"wmv2", "ffmpeg_vd_mpeg4", CODEC_ID_WMV2},
  {"vc1", "ffmpeg_vd_mpeg4", CODEC_ID_VC1},
  {"wmv3", "ffmpeg_vd_mpeg4", CODEC_ID_WMV3},
//  
  {"theora", "ffmpeg_vd_mpegvideo", CODEC_ID_THEORA},
  {"vp3", "ffmpeg_vd_mpegvideo", CODEC_ID_VP3},
  {"vp5", "ffmpeg_vd_mpegvideo", CODEC_ID_VP5},
  {"vp6", "ffmpeg_vd_mpegvideo", CODEC_ID_VP6},
  {"vp6f", "ffmpeg_vd_mpegvideo", CODEC_ID_VP6F},
  {"vp6a", "ffmpeg_vd_mpegvideo", CODEC_ID_VP6A},
  {"rv10", "ffmpeg_vd_mpegvideo", CODEC_ID_RV10},
  {"rv20", "ffmpeg_vd_mpegvideo", CODEC_ID_RV20},
  {"mpeg1video", "ffmpeg_vd_mpegvideo", CODEC_ID_MPEG1VIDEO},
  {"mpeg2video", "ffmpeg_vd_mpegvideo", CODEC_ID_MPEG2VIDEO},
  {"mpegvideo", "ffmpeg_vd_mpegvideo", 0},
  {"mdec", "ffmpeg_vd_mpegvideo", CODEC_ID_MDEC},
  {"asv1", "ffmpeg_vd_mpegvideo", CODEC_ID_ASV1},
  {"asv2", "ffmpeg_vd_mpegvideo", CODEC_ID_ASV2},
  {"avs", "ffmpeg_vd_mpegvideo", CODEC_ID_AVS},
  {"cavs", "ffmpeg_vd_mpegvideo", CODEC_ID_CAVS},
  {"sp5x", "ffmpeg_vd_mpegvideo", CODEC_ID_SP5X},
  {"amv", "ffmpeg_vd_mpegvideo", CODEC_ID_AMV},
  {"nuv", "ffmpeg_vd_mpegvideo", CODEC_ID_NUV},
  {"h261", "ffmpeg_vd_mpegvideo", CODEC_ID_H261},
  {"mjpeg", "ffmpeg_vd_mpegvideo", CODEC_ID_MJPEG},
  {"thp", "ffmpeg_vd_mpegvideo", CODEC_ID_THP},
  {"jpegls", "ffmpeg_vd_mpegvideo", CODEC_ID_JPEGLS},
  {"mjpegb", "ffmpeg_vd_mpegvideo", CODEC_ID_MJPEGB},
  {"sgi", "ffmpeg_vd_mpegvideo", CODEC_ID_SGI},
  {"smackvid", "ffmpeg_vd_mpegvideo", CODEC_ID_SMACKVIDEO},
  {"tiertexseqvideo", "ffmpeg_vd_mpegvideo", CODEC_ID_TIERTEXSEQVIDEO},
  {"tiff", "ffmpeg_vd_mpegvideo", CODEC_ID_TIFF},
  {"truemotion1", "ffmpeg_vd_mpegvideo", CODEC_ID_TRUEMOTION1},
  {"truemotion2", "ffmpeg_vd_mpegvideo", CODEC_ID_TRUEMOTION2},
  {"camtasia", "ffmpeg_vd_mpegvideo", CODEC_ID_TSCC},
  {"txd", "ffmpeg_vd_mpegvideo", CODEC_ID_TXD},
  {"ultimotion", "ffmpeg_vd_mpegvideo", CODEC_ID_ULTI},
  {"vcr1", "ffmpeg_vd_mpegvideo", CODEC_ID_VCR1},
  {"vmdvideo", "ffmpeg_vd_mpegvideo", CODEC_ID_VMDVIDEO},
  {"VMware video", "ffmpeg_vd_mpegvideo", CODEC_ID_VMNC},
  {"vqavideo", "ffmpeg_vd_mpegvideo", CODEC_ID_WS_VQA},
  {"wnv1", "ffmpeg_vd_mpegvideo", CODEC_ID_WNV1},
  {"xan_wc3", "ffmpeg_vd_mpegvideo", CODEC_ID_XAN_WC3},
  {"xl", "ffmpeg_vd_mpegvideo", CODEC_ID_VIXL},
  {"xsub", "ffmpeg_vd_mpegvideo", CODEC_ID_XSUB},
//
  {"bethsoftvid", "ffmpeg_vd_mpegmisc", CODEC_ID_BETHSOFTVID},
  {"aasc", "ffmpeg_vd_mpegmisc", CODEC_ID_AASC},
  {"cinepak", "ffmpeg_vd_mpegmisc", CODEC_ID_CINEPAK},
  {"cljr", "ffmpeg_vd_mpegmisc", CODEC_ID_CLJR},
  {"camstudio", "ffmpeg_vd_mpegmisc", CODEC_ID_CSCD},
  {"cyuv", "ffmpeg_vd_mpegmisc", CODEC_ID_CYUV},
  {"dnxhd", "ffmpeg_vd_mpegmisc", CODEC_ID_DNXHD},
  {"ffv1", "ffmpeg_vd_mpegmisc", CODEC_ID_FFV1},
  {"huffyuv", "ffmpeg_vd_mpegmisc", CODEC_ID_HUFFYUV},
  {"ffvhuff", "ffmpeg_vd_mpegmisc", CODEC_ID_FFVHUFF},
  {"flic", "ffmpeg_vd_mpegmisc", CODEC_ID_FLIC},
  {"4xm", "ffmpeg_vd_mpegmisc", CODEC_ID_4XM},
  {"fraps", "ffmpeg_vd_mpegmisc", CODEC_ID_FRAPS},
  {"dsicinvideo", "ffmpeg_vd_mpegmisc", CODEC_ID_DSICINVIDEO},
  {"dvvideo", "ffmpeg_vd_mpegmisc", CODEC_ID_DVVIDEO},
  {"8bps", "ffmpeg_vd_mpegmisc", CODEC_ID_8BPS},
  {"gif", "ffmpeg_vd_mpegmisc", CODEC_ID_GIF},
  {"bmp", "ffmpeg_vd_mpegmisc", CODEC_ID_BMP},
  {"c93", "ffmpeg_vd_mpegmisc", CODEC_ID_C93},
  {"idcinvideo", "ffmpeg_vd_mpegmisc", CODEC_ID_IDCIN},
  {"indeo2", "ffmpeg_vd_mpegmisc", CODEC_ID_INDEO2},
  {"indeo3", "ffmpeg_vd_mpegmisc", CODEC_ID_INDEO3},
  {"interplayvideo", "ffmpeg_vd_mpegmisc", CODEC_ID_INTERPLAY_VIDEO},
  {"kmvc", "ffmpeg_vd_mpegmisc", CODEC_ID_KMVC},
  {"loco", "ffmpeg_vd_mpegmisc", CODEC_ID_LOCO},
//
  {"svq1", "ffmpeg_vd_mpegmisc2", CODEC_ID_SVQ1},
  {"mmvideo", "ffmpeg_vd_mpegmisc2", CODEC_ID_MMVIDEO},
  {"msrle", "ffmpeg_vd_mpegmisc2", CODEC_ID_MSRLE},
  {"msvideo1", "ffmpeg_vd_mpegmisc2", CODEC_ID_MSVIDEO1},
  {"mszh", "ffmpeg_vd_mpegmisc2", CODEC_ID_MSZH},
  {"zlib", "ffmpeg_vd_mpegmisc2", CODEC_ID_ZLIB},
  {"smc", "ffmpeg_vd_mpegmisc2", CODEC_ID_SMC},
  {"targa", "ffmpeg_vd_mpegmisc2", CODEC_ID_TARGA},
  {"ptx", "ffmpeg_vd_mpegmisc2", CODEC_ID_PTX},
  {"qdraw", "ffmpeg_vd_mpegmisc2", CODEC_ID_QDRAW},
  {"qpeg", "ffmpeg_vd_mpegmisc2", CODEC_ID_QPEG},
  {"qtrle", "ffmpeg_vd_mpegmisc2", CODEC_ID_QTRLE},
  {"rawvideo", "ffmpeg_vd_mpegmisc2", CODEC_ID_RAWVIDEO},
  {"roqvideo", "ffmpeg_vd_mpegmisc2", CODEC_ID_ROQ},
  {"rpza", "ffmpeg_vd_mpegmisc2", CODEC_ID_RPZA},

  /* ffmpeg audio codec plugin */
  {"smackaud", "", CODEC_ID_SMACKAUDIO},
  {"vmdaudio", "", CODEC_ID_VMDAUDIO},
  {"dsicinaudio", "", CODEC_ID_DSICINAUDIO},
  {"wmav1", "ffmpeg_ad_one", CODEC_ID_WMAV1},
  {"wmav2", "ffmpeg_ad_one", CODEC_ID_WMAV2},
  {"mace3", "ffmpeg_ad_one", CODEC_ID_MACE3},
  {"mace6", "ffmpeg_ad_one", CODEC_ID_MACE6},
  {"shorten", "ffmpeg_ad_one", CODEC_ID_SHORTEN},
  {"sonic", "ffmpeg_ad_one", CODEC_ID_SONIC},
  {"ws_snd1", "ffmpeg_ad_one", CODEC_ID_WESTWOOD_SND1},

  {"mp2", "ffmpeg_ad_two", CODEC_ID_MP2},
  {"mp3", "ffmpeg_ad_two", CODEC_ID_MP3},
  {"mp3adu", "ffmpeg_ad_two", CODEC_ID_MP3ADU},
  {"mp3on4", "ffmpeg_ad_two", CODEC_ID_MP3ON4},

  {"cook", "ffmpeg_ad_three", CODEC_ID_COOK},
  {"real_288", "ffmpeg_ad_three", CODEC_ID_RA_288},
  {"real_144", "ffmpeg_ad_three", CODEC_ID_RA_144},
  {"flac", "ffmpeg_ad_three", CODEC_ID_FLAC},
  {"dca", "ffmpeg_ad_three", CODEC_ID_DTS},
  {"alac", "ffmpeg_ad_three", CODEC_ID_ALAC},
  {"ape", "ffmpeg_ad_three", CODEC_ID_APE},
  /* pcm */
  {"pcm_s32le", "ffmpeg_ad_three", CODEC_ID_PCM_S32LE},
  {"pcm_s32be", "ffmpeg_ad_three", CODEC_ID_PCM_S32BE},
  {"pcm_u32le", "ffmpeg_ad_three", CODEC_ID_PCM_U32LE},
  {"pcm_u32be", "ffmpeg_ad_three", CODEC_ID_PCM_U32BE},
  {"pcm_s24le", "ffmpeg_ad_three", CODEC_ID_PCM_S24LE},
  {"pcm_s24be", "ffmpeg_ad_three", CODEC_ID_PCM_S24BE},
  {"pcm_u24le", "ffmpeg_ad_three", CODEC_ID_PCM_U24LE},
  {"pcm_u24be", "ffmpeg_ad_three", CODEC_ID_PCM_U24BE},
  {"pcm_s24daud", "ffmpeg_ad_three", CODEC_ID_PCM_S24DAUD},
  {"pcm_s16le", "ffmpeg_ad_three", CODEC_ID_PCM_S16LE},
  {"pcm_s16be", "ffmpeg_ad_three", CODEC_ID_PCM_S16BE},
  {"pcm_u16le", "ffmpeg_ad_three", CODEC_ID_PCM_U16LE},
  {"pcm_u16be", "ffmpeg_ad_three", CODEC_ID_PCM_U16BE},
  {"pcm_s8", "ffmpeg_ad_three", CODEC_ID_PCM_S8},
  {"pcm_u8", "ffmpeg_ad_three", CODEC_ID_PCM_U8},
  {"pcm_alaw", "ffmpeg_ad_three", CODEC_ID_PCM_ALAW},
  {"pcm_mulaw", "ffmpeg_ad_three", CODEC_ID_PCM_MULAW},
  {"pcm_zork", "ffmpeg_ad_three", CODEC_ID_PCM_ZORK},
  /* dpcm */
  {"roq_dpcm", "ffmpeg_ad_three", CODEC_ID_ROQ_DPCM},
  {"interplay_dpcm", "ffmpeg_ad_three", CODEC_ID_INTERPLAY_DPCM},
  {"xan_dpcm", "ffmpeg_ad_three", CODEC_ID_XAN_DPCM},
  {"sol_dpcm", "ffmpeg_ad_three", CODEC_ID_SOL_DPCM},
  /* adpcm */
  {"g726", "ffmpeg_ad_three", CODEC_ID_ADPCM_G726},
  {"adpcm_adx", "ffmpeg_ad_three", CODEC_ID_ADPCM_ADX},
  {"adpcm_4xm", "ffmpeg_ad_three", CODEC_ID_ADPCM_4XM},
  {"adpcm_ct", "ffmpeg_ad_three", CODEC_ID_ADPCM_CT},
  {"adpcm_ea", "ffmpeg_ad_three", CODEC_ID_ADPCM_EA},
  {"adpcm_ima_amv", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_AMV},
  {"adpcm_ima_dk3", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_DK3},
  {"adpcm_ima_dk4", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_DK4},
  {"adpcm_ima_qt", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_QT},
  {"adpcm_ima_smjpeg", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_SMJPEG},
  {"adpcm_ima_wav", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_WAV},
  {"adpcm_ima_ws", "ffmpeg_ad_three", CODEC_ID_ADPCM_IMA_WS},
  {"adpcm_ms", "ffmpeg_ad_three", CODEC_ID_ADPCM_MS},
  {"adpcm_sbpro_2", "ffmpeg_ad_three", CODEC_ID_ADPCM_SBPRO_2},
  {"adpcm_sbpro_3", "ffmpeg_ad_three", CODEC_ID_ADPCM_SBPRO_3},
  {"adpcm_sbpro_4", "ffmpeg_ad_three", CODEC_ID_ADPCM_SBPRO_4},
  {"adpcm_swf", "ffmpeg_ad_three", CODEC_ID_ADPCM_SWF},
  {"adpcm_thp", "ffmpeg_ad_three", CODEC_ID_ADPCM_THP},
  {"adpcm_xa", "ffmpeg_ad_three", CODEC_ID_ADPCM_XA},
  {"adpcm_yamaha", "ffmpeg_ad_three", CODEC_ID_ADPCM_YAMAHA},

  {"mpc sv7", "ffmpeg_ad_four", CODEC_ID_MUSEPACK7},
  {"qdm2", "ffmpeg_ad_four", CODEC_ID_QDM2},
  {"truespeech", "ffmpeg_ad_four", CODEC_ID_TRUESPEECH},
  {"tta", "ffmpeg_ad_four", CODEC_ID_TTA},
  {"wavpack", "ffmpeg_ad_four", CODEC_ID_WAVPACK},
  {"imc", "ffmpeg_ad_four", CODEC_ID_IMC},

  {"ac3", "ffmpeg_ad_five", CODEC_ID_AC3},
  {"vorbis", "ffmpeg_ad_five", CODEC_ID_VORBIS},

  {"libamr_nb", "ffmpeg_ad_six", CODEC_ID_AMR_NB},
  {"atrac 3", "ffmpeg_ad_six", CODEC_ID_ATRAC3},

  /* end */
  {"", "", 0},
};

