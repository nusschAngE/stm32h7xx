
#include "stm32h7xx.h"
#include "qspi_falsh.h"
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
#define W25Q_READ_PARA         0xc0

/*******************  ***************************/

static uint8_t OPRBUFFER[QFLASH_SECTORSIZE];

SPIFlashDevice SPIFlash;
static QSPI_HandleTypeDef QSPI_Handler;

/*
*  W25Qxxx Status Registers 
*/
typedef enum
{
    W25Qxxx_SR1 = 0,
    W25Qxxx_SR2,
    W25Qxxx_SR3
}W25Qxxx_SR;

/*************** private function ***************/

static uint8_t w25qxxx_WriteEnable(void);
static uint8_t w25qxxx_WriteDisable(void);
static uint8_t w25qxxx_WriteEnableState(void);
static uint8_t w25qxxx_SetAddressMode(uint8_t mode);
static uint8_t w25qxxx_GetAddressMode(void);
static uint8_t w25qxxx_SetReadParameter(uint8_t dummy, uint8_t wrap);
static uint8_t w25qxxx_GetDeviceID(void);
static uint8_t w25qxxx_ReadSR(W25Qxxx_SR SRx, uint8_t *rVal);
static uint8_t w25qxxx_WriteSR(W25Qxxx_SR SRx, uint8_t wVal);
static uint8_t w25qxxx_QSPI_Enable(void);
static uint8_t w25qxxx_QSPI_Disable(void);
static uint8_t w25qxxx_WaitBusy(void);
static uint8_t w25qxxx_EraseSector(uint32_t sector);
static uint8_t w25qxxx_Read(uint8_t *pBuff, uint32_t Addr, uint16_t rSize);
static uint8_t w25qxxx_WritePage(uint32_t Addr, uint8_t *pBuff, uint16_t wSize);
//static uint8_t w25qxxx_WriteSector(uint8_t *pBuff, uint32_t Addr, uint16_t wSize, uint8_t check);

/*
*   QSPI send Command
*/
static uint8_t qspi_SendCommand(uint32_t Inst, uint32_t Addr, 
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
    if(HAL_QSPI_Command(&QSPI_Handler, &Cmdhandler, 0xffff) != HAL_OK)
    {
        return F_OPR_ERR
    }

    return F_OPR_OK;
}

static uint8_t qspi_Transmit(uint8_t *pBuff, uint32_t wSize)
{
    QSPI_Handler.Instance->DLR = wSize;
    if(HAL_QSPI_Transmit(&QSPI_Handler, pBuff, 0xff) == HAL_OK)
    {
        return F_SPI_OK;
    }
    return F_SPI_ERR;
}

static uint8_t qspi_Receive(uint8_t *pBuff, uint32_t rSize)
{
    QSPI_Handler.Instance->DLR = rSize;
    if(HAL_QSPI_Receive(&QSPI_Handler, pBuff, 0xff) == HAL_OK)
    {
        return F_SPI_OK;
    }
    return F_SPI_ERR;
}

/********************** W25Qxxx control ********************************/


static uint8_t w25qxxx_WriteEnable(void)
{
    uint8_t count = 10, tempSR = 0;
    uint8_t ret = F_OPR_ERR;

    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(W25Q_WRITE_ENABLE, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(W25Q_WRITE_ENABLE, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        SPIFlash.bWrite = 0;
        return F_OPR_ErrSPI;
    }    

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);
            
    while(w25qxxx_WriteEnableState() == 0)
    {
        ShortDelay(100);
        if(--count == 0)
        {
           return F_OPR_ERR; 
        }
    }

    return F_OPR_OK;
}

