
include ../configure.mak
include ../config.mak

LIBNAME_COMMON = libmplugin.a

SRCS_COMMON := av_codec_table.c     \
\
               ad_libmad_start.c    \
               ad_realaac_start.c   \
               ad_faad_start.c      \
               ad_msgsm_start.c     \
               ad_hwac3_start.c     \
               ad_liba52_start.c    \
               ffmpeg_one_start.c   \
	       ffmpeg_two_start.c   \
	       ffmpeg_three_start.c \
	       ffmpeg_four_start.c  \
	       ffmpeg_five_start.c  \
	       ffmpeg_six_start.c   \
\
               vd_realvid_start.c        \
               vd_xvid_start.c        \
               vd_libmpeg2_start.c       \
               ffmpeg_h264_start.c       \
               ffmpeg_mpeg4_start.c      \
               ffmpeg_mpegvideo_start.c  \
               ffmpeg_mpegmisc_start.c   \
               ffmpeg_mpegmisc2_start.c  \

include ../mpcommon.mak

clean::
	rm -fv *.a *.o
