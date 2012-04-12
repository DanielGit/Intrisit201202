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

# DUMP
DUMPFILE := $(DUMPOUT)

# 只需要修改 PRODUCT和P_ID变量的值和config.h定义的一样即可。

# 输出目标文件定义(包含配置文件、系统文件、BIOS文件、系统文件标识、BIOS文件标识、产品ID)

#产品名称与config.h中CONFIG_MAC_(产品型号)宏里的名称相同
PRODUCT := ND800

#产品ID与config.h中CONFIG_USB_IDENTIFY_CODE宏的值相同
P_ID := 0xD800000A

CFGFILE := $(PRODUCT).h
SYSFILE := $(PRODUCT).bin
BIOSFILE := $(PRODUCT)bios.bin
SYSFILEID := NOAHOS_$(PRODUCT)
BIOSFILEID := NOAH_$(PRODUCT)

TARGETDIR := sys
ifeq ($(MAKECMDGOALS),bios)
TARGETDIR := bios
endif

# 源文件目录
TOP	:= ../..
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cfgsys)
ifneq ($(MAKECMDGOALS),cfgbios)
ifneq ($(MAKECMDGOALS),cfgburn)
SRCDIRS := 

SRCDIRS += $(TOP)/Direct
SRCDIRS += $(TOP)/Direct/Media/JZ4750

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

LIBS  = $(addprefix $(TOP)/UsrLib/libout/JZ4750L/, $(MLIBS))
LIBS 	+= _noahos.a $(TOP)/Usrlib/libout/libgcc_mipsel.a $(TOP)/Usrlib/libout/mips_klibc.a

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
MKDIR   := mkdir

# 允许下面两行，将把C文件按C++编译
#CC       = $(CXX) 
#CFLAGS   = $(CXXFLAGS) 


## 获取所有源文件/目标文件
#--------------------------------------------------- 
SHELL   = /bin/sh 
VPATH   := $(SRCDIRS)
PROGRAM = $(addsuffix .elf, $(TARGET))
SOURCES = $(foreach d, $(SRCDIRS), $(wildcard $(addprefix $(d)/*, $(SRCEXTS))))
OBJS	= boot.o $(addsuffix .o, $(basename $(notdir $(SOURCES))))
DEPS    = $(patsubst %.o, %.d, $(OBJS)) 

.PHONY : all objs clean cleanall rebuild lib bios cfgbios usbboot

# 执行命令. 
#---------------------------------------------- 
all : $(PROGRAM) 
	@echo 
	@echo Making logo application...
	@cp $(TOP)/AppLogo/main.c $(TOP)/AppLogo/main.tmp
	@mv $(TOP)/AppLogo/main.tmp $(TOP)/AppLogo/main.c
	@cd $(TOP)/AppLogo/build/MIPS && make
	@cp -p $(TOP)/AppLogo/build/MIPS/logo.bin ./

	make_3.81 -C plugin
	@echo 
	@echo Creating binary...
	cp ./plugin/mplayer_plugin.bin ./
	$(OBJCOPY) --remove-section .plugin -O binary $(PROGRAM) $(TARGET).bin
	@echo output dump file 
	$(if $(DUMPFILE), $(OBJDUMP) -d $(PROGRAM) > $(TARGET).dump)
	$(NM) $(PROGRAM) | sort > $(TARGET).sym
	$(OBJDUMP) -h $(PROGRAM) > $(TARGET).map
	@sh _release $(P_ID) $(SYSFILEID) $(SYSFILE)

cfgsys :
	cp -p $(TOP)/Include/config/$(CFGFILE) $(TOP)/Include/config.h

bios : $(PROGRAM)
	@echo 
	@echo Making bootloader...

	@make -C $(TOP)/Bootloader/JZ4750L/build_boot clean
	@make -C $(TOP)/Bootloader/JZ4750L/build_boot
	@cp $(TOP)/Bootloader/JZ4750L/build_boot/boot.bin ./
	
	@make -C $(TOP)/Bootloader/JZ4750L/build_loader clean
	@make -C $(TOP)/Bootloader/JZ4750L/build_loader
	@cp $(TOP)/Bootloader/JZ4750L/build_loader/loader.bin ./

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
	@sh _bios $(P_ID) $(BIOSFILEID) $(BIOSFILE)
	
cfgbios :
	mv -f $(TOP)/Include/config.h $(TOP)/Include/config.h.old
	sed 's/^\/\/#define CONFIG_MAKE_BIOS\|^\/\/#define CONFIG_MAKE_BURN_TOOL\|^#define CONFIG_MAKE_BURN_TOOL/#define CONFIG_MAKE_BIOS/' $(TOP)/Include/config.h.old > $(TOP)/include/config.h
	cat $(TOP)/include/config.h | grep CONFIG_MAKE

usbboot : $(PROGRAM) 
	@echo 
	@echo Making USBBoot...
	$(OBJCOPY) -O binary $(PROGRAM) $(TARGET).bin
	cp $(TARGET).bin $(TOP)/ToolChain/uboot-fix/JZ4750L/

$(PROGRAM) : $(OBJS)  $(LIBS)
	@echo 
	@echo Linking...
	$(CC) -o $(PROGRAM) $(OBJS) $(LIBS) $(LDFLAGS) 

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
	@sh _release $(P_ID) $(SYSFILEID) $(SYSFILE)


objs : $(OBJS) 

rebuild: clean all 

clean : 
	$(RM) *.o *.d $(TARGET).* *.bin

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

