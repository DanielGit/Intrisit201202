include ../configure.mak
include ../config.mak

LIBNAME_MPLAYER = libao2.a

SRCS_MPLAYER = audio_out.c \
               ao_mpegpes.c \
               ao_null.c \

ifeq ($(JZ47_LINUX),1)
SRCS_MPLAYER += ao_pcm.c $(AO_SRCS) 
else ifeq ($(JZ4750_BSP),1)
SRCS_MPLAYER += ao_pcm_noahos.c ao_pcm.c
else ifeq ($(JZ4755_BSP),1)
SRCS_MPLAYER += ao_pcm_noahos.c ao_pcm.c
else ifeq ($(JZ4725_BSP),1)
SRCS_MPLAYER += ao_pcm_noahos.c ao_pcm.c
#SRCS_MPLAYER += ao_pcm_minios_jz4750.c
else
SRCS_MPLAYER += ao_pcm_minios.c ao_jz4740_vol.c
endif
include ../mpcommon.mak

