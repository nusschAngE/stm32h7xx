
#include "stm32h7xx.h"
#include "usart_printf.h"

/* USART Handler */
static UART_HandleTypeDef DBGUart_Handler;

/***************** private ********************/
/*
*   
*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
	
    GPIO_Init.Pin = GPIO_PIN_9 | GPIO_PIN_10;		
    GPIO_Init.Mode = GPIO_MODE_AF_PP;		
    GPIO_Init.Pull = GPIO_PULLUP;			
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init.Alternate = GPIO_AF7_USART1;	
    HAL_GPIO_Init(GPIOA, &GPIO_Init);	   		  
}

static void DebugUSART_Init(uint32_t BaudRate)
{
	DBGUart_Handler.Instance = USART1;					
	DBGUart_Handler.Init.BaudRate = BaudRate;				  
	DBGUart_Handler.Init.WordLength = UART_WORDLENGTH_8B;  
	DBGUart_Handler.Init.StopBits = UART_STOPBITS_1;	   
	DBGUart_Handler.Init.Parity = UART_PARITY_NONE;		  
	DBGUart_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   
	DBGUart_Handler.Init.Mode = UART_MODE_TX_RX;	
	DBGUart_Handler.Init.Prescaler = UART_PRESCALER_DIV12;//20mhz
	DBGUart_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	DBGUart_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	DBGUart_Handler.Init.FIFOMode = UART_FIFOMODE_DISABLE;
	HAL_UART_Init(&DBGUart_Handler);		   
}

/*********** usart debug configuration *************/

void PrintfModule_Init(uint32_t BaudRate)
{
    DebugUSART_Init(BaudRate);
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
    //__HAL_UART_CLEAR_FLAG(&DBGUart_Handler, UART_FLAG_TC);

    while(__HAL_UART_GET_FLAG(&DBGUart_Handler, UART_FLAG_TXE) == RESET)
    {
    }
    
    DBGUart_Handler.Instance->TDR = (uint8_t) ch;      
    
	return ch;
}


