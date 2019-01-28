
#include "app_demo.h"

#include "delay.h"
#include "led.h"
#include "touch.h"
#include "timer.h"

#include "GUIDemo.h"

#include "GUI.h"
#include "WM.h"
#include "TEXT.h"
#include "BUTTON.h"

#if (MODULE_EMWIN_TEST)

extern volatile GUI_TIMER_TIME OS_TimeMS;

static void GUI_Tim3Callback(void)
{
    OS_TimeMS++;
}

static void GUI_Tim4Callback(void)
{
    bool tpInt = TRUE;

    //tpInt = TP_PointChecked() ? TRUE : FALSE;
    if(tpInt) {
        TP_ReadPoints(&tpPoints);
    } else {
        TP_Release(&tpPoints);
    }

    GUI_TOUCH_Exec();
}

void emWin_Test(void)
{   
    TimInit_t TimInit;
    uint16_t actCount = 0;
    bool tpInt = 0;
    
    printf("emWin test start...\r\n");

    TimInit.arr = 5;//1ms
    TimInit.psc = 40000 - 1;//200us
    TimInit.TriggerFunc = GUI_Tim3Callback;
    TIM3_Init(&TimInit);

    TimInit.arr = 75;//15ms
    TimInit.psc = 40000 - 1;//200us
    TimInit.TriggerFunc = GUI_Tim4Callback;
    TIM4_Init(&TimInit);

    GUI_Init();
    WM_SetCreateFlags(WM_CF_MEMDEV);
    WM_MULTIBUF_Enable(1);

    //TEXT_SetDefaultFont(&GUI_Font6x8);
    //TEXT_SetDefaultTextColor(0x4f4f);
    //TEXT_SetText(textHandle, "emWin Test..");
    //BUTTON_Create(10, 10, 20, 20, 1, BUTTON_CF_HIDE);    

    //GUIDEMO_Main();
    
    while(1)
    {
        /* loop delay */
        LED_Toggle(LED_GREEN);
        TimDelayMs(200);
    }
}




#endif //MODULE_EMWIN_TEST



