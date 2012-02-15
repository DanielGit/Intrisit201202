include ../configure.mak
include ../config.mak
LIBNAME_COMMON = liba52.a
OPTFLAGS += -DLIBA52_FIXED -DA52_STEREO3
ASFLAGS += -DLIBA52_FIXED -DA52_STEREO3
SRCS_COMMON = resample.c \
							crc.c \
              cpu_accel.c \
              bit_allocate.c \
              bitstream.c \
              downmix.c \
              imdct.c \
              parse.c \
							cpu_state.c \
							
					
							
include ../mpcommon.mak
flags:
	echo $(CFLAGS)
test: test.c ../osdep/libosdep.a $(LIBNAME_COMMON)
	$(CC) $(CFLAGS) -o $@ $^ -lm
