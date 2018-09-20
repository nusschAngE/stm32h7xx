
#include "stm32h7xx.h"
#include "led.h"
#include "delay.h"
#include "task_led.h"



volatile uint32_t ledTaskTick = 0;


void task_led(void)
{
    while(1)
    {    
        switch(ledRedStatus)
        {
            case LED_ON:
                if(!ledRed.on)
                    LED_Onoff(&ledRed, TRUE);
                break;
            case LED_OFF:
                if(ledRed.on)
                    LED_Onoff(&ledRed, FALSE);
                break;
            case LED_BLINK:
                if(ledTaskTick == 0)
                {
                    ledTaskTick = ledRed.ticks;
                    
                    if(ledRed.on)
                        LED_Onoff(&ledRed, FALSE);
                    else
                        LED_Onoff(&ledRed, TRUE);
                }
                break;
            default : 
                ledTaskTick = 0;
                if(ledRed.on)
                    LED_Onoff(&ledRed, FALSE);
                break;
        }

        switch(ledGreenStatus)
        {
            case LED_ON:
                if(!ledGreen.on)
                    LED_Onoff(&ledGreen, TRUE);
                break;
            case LED_OFF:
                if(ledGreen.on)
                    LED_Onoff(&ledGreen, FALSE);
                break;
            case LED_BLINK:
                if(ledTaskTick == 0)
                {
                    ledTaskTick = ledGreen.ticks;
                    
                    if(ledGreen.on)
                        LED_Onoff(&ledGreen, FALSE);
                    else
                        LED_Onoff(&ledGreen, TRUE);
                }
                break;
            default : 
                ledTaskTick = 0;
                if(ledGreen.on)
                    LED_Onoff(&ledGreen, FALSE);
                break;
        }
    }
}

