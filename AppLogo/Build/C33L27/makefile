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
TARGET  := logo

# 连接脚本文件
LDS     := link.lds

# 源文件目录
TOP	:= ../..
SRCDIRS = $(TOP)/
SRCDIRS += $(TOP)/App
TOOL_DIR = ../../../ToolChain/gcc-c33

# 库文件
LIBS = ../../../Usrlib/libout/v2usr_c33.a
LIBS += ../../../Usrlib/libout/v2c33.a
LIBS += ../../../Usrlib/libout/C33L27/c33libgcc.a
LIBS += ../../../Usrlib/libout/C33L27/c33libgcc2.a

# 源文件类型，支持下面源文件类型：
# .s, .S, .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx. 
SRCEXTS    := .c

# 预编译参数
CPPFLAGS   := 

# 汇编编译参数
ASFLAGS    := 

# C编译参数
CFLAGS     := -B$(TOOL_DIR)/ -c -mc33pe -fpic -mno-sjis-filt -gstabs -O1 -fno-builtin -mlong-calls -Wall -Werror-implicit-function-declaration -mno-memcpy
CFLAGS     += -I../../../include

# C++编译参数
CXXFLAGS   := 
CXXFLAGS   += 

# 连接参数
LDFLAGS    := -T $(LDS) -Map $(TARGET).map -N


# 编译工具路径
CC      := /noahos/ToolChain/gcc-c33/xgcc
CXX     := /noahos/ToolChain/gcc-c33/xgcc
AS      := /noahos/ToolChain/gcc-c33/xgcc
AR	:= /noahos/ToolChain/gcc-c33/ar
LD	:= /noahos/ToolChain/gcc-c33/ld 
OBJCOPY	:= /noahos/ToolChain/gcc-c33/objcopy
OBJDUMP	:= /noahos/ToolChain/gcc-c33/objdump
RM      := rm -fr 
NM		:= /noahos/ToolChain/gcc-arm/bin/arm-elf-nm

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

DEPCMD_CC	= $(CC) -M -MG $(CFLAGS) $< > $@

.PHONY : all objs clean cleanall rebuild lib

# 执行命令. 
#---------------------------------------------- 
all : $(PROGRAM) 
	nef.exe -r -i $(PROGRAM) -o $(TARGET).bin
	$(OBJDUMP) -D $(PROGRAM) > $(TARGET).dump
	$(NM) $(PROGRAM) | sort > $(TARGET).sym

$(PROGRAM) : $(OBJS)
	@echo LINKING...
	$(LD) -o $(PROGRAM) $(LDFLAGS) $(OBJS) $(LIBS) 

lib : $(OBJS)
	$(AR) $(TARGET).a $(OBJS)

objs : $(OBJS) 

rebuild: clean all 

clean : 
	$(RM) *.o *.d $(TARGET).* *.bin

# 生成依赖文件规则
#--------------------------------------------------- 
%.d : %.c
	@echo COMPILING $<
	$(DEPCMD_CC)

# 生产目标文件规则
#--------------------------------------------------- 
%.o : %.c
	@echo COMPILING $<
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# 缺省规则
#--------------------------------------------------- 
.DEFAULT:;

-include $(DEPS) 

