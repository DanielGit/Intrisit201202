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
TARGET  := Logo

# 连接脚本文件
LDS     := link.xn

# 源文件目录
TOP	:= ../..
SRCDIRS :=
ifneq ($(MAKECMDGOALS),clean)
SRCDIRS += $(TOP)/
endif

# 库文件
LIBS = ../../../Usrlib/libout/v2mips.a
LIBS += ../../../Usrlib/libout/v2usr_mips.a
LIBS += ../../../Usrlib/libout/libgcc_mipsel.a

# 源文件类型，支持下面源文件类型：
# .s, .S, .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx. 
SRCEXTS    := .S .c

# 预编译参数
CPPFLAGS   := 

# 汇编编译参数
ASFLAGS    := -D_ASSEMBLER_ -D__ASSEMBLY__

# C编译参数
CFLAGS     := -O2 -mips32 -fno-builtin -msoft-float -G 0 
CFLAGS     += -I../../../include
CFLAGS     += 

# C++编译参数
CXXFLAGS   := 
CXXFLAGS   += 

# 连接参数
LDFLAGS    := -nostdlib -T $(LDS)


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
SOURCES = $(foreach d, $(SRCDIRS), $(wildcard $(addprefix $(d)/*, $(SRCEXTS)))) 
OBJS	= $(addsuffix .o, $(basename $(notdir $(SOURCES))))
DEPS    = $(patsubst %.o, %.d, $(OBJS)) 

.PHONY : all objs clean cleanall rebuild lib bios

# 执行命令. 
#---------------------------------------------- 
all : $(PROGRAM) 
#	$(OBJCOPY) -O binary $(PROGRAM) $(TARGET).bin
	nef -r -i $(PROGRAM) -o $(TARGET).bin
	$(OBJDUMP) -d $(PROGRAM) > $(TARGET).dump
	$(NM) $(PROGRAM) | sort > $(TARGET).sym
	$(OBJDUMP) -h $(PROGRAM) > $(TARGET).map

$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LIBS) $(LIBS) $(LDFLAGS) 

lib : $(OBJS)
	$(AR) $(TARGET).a $(OBJS)

objs : $(OBJS) 

bios :
	(cd $(TOP)/bios/Build/MIPS ; make)

rebuild: clean all 

clean : 
	$(RM) *.o *.d $(TARGET).*
	(cd $(TOP)/bios/Build/MIPS ; make clean)

# 生成依赖文件规则
#--------------------------------------------------- 
%.d : %.s 
	@echo COMPILING $<
	@$(CC) -MM -MD $(CFLAGS)$(ASFLAGS) $< 

%.d : %.S 
	@echo COMPILING $<
	@$(CC) -MM -MD $(CFLAGS) $(ASFLAGS) $< 

%.d : %.c 
	@echo COMPILING $<
	@$(CC) -MM -MD $(CFLAGS) $< 

%.d : %.C 
	@echo COMPILING $<
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

