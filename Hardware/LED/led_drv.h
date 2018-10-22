
#ifndef _LED_DRV_H_
#define _LED_DRV_H_

#include "public.h"

/* led device
*/
typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t      Pinx;
}_ledDev;


/* led device struct init */
#define LED_DEVICE_FIELD(gpiox, pinx) {gpiox, pinx}


/* global */
extern _ledDev ledRed;
extern _ledDev ledGreen;

/* global function */
extern void led_Init(void);
extern void led_SetOnOff(_ledDev *led, bool on);


#endif

