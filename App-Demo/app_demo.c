
#include "stm32h7xx.h"
#include "app_demo.h"


void app_demo(void)
{
#if (MODULE_SDRAM_TEST)
	SDRAM_Test();
#endif //MODULE_SDRAM_TEST
#if (MODULE_QSPIFLASH_TEST)
    QSPI_Flash_Test();
#endif //MODULE_QSPIFLASH_TEST    
#if (MODULE_SDCARD_TEST)
	SDCard_Test();
#endif //MODULE_SDCARD_TEST
#if (MODULE_SDFATFS_TEST)
    SDFatFS_Test();
#endif //MODULE_SDFATFS_TEST
#if (MODULE_WAVDECODE_TEST)
	WavDecode_Test();
#endif //MODULE_WAVDECODE_TEST
#if (MODULE_MYMALLOC_TEST)
    MyMalloc_Test();
#endif //MODULE_MYMALLOC_TEST
#if (MODULE_HZKFONT_TEST)
    HZKFont_Test();
#endif //MODULE_HZKFONT_TEST
#if (MODULE_DS18B20_TEST)
    DS18B20_Test();
#endif //MODULE_DS18B20_TEST
#if (MODULE_ESP8266_TEST)
	ESP8266_Test();
#endif //MODULE_ESP8266_TEST
#if (MODULE_UART_RXTX_TEST)
	ATKUart_Test();
#endif //MODULE_UART_RXTX_TEST
#if (MODULE_USBHOST_TEST)
    USB_Host_Test();
#endif //MODULE_USBHOST_TEST    
#if (MODULE_TP_TSET)
	Touch_Test();
#endif //MODULE_LCD_DMA_TEST
#if (MODULE_EMWIN_TEST)
    emWin_Test();    
#endif //MODULE_EMWIN_TEST
}









