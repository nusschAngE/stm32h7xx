
#ifndef _TEMP_TRACK_H
#define _TEMP_TRACK_H

#include "public.h"


enum
{
    DS18B20_STA_RESET = 0,
    DS18B20_STA_INIT,
    DS18B20_STA_CONVERT,
    DS18B20_STA_READ,
};

enum
{
    DS18B20_ERR_NONE = 0,
    DS18B20_ERR_INIT,
    DS18B20_ERR_CONVERT,
    DS18B20_ERR_READ,
};

typedef struct
{
    bool success;
    uint16_t value;
}TempMessage_Typedef;

extern OS_EVENT *tempMbox;

/* PUBLIC FUNCTION */
extern void temperature_Task(void *p_arg);
extern bool TempSendMbox(TempMessage_Typedef *mesg);
extern bool TempAcceptMbox(uint16_t *tempVal);
extern uint16_t GetTemperatureValue(void);
extern float GetTemperature(void);


#endif

