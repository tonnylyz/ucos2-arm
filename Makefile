#############################################################
# uC/OS II ARM Port Makefile
# Author: New Type OS Research Group
#############################################################


# Cross Compile Toolchains
######################################
BUILD_TOOL_CROSS := arm-none-eabi
BUILD_TOOL_ROOT := /usr/local/gcc-arm-none-eabi
CROSS_COMPILE := $(BUILD_TOOL_CROSS)-

AS		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)as
LD		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)ld
CC		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)gcc
AR		= $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)ar
OBJCOPY = $(BUILD_TOOL_ROOT)/bin/$(CROSS_COMPILE)objcopy

CFLAGS := -c -Wall
CFLAGS += -mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard
CFLAGS += -ffreestanding -fno-builtin-printf
CFLAGS += -fno-stack-protector

# Objects
######################################
# Task Program Objects
TASK_OBJS := task/app.o task/snprintf.o

# Bootstrap Objects
BSP_OBJS := boot/bsp.o boot/page_table.o boot/start.o

# Platform Specified Objects
PLATFORM_OBJS := platform/arm/os_cpu_c.o platform/arm/os_cpu_a.o \
                 platform/csp/timer.o platform/csp/uart.o platform/csp/gic.o

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.o  kernel/os_flag.o kernel/os_mbox.o kernel/os_mem.o \
               kernel/os_mutex.o kernel/os_q.o    kernel/os_sem.o  kernel/os_task.o \
               kernel/os_time.o  kernel/os_tmr.o  kernel/os_dbg.o

# Targets
######################################

.PHONY: all clean

all: zImage

zImage: ucosii.a ucosii.lds $(TASK_OBJS)
	$(LD) -o ucosii.axf $(LDFLAGS) --gc-sections -Bstatic --gc-sections --start-group --script=ucosii.lds $(TASK_OBJS) ucosii.a -nostdlib -Map=ucosii.map
	$(OBJCOPY) -O binary -R .note -R .comment -S ucosii.axf zImage

ucosii.a: $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS)
	$(AR) -r $@ $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS)


clean:
	$(RM) $(TASK_OBJS) $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) ucosii.axf ucosii.a zImage -f

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) $(BUILDFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(BUILDFLAGS) -o $@ $<

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)


INCLUDES = -Iboot -Ikernel -Ilib -Iplatform/arm -Iplatform/csp -I task
