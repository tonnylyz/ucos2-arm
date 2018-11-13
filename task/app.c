#include <uart.h>
#include <ucos_ii.h>

char App_TaskStartStk[512u];

void App_TaskStart(void *p_arg) {
#if (OS_TASK_STAT_EN > 0u)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    while (1) {                                          /* Task body, always written as an infinite loop.       */
        uart_print_dec(1);
        int j = 0xffff;
        while (j--) {
            asm volatile ("nop");
        }
    }
}

void App_TaskCreateHook(struct os_tcb *ptcb) {

}

void App_TaskDelHook(struct os_tcb *ptcb) {

}

void App_TaskIdleHook(void) {
}

void App_TaskStatHook(void) {
}

void App_TaskReturnHook(struct os_tcb *ptcb) {

    (void) ptcb;
}

void App_TaskSwHook(void) {

}

void App_TCBInitHook(struct os_tcb *ptcb) {

}

void App_TimeTickHook(void) {

}