############################################################################### 
# 				             NOAH-TECH Co,LTD
#                               2009-01-07
# 使用方法: 
#    make            编译并连接程序
#    make objs       编译目标文件，但不连接程序
#    make clean      清除目标文件和依赖文件和程序文件
#    make rebuild    等同于make clean && make all
#    make lib	     生成库文件
############################################################################### 

# 程序文件名称
TARGET  := _noahos

# 连接脚本文件
LDS     := link.xn

# 源文件目录
TOP	:= ../..
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cfgsys)
ifneq ($(MAKECMDGOALS),cfgbios)
ifneq ($(MAKECMDGOALS),cfgburn)
SRCDIRS := 

SRCDIRS += $(TOP)/Init/JZ4750

SRCDIRS += $(TOP)/Kernel/kArch
SRCDIRS += $(TOP)/Kernel/kCore
SRCDIRS += $(TOP)/Kernel/kIpc
SRCDIRS += $(TOP)/Kernel/kLib
SRCDIRS += $(TOP)/Kernel/kMem
SRCDIRS += $(TOP)/Kernel/kSync
SRCDIRS += $(TOP)/FileSys/FTLV2
SRCDIRS += $(TOP)/FileSys/FsIo
SRCDIRS += $(TOP)/FileSys/RFS
SRCDIRS += $(TOP)/FileSys/FAT
SRCDIRS += $(TOP)/FileSys/FS
SRCDIRS += $(TOP)/FileSys/MISC
SRCDIRS += $(TOP)/FileSys/DEVF
SRCDIRS += $(TOP)/FileSys/iso9660
SRCDIRS += $(TOP)/FileSys/iNAND
SRCDIRS += $(TOP)/FileSys/SDIO
SRCDIRS += $(TOP)/FileSys/Burn
SRCDIRS += $(TOP)/GUI/Win
SRCDIRS += $(TOP)/GUI/WinApi
SRCDIRS += $(TOP)/GUI/WinDrv
SRCDIRS += $(TOP)/GUI/WinGdi
SRCDIRS += $(TOP)/GUI/WinMsg
SRCDIRS += $(TOP)/GUI/WinTree
SRCDIRS += $(TOP)/SysLib
SRCDIRS += $(TOP)/Font/FontApi
SRCDIRS += $(TOP)/Font/FontDot
SRCDIRS += $(TOP)/Font/FontDrv
SRCDIRS += $(TOP)/Font/TrueType/autofit
SRCDIRS += $(TOP)/Font/TrueType/base
SRCDIRS += $(TOP)/Font/TrueType/drive
SRCDIRS += $(TOP)/Font/TrueType/sfnt
SRCDIRS += $(TOP)/Font/TrueType/smooth
SRCDIRS += $(TOP)/Font/TrueType/truetype
SRCDIRS += $(TOP)/Direct
ifneq ($(MAKECMDGOALS),lib)
SRCDIRS += $(TOP)/Direct/Media/JZ4750
endif
SRCDIRS += $(TOP)/Direct/Media/MediaLib
SRCDIRS += $(TOP)/Direct/Media/MediaLib/Mp3Dec
SRCDIRS += $(TOP)/Direct/Media/MediaLib/Midi
SRCDIRS += $(TOP)/Direct/Surface/JZ4750
SRCDIRS += $(TOP)/Direct/HandWrite
SRCDIRS += $(TOP)/Direct/Osd/JZ4750
SRCDIRS += $(TOP)/Direct/WaveOut/JZ4750

SRCDIRS += $(TOP)/Peripheral/MultiIo
SRCDIRS += $(TOP)/Peripheral/Audio
SRCDIRS += $(TOP)/Peripheral/Clock
SRCDIRS += $(TOP)/Peripheral/Indicator
SRCDIRS += $(TOP)/Peripheral/Keyboard
SRCDIRS += $(TOP)/Peripheral/Lcdc
SRCDIRS += $(TOP)/Peripheral/NAND
SRCDIRS += $(TOP)/Peripheral/iNand
SRCDIRS += $(TOP)/Peripheral/Power
SRCDIRS += $(TOP)/Peripheral/RTC
SRCDIRS += $(TOP)/Peripheral/SD
SRCDIRS += $(TOP)/Peripheral/Touch
SRCDIRS += $(TOP)/Peripheral/UART
SRCDIRS += $(TOP)/Peripheral/Usb
SRCDIRS += $(TOP)/Peripheral/Usb/UsbStack
SRCDIRS += $(TOP)/Peripheral/Usb/JZ4750/UsbDev
SRCDIRS += $(TOP)/Peripheral/Watchdog
SRCDIRS += $(TOP)/Peripheral/Memory
SRCDIRS += $(TOP)/Peripheral/Codec

