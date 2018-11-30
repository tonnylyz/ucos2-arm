//
//  snprintf.h
//  SXXOS
//
//  Created by SXX on 26/11/2017.
//  Copyright © 2017 SXX. All rights reserved.
//

#ifndef snprintf_h
#define snprintf_h

#include <types.h>

size_t vsprintf(char *str, const char *fmt, va_list args);

size_t vsnprintf(char *str, size_t count, const char *fmt, va_list args);

int vasprintf(char **ptr, const char *format, va_list ap);

size_t sprintf(char *str, const char *fmt, ...);

size_t snprintf(char *str, size_t size, const char *format, ...);

int printf(const char *fmt, ...);

#endif /* snprintf_h */
