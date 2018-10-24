
#include "stm32h7xx.h"
#include "main.h"
#include "public.h"
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

#if (RTOS_uCOS_II == 1U)
#include <ucos_ii.h>
#include "app_startup.h"
#include "app_task1.h"

static OS_STK StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
static OS_STK TestTaskStk[APP_CFG_TEST_TASK_STK_SIZE];

#endif


int main(void)
{
	uint32_t Value = 0, Count = 0, i = 0;
	
    /* mcu configuration */
    system_RCCConfig();
    system_SCBCacheConfig();
    system_MPUConfig();

	uSleep(500);
    /* SDRAM initialize */
    sdram_Init();

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
    printf("demo start...\r\n");
	lcd_ShowString(10, 10, "demo start...", FONT_ASC1608, 0xffff, 0x0000);
    
#if 0
	SDRAM_RWTestFunc();
	SDRAM_RAMTestFunc();
#endif

#if 0
    SPIFlash_RWTest();
#endif    

#if 1
    touchScan_Task();
#endif

	while(1)
	{
	    led_Toggle(LED_RED);
	    led_Toggle(LED_GREEN);
		uSleep(500000);
	}  
	
#else 
/*
*   add use app code here
*/ 
    printf("app start...\r\n");
    lcd_ShowString(10, 10, "app start...", FONT_ASC1608, 0xffff, 0x0000);
#if (RTOS_uCOS_II == 1U)
    system_SysTickInit();

    OSInit();

    /* startup task */
    OSTaskCreateExt(task_startup, 
                    NULL, 
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u], 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    &StartupTaskStk[0u], 
                    APP_CFG_STARTUP_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK);

    OSTaskCreateExt(task_test, 
                    NULL, 
                    &TestTaskStk[APP_CFG_TEST_TASK_STK_SIZE - 1u], 
                    APP_CFG_TEST_TASK_PRIO, 
                    APP_CFG_TEST_TASK_PRIO, 
                    &TestTaskStk[0u], 
                    APP_CFG_TEST_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK);

    
    OSStart();
#endif
    while(1)
    {
        
    }
#endif	
    return 0;
}