endif
endif
endif
endif


# 源文件类型，支持下面源文件类型：
# .s, .S, .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx. 
SRCEXTS    := .S .c

# 预编译参数
CPPFLAGS   := 

# 汇编编译参数
ASFLAGS    := -D_ASSEMBLER_ -D__ASSEMBLY__

# C编译参数
CFLAGS     := -mips32 -O2 -mno-abicalls -fno-pic -fno-builtin 
CFLAGS     += -fno-exceptions -Wall  
CFLAGS     += -fomit-frame-pointer -msoft-float -G 0 
CFLAGS     += -I../../include -I../../include/font
CFLAGS     += -DSTC_EXP -DKERNEL

# C++编译参数
CXXFLAGS   := 
CXXFLAGS   += 

# 连接参数
LDFLAGS    := -nostdlib -T $(LDS)

# 库文件
MLIBS := mplayer.a libvo.a libao2.a libinput.a libmpcodecs.a libaf.a libmpdemux.a stream.a libswscale.a libosd.a libavformat.a libavcodec.a libavutil.a \
         libpostproc.a liba52.a libmpeg2.a libfaad2.a libvorbisidec.a libosdep.a libmad.a spc.a fipop.a av_sync.a

LIBS  = $(addprefix $(TOP)/UsrLib/libout/JZ4750/, $(MLIBS))
LIBS 	+= $(TOP)/Usrlib/libout/libgcc_mipsel.a $(TOP)/Usrlib/libout/mips_klibc.a

ifeq ($(MAKECMDGOALS), drv)
LIBS 	+= $(TARGET).a
endif

# 编译工具路径
CC      := /noahos/ToolChain/gcc-mips/bin/noahos-gcc 
CXX     := /noahos/ToolChain/gcc-mips/bin/noahos-g++ 
AR		:= /noahos/ToolChain/gcc-mips/bin/noahos-ar rcsv
LD		:= /noahos/ToolChain/gcc-mips/bin/noahos-ld
OBJCOPY	:= /noahos/ToolChain/gcc-mips/bin/noahos-objcopy
NM		:= /noahos/ToolChain/gcc-mips/bin/noahos-nm
OBJDUMP	:= /noahos/ToolChain/gcc-mips/bin/noahos-objdump
RM      := rm -fr 

# 允许下面两行，将把C文件按C++编译
#CC       = $(CXX) 
#CFLAGS   = $(CXXFLAGS) 


