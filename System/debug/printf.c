
#include <stdio.h>
#include "uart_printf.h"



/*********** usart debug configuration *************/

void PrintfModule_Init(void)
{
    UartPrintf_Init(PRINTF_BAUDRATE);
}

/********** printf config ***********/
//#pragma import(__use_no_semihosting)             
              
//struct __FILE 
//{ 
//	int handle; 
//}; 

FILE __stdout;       
  
void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{ 	
	UartPrintf_SendChar((uint8_t)ch);
	
	return ch;
}


