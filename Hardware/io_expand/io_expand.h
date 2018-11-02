
#ifndef _IO_EXPAND_H
#define _IO_EXPAND_H

#include "public.h"


/* IO */
#define IO_BEEP_PWR         (0x01)
#define IO_AP32xx_INT       (0x02)
#define IO_DCMI_PWR         (0x04)
#define IO_USB_PWR          (0x08)
#define IO_USER_EX          (0x10)
#define IO_MPU_INT          (0x20)
#define IO_RX485_RE         (0x40)
#define IO_ETH_REST         (0x80)

#define IO_ALL_PIN          (0xFF)

/* public */
void IOExpand_Init(void);
void IOExpand_SetIOx(uint8_t IOx, uint8_t sta);
uint8_t IOExpand_GetIOx(uint8_t IOx);
void IOExpand_IntCheck(void);

#endif

