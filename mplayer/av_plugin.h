
#define VD_PLUGIN_ADDR     (0x83800000)
#define AD_PLUGIN_ADDR     (0x83a00000)
#define TABLE_LOADER_ADDR  (0x83a00000)

#define CODEC_TABLE_NAME   "av_codec_table"

#define FFMPEG_PLUGIN_TABLE  1

struct AV_PLUGIN_TBL 
{
  char *name;
  char *plugin_name;
  unsigned int codec_id;
};

extern void *load_plugin_codec_id (int codec_id, char video_plg, unsigned int arg0);
extern void *load_plugin (char *drv_name, char *dll_name, char video_plg, unsigned int arg0);

