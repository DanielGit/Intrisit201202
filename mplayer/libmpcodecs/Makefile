# LINUX Makefile made by A'rpi / Astral
# Some cleanup by LGB: 	* 'make -C dir' instead of 'cd dir;make;cd..'
#			* for loops instead of linear sequence of make directories
#			* some minor problems with make clean and distclean were corrected
#			* DVD support

include ../configure.mak

ifeq ($(JZ47_LINUX),1)
ifeq ($(HAVE_ANDROID_OS),1)
include ../config.mak
include Makefile.android
else
include Makefile.linux
endif
else
include Makefile.minios
endif

