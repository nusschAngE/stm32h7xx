
#include "stm32h7xx.h"
#include "system_core_conf.h"

#include "delay.h"
#include "printf.h"
#include "text.h"
#include "my_malloc.h"
#include "ucos_ii.h"

#include "sdram.h"
#include "led.h"
#include "io_expand.h"
#include "lcd.h"
#include "qspi_flash.h"
#include "touch.h"
#include "io_keypad.h"
#include "ir_keypad.h"
#include "ds18b20.h"

#include "app_demo.h"
#ifdef RTOS_uCOS_II
#include "gui_startup.h"
#endif //APP_GUI_SYSTEM

/*  STATIC 
*/


/*  PUBLIC
*/
int main(void)
{
	uint32_t Value = 0, Count = 0, i = 0;
    uint8_t ret = 0, errIndex = 0;

    //uint8_t *tmpBuff = NULL;
	
    /* mcu configuration */
    system_SysTickInit(1000);
    //HAL_MspInit();
#if 0//!defined(ExtSDRAM_ENABLE)
    ExtSDRAM_Init(); 
#endif //ExtSDRAM_ENABLE   	
   	system_SCBCacheConfig();
    //system_MPUConfig();

	//tmpBuff = (uint8_t *)myMalloc(MLCSRC_AXISRAM, 32);
	//if(tmpBuff == NULL) {errIndex = 0; goto Main_ErrHandle;}

    /* TIM delay us/ms */
    ret = DelayModule_Init();
    if(ret != SYSTEM_ERROR_NONE) {HANDLE_ERROR();}
    /* USART1 printf */
    ret = PrintfModule_Init();
    if(ret != SYSTEM_ERROR_NONE) {HANDLE_ERROR();}

    /* system start */
    printf("hardware init...\r\n");

    /* hardware init */
    ret = LED_Init();//LED Init
    if(ret != 0) {errIndex = 1; goto Main_ErrHandle;}
    
	ret = LCD_Init();//LCD Init
	if(ret != 0) {errIndex = 2; goto Main_ErrHandle;}
	
    ret = IOEx_Init();//IO Expand Init
    if(ret != 0) {errIndex = 3; goto Main_ErrHandle;}
    
    ret = QSPI_Flash_Init();//QSPI Flash Init
    if(ret != 0) {errIndex = 4; goto Main_ErrHandle;}
    
    ret = IoKey_Init();//GPIO Key Init
    if(ret != 0) {errIndex = 5; goto Main_ErrHandle;}
    
    ret = IRKey_Init();//IR-remote Key Init
    if(ret != 0) {errIndex = 6; goto Main_ErrHandle;}
    
	ret = TP_Init();//Touch Pad Init
	if(ret != 0) {errIndex = 7; goto Main_ErrHandle;}
    
    ret = HZKFont_Check();//HZK Font Bin
    if(ret != 0) 
    {
        printf("HZK font is not ready, ret = %d\r\n", ret);
    }

    /* hardware init done */
    printf("hardware init done....\r\n");


#ifdef RTOS_uCOS_II
/* GUI app system 
*/  
    printf("app start...\r\n");
    
    GUI_SystemStartup();

#else //!RTOS_uCOS_II    
/*
*   add demo code here
*/
    printf("demo start...\r\n");
	//ShowTextLineAscii(10, 10, "demo start...", FontPreset(1));
    app_demo();
    
#endif//RTOS_uCOS_II

    LED_Onoff(LED_RED, FALSE);
    LED_Onoff(LED_GREEN, FALSE);
    /* while(1) loop */
	while(1)
	{
	    LED_Toggle(LED_RED);
	    LED_Toggle(LED_GREEN);
		TimDelayMs(500);
	}  

Main_ErrHandle:
    printf("handle error, index = %d\r\n", errIndex);
    LED_Onoff(LED_GREEN, FALSE);
    /* while(1) loop */
    while(1)
	{
	    LED_Toggle(LED_GREEN);
		TimDelayMs(200);
	}
    /* never reach here */
    return 0;
}



