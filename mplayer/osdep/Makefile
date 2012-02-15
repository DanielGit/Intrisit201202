
include ../configure.mak
include ../config.mak

LIBNAME_COMMON = libosdep.a

SRCS_COMMON-$(HAVE_SYS_MMAN_H)       += mmap_anon.c
SRCS_COMMON-$(MACOSX_FINDER_SUPPORT) += macosx_finder_args.c

SRCS_COMMON-$(NEED_GETTIMEOFDAY)     += gettimeofday.c
SRCS_COMMON-$(NEED_GLOB)             += glob-win.c
SRCS_COMMON-$(NEED_SETENV)           += setenv.c
SRCS_COMMON-$(NEED_SHMEM)            += shmem.c
SRCS_COMMON-$(NEED_STRSEP)           += strsep.c
SRCS_COMMON-$(NEED_SWAB)             += swab.c
SRCS_COMMON-$(NEED_VSSCANF)          += vsscanf.c

getch = getch2.c

ifeq ($(JZ47_LINUX),1)
timer = timer-lx.c
else
timer = timer-minios.c
ifeq ($(JZ4750_BSP),1)
timer = timer_minios_jz4750.c
endif
ifeq ($(JZ4755_BSP),1)
timer = timer_minios_jz4750.c
endif
ifeq ($(JZ4725_BSP),1)
timer = timer_minios_jz4750.c
endif

endif

ifeq ($(TARGET_WIN32),yes)
timer = timer-win2.c
endif
ifeq ($(TARGET_OS),Darwin)
timer = timer-darwin.c
endif
ifeq ($(TARGET_OS),MINGW32)
getch = getch2-win.c
endif
SRCS_COMMON += $(timer)
SRCS_COMMON += $(getch)
                
include ../mpcommon.mak                  

mplayer-rc.o: mplayer.rc
	windres -o $@ $<