## 获取所有源文件/目标文件
#--------------------------------------------------- 
SHELL   = /bin/sh 
VPATH   := $(SRCDIRS)
PROGRAM = $(addsuffix .elf, $(TARGET))
PROGRAM = $(addsuffix .elf, $(TARGET))
SOURCES = $(foreach d, $(SRCDIRS), $(wildcard $(addprefix $(d)/*, $(SRCEXTS)))) 
OBJS	= $(addsuffix .o, $(basename $(notdir $(SOURCES))))
DEPS    = $(patsubst %.o, %.d, $(OBJS)) 

.PHONY : all objs clean cleanall rebuild lib bios cfgbios

# 执行命令. 
#---------------------------------------------- 
all : $(PROGRAM) 
	@echo 
	@echo Making logo application...
	@cp $(TOP)/AppLogo/main.c $(TOP)/AppLogo/main.tmp
	@mv $(TOP)/AppLogo/main.tmp $(TOP)/AppLogo/main.c
	@cd $(TOP)/AppLogo/build/MIPS && make
	@cp -p $(TOP)/AppLogo/build/MIPS/logo.bin ./

	make1 -C plugin
	@echo 
	@echo Creating binary...
	cp ./plugin/mplayer_plugin.bin ./
	$(OBJCOPY) --remove-section .plugin -O binary $(PROGRAM) $(TARGET).bin
#	$(OBJDUMP) -d $(PROGRAM) > $(TARGET).dump
	$(NM) $(PROGRAM) | sort > $(TARGET).sym
	$(OBJDUMP) -h $(PROGRAM) > $(TARGET).map
	@sh _release
#	cp $(TARGET).bin $(TOP)/ToolChain/uboot-fix/JZ4750/

cfgsys : 
	cp -p $(TOP)/Include/config/JZ4750.h $(TOP)/Include/config.h

bios : $(PROGRAM) Loader.bin
	@echo 
	@echo Making bootloader...
	@cd $(TOP)/Bootloader/JZ4750/build_boot && make
	@cp $(TOP)/Bootloader/JZ4750/build_boot/Boot.bin ./
	@cd $(TOP)/Bootloader/JZ4750/build_loader && make
	@cp $(TOP)/Bootloader/JZ4750/build_loader/Loader.bin ./
	@echo 
	@echo Making bios application...
	@cd $(TOP)/AppLogo/build/MIPS && make bios
	@cp $(TOP)/AppLogo/bios/build/MIPS/BiosApp.bin ./
	@echo 
	@echo Creating binary...
	$(OBJCOPY) -O binary $(PROGRAM) $(TARGET).bin
#	$(OBJDUMP) -d $(PROGRAM) > $(TARGET).dump
	$(NM) $(PROGRAM) | sort > $(TARGET).sym
	$(OBJDUMP) -h $(PROGRAM) > $(TARGET).map
	@sh _bios
#	cp $(TARGET).bin $(TOP)/ToolChain/uboot-fix/JZ4750/
	
cfgbios :
	mv -f $(TOP)/Include/config.h $(TOP)/Include/config.h.old
	sed 's/^\/\/#define CONFIG_MAKE_BIOS\|^\/\/#define CONFIG_MAKE_BURN_TOOL\|^#define CONFIG_MAKE_BURN_TOOL/#define CONFIG_MAKE_BIOS/' $(TOP)/Include/config.h.old > $(TOP)/include/config.h
	cat $(TOP)/include/config.h | grep CONFIG_MAKE


$(PROGRAM) : $(OBJS)  $(LIBS)
	@echo 
	@echo Linking...
	$(CC) -o $(PROGRAM) $(OBJS) $(LIBS) $(LIBS) $(LDFLAGS) 

lib : $(OBJS)
	$(AR) $(TARGET).a $(OBJS)

drv : $(PROGRAM) 
	@echo 
	@echo Making logo application...
	@cp $(TOP)/AppLogo/main.c $(TOP)/AppLogo/main.tmp
	@mv $(TOP)/AppLogo/main.tmp $(TOP)/AppLogo/main.c
	@cd $(TOP)/AppLogo/build/MIPS && make
	@cp -p $(TOP)/AppLogo/build/MIPS/logo.bin ./
	@echo 
	@echo Creating binary...
	$(OBJCOPY) -O binary $(PROGRAM) $(TARGET).bin
	$(OBJDUMP) -d $(PROGRAM) > $(TARGET).dump
	$(NM) $(PROGRAM) | sort > $(TARGET).sym
	$(OBJDUMP) -h $(PROGRAM) > $(TARGET).map
	@sh _release

objs : $(OBJS) 

rebuild: clean all 

clean : 
	$(RM) *.o *.d $(TARGET).*

# 生成依赖文件规则
#--------------------------------------------------- 
%.d : %.s 
	@echo DEPENDING $<
	@$(CC) -MM -MD $(CFLAGS)$(ASFLAGS) $< 

%.d : %.S 
	@echo DEPENDING $<
	@$(CC) -MM -MD $(CFLAGS) $(ASFLAGS) $< 

%.d : %.c 
	@echo DEPENDING $<
	@$(CC) -MM -MD $(CFLAGS) $< 

%.d : %.C 
	@echo DEPENDING $<
	@$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.cc 
	$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.cpp 
	$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.CPP 
	$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.c++ 
	$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.cp 
	$(CC) -MM -MD $(CXXFLAGS) $< 

%.d : %.cxx 
	$(CC) -MM -MD $(CXXFLAGS) $< 


# 生产目标文件规则
#--------------------------------------------------- 
%.o : %.s
	@echo COMPILING $<
	@$(CC) $(CFLAGS) $(ASFLAGS) -c $<

%.o : %.S
	@echo COMPILING $<
	@$(CC) $(CFLAGS) $(ASFLAGS) -c $<

%.o : %.c
	@echo COMPILING $<
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

%.o : %.C 
	@echo COMPILING $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.cpp 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.CPP 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.c++ 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.cp 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

%.o : %.cxx 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

# 缺省规则
#--------------------------------------------------- 
.DEFAULT:;

-include $(DEPS) 

