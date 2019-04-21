#include <ucos_ii.h>
#include <types.h>
#include <uart.h>
#include <timer.h>
#include <snprintf.h>

#include <ti/board/board.h>
#include <mmio.h>
#include <gic.h>
#include <mmc.h>
#include <dsp.h>

extern void MyTask(void *p_arg);

INT8U Stk1[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk2[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk3[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk4[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));
INT8U Stk5[APP_TASK_START_STK_SIZE]  __attribute__ ((aligned (APP_TASK_START_STK_SIZE)));

int main() {

    char sTask1[] = "Task 1";
    char sTask2[] = "Task 2";
    char sTask3[] = "Task 3";
    char sTask4[] = "Task 4";
    char sTask5[] = "Task 5";


    Board_initCfg boardCfg;
    boardCfg = BOARD_INIT_UNLOCK_MMR | BOARD_INIT_UART_STDIO |
               BOARD_INIT_MODULE_CLOCK | BOARD_INIT_PINMUX_CONFIG;
    Board_init(boardCfg);

    printf("ARM OS Build: %s %s\n", __DATE__, __TIME__);
    printf("Board_init done\n");

    //mmc_init();
    printf("Ready to start DSP 1\n");
    printf("====================\n");
    dsp1_start_core();

    while (1) {
        asm volatile ("nop");
    }

    gic_init();
    printf("gic_init done\n");

    irq_init();
    printf("irq_init done\n");

    timer_init();
    printf("timer_init finished\n");
    OS_CPU_SR_INT_En();

    OSInit();
    printf("OSInit done\n");


    OSTaskCreate(MyTask, sTask1,
                 (void *) &Stk1[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_1_PRIO);

    OSTaskCreate(MyTask, sTask2,
                 (void *) &Stk2[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_2_PRIO);
    OSTaskCreate(MyTask, sTask3,
                 (void *) &Stk3[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_3_PRIO);

    OSTaskCreate(MyTask, sTask4,
                 (void *) &Stk4[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_4_PRIO);

    OSTaskCreate(MyTask, sTask5,
                 (void *) &Stk5[APP_TASK_START_STK_SIZE - 1],
                 APP_TASK_5_PRIO);

    printf("task create done\n");
    OSStart();
    while (1) {
        asm volatile ("nop");
    }
}

static inline u32 get_ifar() {
    register u32 r;
    asm volatile ("mrc p15, 0, %0, c6, c0, 2" : "=r"(r));
    return r;
}

static inline u32 get_dfar() {
    register u32 r;
    asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r"(r));
    return r;
}


static inline u32 get_mpidr() {
    register u32 r;
    asm volatile ("mrc p15, 0, %0, c0, c0, 5" : "=r"(r));
    return r;
}

static inline u8 get_cpuid() {
    return (u8) (get_mpidr() & 0b11);
}


void OS_CPU_PrefetchAbortHandler() {
    printf("%s\n", __FUNCTION__);

    printf("ifar: [%08x]\n", get_ifar());
    printf("dfar: [%08x]\n", get_dfar());

    while (1) {
        asm volatile ("nop");
    }
}

void OS_CPU_DataAbortHandler() {
    printf("%s\n", __FUNCTION__);

    printf("ifar: [%08x]\n", get_ifar());
    printf("dfar: [%08x]\n", get_dfar());

    while (1) {
        asm volatile ("nop");
    }
}

void OS_CPU_UndefHandler() {
    printf("%s\n", __FUNCTION__);
    while (1) {
        asm volatile ("nop");
    }
}