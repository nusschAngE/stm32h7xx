
#include "stm32h7xx.h"
#include "main.h"
#include "system.h"

#include "font.h"
#include "delay.h"
#include "usart_printf.h"

#include "sdram_drv.h"
#include "led_drv.h"
#include "lcd_drv.h"
#include "key_drv.h"
#include "SPIFlash_drv.h"
#include "sdcard_drv.h"
#include "touch_drv.h"

int main(void)
{
	uint32_t Value = 0, Count = 0;
	
    /* mcu configuration */
    system_RCCConfig();
    system_SCBCacheConfig();
    system_MPUConfig();

	delay_ms(100);
    /* SDRAM initialize */
    sdram_Init();

    /* system initialize */
    DelayModule_Init();
    PrintfModule_Init(115200);

    printf("hardware init...\r\n");
    /* hardware init */
	lcd_Init();
    led_Init();
    SPIFlash_Init();
	touch_Init();

    printf("system start...\r\n");
    /* user app */

#if (TEMPLATE_DEBUG == 1U)  

/*
*   add demo code here
*/
    led_SetOnOff(&ledRed, TRUE);
    led_SetOnOff(&ledGreen, TRUE);

    printf("demo start...\r\n");
	lcd_ShowString(10, 10, "demo start...", FONT_ASC1608, 0xffff, 0x0000);
    
#if 0
	SDRAM_RWTestFunc();
	SDRAM_RAMTestFunc();
#endif

#if 0
    SPIFlash_RWTest();
#endif    

#if 0
	
#endif

	while(1)
	{
	    led_SetOnOff(&ledRed, TRUE);
	    led_SetOnOff(&ledGreen, FALSE);
		delay_ms(500);
		led_SetOnOff(&ledRed, FALSE);
	    led_SetOnOff(&ledGreen, TRUE);
		delay_ms(500);
	}  
	
#else 
/*
*   add use app code here
*/ 
    while(1)
    {
        
    }
#endif	
    return 0;
}

