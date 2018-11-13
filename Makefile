#############################################################
#
# BootROM Ucosii top level Makefile
#
# Copyright (c) 2011 Altera Corporation
# All Rights Reserved.
#
# Clive Davies
#
# 26 Sept 2011  Initial version based on BootROM makefile
#
#############################################################


############################################################
# User config section

# Set BUILD_TOOL_ROOT to point to your toolchain installation
#BUILD_TOOL_ROOT := C:/Program\ Files/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI
BUILD_TOOL_CROSS := arm-none-eabi
BUILD_TOOL_ROOT := /usr/local/gcc-arm-none-eabi

#set to 1 to enable AMP (default is single core)
CONFIG_AMP ?= 
#############################################################

CROSS_COMPILE := $(BUILD_TOOL_CROSS)-

APP_OBJS := task/app.o

BSP_OBJS := boot/bsp.o boot/page_table.o boot/start.o

UCLIB_OBJS := lib/lib_ascii.o lib/lib_math.o lib/lib_mem.o lib/lib_str.o

UCOSII_OBJS := platform/arm/os_cpu_c.o platform/arm/os_cpu_a.o platform/arm/cpu_a.o platform/arm/cpu_core.o \
    platform/csp/timer.o platform/csp/uart.o \
    kernel/os_core.o kernel/os_flag.o kernel/os_mbox.o kernel/os_mem.o kernel/os_mutex.o kernel/os_q.o kernel/os_sem.o \
	kernel/os_task.o kernel/os_time.o kernel/os_tmr.o kernel/os_dbg.o

all: ucosii.axf Makefile

ucosii.axf: ucosii.a ucosii.lds $(APP_OBJS) 
	$(LD) -o ucosii.axf $(LDFLAGS) --gc-sections -Bstatic --gc-sections --start-group  --script=ucosii.lds $(APP_OBJS) ucosii.a $(GCC_LIBS) -nostdlib -Map=ucosii.map    
	#$(OBJCOPY) -I elf32-little -O binary $@ ucosii.bin    
	$(OBJCOPY) -O binary -R .note -R .comment -S  ucosii.axf zImage
	$(OBJCOPY) -O binary -j .text -j .secure_text -j .rodata -j .hash -j .data -j .got -j .got.plt -j .u_boot_list -j .rel.dyn -S ucosii.axf ucosii.bin
                             
          
ucosii.a: $(BSP_OBJS) $(UCCPU_OBJS) $(UCCSP_OBJS) $(UCSERIAL_OBJS) $(UCLIB_OBJS) $(UCOSII_OBJS)
	$(AR) -r $@ $(BSP_OBJS) $(UCCPU_OBJS) $(UCCSP_OBJS) $(UCSERIAL_OBJS) $(UCLIB_OBJS) $(UCOSII_OBJS)

build: build-ucosii 

build-ucosii: $(APP_OBJS) $(BSP_OBJS) $(UCLIB_OBJS) $(UCOSII_OBJS)

.PHONY: all build build-ucosii clean

#Do Not allow gnu make implicit rules
.SUFFIXES:

# AS		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)gcc
# LD		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)gcc
# CC		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)gcc 
# AR		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)ar
# RM              = $(BUILD_TOOL_ROOT)/bin/cs-rm
# OBJCOPY = $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)objcopy
export CC = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
export CXX = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
export LD = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-ld
export AS = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
export AR = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-ar
export OBJCOPY = /usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy

NM      = echo unknown NM
LDR     = echo unknown LDR
STRIP   = echo unknown STRIP
RANLIB  = echo unknown RANLIB
MKDIR	= mkdir
MV		= move

INCDIRS = boot kernel lib platform/arm platform/csp

BUILD_DEBUG = 1

ifeq ($(BUILD_DEBUG),1)
BUILD_DEFINES	= -D__DEBUG__
OPT_DEFINES		= -O0
DEBUG_TARGET	=  debug
else
BUILD_DEFINES	= -DRELEASE__=1
OPT_DEFINES		= -O3
DEBUG_TARGET	=  release
endif
BINARY_DIR_ROOT	:= $(DEBUG_TARGET)

BUILD_TARGET    := ROM
ifeq ($(MAKECMDGOALS),codelink)
  BUILD_TARGET      := codelink
endif
ifeq ($(MAKECMDGOALS),pluto)
  BUILD_TARGET 			:= pluto
