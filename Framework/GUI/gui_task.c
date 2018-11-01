
#include "stm32h7xx.h"
#include <ucos_ii.h>

#include "ui_event.h"
#include "io_event.h"
#include "ui_handler.h"
#include "ui_screen.h"
#include "ui_apps.h"

#include "iokpd_track.h"
#include "irkpd_track.h"
#include "tp_track.h"
#include "lcd.h"

/* TEST */
bool UiAppIODemo(UiEvent *evt, void *arg);
bool UiAppTPDemo(UiEvent *evt, void *arg);

/*********************** GLOBAL FUNCTION ***********************/
void gui_Init(void)
{
    iokpd_track_Init();
    irkpd_track_Init();
    tp_track_Init();
    UiHandler_Init();
    UiApp_Init();
    UiScreen_Init();
}

/* task body */
void gui_Task(void *p_arg)
{
    (void)p_arg;
    static uint8_t i = 0;
    static bool handled = FALSE, ret;
    static UiEvent evt;
    
    while(1)
    {
        ret = UiGetUiEvent(&evt);
#if 1
		if(ret)
		{
			handled = UiAppIODemo(&evt, NULL);
			handled = UiAppTPDemo(&evt, NULL);
		}
#else
        if(ret)
        {
            handled = UiScreenHandleEvent(&evt, NULL);
        }

        if(handled == FALSE)
        {
            handled = UiAppHandlerEvent(&evt, NULL);
        }

        UiScreenRefresh();
#endif
        if(handled)
        {
            evt.code = 0;
            evt.data = NULL;
        }
        
    /* task delay */
        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}


bool UiAppIODemo(UiEvent *evt, void *arg)
{
	static int prev_x = 0, prev_y = 0;

	switch (evt->code)
	{
		case UI_EVT_IO(IO_EVT_UP, IO_KEY_DOWN)://up
		case UI_EVT_IO(IO_EVT_UP, IO_KEY_REPEAT):
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_y -= 40;
			if(prev_y < 0)
			{
				prev_y = DEFAULT_LCD_HEIGHT - 40;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0xffff);	
			break;

		case UI_EVT_IO(IO_EVT_DOWN, IO_KEY_DOWN)://down 
		case UI_EVT_IO(IO_EVT_DOWN, IO_KEY_REPEAT):
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_y += 40;
			if(prev_y >= DEFAULT_LCD_HEIGHT)
			{
				prev_y = 0;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0xffff);	
			break;

		
		case UI_EVT_IO(IO_EVT_LEFT, IO_KEY_DOWN)://left 
		case UI_EVT_IO(IO_EVT_LEFT, IO_KEY_REPEAT):
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_x -= 40;
			if(prev_x < 0)
			{
				prev_x = DEFAULT_LCD_WIDTH - 40;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0xffff);	
			break;
		
		case UI_EVT_IO(IO_EVT_RIGHT, IO_KEY_DOWN)://right 
		case UI_EVT_IO(IO_EVT_RIGHT, IO_KEY_REPEAT):
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_x += 40;
			if(prev_x >= DEFAULT_LCD_WIDTH)
			{
				prev_x = 0;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0xffff);	
			break;
	}

	return TRUE;
}

bool UiAppTPDemo(UiEvent *evt, void *arg)
{
	static int prev_x = 0, prev_y = 0;
	static LCD_COLOR color = 0xffff;

	switch (evt->code)
	{
		case UI_EVT_TP_GESTURE(TP_GESTURE_SLIDE_UP)://up
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_y -= 40;
			if(prev_y < 0)
			{
				prev_y = DEFAULT_LCD_HEIGHT - 40;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
			break;

		case UI_EVT_TP_GESTURE(TP_GESTURE_SLIDE_DOWN)://down 
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_y += 40;
			if(prev_y >= DEFAULT_LCD_HEIGHT)
			{
				prev_y = 0;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
			break;

		
		case UI_EVT_TP_GESTURE(TP_GESTURE_SLIDE_LEFT)://left 
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_x -= 40;
			if(prev_x < 0)
			{
				prev_x = DEFAULT_LCD_WIDTH - 40;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
			break;
		
		case UI_EVT_TP_GESTURE(TP_GESTURE_SLIDE_RIGHT)://right 
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
			prev_x += 40;
			if(prev_x >= DEFAULT_LCD_WIDTH)
			{
				prev_x = 0;
			}
			lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
			break;

	    case UI_EVT_TP_PRESS(TP_KEY_UP_BEFORE_HOLD):
            //lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
            //UiTpEventGetXY(evt, &prev_x, &prev_y);
			//lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
	        break;

	    case UI_EVT_TP_PRESS(TP_KEY_HOLD):
	    case UI_EVT_TP_PRESS(TP_KEY_REPEAT):
            //lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
            //if(color == 0)
            //    color = 0xffff;
            //color <<= 1;
			//lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
	        break;

        //case UI_EVT_TP_PRESS(TP_KEY_UP_BEFORE_HOLD):
	    case UI_EVT_TP_PRESS(TP_KEY_UP_AFTER_HOLD):
            //lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, 0x0000);
            //color = 0xffff;
			//lcd_FieldBlock((uint16_t)prev_x, (uint16_t)prev_y, 40, 40, color);	
	        break;
	}

	return TRUE;
}





