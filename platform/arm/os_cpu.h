/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*
*                           (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                    All rights reserved.  Protected by international copyright laws.
*
;                                      Generic ARM CORTEX-A9 Port
*
* File      : OS_CPU.H
* Version   : V2.92
* By        : NB
*
* For       : ARM Cortex-A9 (ARMv7)
* Mode      : ARM or Thumb
* Toolchain : gcc
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H

#include <os_cpu_cfg.h>

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif


#ifndef  OS_CPU_EXCEPT_STK_SIZE
#define  OS_CPU_EXCEPT_STK_SIZE    128u                         /* Default exception stack size is 128 OS_STK entries.  */
#endif


/*
*********************************************************************************************************
*                                       CONFIGURATION DEFAULTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  OS_CPU_ARM_ENDIAN_LITTLE                         1u
#define  OS_CPU_ARM_ENDIAN_BIG                            2u


#define  OS_CPU_ARM_FP_REG_NBR                           32u

#if (OS_CPU_CFG_ARM_LITTLE_ENDIAN == 1)
#define  OS_CPU_ARM_ENDIAN_TYPE                           OS_CPU_ARM_ENDIAN_LITTLE
#else
#define  OS_CPU_ARM_ENDIAN_TYPE                           OS_CPU_ARM_ENDIAN_BIG
#endif


/*
*********************************************************************************************************
*                                       ARM EXCEPTION DEFINES
*********************************************************************************************************
*/

                                                            /* ARM exception IDs                                      */
#define  OS_CPU_ARM_EXCEPT_RST                         0x00u
#define  OS_CPU_ARM_EXCEPT_UND                         0x01u
#define  OS_CPU_ARM_EXCEPT_SWI                         0x02u
#define  OS_CPU_ARM_EXCEPT_ABORT_PREFETCH              0x03u
#define  OS_CPU_ARM_EXCEPT_ABORT_DATA                  0x04u
#define  OS_CPU_ARM_EXCEPT_RSVD                        0x05u
#define  OS_CPU_ARM_EXCEPT_IRQ                         0x06u
#define  OS_CPU_ARM_EXCEPT_FIQ                         0x07u


                                                            /* ARM exception vectors addresses                        */
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_RST                (OS_CPU_ARM_EXCEPT_RST            * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_UND                (OS_CPU_ARM_EXCEPT_UND            * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_SWI                (OS_CPU_ARM_EXCEPT_SWI            * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_ABORT_PREFETCH     (OS_CPU_ARM_EXCEPT_ABORT_PREFETCH * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_ABORT_DATA         (OS_CPU_ARM_EXCEPT_ABORT_DATA     * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_IRQ                (OS_CPU_ARM_EXCEPT_IRQ            * 0x04u + 0x00u)
#define  OS_CPU_ARM_EXCEPT_VECT_ADDR_FIQ                (OS_CPU_ARM_EXCEPT_FIQ            * 0x04u + 0x00u)

                                                            /* ARM exception handlers addresses                       */
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_RST             (OS_CPU_ARM_EXCEPT_RST            * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_UND             (OS_CPU_ARM_EXCEPT_UND            * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_SWI             (OS_CPU_ARM_EXCEPT_SWI            * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_ABORT_PREFETCH  (OS_CPU_ARM_EXCEPT_ABORT_PREFETCH * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_ABORT_DATA      (OS_CPU_ARM_EXCEPT_ABORT_DATA     * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_IRQ             (OS_CPU_ARM_EXCEPT_IRQ            * 0x04u + 0x20u)
#define  OS_CPU_ARM_EXCEPT_HANDLER_ADDR_FIQ             (OS_CPU_ARM_EXCEPT_FIQ            * 0x04u + 0x20u)

                                                            /* ARM "Jump To Self" asm instruction                     */
#define  OS_CPU_ARM_INSTR_JUMP_TO_SELF                 0xEAFFFFFEu
                                                            /* ARM "Jump To Exception Handler" asm instruction        */
#define  OS_CPU_ARM_INSTR_JUMP_TO_HANDLER              0xE59FF018u

#define  OS_CPU_ARM_BIT_CPSR_N                     (1u  << 31u)
#define  OS_CPU_ARM_BIT_CPSR_Z                     (1u  << 30u)
#define  OS_CPU_ARM_BIT_CPSR_C                     (1u  << 29u)
#define  OS_CPU_ARM_BIT_CPSR_V                     (1u  << 28u)
#define  OS_CPU_ARM_BIT_CPSR_Q                     (1u  << 27u)
#define  OS_CPU_ARM_BIT_CPSR_J                     (1u  << 24u)
#define  OS_CPU_ARM_MSK_CPSR_GE                    (0xF << 16u)

#define  OS_CPU_ARM_BIT_CPSR_E                     (1u << 9u)
#define  OS_CPU_ARM_BIT_CPSR_A                     (1u << 8u)
#define  OS_CPU_ARM_BIT_CPSR_I                     (1u << 7u)
#define  OS_CPU_ARM_BIT_CPSR_F                     (1u << 6u)
#define  OS_CPU_ARM_BIT_CPSR_T                     (1u << 5u)
#define  OS_CPU_ARM_MSK_CPSR_MODE                         0x1Fu
#define  OS_CPU_ARM_BIT_CPSR_MODE_USER                    0x10u
#define  OS_CPU_ARM_BIT_CPSR_MODE_FIQ                     0x11u
#define  OS_CPU_ARM_BIT_CPSR_MODE_IRQ                     0x12u
#define  OS_CPU_ARM_BIT_CPSR_MODE_SUPERVISOR              0x13u
#define  OS_CPU_ARM_BIT_CPSR_MODE_ABORT                   0x17u
#define  OS_CPU_ARM_BIT_CPSR_MODE_UNDEFINED               0x1Bu
#define  OS_CPU_ARM_BIT_CPSR_MODE_SYSTEM                  0x1Fu

#define  OS_CPU_ARM_BIT_FPEXC_EN                   (1u << 30u)


/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */

/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#define  OS_TASK_SW()                           OSCtxSw()
#define  OS_STK_GROWTH                           1u         /* Stack grows from HIGH to LOW memory on ARM        */

/*
*********************************************************************************************************
*                                                ARM
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD    3u


#if      OS_CRITICAL_METHOD == 3u

#if      OS_CPU_INT_DIS_MEAS_EN > 0u

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();  \
                               OS_CPU_IntDisMeasStart();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_IntDisMeasStop();   \
                               OS_CPU_SR_Restore(cpu_sr);}

#else

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}

#endif

#endif

/*
*********************************************************************************************************
*                                       TIMESTAMP CONFIGURATION
*
* Note(s) : (1) OS_TS_GET() is generally defined as CPU_TS_Get32() to allow CPU timestamp timer to be of
*               any data type size.
*
*           (2) For architectures that provide 32-bit or higher precision free running counters 
*               (i.e. cycle count registers):
*
*               (a) OS_TS_GET() may be defined as CPU_TS_TmrRd() to improve performance when retrieving
*                   the timestamp.
*
*               (b) CPU_TS_TmrRd() MUST be configured to be greater or equal to 32-bits to avoid
*                   truncation of TS.
*********************************************************************************************************
*/

#if      OS_CFG_TS_EN == 1u
#define  OS_TS_GET()               (CPU_TS)CPU_TS_TmrRd()   /* See Note #2a.                                          */
#else
#define  OS_TS_GET()               (CPU_TS)0u
#endif


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                            /* Variables used to measure interrupt disable time      */
#if OS_CPU_INT_DIS_MEAS_EN > 0u
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasNestingCtr;
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasCntsEnter;
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasCntsExit;
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasCntsMax;
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasCntsDelta;
OS_CPU_EXT  INT16U   OS_CPU_IntDisMeasCntsOvrhd;
#endif

OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkPtr;    

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u                                /* See OS_CPU_A.ASM                                  */
OS_CPU_SR  OS_CPU_SR_Save                     (void);
void       OS_CPU_SR_Restore                  (OS_CPU_SR cpu_sr);
#endif

void       OS_CPU_SR_INT_Dis                  (void);
void       OS_CPU_SR_INT_En                   (void);
void       OS_CPU_SR_FIQ_Dis                  (void);
void       OS_CPU_SR_FIQ_En                   (void);
void       OS_CPU_SR_IRQ_Dis                  (void);
void       OS_CPU_SR_IRQ_En                   (void);

void       OSCtxSw                            (void);
void       OSIntCtxSw                         (void);
void       OSStartHighRdy                     (void);

void       OS_CPU_ARM_ExceptUndefInstrHndlr   (void);
void       OS_CPU_ARM_ExceptSwiHndlr          (void);
void       OS_CPU_ARM_ExceptPrefetchAbortHndlr(void);
void       OS_CPU_ARM_ExceptDataAbortHndlr    (void);
void       OS_CPU_ARM_ExceptIrqHndlr          (void);
void       OS_CPU_ARM_ExceptFiqHndlr          (void);

void       OS_CPU_IntHandler                  (INT32U  src_id);

INT32U     OS_CPU_ExceptStkChk                (void);

#if OS_CPU_INT_DIS_MEAS_EN > 0u
void       OS_CPU_IntDisMeasInit              (void);
void       OS_CPU_IntDisMeasStart             (void);
void       OS_CPU_IntDisMeasStop              (void);
INT16U     OS_CPU_IntDisMeasTmrRd             (void);
#endif

#if OS_CPU_ARM_DCC_EN > 0u
void       OSDCC_Handler                      (void);
#endif

void       OS_CPU_ARM_CtxID_Set               (INT32U  ctx_id);
INT32U     OS_CPU_ARM_CtxID_Get               (void);
#endif
