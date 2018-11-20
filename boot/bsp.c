#include <ucos_ii.h>
#include <cpu_core.h>
#include <uart.h>
#include <timer.h>
#include <types.h>

extern void MyTask(void *p_arg);

int main() {

    INT8U err;

    INT8U Stk1[APP_TASK_START_STK_SIZE];
    INT8U Stk2[APP_TASK_START_STK_SIZE];
    INT8U Stk3[APP_TASK_START_STK_SIZE];
    INT8U Stk4[APP_TASK_START_STK_SIZE];
    INT8U Stk5[APP_TASK_START_STK_SIZE];

    char sTask1[] = "Task 1";
    char sTask2[] = "Task 2";
    char sTask3[] = "Task 3";
    char sTask4[] = "Task 4";
    char sTask5[] = "Task 5";


    uart_init();
    uart_puts("OS Build: ");
    uart_puts(__DATE__);
    uart_putc(' ');
    uart_puts(__TIME__);
    uart_putc('\n');
    uart_putc('\n');
    uart_puts("uart_init finished\n");

    CPU_Init();
    CPU_IntEn();                                               /* Initialize the uC/CPU services                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)CSP_DEV_NAME,
                (CPU_ERR  *)&cpu_err);
#endif

    OSInit();                                                   /* Init uC/OS-II.                                       */
    uart_puts("OSInit done\n");


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

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

}

extern void timer_clear_irq();

void OS_CPU_IntHandler(u32 src_id) {
    uart_puts("OS_CPU_IntHandler called\n");
    timer_clear_irq();
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

u32 CPU_TS_TmrRd() {
    uart_puts("CPU_TS_TmrRd called\n");
    while (1);
    return 0;
}

void CPU_TS_TmrInit() {
    uart_puts("CPU_TS_TmrInit called\n");
    timer_init();
}
