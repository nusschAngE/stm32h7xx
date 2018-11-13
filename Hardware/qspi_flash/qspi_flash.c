
#include "stm32h7xx.h"
#include "qspi_flash.h"
#include "delay.h"

/********* W25Qxxx command ***********/
/* write enable/disable cmd */
#define W25Q_WRITE_ENABLE     0x06
#define W25Q_WRITE_ENVSR      0x50//volatile status register
#define W25Q_WRITE_DISABLE    0x04
/* read status register */
#define W25Q_READ_SR1         0x05
#define W25Q_READ_SR2         0x35
#define W25Q_READ_SR3         0x15
/* write status register */
#define W25Q_WRITE_SR1        0x01
#define W25Q_WRITE_SR2        0x31
#define W25Q_WRITE_SR3        0x11
/* read id */
#define W25Q_READID           0x90
#define W25Q_READID_DSPI      0x92
#define W25Q_READID_QSPI      0x94
/* QSPI Enable &Disable */
#define W25Q_QSPI_ENABLE      0x38
#define W25Q_QSPI_DISABLE     0xff
/* address mode */
#define W25Q_ADDR_3BYTE       0xe9
#define W25Q_ADDR_4BYTE       0xb7
/* fast read */
#define W25Q_FAST_READ        0x0b
#define W25Q_FAST_READ4B      0x0c
/* page program */
#define W25Q_PAGE_PROGRAM     0x02
/* sector erase */
#define W25Q_SECTOR_ERASE     0x20
/* set read parameter */
#define W25Q_READ_PARA        0xc0
/* reset */
#define W25Q_RESET_ENABLE	  0x66
#define W25Q_RESET			  0x99

/*
*  W25Qxxx Status Registers 
*/
typedef enum
{
    W25Qxx_SR1 = 0,
    W25Qxx_SR2,
    W25Qxx_SR3
}W25Qxxx_SR;

/* QSPI interface state */
enum
{
    QSPI_OK          = 0,
    QSPI_ERR_CMD     = 1,
    QSPI_ERR_READ    = 2,
    QSPI_ERR_WRITE   = 3,

    QSPI_ERR,
};

/* SPI Flash address mode */
enum
{
    ADDR_3BYTES = 0,
    ADDR_4BYTES,
};

/* QSPI Flash using SPI mode */
enum
{
    SerialSPI     = 0,
    SerialDUALSPI,
    SerialQUADSPI,
};

/*  W25Qxx return status
*/
enum
{
    W25Q_OK        = 0,
    W25Q_ERR_SERIAL    = 1,
    W25Q_ERR_VALUE     = 2,
    W25Q_ERR_READ      = 3,
    W25Q_ERR_WRITE     = 4,
    W25Q_ERR_TIMEOUT   = 5,
    W25Q_ERR_DEVICE    = 6,

    W25Q_ERR,    
};

static uint8_t OPRBUFFER[SPIFLASH_SECTORSIZE];

static uint8_t QFlashSPIMode = SerialSPI;
static QSPI_HandleTypeDef QSPI_Handler;

/*  STATIC
*/
static void QSPI_Init(void);
static uint8_t QSPI_SendCommand(uint32_t Inst, uint32_t Addr, 
                                uint32_t dCycles, uint32_t InstMode, 
                                uint32_t AddrMode, uint32_t AddrSize, 
                                uint32_t DataMode);
