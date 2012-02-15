#
# common bits used by all libraries
#

VPATH = $(SRC_PATH_BARE)/lib$(NAME)
SRC_DIR = "$(VPATH)"

CFLAGS   += $(CFLAGS-yes)
OBJS     += $(OBJS-yes)
ASM_OBJS += $(ASM_OBJS-yes)
CPP_OBJS += $(CPP_OBJS-yes)

OBJS2     += $(OBJS2-yes)
ASM_OBJS2 += $(ASM_OBJS2-yes)
CPP_OBJS2 += $(CPP_OBJS2-yes)

CFLAGS += -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
          -D_ISOC9X_SOURCE -I$(BUILD_ROOT) -I$(SRC_PATH) \
          -I$(SRC_PATH)/libavutil $(OPTFLAGS)

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.S) $(CPPOBJS:.o=.cpp)
OBJS := $(OBJS) $(ASM_OBJS) $(CPPOBJS)
OBJS2 := $(OBJS2) $(ASM_OBJS2) $(CPPOBJS2)


STATIC_OBJS := $(OBJS) $(STATIC_OBJS)
SHARED_OBJS := $(OBJS) $(SHARED_OBJS)
STATIC_OBJS2 := $(OBJS2)

# Remove the duplicate obj, --Wolfgang, 2009-07-27
STATIC_OBJS :=$(sort $(STATIC_OBJS))
SHARED_OBJS :=$(sort $(SHARED_OBJS))
STATIC_OBJS2 :=$(sort $(STATIC_OBJS2))

MIPS_STATIC_OBJS := $(STATIC_OBJS:.o=.s)
ASMS_STATIC_OBJS := $(STATIC_OBJS:.o=.mid)

MIPS_STATIC_OBJS += $(STATIC_OBJS2:.o=.s)
ASMS_STATIC_OBJS += $(STATIC_OBJS2:.o=.mid)


all: $(EXTRADEPS) $(LIB) $(SLIBNAME) $(LIB2)

$(LIB): $(STATIC_OBJS) $(EXTRAOBJS)
	rm -f $@
	$(AR) rc $@ $^ $(EXTRAOBJS)
	$(RANLIB) $@

$(LIB2): $(STATIC_OBJS2) $(EXTRAOBJS2)
	rm -f $@
	$(AR) rc $@ $^ $(EXTRAOBJS2)
	$(RANLIB) $@

$(SLIBNAME): $(SLIBNAME_WITH_MAJOR)
	ln -sf $^ $@

$(SLIBNAME_WITH_MAJOR): $(SHARED_OBJS)
	$(CC) $(SHFLAGS) $(LDFLAGS) -o $@ $^ $(EXTRALIBS) $(EXTRAOBJS)
	$(SLIB_EXTRA_CMD)

###############################################################################
#
$(STATIC_OBJS) $(STATIC_OBJS2):%.o:%.s
	$(CC) $(CFLAGS) $(LIBOBJFLAGS) -c -o $@ $<
$(MIPS_STATIC_OBJS):%.s:%.mid
	mxu_as $< > $@
$(ASMS_STATIC_OBJS):%.mid:%.c
	$(CC) $(CFLAGS) $(LIBOBJFLAGS) -S -o $@ $<
%.o: %.S
	$(CC) $(CFLAGS) $(LIBOBJFLAGS) -c -o $@ $<
ls:
	echo $(COBJS) $(STATIC_OBJS)
################################################################################


%: %.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ $(EXTRALIBS)

depend dep: $(SRCS)
	$(CC) -MM $(CFLAGS) $^ 1>.depend

clean::
	rm -f *.o *~ *.a *.lib *.so *.so.* *.dylib *.dll \
	      *.def *.dll.a *.exp *.mid *.s

distclean: clean
	rm -f .depend

ifeq ($(BUILD_SHARED),yes)
INSTLIBTARGETS += install-lib-shared
endif
ifeq ($(BUILD_STATIC),yes)
INSTLIBTARGETS += install-lib-static
endif

install: install-libs install-headers

install-libs: $(INSTLIBTARGETS)

install-lib-shared: $(SLIBNAME)
	install -d "$(shlibdir)"
	install -m 755 $(SLIBNAME) "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	$(STRIP) "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	cd "$(shlibdir)" && \
		ln -sf $(SLIBNAME_WITH_VERSION) $(SLIBNAME_WITH_MAJOR)
	cd "$(shlibdir)" && \
		ln -sf $(SLIBNAME_WITH_VERSION) $(SLIBNAME)
	$(SLIB_INSTALL_EXTRA_CMD)

install-lib-static: $(LIB)
	install -d "$(libdir)"
	install -m 644 $(LIB) "$(libdir)"
	$(LIB_INSTALL_EXTRA_CMD)

install-headers:
	install -d "$(incdir)"
	install -d "$(libdir)/pkgconfig"
	install -m 644 $(addprefix $(SRC_DIR)/,$(HEADERS)) "$(incdir)"
	install -m 644 $(BUILD_ROOT)/lib$(NAME).pc "$(libdir)/pkgconfig"

uninstall: uninstall-libs uninstall-headers

uninstall-libs:
	-rm -f "$(shlibdir)/$(SLIBNAME_WITH_MAJOR)" \
	       "$(shlibdir)/$(SLIBNAME)"            \
	       "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	-rm -f "$(libdir)/$(LIB)"

uninstall-headers::
	rm -f $(addprefix "$(incdir)/",$(HEADERS))
	rm -f "$(libdir)/pkgconfig/lib$(NAME).pc"

.PHONY: all depend dep clean distclean install* uninstall*

-include .depend
