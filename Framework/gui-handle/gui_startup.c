
#include "stm32h7xx.h"
#include "ucos_ii.h"
#include "fatfs_ex.h"

#include "kpd_track.h"
#include "tp_track.h"
#include "usb_host_track.h"

#include "screen_refresh.h"
#include "ui_applet.h"
#include "ui_screen.h"
#include "ui_event.h"
#include "gui_app_handle.h"
#include "gui_startup.h"

#include "app_move_block.h"
#include "base_home.h"

#ifdef RTOS_uCOS_II
/* task stack */

__ALIGNED(4) OS_STK StartupTaskStk[STARTUP_TASK_STK_SIZE];

__ALIGNED(4) OS_STK AppGUITaskStk[APP_GUI_TASK_STK_SIZE];
__ALIGNED(4) OS_STK KpdTaskStk[KPD_TASK_STK_SIZE];
__ALIGNED(4) OS_STK TpTaskStk[TP_TASK_STK_SIZE];

__ALIGNED(4) OS_STK LedTaskStk[LED_TASK_STK_SIZE];

/* test led task */
//#define TEST_LED_TASK   
#ifdef TEST_LED_TASK
#include "led.h"

void app_led(void *p_arg)
{
    (void)p_arg;

    while(1)
    {
        LED_Toggle(LED_RED);
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}
#endif //TEST_LED_TASK

static void GUI_AppStartupTask(void *p_arg)
{
    (void)p_arg;
    uint8_t ret = 0;
#if OS_CRITICAL_METHOD == 3u    
    OS_CPU_SR cpu_sr;
#endif
    /*  */
    OS_ENTER_CRITICAL();
    /* Init */
    //USB_HostTrackInit();
    kpd_TrackInit();
    UiAppInit();

    ret = UiEvtInit();
    if(ret != 0) 
    {
        printf("UiEvtInit(), error!!\r\n");
    }

    //ret = SDCard_Mount();
    //if(ret != 0) 
    //{
    //    printf("SDCard_Mount(), error, ret = %d!!\r\n", ret);
    //}
	
    /*  load app */
    //Home_LoadApp();
    MoveBlock_LoadApp(NULL);
    /* create task */
#if 1
	/* app handler task */
	OSTaskCreateExt(app_HandleTask, 
					NULL, 
					&AppGUITaskStk[APP_GUI_TASK_STK_SIZE - 1u], 
					APP_GUI_TASK_PRIO, 
					APP_GUI_TASK_PRIO, 
					&AppGUITaskStk[0u], 
					APP_GUI_TASK_STK_SIZE, 
					NULL, 
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif 
#if 1
	/* key track task */
	OSTaskCreateExt(kpd_TrackTask, 
					NULL, 
					&KpdTaskStk[KPD_TASK_STK_SIZE - 1u], 
					KPD_TASK_PRIO, 
					KPD_TASK_PRIO, 
					&KpdTaskStk[0u], 
					KPD_TASK_STK_SIZE, 
					NULL, 
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif
#if 1
    /* tp track task */
    OSTaskCreateExt(tp_TrackTask, 
                    NULL, 
                    &TpTaskStk[TP_TASK_STK_SIZE - 1u], 
                    TP_TASK_PRIO, 
                    TP_TASK_PRIO, 
                    &TpTaskStk[0u], 
                    TP_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif
#ifdef TEST_LED_TASK   
    /* led test task */
    OSTaskCreateExt(app_led, 
                    NULL, 
                    &LedTaskStk[LED_TASK_STK_SIZE - 1u], 
                    LED_TASK_PRIO, 
                    LED_TASK_PRIO, 
                    &LedTaskStk[0u], 
                    LED_TASK_STK_SIZE, 
                    NULL, 
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif//TEST_LED_TASK
    /* delete startup task */
    OSTaskDel(STARTUP_TASK_PRIO);
    OS_EXIT_CRITICAL();

    while(1)
    {
        OSTimeDlyHMSM(0, 0, 10, 0);
    }
}

/************ PUBLIC FUNCTION *****************/
void GUI_SystemStartup(void)
{    
    OSInit();
#if 1
	/* app handler task */
	OSTaskCreateExt(GUI_AppStartupTask, 
					NULL, 
					&StartupTaskStk[STARTUP_TASK_STK_SIZE - 1u], 
					STARTUP_TASK_PRIO, 
					STARTUP_TASK_PRIO, 
					&StartupTaskStk[0u], 
					STARTUP_TASK_STK_SIZE, 
					NULL, 
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif
    OSStart();
}

#endif //RTOS_uCOS_II