static uint8_t QSPI_Transmit(uint8_t *pBuff, uint32_t wSize);   
static uint8_t QSPI_Receive(uint8_t *pBuff, uint32_t rSize);
static void W25Qxx_Reset(void);
static void W25Qxx_WriteEnable(void);
static void W25Qxx_WriteDisable(void);
static bool W25Qxx_IsWriteEnable(void);
static void W25Qxx_SetAddressMode(uint8_t mode);
static void W25Qxx_SetReadParameter(uint8_t dummy, uint8_t wrap);
static uint16_t W25Qxx_GetDeviceID(void);
static uint8_t W25Qxx_ReadSR(W25Qxxx_SR SRx, uint8_t *rVal);
static uint8_t W25Qxx_WriteSR(W25Qxxx_SR SRx, uint8_t wVal);
static void W25Qxx_QSPIEnable(void);
static void W25Qxx_QSPIDisable(void);
static bool W25Qxx_WaitBusy(uint32_t timeout);
static uint8_t W25Qxx_EraseSector(uint32_t sector);
static uint8_t W25Qxx_PageProgram(uint8_t *pBuff, uint32_t Address, uint16_t wSize);
static uint8_t W25Qxx_Read(uint8_t *pBuff, uint32_t Address, uint16_t rSize);
//static uint8_t W25Qxx_WriteSector(uint8_t *pBuff, uint32_t Addr, uint16_t wSize, uint8_t check);


/******************* PUBLIC FUNCTION ***************************/
/*
*   called in HAL_QSPI_Init();
*/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Init;
    
    __HAL_RCC_QSPI_CLK_ENABLE();       
    __HAL_RCC_GPIOB_CLK_ENABLE();      
    __HAL_RCC_GPIOF_CLK_ENABLE();      
    
    GPIO_Init.Pin = GPIO_PIN_6;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;        
    GPIO_Init.Pull = GPIO_PULLUP;              
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
    GPIO_Init.Alternate = GPIO_AF10_QUADSPI;   
    HAL_GPIO_Init(GPIOB, &GPIO_Init);
    
    GPIO_Init.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL;              
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   
    GPIO_Init.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_Init);
    
    GPIO_Init.Pin = GPIO_PIN_2;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL; 
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF9_QUADSPI;   
    HAL_GPIO_Init(GPIOB, &GPIO_Init);
    
    GPIO_Init.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL; 
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_Init);  

    GPIO_Init.Pin = GPIO_PIN_6;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL; 
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_Init); 
}

/* w25qxxx init */
uint8_t SPIFlash_Init(void)
{
	uint8_t ret = 0;
	uint8_t TempSR = 0;
	uint16_t DeviceID = 0;

	QFlashSPIMode = SerialSPI;
    /* QSPI initialize */
    QSPI_Init();
    DelayUs(10000);
    //W25Qxx_Reset();
    //W25Qxx_WaitBusy(0xfff);
    /* enable QSPI mode */
    W25Qxx_QSPIEnable();
    DelayUs(50000);
    /* get flash id */
    DeviceID = W25Qxx_GetDeviceID();
    printf("Get flash ID, DeviceID = 0x%04x\r\n", DeviceID);
    if(DeviceID != 0xEF18)
    {
        printf("Flash type isn't W25Q256!\r\n");
        return (0);
    }

    /* set 4byte address mode */
    W25Qxx_SetAddressMode(ADDR_4BYTES);
    /* set read parameter */
    W25Qxx_SetReadParameter(3, 0);

    return (1);
}

uint8_t SPIFlash_EraseSectors(uint32_t StartSector, uint32_t SectorNbr)
{
    uint8_t ret;
    uint32_t currSector = StartSector;

    while(SectorNbr)
    {
        //0 ~ SPIFLASH_SECTORNBR-1
        if(currSector >= SPIFLASH_SECTORNBR) 
            break;
        
        ret = W25Qxx_EraseSector(currSector);
        if(ret != W25Q_OK)
            return (SPIFLASH_ERR_ERASE);
            
        currSector++;
        SectorNbr--;
    }

    return (SPIFLASH_OK);
}

/* SPI flash read */
uint8_t SPIFlash_Read(uint8_t *pBuff, uint32_t Address, uint16_t rSize)
{
    uint8_t ret = 0;

    ret = W25Qxx_Read(pBuff, Address, rSize);
    if(ret != W25Q_OK)
        return (SPIFLASH_ERR_READ);

    return (SPIFLASH_OK);
}