static uint8_t w25qxxx_WriteDisable(void)
{
    uint8_t ret = F_OPR_ERR;
    
    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(W25Q_WRITE_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(W25Q_WRITE_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        return F_OPR_ErrSPI;
    }

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    SPIFlash.bWrite = 0;
    return F_OPR_OK;
}

/* return current write enable or disable state */
static uint8_t w25qxxx_WriteEnableState(void)
{
    uint8_t tempSR = 0;
    uint8_t ret = 0;

    ret = w25qxxx_ReadSR(W25Qxxx_SR1, &tempSR);
    printf("w25qxxx_ReadSR, ret = %d, reg = 0x%02x\r\n", ret, tempSR);
    if(ret == F_OPR_OK)
    {
        if((tempSR & 0x02))
            SPIFlash.bWrite = 1;
        else
            SPIFlash.bWrite = 0;
    }  
    else
    {
        SPIFlash.bWrite = 0;
    }

    return (SPIFlash.bWrite);
}

static uint8_t w25qxxx_SetAddressMode(uint8_t mode)
{
    uint8_t temp = 0, ret = F_OPR_ERR;
    uint8_t count = 10;

    if(mode == F_ADDR_3B)
        temp = W25Q_ADDR_3BYTE;
    else if(mode == F_ADDR_4B)
        temp = W25Q_ADDR_4BYTE;
    else
        return W25Q_ADDR_3BYTE;

    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(temp, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(temp, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        SPIFlash.AddressMode = F_ADDR_3B;
        return F_OPR_ErrSPI;
    }    

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    /* check current address mode */
    while(w25qxxx_GetAddressMode() != mode)
    {
        ShortDelay(100);
        if(--count == 0)
        {
            return F_OPR_ERR;
        }
    }

    return F_OPR_OK;
}

/* return current address mode */
static uint8_t w25qxxx_GetAddressMode(void)
{
    uint8_t tempSR = 0;
    if(w25qxxx_ReadSR(W25Qxxx_SR3, &tempSR) == F_OPR_OK)
    {
        if((tempSR & 0x01))
            SPIFlash.AddressMode = F_ADDR_4B;
        else
            SPIFlash.AddressMode = F_ADDR_3B;
    }
    else
    {
        SPIFlash.AddressMode = F_ADDR_3B;
    }

    return (SPIFlash.AddressMode);
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
static uint8_t w25qxxx_SetReadParameter(uint8_t dummy, uint8_t wrap)
{
    uint8_t temp = 0, ret = F_OPR_ERR;
    
    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(W25Q_READ_PARA, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        return F_OPR_ErrSPI;
    }    

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    temp = wrap | dummy << 4;
    if(qspi_Transmit(&temp, 1) != F_SPI_OK)
    {
        return F_OPR_ERR;
    }

    return F_OPR_OK;
}

static uint8_t w25qxxx_GetDeviceID(void)
{
    uint8_t Recv[2];
    uint8_t ret = F_OPR_ERR;

    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(W25Q_READID, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(W25Q_READID_QSPI, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        SPIFlash.id = 0;
        return F_OPR_ErrSPI;
    }

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    if(qspi_Receive(Recv, 2) == F_SPI_OK)
    {
        SPIFlash.id = (uint32_t)((Recv[0] << 8) | Recv[1]);
        return F_OPR_OK;
    }
    else
    {
        SPIFlash.id = 0;
        return F_OPR_ERR;
    }
}

static uint8_t w25qxxx_ReadSR(W25Qxxx_SR SRx, uint8_t *rVal)
{
    uint8_t Cmd = 0, Recv = 0;
    uint8_t ret = F_OPR_ERR;

    /* select command ready to sent */
    switch (SRx)
    {
        case W25Qxxx_SR1:
            Cmd = W25Q_READ_SR1;
            break;
        case W25Qxxx_SR2:
            Cmd = W25Q_READ_SR2;
            break;
        case W25Qxxx_SR3:
            Cmd = W25Q_READ_SR3;
            break; 
        default:
            Cmd = 0;
            return F_OPR_ErrCMD;
    }

    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        *rVal = 0;
        return F_OPR_ErrSPI;
    }

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    if(qspi_Receive(&Recv, 1) == F_SPI_OK)
    {
        *rVal = Recv;
        return F_OPR_OK;
    }
    else
    {
        *rVal = 0;
        return F_OPR_ERR;
    }
}

static uint8_t w25qxxx_WriteSR(W25Qxxx_SR SRx, uint8_t wVal)
{
    uint8_t Cmd = 0;
    uint8_t ret = F_OPR_ERR;

    /* select command ready to sent */
    switch (SRx)
    {
        case W25Qxxx_SR1:
            Cmd = W25Q_WRITE_SR1;
            break;
        case W25Qxxx_SR2:
            Cmd = W25Q_WRITE_SR2;
            break;
        case W25Qxxx_SR3:
            Cmd = W25Q_WRITE_SR3;
            break; 
        default:
            Cmd = 0;
            return F_OPR_ErrCMD;
    }

    if(w25qxxx_WriteEnableState() == 0)//write disable
    {
        if(w25qxxx_WriteEnable() != F_OPR_OK)
        {
            return F_OPR_ErrWDIS;
        }
    }

    /* QSPI send command */
    if(SPIFlash.SPIMode == F_SerialSPI)
    {
        ret = qspi_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_1_LINE, 
                                QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_1_LINE);
    }
    else if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(Cmd, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
    }
    else
    {
        /* SPI mode not support */
        return F_OPR_ErrSPI;
    }

    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);

    if(qspi_Transmit(&wVal, 1) == F_SPI_OK)
        return F_OPR_OK;
    else
        return F_OPR_ERR;
}

static uint8_t w25qxxx_QSPI_Enable(void)
{
    uint8_t tempSR = 0;
    uint8_t ret = F_OPR_ERR;

    if(w25qxxx_WriteEnableState() == 0)//write disable
    {
        if(w25qxxx_WriteEnable() != F_OPR_OK)
        {
            return F_OPR_ErrWDIS;
        }
    }
    
    if(w25qxxx_ReadSR(W25Qxxx_SR2, &tempSR) == F_OPR_OK)
    {
        if((tempSR & 0x02) == 0)
        {
            /* QSPI enable */
            tempSR |= 0x02;
            if(w25qxxx_WriteSR(W25Qxxx_SR2, tempSR) != F_OPR_OK)
            {
                return F_OPR_ERR;
            }    
            
            ret = qspi_SendCommand(W25Q_QSPI_ENABLE, 0, 
                                    0, QSPI_INSTRUCTION_1_LINE, 
                                    QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, 
                                    QSPI_DATA_1_LINE);
            /* send command error */ 
            if(ret != F_OPR_OK) return (F_OPR_ErrCMD);                        

            SPIFlash.SPIMode = F_SerialQUADSPI;
            return F_OPR_OK;
        }
    }
    else
    {
        SPIFlash.SPIMode = F_SerialNUM;
        return F_OPR_ERR;
    }
    
    return F_OPR_OK;
}

static uint8_t w25qxxx_QSPI_Disable(void)
{
    uint8_t ret = F_OPR_ERR;

    if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        ret = qspi_SendCommand(W25Q_QSPI_DISABLE, 0, 
                                0, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, 
                                QSPI_DATA_4_LINES);
        /* send command error */ 
        if(ret != F_OPR_OK) return (F_OPR_ErrCMD);
    }
    
    SPIFlash.SPIMode = F_SerialSPI;
    return F_OPR_OK;
}

