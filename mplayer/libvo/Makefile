include ../configure.mak
include ../config.mak
LIBNAME_COMMON  = libosd.a
LIBNAME_MPLAYER = libvo.a

SRCS_MPLAYER = aspect.c \
               geometry.c \
               spuenc.c \
               video_out.c \
               vo_mpegpes.c \
               vo_null.c \
               vo_yuv4mpeg.c 

ifeq ($(JZ47_LINUX),1)
SRCS_MPLAYER += $(VO_SRCS)
else
SRCS_MPLAYER += vo_fbdev_minios.c
endif

SRCS_MPLAYER-$(VIDIX)          +=  vosub_vidix.c

SRCS_COMMON = aclib.c \
              osd.c \
              sub.c \

SRCS_COMMON-$(BITMAP_FONT) +=  font_load.c
SRCS_COMMON-$(FREETYPE)    +=  font_load_ft.c

include ../mpcommon.mak

%.o: %.m
	$(CC) $(CFLAGS) -c -o $@ $<

vo_png.o: CFLAGS := $(filter-out -I../libavcodec,$(CFLAGS))