/*  SPI Flash write, not check 
*   page auto shift
*/
uint8_t SPIFlash_WriteNoChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize)
{
    uint8_t ret;
    uint16_t curWriteSize, remainPageSize;

    //0 ~ SPIFLASH_MEMSIZE-1
    if((Address + wSize) > SPIFLASH_MEMSIZE)
        return (SPIFLASH_ERR_VALUE);

    /* init value 
    */
    // remian bytes in current page
    remainPageSize = SPIFLASH_PAGESIZE - Address % SPIFLASH_PAGESIZE;
    // current write size
    curWriteSize = (wSize <= remainPageSize) ? (wSize) : (remainPageSize);

    while(wSize)
    {
        ret = W25Qxx_PageProgram(pBuff, Address, curWriteSize);
        if(ret != W25Q_OK)
            return (SPIFLASH_ERR_WRITE);

        pBuff += curWriteSize;
        Address += curWriteSize;
        wSize -= curWriteSize;
        if(wSize > SPIFLASH_PAGESIZE)
            curWriteSize = SPIFLASH_PAGESIZE;
        else
            curWriteSize = wSize;
    }

    return (SPIFLASH_OK);
}

/*  SPI Flash write with check
*   page auto shift
*   sector check and erase
*/
uint8_t SPIFlash_WriteWithChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize)
{
    uint8_t ret;
    //page program value
    uint16_t curWriteSize;
    //sector check value
    uint32_t curSector, SectorOfs, remainSectorSize;
    uint16_t i, NeedErase = 0;
    
    //0 ~ SPIFLASH_MEMSIZE-1
    if((Address + wSize) > SPIFLASH_MEMSIZE)
        return (SPIFLASH_ERR_VALUE);

    /* init value */
    // start sector : 0 ~ SPIFLASH_SECTORNBR-1
    curSector = Address / SPIFLASH_SECTORSIZE;
    // offset in current sector
    SectorOfs = Address % SPIFLASH_SECTORSIZE;
    // remain bytes in current sector
    remainSectorSize = SPIFLASH_SECTORSIZE - SectorOfs;
	// current write size
	curWriteSize = (wSize <= remainSectorSize) ? (wSize) : (remainSectorSize);

    while(wSize)
    {
    	//printf("size = %d, ofs = %d\r\n", curWriteSize, SectorOfs);
        // read current sector
        ret = W25Qxx_Read(OPRBUFFER, curSector * SPIFLASH_SECTORSIZE, SPIFLASH_SECTORSIZE);
        if(ret != W25Q_OK)
            return (SPIFLASH_ERR_READ);

        // check
        for(i = 0; i < curWriteSize; i++)
        {
        	//printf("%d,", OPRBUFFER[i + SectorOfs]);
            if(OPRBUFFER[i + SectorOfs] != 0xFF)
            {
            	NeedErase = 1;
                break;
            }
        }

        // need erase
        if(NeedErase)
        {
        	printf("sector[%d] need erase\r\n", curSector);
            ret = W25Qxx_EraseSector(curSector);
            if(ret != W25Q_OK)
                return (SPIFLASH_ERR_ERASE);

            for(i = 0; i < curWriteSize; i++)
            {
                OPRBUFFER[i + SectorOfs] = pBuff[i];
            }

            ret = SPIFlash_WriteNoChk(OPRBUFFER, curSector * SPIFLASH_SECTORSIZE, SPIFLASH_SECTORSIZE);
            if(ret != SPIFLASH_OK)
                return (SPIFLASH_ERR_WRITE);
        }
        else
        {
            ret = SPIFlash_WriteNoChk(pBuff, Address, curWriteSize);
            if(ret != SPIFLASH_OK)
                return (SPIFLASH_ERR_WRITE);
        }

        curSector += 1;
        SectorOfs = 0;//init state process done, set byte off to 0
        NeedErase = 0;

        pBuff += curWriteSize;
        Address += curWriteSize;
        wSize -= curWriteSize;
        if(wSize > SPIFLASH_SECTORSIZE)
            curWriteSize = SPIFLASH_SECTORSIZE;
        else
            curWriteSize = wSize;
    }

    return (SPIFLASH_OK);
}

