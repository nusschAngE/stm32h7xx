
#include "stm32h7xx.h"
#include "app_demo.h"

#include "delay.h"
#include "qspi_flash.h"

#if (MODULE_QSPIFLASH_TEST)
/* QSPI Flash test
*/
void QSPI_Flash_Test(void)
{
    uint16_t i;
    uint8_t ret;
	uint8_t *tmpBuffer = NULL;

    printf("--- QSPI_Flash_Test start -----------------\r\n");

	tmpBuffer = myMalloc(MLCSRC_AXISRAM, 512);
	if(tmpBuffer == NULL)
	{
		printf("tmpBuffer malloc error\r\n");
		goto QSPI_Flash_Test_End;
	}

    for(i = 0; i < 512; i++) tmpBuffer[i] = i;

    //W25Qxx_EraseSector(0);
    //ret = QSPI_Flash_WriteNoChk(testBuff, 0, 512);
    //ret = QSPI_Flash_WriteWithChk(tmpBuffer, 0, 512);
    //ret = W25Qxx_PageProgram(testBuff, 0, 512);
    //W25Qxx_WaitBusy(0xfff);
    //if(ret != SPIFLASH_OK)
    //if(ret != 0)
    //{
    //    printf("QSPI_Flash_RWTest write error, ret = %d\r\n", ret);
    //    return ;
    //}

    for(i = 0; i < 512; i++)
    {
        tmpBuffer[i] = 0;
    }
    ret = QSPI_Flash_Read(tmpBuffer, HZKFONT_ADDR, 512);
    if(ret != QSPIFLASH_ERROR_NONE)
    {
        printf("QSPI_Flash_RWTest read error, ret = %d\r\n", ret);
    }
	else
	{	
		MemPrintf("spi flash test", tmpBuffer, 512);
	}

QSPI_Flash_Test_End:
	myFree(MLCSRC_AXISRAM, tmpBuffer);
    printf("--- QSPI_Flash_Test start -----------------\r\n");
}

#endif


