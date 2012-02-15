
include ../configure.mak
include ../config.mak

# Overload incdir, postproc include files go in a different directory.
INCDIR=$(prefix)/include/postproc

EXTRALIBS := -L$(BUILD_ROOT)/libavutil -lavutil$(BUILDSUF) $(EXTRALIBS)

NAME=postproc
LIBVERSION=$(SPPVERSION)
LIBMAJOR=$(SPPMAJOR)

ifeq ($(JZ47_LINUX),1)
STATIC_OBJS=postprocess.o
SHARED_OBJS=postprocess_pic.o
else
OBJS=postprocess.o
endif

HEADERS = postprocess.h

include ../common.mak

ifeq ($(JZ47_LINUX),1)
depend dep: postprocess.c

postprocess_pic.o: postprocess.c
	$(CC) -c $(CFLAGS) -fomit-frame-pointer -fPIC -DPIC -o $@ $<

uninstall-headers::
	-rmdir "$(prefix)/include/postproc"
endif