/* spi flash test */
void SPIFlash_RWTest(void)
{
    uint8_t testBuff[256];
    uint16_t i;
    uint8_t ret;

    printf("SPIFlash_RWTest start...\r\n");

    for(i = 0; i < 256; i++) testBuff[i] = i;
    //for(i = 256; i > 0; i--) testBuff[256 - i] = i - 1;

    //W25Qxx_EraseSector(0);
    //ret = SPIFlash_WriteNoChk(testBuff, 0, 256);
    ret = SPIFlash_WriteWithChk(testBuff, 0, 256);
    //ret = W25Qxx_PageProgram(testBuff, 0, 256);
    //W25Qxx_WaitBusy(0xfff);
    //if(ret != SPIFLASH_OK)
    if(ret != W25Q_OK)
    {
        printf("QSPI_Flash_RWTest write error, ret = %d\r\n", ret);
        return ;
    }

    for(i = 0; i < 256; i++)
    {
        testBuff[i] = 0;
    }
    ret = SPIFlash_Read(testBuff, 0, 256);
    if(ret != SPIFLASH_OK)
    {
        printf("QSPI_Flash_RWTest read error, ret = %d\r\n", ret);
    }
	else
	{	
		for(i = 0; i < 256; i++)
		{
			printf("%d, ", testBuff[i]);
		}
		printf("\r\n");
	}
	
    printf("QSPI_Flash_RWTest finish...\r\n");
}


/******************** PUBLIC FUNCTION ********************/
/*
*   QSPI initialize
**/
static void QSPI_Init(void)
{
    QSPI_Handler.Instance = QUADSPI;

    QSPI_Handler.Init.ClockPrescaler = 1; //QSPI clock frequency -> 200/(CP + 1) = 100mhz
    QSPI_Handler.Init.FifoThreshold = 4;
    QSPI_Handler.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;//must set 0 in DDR mode
    QSPI_Handler.Init.FlashSize = (SPIFLASH_MEMSIZE - 1);//32Mbyte
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;//55ns
    QSPI_Handler.Init.ClockMode = QSPI_CLOCK_MODE_0;
    QSPI_Handler.Init.FlashID = QSPI_FLASH_ID_1;
    QSPI_Handler.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    HAL_QSPI_Init(&QSPI_Handler);
}

/*
*   QSPI send Command
*/
static uint8_t QSPI_SendCommand(uint32_t Inst, uint32_t Addr, 
                                uint32_t dCycles, uint32_t InstMode, 
                                uint32_t AddrMode, uint32_t AddrSize, 
                                uint32_t DataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction = Inst;                 	
    Cmdhandler.Address = Addr;                            
    Cmdhandler.DummyCycles = dCycles;                   
    Cmdhandler.InstructionMode = InstMode;				
    Cmdhandler.AddressMode = AddrMode;   					
    Cmdhandler.AddressSize = AddrSize;   				
    Cmdhandler.DataMode = DataMode;             				
    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       	
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; 
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;           	
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    if(HAL_QSPI_Command(&QSPI_Handler, &Cmdhandler, 0xffff) == HAL_OK)
    {
        return (QSPI_OK);
    }

    return (QSPI_ERR);
}

static uint8_t QSPI_Transmit(uint8_t *pBuff, uint32_t wSize)
{
    QSPI_Handler.Instance->DLR = wSize - 1;
    if(HAL_QSPI_Transmit(&QSPI_Handler, pBuff, 5000) == HAL_OK)
    {
        return (QSPI_OK);
    }
    return (QSPI_ERR);
}

