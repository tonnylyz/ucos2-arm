#############################################################
# uC/OS II ARM Port Makefile
# Author: New Type OS Research Group
#############################################################

# PDK
######################################
PDK_INCLUDE := -I/mnt/c/ti/pdk_am57xx_1_0_14/packages


# Cross Compile Toolchains
######################################
BUILD_TOOL_CROSS := arm-eabi
BUILD_TOOL_ROOT := /home/tonny/gcc-linaro-7.4.1-2019.02-x86_64_arm-eabi
CROSS_COMPILE := $(BUILD_TOOL_CROSS)-
LIBGCC_HARD := -L$(BUILD_TOOL_ROOT)/lib/gcc/arm-eabi/7.4.1/v7-a/fpv3/hard -lgcc

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

all: uImage

uImage: ucosii.a ucosii.lds $(TASK_OBJS)
	$(LD) -o ucosii.axf $(LDFLAGS) --gc-sections -Bstatic --gc-sections --start-group --script=ucosii.lds $(TI_VENDOR_OBJS) ucosii.a -nostdlib -Map=ucosii.map $(LIBGCC_HARD)
	$(OBJCOPY) -O binary -R .note -R .comment -S ucosii.axf uImage

ucosii.a: $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) $(TASK_OBJS)
	$(AR) -r $@ $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) $(TASK_OBJS)


clean:
	$(RM) $(TASK_OBJS) $(BSP_OBJS) $(PLATFORM_OBJS) $(UCOSII_OBJS) ucosii.axf ucosii.a ucosii.map uImage -f

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

burn: uImage
	cp uImage /media/tonny/BOOT/uImage
	sync
	umount /dev/sdc1
	udisksctl power-off -b /dev/sdc

win:
	sudo mount -t drvfs e: /mnt/e
	cp uImage /mnt/e/uImage
	sync
	sleep 1
	sudo umount /mnt/e
	RemoveDrive.exe e: -L

INCLUDES = -Iboot -Ikernel -Ilib -Iplatform/arm -Iplatform/csp -Itask -Iplatform/csl $(PDK_INCLUDE)
