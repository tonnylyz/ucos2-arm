#############################################################
# uC/OS II ARM Port Makefile
# Author: New Type OS Research Group
#############################################################


# Cross Compile Toolchains
######################################
BUILD_TOOL_CROSS := arm-none-eabi
BUILD_TOOL_ROOT := /home/tonny/ti/gcc-arm-none-eabi-6-2017-q1-update
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
CFLAGS += -D SOC_AM572x

# Objects
######################################
# Task Program Objects
TASK_OBJS := task/app.o task/snprintf.o

# Bootstrap Objects
BSP_OBJS := boot/bsp.o boot/page_table.o boot/start.o

# Platform Specified Objects
PLATFORM_OBJS := platform/arm/os_cpu_c.o platform/arm/os_cpu_a.o \
                 platform/csp/timer.o platform/csp/uart.o platform/csp/gic.o platform/csp/mmc.o platform/csp/dsp.o

# uC/OS-II Kernel Objects
UCOSII_OBJS := kernel/os_core.o  kernel/os_flag.o kernel/os_mbox.o kernel/os_mem.o \
               kernel/os_mutex.o kernel/os_q.o    kernel/os_sem.o  kernel/os_task.o \
               kernel/os_time.o  kernel/os_tmr.o  kernel/os_dbg.o

TI_VENDOR_OBJS := lib/ti.board.aa15fg lib/ti.csl.init.aa15fg lib/ti.csl.aa15fg lib/ti.drv.uart.aa15fg lib/ti.osal.aa15fg

# Targets
######################################

.PHONY: all clean burn

all: zImage

zImage: ucosii.a ucosii.lds $(TASK_OBJS)
	$(LD) -o ucosii.axf $(LDFLAGS) --gc-sections -Bstatic --gc-sections --start-group --script=ucosii.lds $(TI_VENDOR_OBJS) ucosii.a -nostdlib -Map=ucosii.map  -L$(BUILD_TOOL_ROOT)/lib/gcc/$(BUILD_TOOL_CROSS)/6.3.1/hard -lgcc
	$(OBJCOPY) -O binary -R .note -R .comment -S ucosii.axf zImage

ucosii.a: $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) $(TASK_OBJS)
	$(AR) -r $@ $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) $(TASK_OBJS)


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

burn: zImage
	cp zImage /media/tonny/BOOT/zImage
	sync
	umount /dev/sdc1
	udisksctl power-off -b /dev/sdc

INCLUDES = -Iboot -Ikernel -Ilib -Iplatform/arm -Iplatform/csp -Itask -Iplatform/csl -I/home/tonny/ti/pdk_am57xx_1_0_12/packages
