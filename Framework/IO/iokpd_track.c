
#include "app_cfg.h"

#include "public.h"
#include "io_keypad.h"
#include "iokpd_track.h"


#include "io_event.h"
#include "ui_event.h"
#include "ui_handler.h"

/*  KEY_MAP 
*/
static const IOKey_Map ioKeyMap[] = 
{
    {GPIO_KEY0, IO_EVT_RIGHT,   KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {GPIO_KEY1, IO_EVT_DOWN,    KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {GPIO_KEY2, IO_EVT_LEFT,    KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {GPIO_KEY3, IO_EVT_UP,      KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY}
};
static const uint8_t ioKeyMapSize = sizeof(ioKeyMap) / sizeof(ioKeyMap[0]);

static IoEvent ioKeyEvt[GPIO_KEY_NUM]; 
static uint16_t holdCnt[GPIO_KEY_NUM];

/*****************************/
static inline void IOKpd_SendEvent(IoEvent *evt);

void iokpd_track_Init(void)
{
    pkgMemset(ioKeyEvt, 0, sizeof(ioKeyEvt));
    pkgMemset(holdCnt, 0, sizeof(holdCnt));
}

/* task body */
void iokpd_Task(void *p_arg)
{
    (void)p_arg;
    KEY_SCAN_VAL keyScan = 0;
    uint8_t kpd = 0, j = 0;
    signed short idx = -1;
    bool sendEvt = FALSE;
 
    while(1)
    {
        keyScan = IOKey_GetKey();

        for(kpd = 0; kpd < GPIO_KEY_NUM; kpd++)
        {
            sendEvt = FALSE;
            idx = -1;
            if(keyScan & (1<<kpd))
            {
                for(j = 0; j < ioKeyMapSize; j++)
                {
                    if(ioKeyMap[j].kpd == kpd)
                    {
                        idx = j;
                        break;
                    }
                }
            }
            //key press
            if(idx >= 0)
            {
                if(holdCnt[kpd] >= 64999)
                {
                    holdCnt[kpd] = 0;
                }
                holdCnt[kpd]++;

                if((holdCnt[kpd] >= KEY_DOWN_DEFAULT_DELAY) 
                    && (holdCnt[kpd] < ioKeyMap[idx].holdDelay))
                {
                    if(ioKeyEvt[kpd].flag == IO_KEY_NONE)
                    {
                        ioKeyEvt[kpd].code = ioKeyMap[idx].key;
                        ioKeyEvt[kpd].flag = IO_KEY_DOWN;
                        sendEvt = TRUE;
                    }
                }

                if(holdCnt[kpd] >= ioKeyMap[idx].holdDelay)
                {
                    if(ioKeyEvt[kpd].flag == IO_KEY_DOWN)
                    {
                        ioKeyEvt[kpd].flag = IO_KEY_HOLD;
                        sendEvt = TRUE;
                    }
                }

                if((holdCnt[kpd] % ioKeyMap[idx].repeatDelay) == 0)
                {
                    if((ioKeyEvt[kpd].flag == IO_KEY_HOLD) || (ioKeyEvt[kpd].flag == IO_KEY_REPEAT))
                    {
                        ioKeyEvt[kpd].flag = IO_KEY_REPEAT;
                        sendEvt = TRUE;
                    }
                }
            }
            else
            {
            //key release
                if(ioKeyEvt[kpd].flag == IO_KEY_DOWN)
                {
                    ioKeyEvt[kpd].flag = IO_KEY_UP_BEFORE_HOLD;
                    sendEvt = TRUE;
                }

                if((ioKeyEvt[kpd].flag == IO_KEY_HOLD) || (ioKeyEvt[kpd].flag == IO_KEY_REPEAT))
                {
                    ioKeyEvt[kpd].flag = IO_KEY_UP_AFTER_HOLD;
                    sendEvt = TRUE;
                }

                holdCnt[kpd] = 0;
            }

            if(sendEvt)
            {
                IOKpd_SendEvent(&ioKeyEvt[kpd]);
            }

            if(holdCnt[kpd] == 0)
            {
                ioKeyEvt[kpd].code = 0;
                ioKeyEvt[kpd].flag = IO_KEY_NONE;
                ioKeyEvt[kpd].data = 0;
            }    
        }

        /* task delay */
        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}

/*****************************/
static inline void IOKpd_SendEvent(IoEvent *evt)
{
    UiSendIoEvent(evt);
}

