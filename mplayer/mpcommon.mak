SRCS_COMMON          += $(SRCS_COMMON-yes)
SRCS_MPLAYER         += $(SRCS_MPLAYER-yes)
SRCS_MENCODER        += $(SRCS_MENCODER-yes)

SRCS2_COMMON          += $(SRCS2_COMMON-yes)

OBJS_COMMON    += $(addsuffix .o, $(basename $(SRCS_COMMON)) )
OBJSCPP_COMMON    += $(addsuffix .o, $(basename $(CPPSRCS_COMMON)) )
OBJS_MPLAYER   += $(addsuffix .o, $(basename $(SRCS_MPLAYER)) )
OBJS_MENCODER  += $(addsuffix .o, $(basename $(SRCS_MENCODER)) )

OBJS2_COMMON    += $(addsuffix .o, $(basename $(SRCS2_COMMON)) )

CFLAGS-$(LIBAVCODEC)     += -I../libavcodec
CFLAGS-$(LIBAVFORMAT)    += -I../libavformat
CFLAGS += $(CFLAGS-yes) $(OPTFLAGS)

LIBS-$(MPLAYER)  += $(LIBNAME_MPLAYER)
LIBS-$(MENCODER) += $(LIBNAME_MENCODER)
LIBS              = $(LIBNAME_COMMON) $(LIBS-yes) $(LIBNAME2_COMMON)

libs: $(LIBS)

$(LIBNAME_COMMON):   $(OBJS_COMMON)
$(LIBNAME2_COMMON):   $(OBJS2_COMMON)
$(LIBNAME_MPLAYER):  $(OBJS_MPLAYER)
$(LIBNAME_MENCODER): $(OBJS_MENCODER)
$(LIBNAME_COMMON) $(LIBNAME_MPLAYER) $(LIBNAME_MENCODER) $(LIBNAME2_COMMON):
	$(AR) r $@ $^
	$(RANLIB) $@

###############################################################################

MIDS_MPLAYER_COMMON := $(OBJS_COMMON:.o=.s)
ASMS_MPLAYER_COMMON := $(OBJS_COMMON:.o=.mid)
MIDS_MPLAYER_COMMON += $(OBJS2_COMMON:.o=.s)
ASMS_MPLAYER_COMMON += $(OBJS2_COMMON:.o=.mid)

$(OBJS_COMMON) $(OBJS2_COMMON):%.o:%.s
	$(CC) $(ASFLAGS) -c -o $@ $<
$(MIDS_MPLAYER_COMMON):%.s:%.mid
	mxu_as $< > $@
$(ASMS_MPLAYER_COMMON):%.mid:%.c 
	$(CC) $(CFLAGS) -S -o $@ $<

###############################################################################

CPPMIDS_MPLAYER_COMMON = $(OBJSCPP_COMMON:.o=.s)
CPPASMS_MPLAYER_COMMON = $(OBJSCPP_COMMON:.o=.mid)

$(OBJSCPP_COMMON):%.o:%.s
	$(CC) $(ASFLAGS) -c -o $@ $<
$(CPPMIDS_MPLAYER_COMMON):%.s:%.mid
	mxu_as $< > $@
$(CPPASMS_MPLAYER_COMMON):%.mid:%.cpp 
	$(CXX) $(CXXFLAGS) $(CFLAGS) -S -o $@ $<

###############################################################################

###############################################################################


MIDS_MPLAYER = $(OBJS_MPLAYER:.o=.s)
ASMS_MPLAYER = $(OBJS_MPLAYER:.o=.mid)

$(OBJS_MPLAYER):%.o:%.s
	$(CC) $(ASFLAGS) -c -o $@ $<
$(MIDS_MPLAYER):%.s:%.mid
	mxu_as $< > $@

$(ASMS_MPLAYER):%.mid:%.c 
	$(CC) $(CFLAGS) -S -o $@ $<

###############################################################################

###############################################################################


MIDS_MENCODER = $(OBJS_MENCODER:.o=.s)
ASMS_MENCODER = $(OBJS_MENCODER:.o=.mid)

$(OBJS_MENCODER):%.o:%.s
	$(CC) $(ASFLAGS) -c -o $@ $<
$(MIDS_MENCODER):%.s:%.mid
	mxu_as $< > $@
$(ASMS_MENCODER):%.mid:%.c 
	$(CC) $(CFLAGS) -S -o $@ $<

###############################################################################


clean::
	 rm -f *.o *.a *~ $(MIDS_MPLAYER) $(ASMS_MPLAYER) $(MIDS_MPLAYER_COMMON) $(ASMS_MPLAYER_COMMON)
	 rm -f $(MIDS_MENCODER) $(ASMS_MENCODER)
	 rm -f $(OBJS_MPLAYER) $(OBJS_COMMON) $(OBJS_MENCODER)
	 rm -f $(CPPMIDS_MPLAYER_COMMON) $(CPPASMS_MPLAYER_COMMON)

distclean:: clean
	rm -f .depend test test2

dep depend::
	$(CC) -MM $(CFLAGS) $(OPTFLAGS) $(SRCS_COMMON) $(SRCS_MPLAYER) $(SRCS_MENCODER) 1>.depend

-include .depend

.PHONY: libs clean distclean dep depend
