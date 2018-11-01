
#ifndef _USART_PRINTF_H
#define _USART_PRINTF_H

#include "public.h"

extern void PrintfModule_Init(uint32_t BaudRate);

extern int printf(const char *format, ...);

#endif


