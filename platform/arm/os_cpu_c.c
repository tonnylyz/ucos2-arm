#include <ucos_ii.h>

OS_STK OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_STK *OS_CPU_ExceptStkBase;
OS_STK *OS_CPU_ExceptStkPtr;

#if OS_CPU_HOOKS_EN > 0u

void OSInitHookBegin(void) {
    INT32U size;
    OS_STK *pstk;

    /* Clear exception stack for stack checking.*/
    pstk = &OS_CPU_ExceptStk[0];
    size = OS_CPU_EXCEPT_STK_SIZE;
    while (size > 0u) {
        size--;
        *pstk++ = (OS_STK) 0;
    }
    /* Align the ISR stack to 8-bytes           */
    OS_CPU_ExceptStkBase = (OS_STK *) &OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
    OS_CPU_ExceptStkBase = (OS_STK *) ((OS_STK) (OS_CPU_ExceptStkBase) & 0xFFFFFFF8);

}

void OSInitHookEnd(void) {}

void OSTaskCreateHook(OS_TCB *ptcb) {
#if OS_APP_HOOKS_EN > 0u
    App_TaskCreateHook(ptcb);
#endif
}

void OSTaskDelHook(OS_TCB *ptcb) {
#if OS_APP_HOOKS_EN > 0u
    App_TaskDelHook(ptcb);
#endif
}

void OSTaskIdleHook(void) {
#if OS_APP_HOOKS_EN > 0u
    App_TaskIdleHook();
#endif
}


void OSTaskReturnHook(OS_TCB *ptcb) {
#if OS_APP_HOOKS_EN > 0u
    App_TaskReturnHook(ptcb);
#endif
}


void OSTaskStatHook(void) {
#if OS_APP_HOOKS_EN > 0u
    App_TaskStatHook();
#endif
}

#endif

OS_STK *OSTaskStkInit(void (*task)(void *p_arg),
                      void *p_arg,
                      OS_STK *ptos,
                      INT16U opt) {
    OS_STK *p_stk;
    INT32U task_addr;

#if (OS_CPU_ARM_FP_EN == 1)
    INT8U i;
#endif

    p_stk = ptos + 1u;                                     /* Load stack pointer                                     */
    p_stk = (OS_STK *) ((OS_STK) (p_stk) & 0xFFFFFFF8u);
    task_addr = (INT32U) task &
                ~1u;                             /* Mask off lower bit in case task is thumb mode          */

    *--p_stk = (OS_STK) task_addr;                              /* Entry Point                                            */
    *--p_stk = (OS_STK) OS_TaskReturn;                          /* R14 (LR)                                               */
    *--p_stk = (OS_STK) 0x12121212u;                            /* R12                                                    */
    *--p_stk = (OS_STK) 0x11111111u;                            /* R11                                                    */
    *--p_stk = (OS_STK) 0x10101010u;                            /* R10                                                    */
    *--p_stk = (OS_STK) 0x09090909u;                            /* R9                                                     */
    *--p_stk = (OS_STK) 0x08080808u;                            /* R8                                                     */
    *--p_stk = (OS_STK) 0x07070707u;                            /* R7                                                     */
    *--p_stk = (OS_STK) 0x06060606u;                            /* R6                                                     */
    *--p_stk = (OS_STK) 0x05050505u;                            /* R5                                                     */
    *--p_stk = (OS_STK) 0x04040404u;                            /* R4                                                     */
    *--p_stk = (OS_STK) 0x03030303u;                            /* R3                                                     */
    *--p_stk = (OS_STK) 0x02020202u;                            /* R2                                                     */
    *--p_stk = (OS_STK) 0x01010101u;                            /* R1                                                     */
    *--p_stk = (OS_STK) p_arg;                                  /* R0 : argument                                          */


#if (OS_CPU_ARM_ENDIAN_TYPE == OS_CPU_ARM_ENDIAN_LITTLE)
    if (((INT32U) task & 0x01u) ==
        0x01u) {                        /* See if task runs in Thumb or ARM mode                  */
        *--p_stk = (OS_STK) (
                OS_CPU_ARM_BIT_CPSR_MODE_SUPERVISOR    /* Set supervisor mode.                                   */
                | OS_CPU_ARM_BIT_CPSR_T);                /* Set Thumb mode.                                        */
    } else {
        *--p_stk = (OS_STK) (0x00000100 | OS_CPU_ARM_BIT_CPSR_MODE_SUPERVISOR);
    }
#else
    if (((INT32U)task & 0x01u) == 0x01u) {                        /* See if task runs in Thumb or ARM mode                  */
       *--p_stk = (OS_STK)(OS_CPU_ARM_BIT_CPSR_MODE_SUPERVISOR    /* Set supervisor mode.                                   */
                |          OS_CPU_ARM_BIT_CPSR_T                  /* Set Thumb mode.                                        */
                |          OS_CPU_ARM_BIT_CPSR_E);                /* Set Endianes bit. Big Endian                           */
    } else {
       *--p_stk = (OS_STK)(OS_CPU_ARM_BIT_CPSR_MODE_SUPERVISOR
                |          OS_CPU_ARM_BIT_CPSR_E);    
    }
#endif

#if (OS_CPU_ARM_FP_EN == 1)
    *--p_stk = (OS_STK) 0;                                       /* Initialize Floating point status & control register     */
    /* Initialize general-purpose Floating point registers     */
    for (i = 0u; i < OS_CPU_ARM_FP_REG_NBR; i++) {
        *--p_stk = (OS_STK) 0;
    }

#endif

    return (p_stk);
}


