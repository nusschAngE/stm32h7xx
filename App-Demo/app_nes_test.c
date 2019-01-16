
#include "app_demo.h"
#include "io_keypad.h"
#include "lcd.h"
#include "audio_api.h"

#include "InfoNES_System.h"
#include "InfoNES.h"
#include "fatfs_ex.h"
#include "my_malloc.h"
#include "delay.h"


#if (MODULE_NES_TEST)
extern uint8_t InfoNES_BuffersInit(void);

void InfoNes_Test(void)
{
    //uint8_t key = GPIO_KEY_NUM;
    uint8_t ret = 0;

    /* Mount SDCard */
    ret = f_mount(&SDFatFS, SdFsDrv, 1);
    if(ret != FR_OK)
    {
        printf("mount sdcard error\r\n");
        return ;
    }

    /* buffer malloc */
    ret = InfoNES_BuffersInit(); 
    if(ret != 0)
    {
        printf("NES_AppInit() error, ret = %d\r\n", ret);
        return ;
    }

    /* load nes rom */
    if(InfoNES_Load((char *)"0:/NES/test.nes") != 0)
    {
		InfoNES_MessageBox("load rom failed!");
		return ;
	}    
	TimDelayMs(3000);
    /* Audio init */
    ret += Audio_Init();
    ret += Audio_SetMute(TRUE);
    ret += Audio_SetVolume(35);
    ret += Audio_SetChannel(CODEC_OUTPUT_SPK);
    if(ret != 0)
    {
        printf("audio driver init error, ret = %d\r\n", ret);
        return ;
    }

    /* screen init */
    LCD_Clear(COLOR_BLACK);
    LCD_SetWorkRegion(0, 0, NES_DISP_WIDTH, NES_DISP_HEIGHT);
    LCD_StartDataSession();    
    
    /* Initialize InfoNES */
    InfoNES_Init();

    while(1)
    {
        /*-------------------------------------------------------------------*/
        /*  To the menu screen                                               */
        /*-------------------------------------------------------------------*/
        if ( InfoNES_Menu() == -1 )
          break;  // Quit
        
        /*-------------------------------------------------------------------*/
        /*  Start a NES emulation                                            */
        /*-------------------------------------------------------------------*/
        InfoNES_Cycle();

        /* key scan */
        //key = IoKey_Scan();
        //if(key == GPIO_KEY3) {keyUp = TRUE;}
        //if(key == GPIO_KEY1) {keyDown = TRUE;}
        //if(key == GPIO_KEY2) {keyLeft = TRUE;}
        //if(key == GPIO_KEY0) {keyRight = TRUE;}
        //if(key == GPIO_KEY_NUM) {keyUp = keyDown = keyLeft = keyRight = FALSE;}
        
    }
    // Completion treatment
    InfoNES_Fin();
}

#endif //MODULE_NES_TEST