static uint8_t QSPI_Receive(uint8_t *pBuff, uint32_t rSize)
{
    QSPI_Handler.Instance->DLR = rSize - 1;
    if(HAL_QSPI_Receive(&QSPI_Handler, pBuff, 5000) == HAL_OK)
    {
        return (QSPI_OK);
    }
    return (QSPI_ERR);
}

/********************** W25Qxxx control ********************************/
static void W25Qxx_Reset(void)
{
	uint8_t ret = 0;

	if(QFlashSPIMode == SerialSPI)
	{
		/* send reset enable cmd */
		QSPI_SendCommand(W25Q_RESET_ENABLE, 0, 
	                   		0, QSPI_INSTRUCTION_1_LINE, 
	                    	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
	                    	QSPI_DATA_NONE);
		/* delay */
	    DelayUs(10000);
	    /* send reset enable */
	    QSPI_SendCommand(W25Q_RESET, 0, 
	                   		0, QSPI_INSTRUCTION_1_LINE, 
	                    	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
	                    	QSPI_DATA_NONE);

  	}
  	else if(QFlashSPIMode == SerialQUADSPI)
  	{
		/* send reset enable cmd */
		QSPI_SendCommand(W25Q_RESET_ENABLE, 0, 
	                   		0, QSPI_INSTRUCTION_4_LINES, 
	                    	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
	                    	QSPI_DATA_NONE);
		/* delay */
	    DelayUs(10000);
	    /* send reset enable */
	    QSPI_SendCommand(W25Q_RESET, 0, 
	                   		0, QSPI_INSTRUCTION_4_LINES, 
	                    	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
	                    	QSPI_DATA_NONE);

  	}
  	else
  	{
  		
  	}
}

static void W25Qxx_WriteEnable(void)
{
	uint8_t ret = QSPI_ERR;

	/* QSPI send command */
	if(QFlashSPIMode == SerialSPI)
	{
		ret = QSPI_SendCommand(W25Q_WRITE_ENABLE, 0, 
                            	0, QSPI_INSTRUCTION_1_LINE, 
                             	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                              	QSPI_DATA_NONE);
	}
	else if(QFlashSPIMode == SerialQUADSPI)
	{
    	ret = QSPI_SendCommand(W25Q_WRITE_ENABLE, 0, 
                           		0, QSPI_INSTRUCTION_4_LINES, 
                             	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                             	QSPI_DATA_NONE);
	}
	else
	{
      	// not support
      	ret = QSPI_ERR;
	}  

	if(ret != QSPI_OK)
	{
	    printf("w25qxx enable write error!\r\n");
	}

#if 0//debug
    DelayUs(10000);
    if(W25Qxx_IsWriteEnable() == FALSE)
    {
        printf("w25qxx write disable!\r\n");
    }
#endif	
}

static void W25Qxx_SRWriteEnable(void)
{
	uint8_t ret = QSPI_ERR;

	/* QSPI send command */
	if(QFlashSPIMode == SerialSPI)
	{
		ret = QSPI_SendCommand(W25Q_WRITE_ENVSR, 0, 
                            	0, QSPI_INSTRUCTION_1_LINE, 
                             	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                              	QSPI_DATA_NONE);
	}
	else if(QFlashSPIMode == SerialQUADSPI)
	{
    	ret = QSPI_SendCommand(W25Q_WRITE_ENVSR, 0, 
                           		0, QSPI_INSTRUCTION_4_LINES, 
                             	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                             	QSPI_DATA_NONE);
	}
	else
	{
      	// not support
      	ret = QSPI_ERR;
	}  

	if(ret != QSPI_OK)
	{
	    printf("w25qxx enable status reg write error!\r\n");
	}	
}

