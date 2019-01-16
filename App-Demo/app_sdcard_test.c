
#include "stm32h7xx.h"
#include "app_demo.h"

#include "delay.h"
#include "sdcard.h"

#if (MODULE_SDCARD_TEST)
/* sdcard test
*/
void SDCard_Test(void)
{
	uint16_t i = 0, cnt = 3;
	uint8_t ret = 0;
	uint8_t *tmpBuffer = NULL;

	printf("----------- SDCard test start ----------------\r\n");

    tmpBuffer = myMalloc(MLCSRC_AXISRAM, 512);
    if(tmpBuffer == NULL)
    {
        printf("tmpBuffer malloc error\r\n");
        goto SDCard_Test_End;
    }
	
	SDCard_Init();
	while(cnt--)
	{
		for(i = 0; i < 512; i++)
	        tmpBuffer[i] = i;//0 ~ 255

	    ret = SDCard_WriteMultipleBlocks(tmpBuffer, cnt, 1);
		printf("sd card write, ret = %d\r\n", ret);
		for(i = 0; i < 512; i++)
	        tmpBuffer[i] = 0;//0 ~ 255

	    ret = SDCard_ReadMultipleBlocks(tmpBuffer, cnt, 1);
	    printf("sd card read, ret = %d\r\n", ret);
	    printf("tmpBuffer : \r\n");
	    for(i = 0; i < 512; i++) 
	    	printf("%d, ", tmpBuffer[i]);
	    printf("\r\n");
	}

SDCard_Test_End:
	myFree(MLCSRC_AXISRAM, tmpBuffer);
    printf("----------- SDCard test end ----------------\r\n");
}

#endif

