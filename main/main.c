
#include "stm32h7xx.h"
#include "main.h"
#include "system.h"

#include "font.h"
#include "delay.h"
#include "usart_printf.h"

#include "fsmc_sdram.h"
#include "led.h"
#include "mpu_lcd.h"
#include "keypad.h"
#include "qspi_flash.h"


int main(void)
{
	uint32_t Value = 0, Count = 0;
	
    /* mcu configuration */
    system_RCCConfig();
    system_SCBCacheConfig();
    system_MPUConfig();

    /* SDRAM init firstly */
    sdram_init();

    /* system init */
    delay_ModuleInit();
    usart_PrintfInit(115200);
    /* hardware configuration */
    delay_ms(50);
	printf("system starting...\r\n");
    
    qspiflash_init();
    led_init();
    lcd_init();

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
    led_setOnOff(&ledRed, TRUE);
    led_setOnOff(&ledGreen, TRUE);

    printf("demo start...\r\n");
	lcd_ShowString(10, 10, "demo start...", FONT_ASC1608, 0xffff, 0x0000);
    
#if 0
	SDRAM_RWTestFunc();
	SDRAM_RAMTestFunc();
#endif

#if 1
    qspiflash_RWTest();
#endif    

	while(1)
	{
	    led_setOnOff(&ledRed, TRUE);
	    led_setOnOff(&ledGreen, FALSE);
		delay_ms(500);
		led_setOnOff(&ledRed, FALSE);
	    led_setOnOff(&ledGreen, TRUE);
		delay_ms(500);
	}
#endif	
    return 0;
}