static void W25Qxx_WriteDisable(void)
{
    uint8_t ret = QSPI_ERR;
    
    /* QSPI send command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(W25Q_WRITE_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_WRITE_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        // not support
    }

	if(ret != QSPI_OK)
	{
	    printf("w25qxx disable write error!r\\n");
	}
}

static bool W25Qxx_IsWriteEnable(void)
{
	uint8_t TempSR = 0, ret;

	ret = W25Qxx_ReadSR(W25Qxx_SR1, &TempSR);
	if(ret != W25Q_OK)
	{
	    return (FALSE);
	}
	
	if(TempSR & 0x02) 
	{
		return (TRUE);
    }
	else
	{
		return (FALSE);

	}	
}

static void W25Qxx_SetAddressMode(uint8_t mode)
{
    uint8_t temp = 0, ret = 0;
    uint8_t count = 10;

    if(mode == ADDR_4BYTES)
        temp = W25Q_ADDR_4BYTE;
    else
        temp = W25Q_ADDR_3BYTE;

    /* QSPI send command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(temp, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_NONE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(temp, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_NONE);
    }
    else
    {
        /* SPI mode not support */
        ret = QSPI_ERR;
    }    

    /* send command error */ 
    if(ret != QSPI_OK)
    {
        printf("w25qxx set address mode error!\r\n");
    }
}

/*
*   QSPI Only
*   dummy[bit5:4] 
*           0 -> 2dummy 40mhz
*           1 -> 4dummy 80mhz
*           2 -> 6dummy 104mhz
*           3 -> 8dummy 104mhz
*   wrap[bit1:0] 
*           0 -> 8byte
*           1 -> 16byte
*           2 -> 32byte
*           3 -> 64byte
*/
static void W25Qxx_SetReadParameter(uint8_t dummy, uint8_t wrap)
{
    uint8_t temp = 0, ret = 0;
    
    /* QSPI send command */
    if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_READ_PARA, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        ret = QSPI_ERR;
    }    

    /* send command error */ 
    if(ret != QSPI_OK)
    {
        printf("w25qxx Set Read Parameter error_1!\r\n");
        while(1)
        {}
    }

    temp = wrap | dummy << 4;
    ret = QSPI_Transmit(&temp, 1);
    if(ret != QSPI_OK)
    {
        printf("w25qxx Set Read Parameter error_2!\r\n");
        while(1)
        {}    
    }
}

static uint16_t W25Qxx_GetDeviceID(void)
{
    uint8_t Recv[2];
    uint8_t ret = 0;

    /* QSPI send command */
#if 1	
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(W25Q_READID, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_READID, 0, 
                                4, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }	
    else
    {
        /* SPI mode not support */
        ret = QSPI_ERR;
    }
#else
	ret = qspi_SendCommand(W25Q_READID, 0, 
                       		0, QSPI_INSTRUCTION_1_LINE, 
                        	QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                        	QSPI_DATA_1_LINE);
#endif

    /* send command error */ 
    if(ret != QSPI_OK)
    {
        printf("w25qxx, get device ID error_1\r\n");
        return 0;
    }

    ret = QSPI_Receive(Recv, 2);
    if(ret != QSPI_OK)
    {
        printf("w25qxx, get device ID error_2\r\n");
        return 0;
    }

    return (uint16_t)((Recv[0] << 8) | Recv[1]);
}

static uint8_t W25Qxx_ReadSR(W25Qxxx_SR SRx, uint8_t *rVal)
{
    uint8_t Cmd = 0, Recv = 0;
    uint8_t ret = 0;

    /* select command ready to sent */
    switch (SRx)
    {
        case W25Qxx_SR1:
            Cmd = W25Q_READ_SR1;
            break;
        case W25Qxx_SR2:
            Cmd = W25Q_READ_SR2;
            break;
        case W25Qxx_SR3:
            Cmd = W25Q_READ_SR3;
            break; 
        default:
            Cmd = 0;
            return (W25Q_ERR_VALUE);
    }

    /* QSPI send command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        *rVal = 0;
        ret = QSPI_ERR;
    }

    /* send command error */ 
    if(ret != QSPI_OK) return (W25Q_ERR_SERIAL);

    if(QSPI_Receive(&Recv, 1) == QSPI_OK)
    {
        *rVal = Recv;
        printf("W25Qxx_ReadSR, SR = %d, Recv = 0x%02x\r\n", SRx, Recv);
        return (W25Q_OK);
    }
    else
    {
        *rVal = 0;
        return (W25Q_ERR_READ);
    }
}