/* return busy status */
static uint8_t w25qxxx_WaitBusy(void)
{
    uint8_t tempSR = 0, busy = 1;
    uint16_t count = 0xff;
    uint8_t ret = F_OPR_ERR;

    while(busy)
    {
        /* busy flag in SR1->BIT0 */
        ret = w25qxxx_ReadSR(W25Qxxx_SR1, &tempSR);
        if(ret == F_OPR_OK)
        {
            if((tempSR & 0x01)) 
                busy = 1;
            else    
                busy = 0;
        }
        else
        {
            busy = 1;
        }
        
        if(--count == 0)
        {
            ret = F_OPR_ErrTO;
            break;
        }
        delay_ms(5);
    }

    return (ret);
}

/* erase time > 150ms */
static uint8_t w25qxxx_EraseSector(uint32_t sector)
{
    uint32_t addr = sector * SPIFlash.sectorSize;
    uint8_t ret = F_OPR_ERR;

    if(w25qxxx_WriteEnableState() == 0)
    {
        if(w25qxxx_WriteEnable() != F_OPR_OK)
            return F_OPR_ErrWDIS;
    }

    if(sector >= SPIFlash.totalSector)
        return F_OPR_ErrSIZE;
        
    /* send erase command */
    ret = qspi_SendCommand(W25Q_SECTOR_ERASE, addr, 
                            0, QSPI_INSTRUCTION_4_LINES, 
                            QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                            QSPI_DATA_4_LINES);
    /* send command error */ 
    if(ret != F_OPR_OK) return (F_OPR_ErrCMD);
    
    /* wait erase done */
    ret = w25qxxx_WaitBusy();
    if(ret != F_OPR_OK) return (F_OPR_ERR);
    
    return F_OPR_OK;
}

