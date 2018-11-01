
#include <ucos_ii.h>
#include "app_cfg.h"

#include "public.h"
#include "led_drv.h"
#include "app_task1.h"

/* task body */
void task_test(void *p_arg)
{
    (void)p_arg;
    
    while(1)
    {
        led_Toggle(LED_GREEN);
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}


