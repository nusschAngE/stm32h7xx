
#include <ucos_ii.h>
#include "app_cfg.h"

#include "stm32h7xx.h"
#include "public.h"
#include "led.h"
#include "iokpd_track.h"
#include "irkpd_track.h"
#include "tp_track.h"

#include "app_startup.h"
#include "gui_task.h"

/* IO kpd task stack */
__ALIGNED(4) OS_STK iokpdTaskStk[APP_CFG_IOKPD_TASK_STK_SIZE];

/* tp task stack */
__ALIGNED(4) OS_STK tpTaskStk[APP_CFG_TP_TASK_STK_SIZE];

/* ui screen task stack */
__ALIGNED(4) OS_STK guiTaskStk[APP_CFG_GUI_TASK_STK_SIZE];

/* task body */
void app_startup(void *p_arg)
{
    (void)p_arg;
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();

    /* ui init */
	gui_Init();
	
    /* create tasks */
    OSTaskCreateExt(iokpd_Task, 
                    NULL, 
                    &iokpdTaskStk[APP_CFG_IOKPD_TASK_STK_SIZE - 1u], 
                    APP_CFG_IOKPD_TASK_PRIO, 
                    APP_CFG_IOKPD_TASK_PRIO, 
                    &iokpdTaskStk[0u], 
                    APP_CFG_IOKPD_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK);

    OSTaskCreateExt(tp_Task, 
                    NULL, 
                    &tpTaskStk[APP_CFG_TP_TASK_STK_SIZE - 1u], 
                    APP_CFG_TP_TASK_PRIO, 
                    APP_CFG_TP_TASK_PRIO, 
                    &tpTaskStk[0u], 
                    APP_CFG_TP_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK);
                   
    OSTaskCreateExt(gui_Task, 
                    NULL, 
                    &guiTaskStk[APP_CFG_GUI_TASK_STK_SIZE - 1u], 
                    APP_CFG_GUI_TASK_PRIO, 
                    APP_CFG_GUI_TASK_PRIO, 
                    &guiTaskStk[0u], 
                    APP_CFG_GUI_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK);

    /* delete startup task */
    OSTaskDel(APP_CFG_STARTUP_TASK_PRIO);

    OS_EXIT_CRITICAL();

    while(1)
    {
        led_Onoff(LED_RED, FALSE);
        led_Onoff(LED_GREEN, FALSE);
        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}