/* QSPI Only */
static uint8_t w25qxxx_Read(uint8_t *pBuff, uint32_t Addr, uint16_t rSize)
{
    uint8_t ret = F_OPR_ERR;

    if(SPIFlash.SPIMode == F_SerialQUADSPI)
    {
        /* send read command */
        ret = qspi_SendCommand(W25Q_FAST_READ, Addr, 
                                8, QSPI_INSTRUCTION_4_LINES, 
                                QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                                QSPI_DATA_4_LINES);
        /* send command error */ 
        if(ret != F_OPR_OK) return (F_OPR_ErrCMD);
        /* QSPI receive data */
        if(qspi_Receive(pBuff, rSize) == F_SPI_OK)
            return F_OPR_OK;
        else
            return F_OPR_ERR;
    }
    else
    {
        return F_OPR_ErrSPI;
    }    
}

/*
*   write one page, wSize <= 256, the data in target page must be '0xFF' before write  
*   QSPI Only
*/
static uint8_t w25qxxx_WritePage(uint32_t Addr, uint8_t *pBuff, uint16_t wSize)
{
    uint8_t ret = F_OPR_ERR;

    if(w25qxxx_WriteEnableState() == 0)
    {
        if(w25qxxx_WriteEnable() != F_OPR_OK)
            return F_OPR_ErrWDIS;
    }

    if(wSize >= SPIFlash.pageSize)
        return F_OPR_ErrSIZE;
    /* send page program command */
    ret = qspi_SendCommand(W25Q_PAGE_PROGRAM, Addr, 
                            0, QSPI_INSTRUCTION_4_LINES, 
                            QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, 
                            QSPI_DATA_4_LINES);
    /* QSPI send data */
    if(qspi_Transmit(pBuff, wSize) == F_SPI_OK)
    {
        ret = w25qxxx_WaitBusy();
        /* wait busy error */
        if(ret != F_OPR_OK) 
            return (ret);
        else
            return F_OPR_OK;
    }
    else
    {
        return F_OPR_ERR;
    }
}

/*
*   call in HAL_QSPI_Init();
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
}

/*
*   w25qxxx controler initialize
**/
static void w25qxxx_ControllerInit(void)
{
    QSPI_Handler.Instance = QUADSPI;

    QSPI_Handler.Init.ClockPrescaler = 1; //QSPI clock frequency -> 200/(CP + 1) = 100mhz
    QSPI_Handler.Init.FifoThreshold = 4;
    QSPI_Handler.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;//must set 0 in DDR mode
    QSPI_Handler.Init.FlashSize = (QFLASH_SIZE - 1);
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;//55ns
    QSPI_Handler.Init.ClockMode = QSPI_CLOCK_MODE_0;
    QSPI_Handler.Init.FlashID = QSPI_FLASH_ID_1;
    QSPI_Handler.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    HAL_QSPI_Init(&QSPI_Handler);
}

/************************* public function ****************************/
/* w25qxxx init */
void SPIFlash_Init(void)
{
	uint8_t ret = 0;
    /* QSPI initialize */
    w25qxxx_ControllerInit();
    /* delay short time */
    ShortDelay(100);
    SPIFlash_DevideInit();
    /* enable write operate */
    ret = w25qxxx_WriteEnable();
    printf("enable write operate, ret = %d\r\n", ret);

    /* enable QSPI mode */
    ret = w25qxxx_QSPI_Enable();
    printf("enable QSPI mode, ret = %d\r\n", ret);

    /* get flash id */
    ret = w25qxxx_GetDeviceID();
    printf("get flash id, ret = %d, id = 0x%04x\r\n", ret, SPIFlash.id);
    
    /* update flash device */
    switch (SPIFlash.id)
    {
        case 0xEF13://W25Q80
            printf("ID : 0x%04x, device unsupport!\r\n", SPIFlash.id);
            break;
        case 0xEF14://W25Q16
            printf("ID : 0x%04x, device unsupport!\r\n", SPIFlash.id);
            break;
        case 0xEF15://W25Q32
            printf("ID : 0x%04x, device unsupport!\r\n", SPIFlash.id);
            break;
        case 0xEF16://W25Q64
            printf("ID : 0x%04x, device unsupport!\r\n", SPIFlash.id);
            break;
        case 0xEF17://W25Q128
            printf("ID : 0x%04x, device unsupport!\r\n", SPIFlash.id);
            break;
        case 0xEF18://W25Q256
            printf("ID : 0x%04x, device support!\r\n", SPIFlash.id);
            SPIFlash.memorySize = QFLASH_SIZE;//32Mbyte
            SPIFlash.pageSize = QFLASH_PAGESIZE;//256Byte
            SPIFlash.sectorSize = QFLASH_SECTORSIZE;//4Kbyte
            SPIFlash.totalSector = QFLASH_SIZE / QFLASH_SECTORSIZE;//8192
            SPIFlash.blockSize = QFLASH_BLOCKSIZE;//64Kbyte
            SPIFlash.totalBlock = QFLASH_SIZE / QFLASH_BLOCKSIZE;//512
            SPIFlash.state = F_STA_OK;
            break;
        default :
            printf("ID : 0x%04x, device unknow!\r\n", SPIFlash.id);
            break;
    }

    /* set 4byte address mode */
    ret = w25qxxx_SetAddressMode(F_ADDR_4B);
    printf("address mode = %d\r\n", ret);  

    /* set read parameter */
    ret = w25qxxx_SetReadParameter(3, 0);
    printf("set read parameter, ret = %d\r\n", ret);
}

