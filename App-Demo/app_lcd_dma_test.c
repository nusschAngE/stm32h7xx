
#include "app_demo.h"
#include "delay.h"
#include "lcd.h"
#include "led.h"
#include "io_keypad.h"
#include "icon.h"
#include "lcd_gram.h"



#if (MODULE_LCD_DMA_TEST)

void LCD_DMA_Test(void)
{
    uint8_t ret = 0;
    uint8_t key = GPIO_KEY_NUM;
    uint16_t TickCnt = 0;
    uint16_t x_pos = 20, y_pos = 20;
    LCD_COLOR color = 0;

    printf("## LCD DMA Test ##\r\n");
    ret = LCDGram_DisplayInit();
    if(ret != 0) 
    {
        printf("LCD DMA Channel Init error\r\n");
        goto LCD_DMA_Test_End;
    }

    while(1)
    {
        key = IoKey_Scan();
        if(key == GPIO_KEY3)//up
        {        	
            color += 100;
            LCDGram_FillBlock(x_pos, y_pos, 40, 40, color);
        }
        if(key == GPIO_KEY2)//left
        {
        }
        if(key == GPIO_KEY1)//down
        {        	
            color -= 100;
            LCDGram_FillBlock(x_pos, y_pos, 40, 40, color);
        }
        if(key == GPIO_KEY0)//right
        {
        }

        TimDelayMs(10);
        if(++TickCnt == 5)
        {
            LCDGram_FrameDisplay();
        }
        
        if(++TickCnt == 50)
        {
            TickCnt = 0;
            LED_Toggle(LED_GREEN);
        }
    }

LCD_DMA_Test_End:
    printf("## LCD DMA Test End ##\r\n");
}




#endif //MODULE_LCD_DMA_TEST