static uint8_t W25Qxx_WriteSR(W25Qxxx_SR SRx, uint8_t wVal)
{
    uint8_t Cmd = 0;
    uint8_t ret = 0;

    /* select command ready to sent */
    switch (SRx)
    {
        case W25Qxx_SR1:
            Cmd = W25Q_WRITE_SR1;
            break;
        case W25Qxx_SR2:
            Cmd = W25Q_WRITE_SR2;
            break;
        case W25Qxx_SR3:
            Cmd = W25Q_WRITE_SR3;
            break; 
        default:
            Cmd = 0;
            return (W25Q_ERR_VALUE);
    }

    /* QSPI send command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        ret = QSPI_ERR;
    }

    /* send command error */ 
    if(ret != QSPI_OK) return (W25Q_ERR_SERIAL);

    if(QSPI_Transmit(&wVal, 1) == QSPI_OK)
    {
        return (W25Q_OK);
    }
    else
    {
        return (W25Q_ERR_WRITE);
    }
}

static void W25Qxx_QSPIEnable(void)
{
    uint8_t TempSR = 0;
    uint8_t ret = 0;

	ret = W25Qxx_ReadSR(W25Qxx_SR2, &TempSR);
	if(ret != W25Q_OK)
	{
	    printf("QSPIEnable, read SR error!\r\n");
	    QFlashSPIMode = SerialSPI;
	    return ;
	}

    printf("QSPIEnable, W25Qxxx_SR2 = 0x%02x\r\n", TempSR);
    if((TempSR & 0x02) == 0)
    {
        W25Qxx_WriteEnable();
        /* QSPI enable */
        TempSR |= 0x02;
        ret = W25Qxx_WriteSR(W25Qxx_SR2, TempSR);
        if(ret != W25Q_OK)
        {
            printf("QSPIEnable, write SR error!\r\n");
            QFlashSPIMode = SerialSPI;
            return ;
        }    
    }

	/* send QSPI Enable cmd */
    ret = QSPI_SendCommand(W25Q_QSPI_ENABLE, 0, 
                            0, QSPI_INSTRUCTION_1_LINE, 
                            QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                            QSPI_DATA_NONE);

    if(ret != QSPI_OK)
    {
        printf("QSPIEnable, send cmd error!\r\n");
        QFlashSPIMode = SerialSPI;
    }
    else
    {
        QFlashSPIMode = SerialQUADSPI;
    }
}

static void W25Qxx_QSPIDisable(void)
{
    uint8_t ret = 0;

    if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_QSPI_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_NONE);
    }

    if(ret == QSPI_OK)
    {
        QFlashSPIMode = SerialSPI;
    }
    else
    {
        printf("w25qxx disable QSPI error!\r\n");
    }
}

/* delay 5ms after one tune 
*   @para
*       timeout : >0
*   @return 
*       TRUE : not busy now
*       FALSE: timeout or error
*/
static bool W25Qxx_WaitBusy(uint32_t timeout)
{
    uint8_t TempSR = 0, ret;
    
    /* QSPI send command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(W25Q_READ_SR1, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_READ_SR1, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        ret = QSPI_ERR;
    }    

    if(ret != QSPI_OK)   return (FALSE);

    while(timeout--)
    {
        /* busy flag in SR1:bit0 */
     	ret = QSPI_Receive(&TempSR, 1);
     	if((ret == QSPI_OK) && ((TempSR & 0x01) == 0))
     	{
     	    return TRUE;
     	}

     	DelayUs(5000);
    }

    return (FALSE);
}

