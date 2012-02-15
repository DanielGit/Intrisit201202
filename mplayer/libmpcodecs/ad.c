/*
   ad.c - audio decoder interface
*/

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <uclib.h>
#include <uclib.h>
#include <uclib.h>
#endif

#include "config.h"

#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
#include "ad.h"

/* Missed vorbis, mad, dshow */

//extern ad_functions_t mpcodecs_ad_null;
extern ad_functions_t mpcodecs_ad_mp3lib;
extern ad_functions_t mpcodecs_ad_ffmpeg;
#ifdef __LINUX__
extern ad_functions_t mpcodecs_ad_liba52;
extern ad_functions_t mpcodecs_ad_hwac3;
#endif
extern ad_functions_t mpcodecs_ad_hwmpa;
extern ad_functions_t mpcodecs_ad_pcm;
extern ad_functions_t mpcodecs_ad_dvdpcm;
extern ad_functions_t mpcodecs_ad_alaw;
extern ad_functions_t mpcodecs_ad_imaadpcm;
extern ad_functions_t mpcodecs_ad_msadpcm;
extern ad_functions_t mpcodecs_ad_dk3adpcm;
extern ad_functions_t mpcodecs_ad_dk4adpcm;
extern ad_functions_t mpcodecs_ad_dshow;
extern ad_functions_t mpcodecs_ad_dmo;
extern ad_functions_t mpcodecs_ad_acm;
#ifdef __LINUX__
extern ad_functions_t mpcodecs_ad_msgsm;
//extern ad_functions_t mpcodecs_ad_realaac;
extern ad_functions_t mpcodecs_ad_faad;
#endif
extern ad_functions_t mpcodecs_ad_libvorbis;
extern ad_functions_t mpcodecs_ad_speex;
#ifdef __LINUX__
extern ad_functions_t mpcodecs_ad_libmad;
#endif
extern ad_functions_t mpcodecs_ad_realaud;
extern ad_functions_t mpcodecs_ad_libdv;
extern ad_functions_t mpcodecs_ad_qtaudio;
extern ad_functions_t mpcodecs_ad_twin;
extern ad_functions_t mpcodecs_ad_libmusepack;
extern ad_functions_t mpcodecs_ad_libdca;

ad_functions_t* mpcodecs_ad_drivers[] =
{
//  &mpcodecs_ad_null,
#ifdef USE_MP3LIB
  &mpcodecs_ad_mp3lib,
#endif
#ifdef USE_LIBA52
#ifdef __LINUX__
  &mpcodecs_ad_liba52,
  &mpcodecs_ad_hwac3,
#endif
#endif
  &mpcodecs_ad_hwmpa,
#ifdef USE_LIBAVCODEC
  &mpcodecs_ad_ffmpeg,
#endif
  &mpcodecs_ad_pcm,
  &mpcodecs_ad_dvdpcm,
  &mpcodecs_ad_alaw,
  &mpcodecs_ad_imaadpcm,
  &mpcodecs_ad_msadpcm,
  &mpcodecs_ad_dk3adpcm,
#ifdef __LINUX__
  &mpcodecs_ad_msgsm,
#endif
#ifdef USE_WIN32DLL
  &mpcodecs_ad_dshow,
  &mpcodecs_ad_dmo,
  &mpcodecs_ad_acm,
  &mpcodecs_ad_twin,
#endif
#if defined(USE_QTX_CODECS) || defined(MACOSX)
  &mpcodecs_ad_qtaudio,
#endif
#ifdef HAVE_FAAD
#ifdef __LINUX__
//  &mpcodecs_ad_realaac,
  &mpcodecs_ad_faad,
#endif
#endif
#ifdef HAVE_OGGVORBIS
#ifndef __LINUX__
  (void *)1,
#else
  &mpcodecs_ad_libvorbis,
#endif
#endif
#ifdef HAVE_SPEEX
  &mpcodecs_ad_speex,
#endif
#ifdef USE_LIBMAD
#ifdef __LINUX__
  &mpcodecs_ad_libmad,
#endif
#endif
#ifdef USE_REALCODECS_AD
  &mpcodecs_ad_realaud,
#endif
#ifdef HAVE_LIBDV095
  &mpcodecs_ad_libdv,
#endif
#ifdef HAVE_MUSEPACK
  &mpcodecs_ad_libmusepack,
#endif
#ifdef USE_LIBDCA
  &mpcodecs_ad_libdca,
#endif
  NULL
};
