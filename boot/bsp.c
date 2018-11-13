#include <ucos_ii.h>
#include <cpu_core.h>
#include <uart.h>
#include <timer.h>
#include <types.h>

OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

extern void App_TaskStart();

int main() {

    INT8U      os_err;

    uart_init();
    uart_puts("OS Build: ");
    uart_puts(__TIMESTAMP__);
    uart_putc('\n');

    uart_puts("uart_init finished\n");



    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)CSP_DEV_NAME,
                (CPU_ERR  *)&cpu_err);
#endif

    CPU_IntDis();

    OSInit();                                                   /* Init uC/OS-II.                                       */
    uart_puts("OSInit done\n");

    OSTaskCreate(App_TaskStart, (void *)0, &App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1], 0);
    uart_puts("OSTaskCreate done\n");

    CPU_IntEn();

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

    while (1);
}

void OS_CPU_IntHandler (u32  src_id) {
    uart_puts("OS_CPU_IntHandler called\n");

}

u32  CPU_TS_TmrRd ()
{
    uart_puts("CPU_TS_TmrRd called\n");
    while (1);
    return 0;
}

void  CPU_TS_TmrInit ()
{
    uart_puts("CPU_TS_TmrInit called\n");
    timer_init();
}


void cpu_undef_ins() {
    uart_puts("cpu_undef_ins called\n");
    while (1);
}

void cpu_svc() {
    uart_puts("cpu_svc called\n");
    while (1);
}

void cpu_prefetch_abort() {
    uart_puts("cpu_prefetch_abort called\n");
    while (1);
}

void cpu_data_abort() {
    uart_puts("cpu_data_abort called\n");
    while (1);
}

void cpu_fiq() {
    uart_puts("cpu_fiq called\n");
    while (1);
}