endif
BINARY_DIR_ROOT := $(BINARY_DIR_ROOT)/$(BUILD_TARGET)


BUILDFLAGS = -DBUILD_TARGET=$(BUILD_TARGET) -DBUILD_TARGET_$(BUILD_TARGET)

PROCESSOR = -mtune=cortex-a15 -mcpu=cortex-a15 -mfpu=neon -mfloat-abi=soft

CCFLAGS = -g -c -fno-builtin-printf -pipe -ftree-vectorize -ffast-math -funsafe-math-optimizations -fsingle-precision-constant -ffreestanding -marm -Wall -Wstrict-prototypes -fno-stack-protector -isystem 
CCFLAGS += -I$(BUILD_TOOL_ROOT)/lib/gcc/$(BUILD_TOOL_CROSS)/7.3.1/include

CFLAGS = -g -D__ARM__ -DCONFIG_ARM -D__KERNEL__ -DHAVE_HPS_BASE_ADDRS_H -DHAVE_HPS_CLOCK_MANAGER_REGS_H -DHAVE_HPS_DW_APB_TIMERS_H -DHAVE_HPS_DW_APB_UART_H -DHAVE_HPS_INTERRUPTS_H -DHAVE_HPS_RESET_MANAGER_REGS_H -DCONFIG_ZERO_BSS -DCONFIG_ENABLE_NEON -DPLATFORM_DEV5XS1 -DEXP_MIDR_VAL=0x412FC092  -DINLINE=inline  

ASFLAGS =  -g -c -march=armv7-a -mtune=cortex-a15 -mcpu=cortex-a15 -mfpu=neon -mfloat-abi=soft -D__ASSEMBLY__
ASFLAGS += $(CCFLAGS) $(CFLAGS)
DEP_FLAGS = -MD
CFLAGS += -DPLATFORM_DEV5XS1

CCFLAGS += $(BUILD_DEFINES)
CCFLAGS += $(OPT_DEFINES)
CCFLAGS	+= $(PROCESSOR) $(CFLAGS)
CCFLAGS += $(BUILDFLAGS)
ifneq ($(CONFIG_AMP),)
CFLAGS += -DCONFIG_AMP
endif

LDFLAGS = 

MAIN_LIBS = 

OBJ_DIR_ROOT			:= OBJS/$(BINARY_DIR_ROOT)
OBJ_DIR					:= $(OBJ_DIR_ROOT)
MAIN_LIB_ROOT			:= $(MAIN_DIR)/lib
MAIN_LIB_BINARY_ROOT	:= $(MAIN_LIB_ROOT)/$(BINARY_DIR_ROOT)

GCC_LIBS := -L$(BUILD_TOOL_ROOT)/$(BUILD_TOOL_CROSS)/lib -lm -lc  -L$(BUILD_TOOL_ROOT)/lib/gcc/$(BUILD_TOOL_CROSS)/7.3.1 -lgcc

ONCHIP_ROM_BASE_ADDRESS	:=0x00000000
ONCHIP_RAM_BASE_ADDRESS	:=0x00008000
ONCHIP_ROM_LENGTH		:=0x00008000
ONCHIP_RAM_LENGTH		:=0x00008000

DUMMY_ADDRESS			:=0xFFFF0000

%.o: %.S
	$(AS) $(ASFLAGS) $(INCDIRS:%=-I%)  -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) $(INCDIRS:%=-I%) $(DEP_FLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.S
	$(AS) $(ASFLAGS) $(INCDIRS:%=-I%) $(DEP_FLAGS) $(BUILDFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CCFLAGS) $(INCDIRS:%=-I%) $(DEP_FLAGS) $(BUILDFLAGS) -o $@ $<

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

.PHONY: clean clean-common
clean: clean-common

DEP_FILES := $(patsubst %.o,%.d,$(APP_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(BSP_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(UCCPU_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(UCCSP_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(UCSERIAL_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(UCLIB_OBJS))
DEP_FILES += $(patsubst %.o,%.d,$(UCOSII_OBJS))

clean-common:
	$(RM) $(APP_OBJS) $(BSP_OBJS) $(UCCPU_OBJS) $(UCCSP_OBJS) $(UCSERIAL_OBJS) $(UCLIB_OBJS) $(UCOSII_OBJS) $(DEP_FILES) ucosii.axf ucosii.a ucosii.bin -f

-include $(DEP_FILES)


