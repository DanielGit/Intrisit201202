# LINUX Makefile made by A'rpi / Astral
# Some cleanup by LGB: 	* 'make -C dir' instead of 'cd dir;make;cd..'
#			* for loops instead of linear sequence of make directories
#			* some minor problems with make clean and distclean were corrected
#			* DVD support

include ../configure.mak

ifeq ($(JZ47_LINUX),1)
include Makefile.linux
else
include Makefile.minios
endif