/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                 task being switched out (i.e. the preempted task).
*              3) If debug variables are enabled, the current process id is saved into the context ID register
*                 found in the system control coprocessor. The Embedded Trace Macrocell (ETM) and the debug logic
*                 use this register. The ETM can broadcast its value to indicate the process that is running currently.
*                         
*                     (a) The proccess id is formed by concatenating the current task priority with the lower 24 bits
*                         from the current task TCB.
*
*                            31              24                     0
*                             +---------------+---------------------+
*                             | OSPrioHighRdy | OSTCBHighRdy[23..0] |
*                             +---------------+---------------------+
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TASK_SW_HOOK_EN > 0u)

void OSTaskSwHook(void) {
#if OS_CFG_DBG_EN > 0u
    INT32U  ctx_id;
#endif

#if OS_APP_HOOKS_EN > 0u
    App_TaskSwHook();
#endif

#if OS_CFG_DBG_EN > 0u
    ctx_id = ((INT32U)(OSPrioHighRdy    << 24u)             )
           | ((INT32U)(OSTCBHighRdy           ) & 0x00FFFFFF);
    OS_CPU_ARM_CtxID_Set(ctx_id);
#endif
}

#endif


#if OS_CPU_HOOKS_EN > 0u

void OSTCBInitHook(OS_TCB *ptcb) {
#if OS_APP_HOOKS_EN > 0u
    App_TCBInitHook(ptcb);
#endif
}

#endif

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)

void OSTimeTickHook(void) {
#if OS_APP_HOOKS_EN > 0u
    App_TimeTickHook();
#endif
}

#endif


/*
*********************************************************************************************************
*                              GET NUMBER OF FREE ENTRIES IN EXCEPTION STACK
*
* Description : This function computes the number of free entries in the exception stack.
*
* Arguments   : None.
*
* Returns     : The number of free entries in the exception stack.
*********************************************************************************************************
*/

INT32U OS_CPU_ExceptStkChk(void) {
    OS_STK *pchk;
    INT32U nfree;
    INT32U size;


    nfree = 0;
    size = OS_CPU_EXCEPT_STK_SIZE;
    pchk = &OS_CPU_ExceptStk[0];
    while ((*pchk++ == (OS_STK) 0) && (size > 0u)) {   /* Compute the number of zero entries on the stk */
        nfree++;
        size--;
    }

    return (nfree);
}
