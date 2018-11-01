
#ifndef _I2C_INTERFACE_H
#define _I2C_INTERFACE_H

#include "public.h"

void i2c_init(void);
uint8_t i2c_SendOneByte(uint8_t Addr, uint8_t sVal);
uint8_t i2c_SendMultipleBytes(uint8_t Addr, uint8_t* pVal, uint32_t size);
uint8_t i2c_ReadOneByte(uint8_t Addr, uint8_t* rVal);
uint8_t i2c_ReadMultipleBytes(uint8_t Addr, uint8_t* pVal, uint32_t size);







#endif

