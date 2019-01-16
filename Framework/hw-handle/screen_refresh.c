

#include "ucos_ii.h"


/**** screen task body *****/
void screen_RefreshTask(void *p_arg)
{
    (void)p_arg;

    while(1)
    {
        
        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}

