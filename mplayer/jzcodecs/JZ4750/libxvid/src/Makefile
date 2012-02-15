#============================= Necessary Input  parameters
# dir:		T_FILE_DIR	I_TEST_DIR	CHIP_DIR	CCROOT
#		ELF_DIR		GCC_EXEC_PREFIX
# file:		MACRO_FILE
#
#============================= Optional Input parameters
# macro:	EXTRA_OBJ (for multi c file test like power test)
#		GCC_EXTRA_OPT	LD_EXTRA_OPT	MC_LIB_EXTRA	GCC_LIB_EXTRA
#		ENDIAN		GCC_OPTIMIZE_OPT

#============================= Path define
DESTDIR 	=
include ../jzsys.mak
#============================= Tools define
PREFIX 		= $(DESTDIR)/usr/local
BINDIR 		= $(DESTDIR)/usr/local/bin
LIBDIR 		= $(DESTDIR)/usr/local/lib

AR 		= ar
CC 		= mipsel-linux-gcc -mips32
CXX 		= mipsel-linux-gcc -mips32
AS		= mipsel-linux-as -mips32
RANLIB		= true

XVID_OBJ	= xvid_dec.o \
		  bitstream/bitstream.o \
	          bitstream/cbp.o \
	          bitstream/mbcoding.o \
	          quant/quant_matrix.o \
	          quant/dequant_h263.o \
	          quant/dequant_mpeg.o \
	          dct/idct.o \
	          image/colorspace.o \
	          image/postprocessing.o \
	          image/interpolate8x8.o \
	          image/qpel.o \
	          motion/gmc.o \
	          prediction/mbprediction.o \
	          utils/mem_align.o \
	          utils/mem_transfer.o \
	          utils/emms.o \
	          utils/timer.o 

#  XVID_OBJ2 is related with jzmacro.h
XVID_OBJ2	= decoder.o \
	          image/image_dec.o

XVID_LIB	= libxvidcore.a

GCC_ENDIAN_OPT	= -mel
LD_ENDIAN_OPT	= -EL
ABI_DIR		= el/mabi32

GCC_EXTRA_OPT   = -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -O4 -pipe -ffast-math -fomit-frame-pointer -DIPU_THROUGHT_MODE

#============================= Internal macro define 
JZC_MACRO	= jzsoc/jzmacro.h

# des changed to have 3 keys
XVID_INC_OPT	= -Ibitstream -Iquant -Idct -Iimage -Imotion -Iprediction -Iutils \
	          -I/usr/X11R6/include -I/opt/motif21/include -Ijzsoc

GCC_OPT		= $(XVID_INC_OPT) $(ARCHITECTURE) $(BUS) $(ENDIANNESS) \
		  $(GCC_ENDIAN_OPT) -imacros $(JZC_MACRO) $(GCC_EXTRA_OPT)

#============================= Make action define
libs: $(XVID_LIB)

$(XVID_OBJ): %.o: %.c
	@echo "Information: compile $@"
	$(CC) $(GCC_OPT) -c -o 	$@ 	$<

$(XVID_OBJ2): %.o: %.c $(JZC_MACRO)
	@echo "Information: compile $@"
	$(CC) $(GCC_OPT) -c -o 	$@ 	$<

$(XVID_LIB):$(XVID_OBJ) $(XVID_OBJ2) $(MXU_OBJ)
	$(AR) r $(XVID_LIB) $(MXU_OBJ) $(XVID_OBJ) $(XVID_OBJ2)
	$(RANLIB) $(XVID_LIB)
quant/dequant_h263.o:quant/dequant_h263.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as dequant_h263.s > dequant_h263_mxu.s
	$(AS) -o $@ dequant_h263_mxu.s
quant/dequant_mpeg.o:quant/dequant_mpeg.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as dequant_mpeg.s > dequant_mpeg_mxu.s
	$(AS) -o $@ dequant_mpeg_mxu.s
dct/idct.o:dct/idct.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as idct.s > idct_mxu.s
	$(AS) -o $@ idct_mxu.s
prediction/mbprediction.o:prediction/mbprediction.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as mbprediction.s > mbprediction_mxu.s
	$(AS) -o $@ mbprediction_mxu.s

utils/mem_transfer.o:utils/mem_transfer.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as mem_transfer.s > mem_transfer_mxu.s
	$(AS) -o $@ mem_transfer_mxu.s
decoder.o:decoder.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as decoder.s > decoder_mxu.s
	$(AS) -o $@ decoder_mxu.s
bitstream/mbcoding.o:bitstream/mbcoding.c
	$(CC) $(GCC_OPT) -S $<
	mxu_as mbcoding.s > mbcoding_mxu.s
	$(AS) -o $@ mbcoding_mxu.s
dep depend::

clean::
	rm -f *.o *.a *~ *.s 
	rm -f bitstream/*.o quant/*.o dct/*.o image/*.o prediction/*.o motion/*.o utils/*.o
	rm -f bitstream/*.s quant/*.s dct/*.s image/*.s prediction/*.s motion/*.s utils/*.s

.PHONY: libs clean dep depend
