
#ifndef _SDRAM_H
#define _SDRAM_H

#include "public.h"

#define SDRAM_SIZE              ((uint32_t)(32 * 1024 * 1024))//((uint32_t)0x2000000) //32MByte
/*  */
#define SDRAM_ADDRESS           ((uint32_t)0xc0000000)/* FMC Bank5 SDRAM */


extern void sdram_Init(void);

#endif

