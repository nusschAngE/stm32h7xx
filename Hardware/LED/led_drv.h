
#ifndef _LED_DRV_H_
#define _LED_DRV_H_

#include "public.h"

#define LED_NONE    0
#define LED_GREEN   1
#define LED_RED     2

/****************** GLOBAL FUNCTION *****************/
extern void led_Init(void);
extern void led_Toggle(uint8_t led);
extern void led_Onoff(uint8_t led, bool on);

#endif

