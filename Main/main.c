
#include "stm32h7xx.h"
#include "main.h"
<<<<<<< HEAD
=======
#include "public.h"
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
#include "system.h"

#include "font.h"
#include "delay.h"
<<<<<<< HEAD
#include "printf.h"
#include "my_malloc.h"

#include "sdram.h"
#include "led.h"
#include "lcd.h"
#include "qspi_flash.h"
#include "sdcard.h"
#include "touch.h"
#include "io_keypad.h"
#include "ir_keypad.h"


#if (RTOS_uCOS_II)
#include <ucos_ii.h>
#include "app_startup.h"

static OS_STK StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
=======
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

>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
#endif


int main(void)
{
	uint32_t Value = 0, Count = 0, i = 0;
	
    /* mcu configuration */
<<<<<<< HEAD
    //HAL_MspInit();
    system_RCCConfig();
    system_SCBCacheConfig();
    system_MPUConfig();
	uSleep(500);

    DelayModule_Init();
    PrintfModule_Init();

    printf("hardware init...\r\n");
    /* hardware init */
    sdram_Init();
    spiFlash_Init();
	lcd_Init();
    led_Init();
    ioKey_Init();
    irKey_Init();
	touch_Init();
	//sdcard_Init();
=======
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
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

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
<<<<<<< HEAD
    spiFlash_RWTest();
#endif    

#if 0
    touchScan_Task();
#endif

#if 0
	sdcard_Test();
#endif

#if 0
	{
		uint8_t *ptrDTCM, *ptrSRAM, *ptrSDRAM;
		ptrDTCM = (uint8_t *)myMalloc(MALLOC_SRC_DTCM, 128);
		printf("ptrDTCM = %p\r\n", ptrDTCM);
		myFree(MALLOC_SRC_DTCM, ptrDTCM);
		
		ptrSRAM = (uint8_t *)myMalloc(MALLOC_SRC_SRAM, 128);
		printf("ptrSRAM = %p\r\n", ptrSRAM);
		myFree(MALLOC_SRC_SRAM, ptrSRAM);

		ptrSDRAM = (uint8_t *)myMalloc(MALLOC_SRC_SDRAM, 128);
		printf("ptrSDRAM = %p\r\n", ptrSDRAM);
		myFree(MALLOC_SRC_SDRAM, ptrSDRAM);
	}
#endif	

=======
    SPIFlash_RWTest();
#endif    

#if 1
    touchScan_Task();
#endif

>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
	while(1)
	{
	    led_Toggle(LED_RED);
	    led_Toggle(LED_GREEN);
		uSleep(500000);
	}  
	
#else 
/*
<<<<<<< HEAD
*   add user app code here
*/ 
    printf("app start...\r\n");
    lcd_ShowString(10, 10, "app start...", FONT_ASC1608, 0xffff, 0x0000);
#if (RTOS_uCOS_II)
=======
*   add use app code here
*/ 
    printf("app start...\r\n");
    lcd_ShowString(10, 10, "app start...", FONT_ASC1608, 0xffff, 0x0000);
#if (RTOS_uCOS_II == 1U)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
    system_SysTickInit();

    OSInit();

    /* startup task */
<<<<<<< HEAD
    OSTaskCreateExt(app_startup, 
=======
    OSTaskCreateExt(task_startup, 
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
                    NULL, 
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u], 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    &StartupTaskStk[0u], 
                    APP_CFG_STARTUP_TASK_STK_SIZE, 
                    NULL, 
<<<<<<< HEAD
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
=======
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

>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
    
    OSStart();
#endif
    while(1)
    {
        
    }
#endif	
    return 0;
}

