#include <ucos_ii.h>
#include <types.h>
#include <uart.h>
#include <timer.h>
#include <gic.h>
#include <snprintf.h>

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


    uart_init();

    printf("OS Build: %s %s\n", __DATE__, __TIME__);

    printf("uart_init finished\n");

    gic_init();
    printf("gic_init finished\n");
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

void OS_CPU_IntHandler(u32 src_id) {
    printf("%s\n", __FUNCTION__);
    timer_clear_irq();
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

void OS_CPU_AbortHandler() {
    printf("%s\n", __FUNCTION__);
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