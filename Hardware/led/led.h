
#ifndef _LED_H
#define _LED_H

#include "public.h"

typedef enum
{
    LED_INIT = 0,
    LED_ON,
    LED_BLINK,
    LED_OFF,

    LED_STATE_CNT
}LEDState_Enum;

typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t      Pinx;
    uint8_t       on;
    uint32_t      ticks;
}LEDDev_Struct;


/* led device struct init */
#define LED_DEVICE_FIELD(gpiox, pinx, on, ticks) {gpiox, pinx, on, ticks}


/* global */
extern LEDDev_Struct ledRed;
extern LEDDev_Struct ledGreen;
volatile extern uint8_t ledRedStatus;
volatile extern uint8_t ledGreenStatus;

/* global function */
extern void LED_Init(void);
extern void LED_Onoff(LEDDev_Struct *led, uint8_t on);
#if 0
extern void led_blink(LEDDev_Struct *led, uint8_t on, uint32_t ticks);
#endif


#endif

