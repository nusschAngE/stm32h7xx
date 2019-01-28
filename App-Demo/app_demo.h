
#ifndef _APP_DEMO_H
#define _APP_DEMO_H

#include "public.h"

/*  
*   module : PRINTF, LED, IOKEY, IRKEY, LCD, SDRAM, QSPI FLASH, HZK FONT, TOUCH
*            .
*            .
*            .
*/
//#define MODULE_SDRAM_TEST 		    1U
//#define MODULE_QSPIFLASH_TEST       1U
//#define MODULE_SDCARD_TEST          1U
//#define MODULE_MYMALLOC_TEST        1U
//#define MODULE_SDFATFS_TEST         1U
//#define MODULE_WAVDECODE_TEST       1U
//#define MODULE_HZKFONT_TEST         1U
//#define MODULE_DS18B20_TEST         1U
//#define MODULE_ESP8266_TEST		    1U
//#define MODULE_UART_RXTX_TEST       1U
//#define MODULE_USBHOST_TEST         1U
//#define MODULE_TP_TSET			    1U
#define MODULE_EMWIN_TEST           1U

void app_demo(void);

void SDRAM_Test(void);
void QSPI_Flash_Test(void);
void SDCard_Test(void);
void MyMalloc_Test(void);
void SDFatFS_Test(void);
void HZKFont_Test(void);
void WavDecode_Test(void);
void DS18B20_Test(void);
void ESP8266_Test(void);
void ATKUart_Test(void);
void USB_Host_Test(void);
void Touch_Test(void);
void emWin_Test(void);

#endif

