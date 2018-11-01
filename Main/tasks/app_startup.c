
#include <ucos_ii.h>
#include "app_cfg.h"

#include "public.h"
#include "led_drv.h"
#include "app_startup.h"

/* task body */
void task_startup(void *p_arg)
{
    (void)p_arg;

    /* enable interrupt */
    OS_CPU_IntEnable();
    
    while(1)
    {
        led_Toggle(LED_RED);
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

