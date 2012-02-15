#============================= Necessary Input  parameters
# dir:		T_FILE_DIR	I_TEST_DIR	CHIP_DIR	CCROOT
#		ELF_DIR		GCC_EXEC_PREFIX
# file:		MACRO_FILE
#
#============================= Optional Input parameters
# macro:	EXTRA_OBJ (for multi c file test like power test)
#		GCC_EXTRA_OPT	LD_EXTRA_OPT	MC_LIB_EXTRA	GCC_LIB_EXTRA
#		ENDIAN		GCC_OPTIMIZE_OPT

include jzsys.mak
#============================= Path define
DESTDIR 	=
#============================= Tools define
PREFIX 		= $(DESTDIR)/usr/local
BINDIR 		= $(DESTDIR)/usr/local/bin
LIBDIR 		= $(DESTDIR)/usr/local/lib

AR 		= ar
CC 		= mipsel-linux-gcc -mips32
CXX 		= mipsel-linux-gcc -mips32
MAKE		= make
COPY		= mipsel-linux-objcopy

XVID_LIB  	= src/libxvidcore.a

TEST_OBJ	= xvid_decraw.o
JZSYS_OBJ	= jz47_soc.o jz47_vae_map.o

REAL_EXE	= xvid_decraw
AUX_BIN		= aux_xvid.bin

GCC_ENDIAN_OPT	= -mel
LD_ENDIAN_OPT	= -EL
ABI_DIR		= el/mabi32

GCC_EXTRA_OPT   = -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -O2 -pipe -ffast-math -fomit-frame-pointer -DIPU_THROUGHT_MODE

#============================= Internal macro define 
# des changed to have 3 keys
JZC_MACRO	= src/jzsoc/jzmacro.h

XVID_INC_OPT	= -Isrc -I/usr/X11R6/include \
		  -I/opt/motif21/include 
GCC_OPT		= $(XVID_INC_OPT) $(ARCHITECTURE) $(BUS) $(ENDIANNESS) \
		  $(GCC_ENDIAN_OPT) -imacros $(JZC_MACRO) $(GCC_EXTRA_OPT)

#============================= Make action define
all: $(REAL_EXE) #$(AUX_BIN)

$(JZSYS_OBJ): %.o: %.c
	@echo "Information: compile $@"
	$(CC) $(GCC_OPT) -c -o 	$@ 	$<

$(XVID_LIB)::  
	$(MAKE) -C src

$(TEST_OBJ): %.o: %.c $(JZC_MACRO)
	@echo "Information: compile $@"
	$(CC) $(GCC_OPT) -c -o 	$@ 	$<



$(REAL_EXE): $(TEST_OBJ) $(XVID_LIB) $(JZSYS_OBJ) 
	$(CC) -Xlinker -T -Xlinker linux_xvid.ld  -L$(LIBDIR) -lc -lm -o $@ $^ 

$(AUX_BIN)::
	$(MAKE) -C src/jzsoc
	cp -f src/jzsoc/aux_xvid.bin .
	@echo "Information: aux bin generated successfully!"

rmaux::
	$(MAKE) -C src/jzsoc clean
	rm -f $(AUX_BIN)

clean::
	$(MAKE) -C src clean
	rm -f *.o *.a $(TEST_OBJ) $(TEST_LIB) $(REAL_EXE)

.PHONY: all rmaux clean