void SPIFlash_DevideInit(void)
{
    SPIFlash.id = 0;
    SPIFlash.memorySize = 0; //total bytes
    SPIFlash.pageSize = 0;   //bytes per page
    SPIFlash.sectorSize = 0; //bytes per sector
    SPIFlash.totalSector = 0;
    SPIFlash.blockSize = 0;  //bytes per block   
    SPIFlash.totalBlock = 0;
    SPIFlash.SPIMode = F_SerialSPI;
    SPIFlash.AddressMode = F_ADDR_3B;
    SPIFlash.bWrite = 0;//write enable?
    SPIFlash.pWrite = 0;//write protect?
    SPIFlash.state = F_STA_INIT;
}

uint8_t SPIFlash_EraseSector(uint32_t sectorAddr, uint32_t sectorNbr)
{
    uint8_t ret, NeedErase = 0;
    //uint8_t tempBuf[4096];//too big , must be careful
    uint16_t i;

    if(sectorAddr >= SPIFlash.totalSector)
    {
        return F_OPR_ErrADDR;
    }

    while(sectorNbr)
    {
        memset(OPRBUFFER, 0, QFLASH_SECTORSIZE);
        
        ret = w25qxxx_Read(OPRBUFFER, (sectorAddr * SPIFlash.sectorSize), SPIFlash.sectorSize);//read 1sector for check
        if(ret != F_OPR_OK)
        {
            return ret;
        }
        /* check read out data */
        for(i = 0; i < SPIFlash.sectorSize; i++)
        {
            if(OPRBUFFER[i] != 0xff)
            {
                NeedErase = 1;
                break;
            }
        }

        if(NeedErase)
        {
            ret = w25qxxx_EraseSector(sectorAddr);
            if(ret != F_OPR_OK)
            {
                return ret;
            }
        }
        sectorAddr++;
        sectorNbr--;
    }

    return F_OPR_OK;
}

#if 0
/* SPI flash write sector */
uint8_t SPIFlash_ReadMultipleSectors(uint8_t *pBuff, uint32_t StartSector, uint32_t SectorNbr, uint32_t rSize)
{
    
}
#endif

/* SPI flash read */
uint8_t SPIFlash_Read(uint8_t *pBuff, uint32_t Addr, uint16_t rSize)
{
    return w25qxxx_Read(pBuff, Addr, rSize);
}

#if 0
/* SPI flash write sector */
uint8_t SPIFlash_WriteMultipleSectors(uint8_t *pBuff, uint32_t StartSector, uint32_t SectorNbr, uint8_t wSize)
{
    uint32_t relSector, wSector;
    uint32_t relSize, curSize;

    if(StartSector >= SPIFlash.totalSector)
    {
        return F_OPR_ErrADDR;
    }

    relSize = wSize;
    wSector = StartSector;
    while(relSize && SectorNbr)
    {
        if(relSize >= SPIFlash.sectorSize)
            curSize = SPIFlash.sectorSize
        else
            curSize = relSize;
        /* write sector */
        if(w25qxxx_WriteSector(&SPIFlash, pBuff, wSector, curSize, 1) != F_OPR_OK)
        {
            return F_OPR_ErrWS;
        }
        /* next sector */
        wSector++;
        /* sector be written */
        SectorNbr--;
        if(wSector >= SPIFlash.totalSector)
        {
            return F_OPR_ERR;
        }
        /* buffer pointer of next sector */
        pBuff += curSize;
        /* size of buffer be written */
        relSize -= curSize;
    }

    return F_OPR_OK;
}
#endif


