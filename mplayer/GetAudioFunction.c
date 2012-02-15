#if 0
enum metadata_s {
  /* common info */
  META_NAME = 0,

  /* video stream properties */
  META_VIDEO_CODEC,
  META_VIDEO_BITRATE,
  META_VIDEO_RESOLUTION,

  /* audio stream properties */
  META_AUDIO_CODEC,
  META_AUDIO_BITRATE,
  META_AUDIO_SAMPLES,

  /* ID3 tags and other stream infos */
  META_INFO_TITLE,
  META_INFO_ARTIST,
  META_INFO_ALBUM,
  META_INFO_YEAR,
  META_INFO_COMMENT,
  META_INFO_TRACK,
  META_INFO_GENRE
};
#endif
typedef struct{
	unsigned int channels;
	unsigned int rate;
	unsigned int format;
	unsigned int outburst;
}audio_info_t;

static int is_valid_metadata_type (metadata_t type) {
  switch (type)
  {
  /* check for valid video stream */
  case META_VIDEO_CODEC:
  case META_VIDEO_BITRATE:
  case META_VIDEO_RESOLUTION:
  {
    if (!mpctx->sh_video)
      return 0;
    break;
  }

  /* check for valid audio stream */
  case META_AUDIO_CODEC:
  case META_AUDIO_BITRATE:
  case META_AUDIO_SAMPLES:
  {
    if (!mpctx->sh_audio)
      return 0;
    break;
  }

  /* check for valid demuxer */
  case META_INFO_TITLE:
  case META_INFO_ARTIST:
  case META_INFO_ALBUM:
  case META_INFO_YEAR:
  case META_INFO_COMMENT:
  case META_INFO_TRACK:
  case META_INFO_GENRE:
  {
    if (!mpctx->demuxer)
      return 0;
    break;
  }

  default:
    break;
  }

  return 1;
}
static char *get_demuxer_info (char *tag) {
  char **info = mpctx->demuxer->info;
  int n;

  if (!info || !tag)
    return NULL;

  for (n = 0; info[2*n] != NULL ; n++)
    if (!strcmp (info[2*n], tag))
      break;

  return info[2*n+1] ? strdup (info[2*n+1]) : NULL;
}

void mplayer_get_audioinfo(audio_info_t *info)
{
	sh_audio_t * const sh_audio = mpctx->sh_audio;
  info->rate = sh_audio->samplerate;
  info->channels = sh_audio->channels;
  info->outburst = ao_data.outburst;
  
}
void mplayer_freemetadata(char *data)
{
	free(data);
}
int sprintf(char *__restrict str, __const char *__restrict fmt, ...);

char *mplayer_get_metadata (metadata_t type) {
  char *meta = NULL;
  sh_audio_t * const sh_audio = mpctx->sh_audio;
    if (!is_valid_metadata_type (type))
    return NULL;
  
  switch (type)
  {
  case META_NAME:
  {
    return strdup (mp_basename2 (filename));
  }
  case META_AUDIO_CODEC:
  {
    if (sh_audio->codec && sh_audio->codec->name)
      meta = strdup (sh_audio->codec->name);
    return meta;
  }
  
  case META_AUDIO_BITRATE:
  {
    meta = malloc (16);
    sprintf (meta, "%d kbps", (int) (sh_audio->i_bps * 8/1000));
    return meta;
  }
  
  case META_AUDIO_SAMPLES:
  {
    meta = malloc (16);
    sprintf (meta, "%d Hz, %d ch.", sh_audio->samplerate, sh_audio->channels);
    return meta;
  }

  /* check for valid demuxer */
  case META_INFO_TITLE:
    return get_demuxer_info ("Title"); //主题
  
  case META_INFO_ARTIST:
    return get_demuxer_info ("Artist");//艺术家

  case META_INFO_ALBUM:
    return get_demuxer_info ("Album");//签名

  case META_INFO_YEAR:
    return get_demuxer_info ("Year"); //年

  case META_INFO_COMMENT:
    return get_demuxer_info ("Comment"); //注释

  case META_INFO_TRACK:
    return get_demuxer_info ("Track"); //轨道

  case META_INFO_GENRE:
    return get_demuxer_info ("Genre"); //流派

  default:
    break;
  }

  return meta;
}
int videoshow = 1;
void SetAudioPlayer()
{
	videoshow = 0;
}
void SetVideoPlayer()
{
	videoshow = 1;
}
static int total_time_ms  = 0;
void SetPlayerTime(int ttime)
{
	total_time_ms = ttime;
}
int GetPlayerTime()
{
	return total_time_ms;
}
