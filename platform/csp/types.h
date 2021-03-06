
#ifndef UCOS2_TYPES_H
#define UCOS2_TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


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

typedef unsigned int   size_t;

typedef __builtin_va_list va_list;

#define va_start(...) __builtin_va_start(__VA_ARGS__)
#define va_end(...) __builtin_va_end(__VA_ARGS__)
#define va_arg(...) __builtin_va_arg(__VA_ARGS__)

#define NULL ((void*)0)
#define FALSE (0)
#define TRUE (1)

#endif //UCOS2_TYPES_H
