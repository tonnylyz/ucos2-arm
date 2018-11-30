#include <uart.h>
#include <ucos_ii.h>
#include "app_cfg.h"
#include "snprintf.h"

void MyTask(void *p_arg) {
    char *sTaskName = (char *) p_arg;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr = 0;
#endif

    while (1) {
        OS_ENTER_CRITICAL();
        uart_puts("Name: ");
        uart_puts(sTaskName);
        uart_putc('\n');
        OS_EXIT_CRITICAL();

        OSTimeDly(50);
    }
}

#if (OS_APP_HOOKS_EN == 1u)
void App_TaskCreateHook(OS_TCB *ptcb) {
    //uart_puts("App_TaskCreateHook called\n");
}

void App_TaskDelHook(OS_TCB *ptcb) {
    uart_puts("App_TaskDelHook called\n");

}

void App_TaskIdleHook(void) {
    //uart_puts("App_TaskIdleHook called\n");
}

void App_TaskStatHook(void) {
    uart_puts("App_TaskStatHook called\n");
}

void App_TaskReturnHook(OS_TCB *ptcb) {
    uart_puts("App_TaskReturnHook called\n");

}

void App_TaskSwHook(void) {
    //uart_puts("App_TaskSwHook called\n");

    //printf("OSTCBHighRdy->OSTCBStkPtr [%08x]\n", OSTCBHighRdy->OSTCBStkPtr);
    //printf("OSTCBHighRdy->PC [%08x]\n", OSTCBHighRdy->OSTCBStkPtr[14]);
}

void App_TCBInitHook(OS_TCB *ptcb) {
    //uart_puts("App_TCBInitHook called\n");

}

void App_TimeTickHook(void) {
    //uart_puts("App_TimeTickHook called\n");

}
#endif