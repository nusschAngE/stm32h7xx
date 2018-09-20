
#include "stm32h7xx.h"
#include "main.h"
#include "system.h"

#include "font.h"
#include "delay.h"
#include "usart_printf.h"

#include "fsmc_sdram.h"
#include "led.h"
#include "mcu_lcd.h"
#include "key_pad.h"
#include "qspi_flash.h"

//#include "led_task.h"


int main(void)
{
	uint32_t Value = 0, Count = 0;
	
    /* mcu configuration */
    System_RCCConfig();
    System_SCBCacheConfig();
    System_MPUConfig();

    /* SDRAM init firstly */
    SDRAM_Init();

    /* system init */
    Delay_Init();
    USART_PrintfInit(115200);
    /* hardware configuration */
    delay_ms(50);
	printf("system starting...\r\n");
    
    SPIFlash_Init();
    LED_Init();
    LCD_Init();

    /* user app */

#ifndef TEMPLATE_DEBUG  
/*
*   add use app code here
*/ 
    while(1)
    {
        
    }
    
#else 
/*
*   add demo code here
*/
    LED_Onoff(&ledRed, TRUE);
    LED_Onoff(&ledGreen, TRUE);

    printf("demo start...\r\n");
	LCD_ShowString(10, 10, "demo start...", FONT_ASC1608, 0xffff, 0x0000);
    
#if 0
	SDRAM_RWTestFunc();
	SDRAM_RAMTestFunc();
#endif

#if 1
    SPIFlash_RWTest();
#endif    

	while(1)
	{
	    LED_Onoff(&ledRed, TRUE);
	    LED_Onoff(&ledGreen, FALSE);
		delay_ms(500);
		LED_Onoff(&ledRed, FALSE);
	    LED_Onoff(&ledGreen, TRUE);
		delay_ms(500);
	}
#endif	
    return 0;
}

