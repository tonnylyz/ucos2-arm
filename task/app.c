#include <uart.h>
#include <ucos_ii.h>
#include <app_cfg.h>

void App_Main() {
    while (1) {
        uart_print_dec(1);
        int j = 0xffff;
        while (j--) {
            asm volatile ("nop");
        }
    }
}

void App_TaskStart(void *p_arg) {
    uart_puts("App_TaskStart entered\n");

//#if (OS_TASK_STAT_EN > 0u)
//    OSStatInit();                                               /* Determine CPU capacity                               */
//    uart_puts("OSStatInit done\n");
//#endif


    App_Main();
}

void App_TaskCreateHook(OS_TCB *ptcb) {
    uart_puts("App_TaskCreateHook called\n");
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
    uart_puts("App_TaskSwHook called\n");

}

void App_TCBInitHook(OS_TCB *ptcb) {
    uart_puts("App_TCBInitHook called\n");

}

void App_TimeTickHook(void) {
    uart_puts("App_TimeTickHook called\n");

}