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
H264_ROOT 	= ../../../
#ICSYS_DIR       = /icdev/toolchain/icsys/bin/
ICSYS_DIR       = ~/mplayer_linux/icsys/bin/
#============================= Tools define

CC 		= $(ICSYS_DIR)./mipsisa32-gcc-3.3
LD		= $(ICSYS_DIR)./mipsisa32-ld
COPY		= $(ICSYS_DIR)./mipsisa32-objcopy

AUX_SRC         = h264_aux_task.c
AUX_BIN		= h264_aux_task.bin
AUX_LIB		= h264_aux_task.lib
AUX_OBJ		= h264_aux_task.o
AUX_MID		= $(AUX_OBJ:.o=.mid.s)
AUX_ASM		= $(AUX_OBJ:.o=.s)

GCC_ENDIAN_OPT	= -mel
LD_ENDIAN_OPT	= -EL
ABI_DIR		= el/mabi32

GCC_EXTRA_OPT   = -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -O2 -pipe -ffast-math -fomit-frame-pointer

#============================= Internal macro define 
JZC_MACRO	= h264_jzmacro.h

# des changed to have 3 keys
H264_INC_OPT	= -I.. -I/usr/X11R6/include -I/opt/motif21/include 

GCC_OPT		= $(H264_INC_OPT) \
		  $(GCC_ENDIAN_OPT) -imacros $(JZC_MACRO) $(GCC_EXTRA_OPT)

#============================= Make action define
libs: $(AUX_BIN)

$(AUX_OBJ):%.o:%.mid.s
	$(CC) $(GCC_OPT) -c -o 	$@ 	$<
$(AUX_MID):%.mid.s:%.s
	source h264_aux_gp $< >| tmp
	mxu_as tmp >| $@
	rm -f tmp
$(AUX_ASM):%.s:%.c
	$(CC) $(GCC_OPT) -S -o 	$@ 	$<

$(AUX_LIB): $(AUX_OBJ) 
	$(LD) -T h264_aux.ld -EL -nostdlib -o $@ $^
$(AUX_BIN): $(AUX_LIB)
	$(COPY) -O binary $< 	$@

dep depend::

clean::
	rm -f *.o *~ *.s *.bin *.lib

.PHONY: libs clean dep depend 
