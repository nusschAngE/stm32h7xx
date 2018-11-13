
#include <ucos_ii.h>

#include "temp_track.h"
#include "ds18b20.h"

static uint16_t tempValue = 0;

TempMessage_Typedef tempMesg;
OS_EVENT *tempMbox = NULL;

/********************* PUBLIC FUNCTION ************************/

void temperature_Task(void *p_arg)
{
    uint8_t DS_NextState = 0;
    uint8_t DS_ErrState = 0;

    tempMbox = OSMboxCreate(&tempMesg);
    
    while(1)
    {
        switch (DS_NextState)
        {
            case DS18B20_STA_INIT:
                if(DS18B20_Init(0x2f, 0x2f, DS_TEMP_9BIT))
                {
                    DS_ErrState = DS18B20_ERR_NONE;
                    DS_NextState = DS18B20_STA_CONVERT;
                    OSTimeDlyHMSM(0, 0, 0, 10);
                }
                else
                {
                    DS_ErrState = DS18B20_ERR_INIT;
                    OSTimeDlyHMSM(0, 0, 0, 500);
                }
                break;
                
            case DS18B20_STA_CONVERT:
                if(DS18B20_StartConvert())
                {
                    uint16_t dly = DS18B20_GetConvertTime();
                    
                    DS_ErrState = DS18B20_ERR_NONE;
                    DS_NextState = DS18B20_STA_READ;
                    OSTimeDlyHMSM(0, 0, 0, dly);
                }
                else
                {
                    DS_ErrState = DS18B20_ERR_CONVERT;
                    DS_NextState = DS18B20_STA_INIT;
                    OSTimeDlyHMSM(0, 0, 0, 500);
                }
                break;

            case DS18B20_STA_READ:
                if(DS18B20_ReadTemperature(&tempValue))
                {
                    DS_ErrState = DS18B20_ERR_NONE;
                    DS_NextState = DS18B20_STA_CONVERT;
                    /* send mbox */
                    tempMesg.success = TRUE;
                    tempMesg.value = tempValue;
                    TempSendMbox(&tempMesg);
                    OSTimeDlyHMSM(0, 0, 0, 500);
                }
                else
                {
                    tempValue = 0;
                    tempMesg.success = FALSE;
                    DS_ErrState = DS18B20_ERR_READ;
                    DS_NextState = DS18B20_STA_INIT;
                    OSTimeDlyHMSM(0, 0, 0, 500);
                }
                break;
            default :
                tempValue = 0;
                tempMesg.success = FALSE;
                DS_ErrState = DS18B20_ERR_NONE;
                DS_NextState = DS18B20_STA_INIT;
                OSTimeDlyHMSM(0, 0, 0, 500);
                break;
        }
    }
}

bool TempSendMbox(TempMessage_Typedef *mesg)
{
    if(tempMbox)
    {
        OSMboxPost(tempMbox, mesg);

        return (TRUE);
    }

    return (FALSE);
}

bool TempAcceptMbox(uint16_t *tempVal)
{
    TempMessage_Typedef *mesg = NULL;
    if(tempMbox)
    {
        mesg = (TempMessage_Typedef *)OSMboxAccept(tempMbox);
        if((mesg != NULL) && (mesg->success))
        {
            *tempVal = mesg->value;
            return (TRUE);
        }
    }

    return (FALSE);
}

uint16_t GetTemperatureValue(void)
{
    return tempValue;
}

float GetTemperature(void)
{
    return DS18B20_CaculateTemperature(tempValue);
}








