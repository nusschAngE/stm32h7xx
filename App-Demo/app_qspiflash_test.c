
#include "stm32h7xx.h"
#include "app_demo.h"
#include "my_malloc.h"
#include "delay.h"
#include "printf.h"
#include "qspi_flash.h"
#include "mytext.h"

#if (MODULE_QSPIFLASH_TEST)
/* QSPI Flash test
*/
void QSPI_Flash_Test(void)
{
    uint16_t i;
    uint8_t ret;
	uint8_t *tBuffer = NULL;
	//uint8_t tAddr = (QFL_SECTORNBR - 1) * QFL_SECTORSIZE;
	uint8_t tAddr = 0;

    ShowTextLineAscii(10, 10, "##QSPI Flash Test##", FontPreset(2));

	tBuffer = myMalloc(MLCSRC_AXISRAM, 512);
	if(tBuffer == NULL)
	{
		printf("tmpBuffer malloc error\r\n");
		goto QSPI_Flash_Test_End;
	}

    for(i = 0; i < 512; i++) {
        tBuffer[i] = i/2;
    }

    //QFL_EraseSector(tAddr);
    //ret = QFL_WriteNoChk(tBuffer, tAddr, 512);
    ret = QFL_WriteWithChk(tBuffer, tAddr, 512);
    //ret = QFL_PageProgram(testBuff, tAddr, 512);
    if(ret != 0)
    {
        printf("flash write error, ret = %d\r\n", ret);
        return ;
    }

    for(i = 0; i < 512; i++) {
        tBuffer[i] = 0;
    }
    
    ret = QFL_Read(tBuffer, tAddr, 512);
    if(ret != 0)
    {
        printf("flash read error, ret = %d\r\n", ret);
    }
	else
	{	
		MemPrintf("QFL-T : ", tBuffer, 512);
	}

QSPI_Flash_Test_End:
	myFree(MLCSRC_AXISRAM, tBuffer);

	printf("QSPI Flash test end.\r\n");
}

#endif


