cmake_minimum_required(VERSION 3.14)
project(ucos2_arm C)

set(CMAKE_C_STANDARD 11)

add_compile_definitions(SOC_AM572x)
add_compile_definitions(__GNUC__)

include_directories(kernel)
include_directories(platform/arm)
include_directories(platform/csp)
include_directories(task)
include_directories(C:/ti/pdk_am57xx_1_0_14/packages)

add_executable(ucos2_arm
        boot/bsp.c
        boot/page_table.c
        kernel/os_cfg.h
        kernel/os_core.c
        kernel/os_dbg.c
        kernel/os_flag.c
        kernel/os_mbox.c
        kernel/os_mem.c
        kernel/os_mutex.c
        kernel/os_q.c
        kernel/os_sem.c
        kernel/os_task.c
        kernel/os_time.c
        kernel/os_tmr.c
        kernel/ucos_ii.h
        platform/arm/os_cpu.h
        platform/arm/os_cpu_c.c
        platform/arm/os_cpu_cfg.h
        platform/csp/dsp.c
        platform/csp/dsp.h
        platform/csp/gic.c
        platform/csp/gic.h
        platform/csp/mmc.c
        platform/csp/mmc.h
        platform/csp/mmio.h
        platform/csp/timer.c
        platform/csp/timer.h
        platform/csp/types.h
        platform/csp/uart.c
        platform/csp/uart.h
        task/app.c
        task/app_cfg.h
        task/snprintf.c
        task/snprintf.h)
