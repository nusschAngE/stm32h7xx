
#include "stm32h7xx.h"
#include "app_demo.h"
#include "my_malloc.h"
#include "delay.h"
#include "mytext.h"
#include "wifi_esp8266.h"
#include "uart_atk.h"

#if (MODULE_ESP8266_TEST)

void ESP8266_Test(void)
{
	uint16_t size = 0;
	uint8_t *tmpBuff = NULL;
	uint8_t ret = 0, cnt = 10;

	ShowTextLineAscii(10, 10, "##ESP8266 Test##", FontPreset(2));

	tmpBuff = myMalloc(MLCSRC_AXISRAM, 2560);
	if(tmpBuff == NULL)
	{
		printf("tmpBuff malloc error\r\n");
		goto Wlan_Test_End;
	}

	ret = ESP8266_Init();
	if(ret != ESP_RES_OK)
	{
        printf("ESP8266_TestATCmd() error, ret = %d\r\n", ret);
	    goto Wlan_Test_End;
	}

    /* wlan mode */
    ret = ESP8266_SetWlanMode(1);
    if(ret != ESP_RES_OK)
	{
        printf("ESP8266_SetWlanMode() error, ret = %d\r\n", ret);
	    goto Wlan_Test_End;
	}

	ret = ESP8266_JoinAP((uint8_t*)"TP-LINK_F59A", (uint8_t*)"dotabear1233");
	if(ret != ESP_RES_OK)
	{
        printf("ESP8266_JoinAP() error, ret = %d\r\n", ret);
	    goto Wlan_Test_End;
	}
	
Wlan_Test_End:
	myFree(MLCSRC_AXISRAM, tmpBuff);
	printf("\r\nESP8266 test end.\r\n");
}
#endif

