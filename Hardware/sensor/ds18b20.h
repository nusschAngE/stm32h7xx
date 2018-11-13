
#ifndef _DS18B20_H
#define _DS18B20_H

#include "public.h"

/* Precise */
#define DS_TEMP_9BIT            (0)
#define DS_TEMP_10BIT           (1)
#define DS_TEMP_11BIT           (2)
#define DS_TEMP_12BIT           (3)

typedef float   DS18B20_TEMP;

/* PUBLIC FUNCTION */
uint8_t DS18B20_Init(uint8_t th, uint8_t tl, uint8_t precise);
uint8_t DS18B20_StartConvert(void);
uint8_t DS18B20_ReadTemperature(uint16_t *tempVal);
float DS18B20_CaculateTemperature(uint16_t tempVal);
uint16_t DS18B20_GetConvertTime(void);
char *DS18B20_Temp2String(float temp);

#endif

