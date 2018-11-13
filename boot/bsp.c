#include <ucos_ii.h>
#include <uart.h>
#include <timer.h>
#include <types.h>

#define  APP_CFG_TASK_START_PRIO                           2u
#define  APP_CFG_TASK_START_STK_SIZE                     512u

extern OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
extern void App_TaskStart();

void main() {

    INT8U      os_err;

    uart_init();
    uart_puts("main\n");
    uart_puts("BSP_PreInit finished\n");

    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)CSP_DEV_NAME,
                (CPU_ERR  *)&cpu_err);
#endif

    CPU_IntDis();

    OSInit();                                                   /* Init uC/OS-II.                                       */

    uart_puts("will create task");
    OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    uart_puts("created task\n");
#if (OS_TASK_NAME_EN > 0u)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, (INT8U *)"Start", &os_err);
#endif
    uart_puts("timer_init\n");
    timer_init();
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

    while (1);
}

void OS_CPU_IntHandler (u32  src_id) {
    uart_puts("OS_CPU_IntHandler");
}

u32  CPU_TS_TmrRd ()
{
    return 0;
}

void  CPU_TS_TmrInit ()
{
    uart_puts("CPU_TS_TmrInit");
}