/* erase time > 150ms */
static uint8_t W25Qxx_EraseSector(uint32_t sector)
{
    uint32_t addr = sector * SPIFLASH_SECTORSIZE;
    uint8_t ret = 0;

    W25Qxx_WriteEnable();
    DelayUs(50000);
    if(W25Qxx_IsWriteEnable() == FALSE)
    {
    	printf("W25Qxx Erase Sector[%d], write disable\r\n", sector);
    	return (SPIFLASH_ERR_DEVICE);
    }
        
    /* send erase command */
    if(QFlashSPIMode == SerialSPI)
    {
        ret = QSPI_SendCommand(W25Q_SECTOR_ERASE, addr, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_32_BITS, 
                                QSPI_DATA_NONE);
    }
    else if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_SECTOR_ERASE, addr, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                                QSPI_DATA_NONE);
    }
    else
    {
        ret = QSPI_ERR;
    }

    if(ret == QSPI_OK)
    {
        //waite > 150ms
        if(W25Qxx_WaitBusy(40) == FALSE)
        {
            printf("w25qxx erase sector[%d] timeout!\r\n", sector);
            return (W25Q_ERR_TIMEOUT);
        }
    }
    else
    {
        printf("w25qxx erase sector[%d] error!\r\n", sector);
        return (W25Q_ERR_SERIAL);
    }

    return (W25Q_OK);
}

/*
*   Page Program, wSize <= 256. Just program in one page(0~65535).
*   QSPI Only
*/
static uint8_t W25Qxx_PageProgram(uint8_t *pBuff, uint32_t Address, uint16_t wSize)
{
    uint8_t ret = 0;

    W25Qxx_WriteEnable();
    DelayUs(50000);
    if(W25Qxx_IsWriteEnable() == FALSE)
    {
    	printf("W25Qxx_PageProgram, write diable, address = 0x%08x\r\n", Address);
    	return (SPIFLASH_ERR_DEVICE);
    }
        
    /* send page program command */
    if(QFlashSPIMode == SerialQUADSPI)
    {
        ret = QSPI_SendCommand(W25Q_PAGE_PROGRAM, Address, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        ret = QSPI_ERR;
    }
    
	/* send page program command error */
	if(ret != QSPI_OK)
	{
	    printf("w25qxx page program error_1, addr = 0x%08x\r\n", Address);
		return (W25Q_ERR_SERIAL);
    }
    
	/* QSPI send data */
	ret = QSPI_Transmit(pBuff, wSize);
	if(ret == QSPI_OK)
	{
		if(W25Qxx_WaitBusy(0xfff) == FALSE)
		{
			printf("w25qxx page program error_2, addr = 0x%08x\r\n", Address);
			return (W25Q_ERR_TIMEOUT);
		}
	}
	else
	{
	    printf("w25qxx page program error_3, addr = 0x%08x\r\n", Address);
		return (W25Q_ERR_WRITE);
	}

	return W25Q_OK;
}

/* QSPI Only */
static uint8_t W25Qxx_Read(uint8_t *pBuff, uint32_t Address, uint16_t rSize)
{
    uint8_t ret = 0;

	/* send read command */
	if(QFlashSPIMode == SerialQUADSPI)
	{
        ret = QSPI_SendCommand(W25Q_FAST_READ, Address, 
                             	8, QSPI_INSTRUCTION_4_LINES, 
                             	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                               	QSPI_DATA_4_LINES);
    }
    else
    {
        ret = QSPI_ERR;
    }
    
	/* send command error */ 
 	if(ret != QSPI_OK)
 	{
 	    printf("w25qxx read error_1!\r\n");
		return (W25Q_ERR_SERIAL);
    }
    
	/* QSPI receive data */
	ret = QSPI_Receive(pBuff, rSize);
	if(ret != QSPI_OK)
 	{
 	    printf("w25qxx read error_2!\r\n");
		return (W25Q_ERR_READ);
    }

    return (W25Q_OK);
}