/* SPI Flash write, not check */
uint8_t SPIFlash_Write(uint8_t *pBuff, uint32_t Addr, uint32_t wSize)
{
    uint32_t relSize, curSize;

    if(Addr >= SPIFlash.memorySize)
    {
        return F_OPR_ErrADDR;
    }

    relSize = wSize;
    while(relSize)
    {
        if(relSize >= SPIFlash.pageSize)
            curSize = SPIFlash.pageSize;
        else
            curSize = relSize;

        if(w25qxxx_WritePage(Addr, pBuff, curSize) != F_OPR_OK)
        {
            return F_OPR_ERR;
        }

        pBuff += curSize;
        relSize -= curSize;
        Addr += curSize;
    }

    return F_OPR_OK;
}


/* SPI Flash write with check*/
uint8_t SPIFlash_Write_Erase(uint8_t *pBuff, uint32_t Addr, uint32_t wSize)
{
    uint32_t procSector, i;
    uint32_t relSize, curSize;
    uint32_t posit, wpSize, wpRelSize, procSize; 
    uint32_t procAddr;
    uint8_t NeedErase = 0;
    uint8_t *pWrite = NULL;

    //uint8_t tempBuf[4096];//too big , must be careful

    if(Addr >= SPIFlash.memorySize)
    {
        return F_OPR_ErrADDR;
    }
    
    memset(OPRBUFFER, 0, QFLASH_SECTORSIZE);

    relSize = wSize;//number byte to write
    while(relSize)
    {
        procSector = Addr / SPIFlash.sectorSize;//the address is included in which sector 
        posit = Addr % SPIFlash.sectorSize;//the posit of the address in sector
        if(procSector >= SPIFlash.totalSector)
        {
            return F_OPR_ERR;
        }
        
        /* read the sector */
        if(w25qxxx_Read(OPRBUFFER, (procSector * SPIFlash.sectorSize), SPIFlash.sectorSize) != F_OPR_OK)
        {
            return F_OPR_ErrREAD;
        }
        /* check the sector */
        for(i = posit; i < SPIFlash.sectorSize; i++)
        {
            if(OPRBUFFER[i] != 0xff)
            {   
                /* need erase */
                NeedErase = 1;
                break;
            }
        }
        
        if(NeedErase)
        {
            /* erase sector */
            if(w25qxxx_EraseSector(procSector) != F_OPR_OK)
            {
                return F_OPR_ErrES;
            }
            wpRelSize = SPIFlash.sectorSize;//need write the whole sector
            procAddr = procSector * SPIFlash.sectorSize;//page program address
        }
        else
        {
            wpRelSize = SPIFlash.sectorSize - posit;//just write the new data only
            procAddr = Addr;//page program address
        }

        /* field cacheBuff with pBuff */
        for(i = posit; i < SPIFlash.sectorSize; i++)
        {
            OPRBUFFER[i] = *pBuff;
            pBuff++;
        }

        pWrite = &OPRBUFFER[posit];//get write buffer on current tune
        relSize -= wpRelSize;//current size be written
        Addr += wpRelSize;//update address for next tune
        
        /* page program */
        while(wpRelSize)
        {
            if(wpRelSize >= SPIFlash.pageSize)
                wpSize = SPIFlash.pageSize;
            else
                wpSize = wpRelSize;

            if(w25qxxx_WritePage(procAddr, pWrite, wpSize) != F_OPR_OK)
            {
                return F_OPR_ERR;
            }
            pWrite += wpSize;
            procAddr += wpSize;
            wpRelSize -= wpSize;
        }
    }

    return F_OPR_OK;
}

/* spi flash test */
void SPIFlash_RWTest(void)
{
    uint8_t testBuff[256];
    uint16_t i;
    uint8_t ret;

    for(i = 0; i < 256; i++)
    {
        testBuff[i] = i;
    }
    
    printf("SPIFlash_RWTest start...\r\n");
    ret = SPIFlash_Write(testBuff, (SPIFlash.memorySize - 256 - 1), 256);
    if(ret != F_OPR_OK)
    {
        printf("SPIFlash_RWTest write error, ret = %d\r\n", ret);
        return ;
    }

    for(i = 0; i < 256; i++)
    {
        testBuff[i] = 0;
    }
    ret = SPIFlash_Read(testBuff, (SPIFlash.memorySize - 256 - 1), 256);
    if(ret != F_OPR_OK)
    {
        printf("SPIFlash_RWTest read error, ret = %d\r\n", ret);
        return ;
    }
    printf("SPIFlash_RWTest finish...\r\n");
}

