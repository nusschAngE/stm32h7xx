
#include "app_cfg.h"

#include "public.h"
#include "ir_keypad.h"
#include "irkpd_track.h"

#include "io_event.h"
#include "ui_event.h"
#include "ui_handler.h"

/*  KEY_MAP 
*/
static const IRKey_Map irKeyMap[] = 
{
    {0x01, IO_EVT_RIGHT,   KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {0x02, IO_EVT_DOWN,    KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {0x03, IO_EVT_LEFT,    KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY},
    {0x04, IO_EVT_UP,      KEY_HOLD_DEFAULT_DELAY, KEY_REPEAT_DEFAULT_DELAY}
};
static const uint8_t irKeyMapSize = sizeof(irKeyMap) / sizeof(irKeyMap[0]);

/*  KEY EVENT
*/
static IoEvent irKeyEvt; 
static uint16_t holdCnt;

static inline void IRKpd_SendEvent(IoEvent *evt);
/******************* PUBLIC FUNCTION *********************/
void irkpd_track_Init(void)
{
    holdCnt = 0;
    irKeyEvt.code = 0;
    irKeyEvt.data = 0;
    irKeyEvt.flag = IO_KEY_NONE;
}

void irkpd_Task(void *p_arg)
{
    (void)p_arg;
    uint8_t keyCode = 0, j = 0;
    signed short idx = -1;
    bool sendEvt = FALSE;    

    while(1)
    {
        keyCode = irKey_GetCode();

        idx = -1;
        sendEvt = FALSE;
        if(keyCode)
        {
            for(j = 0; j < irKeyMapSize; j++)
            {
                if(irKeyMap[j].code == keyCode)
                {
                    idx = j;
                    break;
                }
            }

            if(idx >= 0)
            {
                //key press
                if(holdCnt > 64999)
                {
                    holdCnt = 0;
                }
                holdCnt++;

                if((holdCnt >= KEY_DOWN_DEFAULT_DELAY) 
                    && (holdCnt < irKeyMap[idx].holdDelay))
                {
                    if(irKeyEvt.flag == IO_KEY_NONE)
                    {
                        irKeyEvt.code = irKeyMap[idx].key;
                        irKeyEvt.data = 1;
                        irKeyEvt.flag = IO_KEY_DOWN;
                        sendEvt = TRUE;
                    }                    
                }

                if(holdCnt >= irKeyMap[idx].holdDelay)
                {
                    if(irKeyEvt.flag == IO_KEY_DOWN)
                    {
                        irKeyEvt.flag = IO_KEY_HOLD;
                        sendEvt = TRUE;
                    }
                }

                if((holdCnt % irKeyMap[idx].repeatDelay) == 0)
                {
                    if((irKeyEvt.flag == IO_KEY_HOLD) 
                        || (irKeyEvt.flag == IO_KEY_REPEAT))
                    {
                        irKeyEvt.flag = IO_KEY_REPEAT;
                        sendEvt = TRUE;
                    }
                }
            }
        }
        else
        {
            //key release
            if(irKeyEvt.flag == IO_KEY_DOWN)
            {
                irKeyEvt.flag = IO_KEY_UP_BEFORE_HOLD;
                sendEvt = TRUE;
            }

            if((irKeyEvt.flag == IO_KEY_HOLD) 
                || (irKeyEvt.flag == IO_KEY_REPEAT))
            {
                    irKeyEvt.flag = IO_KEY_UP_AFTER_HOLD;
                    sendEvt = TRUE;
            }
            holdCnt = 0;
        }

        if(sendEvt)
        {
            IRKpd_SendEvent(&irKeyEvt);
        }

        if(holdCnt == 0)
        {
            irKeyEvt.code = 0;
            irKeyEvt.data = 0;
            irKeyEvt.flag = IO_KEY_NONE;
        }

        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}



/*****************************/
static inline void IRKpd_SendEvent(IoEvent *evt)
{
    UiSendIoEvent(evt);
}

