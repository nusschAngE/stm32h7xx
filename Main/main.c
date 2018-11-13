
#include "stm32h7xx.h"
#include "main.h"
#include "system.h"

#include "font.h"
#include "delay.h"
#include "printf.h"
#include "my_malloc.h"

#include "sdram.h"
#include "led.h"
#include "io_expand.h"
#include "lcd.h"
#include "qspi_flash.h"
#include "sdcard.h"
#include "touch.h"
#include "io_keypad.h"
#include "ir_keypad.h"
#include "ds18b20.h"


#if (RTOS_uCOS_II)
#include <ucos_ii.h>
#include "app_startup.h"

static OS_STK StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
#endif


int main(void)
{
	uint32_t Value = 0, Count = 0, i = 0;
    uint8_t ret = 0;
	
    /* mcu configuration */
    //HAL_MspInit();
    system_RCCConfig();
    system_SCBCacheConfig();
    system_MPUConfig();
	DelayUs(500);

#if (RTOS_uCOS_II)
    system_SysTickInit();
#endif

    ret = DelayModule_Init();
    if(ret == 0)
    {
        SYS_ERROR();
    }
    
    ret = PrintfModule_Init();
    if(ret == 0)
    {
        SYS_ERROR();
    }

	printf("system core frequency = %d\r\n", SystemCoreClock);
    printf("hardware init...\r\n");
    /* hardware init */
    ret = SDRAM_Init();
    if(ret == 0)
    {   
        printf("SDRAM_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
    ret = IOExpand_Init();
    if(ret == 0)
    {   
        printf("IOExpand_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
    ret = SPIFlash_Init();
    if(ret == 0)
    {   
        printf("SPIFlash_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
	ret = LCD_Init();
	if(ret == 0)
    {   
        printf("LCD_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
    ret = LED_Init();
    if(ret == 0)
    {   
        printf("LED_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
    ret = IOKey_Init();
    if(ret == 0)
    {   
        printf("IOKey_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
    ret = IRKey_Init();
    if(ret == 0)
    {   
        printf("IRKey_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
	ret = TOUCH_Init();
	if(ret == 0)
    {   
        printf("TOUCH_Init(),ret error\r\n");
        SYS_ERROR();
    }
    
	ret = SD_Init();
	if(ret == 0)
    {   
        printf("SD_Init(),ret error\r\n");
        SYS_ERROR();
    }

    printf("system start...\r\n");
/* user app */

#if (TEMPLATE_DEBUG == 1U)  

/*
*   add demo code here
*/
    printf("demo start...\r\n");
	LCD_ShowString(10, 10, "demo start...", FONT_ASC1608, 0xffff, 0x0000);
    
#if 0
	SDRAM_RWTestFunc();
	SDRAM_RAMTestFunc();
#endif

#if 0
    SPIFlash_RWTest();
#endif    

#if 1
	SD_Test();
#endif

#if 0
    DS18B20_Test();
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

	while(1)
	{
	    LED_Toggle(LED_RED);
	    LED_Toggle(LED_GREEN);
		TimDelayMs(500);
	}  
	
#else 
/*
*   add user app code here
*/ 
    printf("app start...\r\n");
    LCD_ShowString(10, 10, "app start...", FONT_ASC1608, 0xffff, 0x0000);
#if (RTOS_uCOS_II)
    //system_SysTickInit();

    OSInit();

    /* startup task */
    OSTaskCreateExt(app_startup, 
                    NULL, 
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u], 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    APP_CFG_STARTUP_TASK_PRIO, 
                    &StartupTaskStk[0u], 
                    APP_CFG_STARTUP_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
    OSStart();
#endif
    while(1)
    {
        
    }
#endif	
    return 0;
}

