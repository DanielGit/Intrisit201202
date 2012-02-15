
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

#include "audio_in.h"
#include "mp_msg.h"
#include "help_mp.h"
#ifdef __LINUX__
#include <string.h>
#else
#include <mplaylib.h>
#endif
#include <errno.h>

// sanitizes ai structure before calling other functions
int audio_in_init(audio_in_t *ai, int type)
{
    ai->type = type;
    ai->setup = 0;

    ai->channels = -1;
    ai->samplerate = -1;
    ai->blocksize = -1;
    ai->bytes_per_sample = -1;
    ai->samplesize = -1;

    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	ai->alsa.handle = NULL;
	ai->alsa.log = NULL;
	ai->alsa.device = strdup("default");
	return 0;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	ai->oss.audio_fd = -1;
	ai->oss.device = strdup("/dev/dsp");
	return 0;
#endif
    default:
	return -1;
    }
}

int audio_in_setup(audio_in_t *ai)
{
    
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	if (ai_alsa_init(ai) < 0) return -1;
	ai->setup = 1;
	return 0;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	if (ai_oss_init(ai) < 0) return -1;
	ai->setup = 1;
	return 0;
#endif
    default:
	return -1;
    }
}

int audio_in_set_samplerate(audio_in_t *ai, int rate)
{
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	ai->req_samplerate = rate;
	if (!ai->setup) return 0;
	if (ai_alsa_setup(ai) < 0) return -1;
	return ai->samplerate;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	ai->req_samplerate = rate;
	if (!ai->setup) return 0;
	if (ai_oss_set_samplerate(ai) < 0) return -1;
	return ai->samplerate;
#endif
    default:
	return -1;
    }
}

int audio_in_set_channels(audio_in_t *ai, int channels)
{
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	ai->req_channels = channels;
	if (!ai->setup) return 0;
	if (ai_alsa_setup(ai) < 0) return -1;
	return ai->channels;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	ai->req_channels = channels;
	if (!ai->setup) return 0;
	if (ai_oss_set_channels(ai) < 0) return -1;
	return ai->channels;
#endif
    default:
	return -1;
    }
}

int audio_in_set_device(audio_in_t *ai, char *device)
{
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    int i;
#endif
    if (ai->setup) return -1;
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	if (ai->alsa.device) free(ai->alsa.device);
	ai->alsa.device = strdup(device);
	/* mplayer cannot handle colons in arguments */
	for (i = 0; i < (int)strlen(ai->alsa.device); i++) {
	    if (ai->alsa.device[i] == '.') ai->alsa.device[i] = ':';
	}
	return 0;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	if (ai->oss.device) free(ai->oss.device);
	ai->oss.device = strdup(device);
	return 0;
#endif
    default:
	return -1;
    }
}

int audio_in_uninit(audio_in_t *ai)
{
    if (ai->setup) {
	switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
	case AUDIO_IN_ALSA:
	    if (ai->alsa.log)
		snd_output_close(ai->alsa.log);
	    if (ai->alsa.handle) {
		snd_pcm_close(ai->alsa.handle);
	    }
	    ai->setup = 0;
	    return 0;
#endif
#ifdef USE_OSS_AUDIO
	case AUDIO_IN_OSS:
	    close(ai->oss.audio_fd);
	    ai->setup = 0;
	    return 0;
#endif
	}
    }
    return -1;
}

int audio_in_start_capture(audio_in_t *ai)
{
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	return snd_pcm_start(ai->alsa.handle);
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	return 0;
#endif
    default:
	return -1;
    }
}

int audio_in_read_chunk(audio_in_t *ai, unsigned char *buffer)
{
    int ret;
    
    switch (ai->type) {
#if defined(HAVE_ALSA9) || defined(HAVE_ALSA1X)
    case AUDIO_IN_ALSA:
	ret = snd_pcm_readi(ai->alsa.handle, buffer, ai->alsa.chunk_size);
	if (ret != ai->alsa.chunk_size) {
	    if (ret < 0) {
		mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_ErrReadingAudio, snd_strerror(ret));
		if (ret == -EPIPE) {
		    if (ai_alsa_xrun(ai) == 0) {
			mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_XRUNSomeFramesMayBeLeftOut);
		    } else {
			mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_ErrFatalCannotRecover);
		    }
		}
	    } else {
		mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_NotEnoughSamples);
	    }
	    return -1;
	}
	return ret;
#endif
#ifdef USE_OSS_AUDIO
    case AUDIO_IN_OSS:
	ret = read(ai->oss.audio_fd, buffer, ai->blocksize);
	if (ret != ai->blocksize) {
	    if (ret < 0) {
		mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_ErrReadingAudio, strerror(errno));
	    } else {
		mp_msg(MSGT_TV, MSGL_ERR, MSGTR_MPDEMUX_AUDIOIN_NotEnoughSamples);
	    }
	    return -1;
	}
	return ret;
#endif
    default:
	return -1;
    }
}
