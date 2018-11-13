
#ifndef _UART_PRINTF_H
#define _UART_PRINTF_H

#include "public.h"



#define PRINTF_BAUDRATE         (115200)

extern uint8_t UartPrintf_Init(uint32_t BaudRate);
extern void UartPrintf_SendChar(uint8_t ch);


#endif

