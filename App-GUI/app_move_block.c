
#include "led.h"
#include "tft_lcd.h"

#include "app_handle.h"

/*  */
#define BLOCK_SIZE      (40U)
static uint16_t x_pos, y_pos;

/* STATIC FUNC */
static void MB_UnloadApp(void);
static bool MB_EvtHandler(UiEvent_t *evt, void *p_arg);

/* APP Content */
AppHandle_t MoveBlockApp = {MB_EvtHandler, MB_UnloadApp};


/*  */
static void MB_UnloadApp(void)
{
	printf("moveblock app unload!!\r\n");
    TFTLCD_FillBlock(0, 0, lcdDev.width, lcdDev.height, 0x0000);
    //App_Deregister(&MoveBlockApp, UI_LEVEL_PLAY);
}

bool MB_EvtHandler(UiEvent_t *evt, void *p_arg)
{
    (void)p_arg;
    bool handled = TRUE;

    switch(evt->code)
    {
        case UI_EVT_IO(IO_EVT_UP, IO_KEY_DOWN):
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0x0000);
            if(y_pos < BLOCK_SIZE) 
            {
                y_pos = lcdDev.height-BLOCK_SIZE-1;
            } 
            else 
            {
                y_pos -= BLOCK_SIZE;
            }                    
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
            break;

        case UI_EVT_IO(IO_EVT_DOWN, IO_KEY_DOWN):
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0x0000);
            if(y_pos+BLOCK_SIZE >= lcdDev.height) 
            {
                y_pos = 0;
            } 
            else 
            {
                y_pos += BLOCK_SIZE;
            }                    
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
            break;

        case UI_EVT_IO(IO_EVT_LEFT, IO_KEY_DOWN):
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0x0000);
            if(x_pos < BLOCK_SIZE) 
            {
                x_pos = lcdDev.width-BLOCK_SIZE-1;
            } 
            else 
            {
                x_pos -= BLOCK_SIZE;
            }                    
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
            break;

        case UI_EVT_IO(IO_EVT_RIGHT, IO_KEY_DOWN):
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0x0000);
            if(x_pos+BLOCK_SIZE > lcdDev.width) 
            {
                x_pos = 0;
            } 
            else 
            {
                x_pos += BLOCK_SIZE;
            }                    
            TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
            break;

		case UI_EVT_TP_PRESS(TP_ACT_DOWN):
			{
				TpEvent_t tpEvent;
				
                TpEvtFromUI(evt, &tpEvent);
                TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0x0000);
                x_pos = tpEvent.xPos;
                y_pos = tpEvent.yPos;
                TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
            }
			break;

        case UI_EVT_SYS(SYS_EVT_TICK_10MS):
            LED_Toggle(LED_RED);
            break;
        case UI_EVT_SYS(SYS_EVT_TICK_1S):
            LED_Toggle(LED_GREEN);
            break;

        default:
            handled = TRUE;
            break;
    }

    return (handled);
}


/* app load */
void MB_LoadApp(void *p_arg)
{
    (void)p_arg;
    printf("moveblock app load!!\r\n");
    x_pos = y_pos = 0;
    TFTLCD_FillBlock(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, 0xffff);
    App_Register(&MoveBlockApp, APP_LEVEL_PLAY);
}








