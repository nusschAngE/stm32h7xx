
#include "stm32h7xx.h"
#include "sdram.h"
#include "delay.h"

/* SDRAM mode register */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

//uint16_t testSdram[4096] __attribute__((section(".ARM.__at_0xC0000000")));

/* SDRAM handle struct */
static SDRAM_HandleTypeDef SDRAM_Handler;

/*  STATIC 
*/
static void SDRAM_FMC_Init(void);
static uint8_t sdram_SendCommand(uint8_t Bank, uint8_t Cmd, uint8_t Refresh, uint32_t RegVal);

/******************* PUBLIC FUNCTION ************************/

/*
*   called in HAL_SDRAM_Init()
*/
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef GPIO_Initure;
    RCC_PeriphCLKInitTypeDef RCCSDRAM_Sture; 
    
	__HAL_RCC_SYSCFG_CLK_ENABLE();				
    __HAL_RCC_FMC_CLK_ENABLE();             
    __HAL_RCC_GPIOC_CLK_ENABLE();             
    __HAL_RCC_GPIOD_CLK_ENABLE();             
    __HAL_RCC_GPIOE_CLK_ENABLE();               
    __HAL_RCC_GPIOF_CLK_ENABLE();           
    __HAL_RCC_GPIOG_CLK_ENABLE();          

	//PC2,3 analog disable
	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC2, SYSCFG_SWITCH_PC2_CLOSE);
	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_CLOSE);
  
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;  
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;         
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       
    GPIO_Initure.Alternate = GPIO_AF12_FMC;         
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);        
    
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 
                        |GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15; 
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;         
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       
    GPIO_Initure.Alternate = GPIO_AF12_FMC;                    
    HAL_GPIO_Init(GPIOD,&GPIO_Initure); 
    
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8
                        | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12
                        | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;         
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       
    GPIO_Initure.Alternate = GPIO_AF12_FMC;                    
    HAL_GPIO_Init(GPIOE,&GPIO_Initure); 
    
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 
                        | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 
                        | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;         
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       
    GPIO_Initure.Alternate = GPIO_AF12_FMC;                    
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 
                        |GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15; 
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;         
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;       
    GPIO_Initure.Alternate = GPIO_AF12_FMC;                    
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);	
}

/* SDRAM hardware initialize */
void sdram_Init(void)
{
    uint32_t temp = 0;

    SDRAM_FMC_Init();
    uSleep(500);

    /*  SDRAM init sequence
    */
    sdram_SendCommand(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0); //sdram clock enable
    uSleep(300);                                        //must delay more than 200us
	sdram_SendCommand(0, FMC_SDRAM_CMD_PALL, 1, 0);       //Precharge 
    sdram_SendCommand(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);//auto refresh times

    /*
    *   mode register
    *   bit0~2 : burst lenght
    *   bit3 : burst type
    *   bit4~6 : CAS
    *   bit7~8 : operate mode
    *   bit9 : write burst mode
    *   bit10~11 : reserve
    */
    temp = SDRAM_MODEREG_BURST_LENGTH_1          	
            | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   	
            | SDRAM_MODEREG_CAS_LATENCY_3           	
            | SDRAM_MODEREG_OPERATING_MODE_STANDARD   
            | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;    
    sdram_SendCommand(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp); 

    /*
    *   SDRAM refresh rate
    *   SDRAM refresh time -> 64ms
    *   SDRAM refresh line -> 2^13 = 8192
    *   SDRAM clock frequency -> 100mhz
    *   SDRAM refresh rate -> 64 * 1000 / 8192 = 7.81us
    *
    *   COUNT = 7.81(us) * 100(mhz) #- 20# = 781
    */
    HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, 781);
}

void sdram_WriteBuffer(uint8_t *pBuff, uint32_t Addr, uint32_t wSize, uint32_t *bWrite)
{
    uint32_t bw = 0;

    if(Addr >= SDRAM_SIZE) 
    {   
        *bWrite = 0;
        return ;
    }

    while(wSize--)
    {
        *(volatile uint8_t *)(SDRAM_ADDRESS + Addr) = *pBuff;
        bw++;
        pBuff++;
        Addr++;
        if(Addr >= SDRAM_SIZE) break;
    }

    *bWrite = bw;
}

void sdram_ReadBuffer(uint8_t *pBuff, uint32_t Addr, uint32_t rSize, uint32_t *bRead)
{
    uint32_t br = 0;

    if(Addr >= SDRAM_SIZE) 
    {
        *bRead = 0;
        return ;
    }

    while(rSize--)
    {
        *pBuff = *(volatile uint8_t *)(SDRAM_ADDRESS + Addr);
        br++;
        pBuff++;
        Addr++;
        if(Addr >= SDRAM_SIZE) break;
    }

    *bRead = br;
}

/****************** PRIVATE FUNCTION *******************/
/* SDRAM controler init */
static void SDRAM_FMC_Init(void)
{
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
                                                     
    SDRAM_Handler.Instance = FMC_SDRAM_DEVICE;                          	 
    SDRAM_Handler.Init.SDBank = FMC_SDRAM_BANK1;                         
    SDRAM_Handler.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;   
    SDRAM_Handler.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;    //address pins    
    SDRAM_Handler.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16; //data pins     
    SDRAM_Handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;  //4 banks
    SDRAM_Handler.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;               
    SDRAM_Handler.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE; //write enable 
    SDRAM_Handler.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;  //SDRAM clock : PLL2_DIVR2/2=200M/2=100M=10ns
    SDRAM_Handler.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;               
    SDRAM_Handler.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;  //read channel delay     

    /* current SDRAM core clock frequency is 100mhz[10ns]
    */
    SDRAM_Timing.LoadToActiveDelay = 2;     //tRSC -> 2 ticks                             
    SDRAM_Timing.ExitSelfRefreshDelay = 8;  //tXSR -> 75ns                              
    SDRAM_Timing.SelfRefreshTime = 5;       //tRAS -> 45ns                                                           
    SDRAM_Timing.RowCycleDelay = 7;         //tRC -> 65ns                               
    SDRAM_Timing.WriteRecoveryTime = 2;     //tWR -> 2 ticks                               
    SDRAM_Timing.RPDelay = 2;               //tRP -> 20ns                                 
    SDRAM_Timing.RCDDelay = 2;              //tRCD -> 20ns                              
    HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing);  	
}

static uint8_t sdram_SendCommand(uint8_t Bank, uint8_t Cmd, uint8_t Refresh, uint32_t RegVal)
{
    uint32_t TargeBank = 0;
    FMC_SDRAM_CommandTypeDef Command;

    if(Bank == 0)
        TargeBank = FMC_SDRAM_CMD_TARGET_BANK1;
    else
        TargeBank = FMC_SDRAM_CMD_TARGET_BANK2;
        
    Command.CommandMode = Cmd;             
    Command.CommandTarget = TargeBank;     
    Command.AutoRefreshNumber = Refresh;     
    Command.ModeRegisterDefinition = RegVal; 
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, 0xff) == HAL_OK)
    {
        return 1;  
    }

    return 0; 
}


#if 0

/* SDRAM RAM test function */
#define TEST_SZIE       (4096)
    
uint16_t testSdram0[TEST_SZIE] __attribute__((section(".ARM.__at_0xC0000000")));
uint16_t testSdram1[TEST_SZIE] __attribute__((section(".ARM.__at_0xC0002000")));
uint16_t testSdram2[TEST_SZIE] __attribute__((section(".ARM.__at_0xC0010000")));
uint16_t testSdram3[TEST_SZIE] __attribute__((section(".ARM.__at_0xC0100000")));
uint16_t testSdram4[TEST_SZIE] __attribute__((section(".ARM.__at_0xC1000000")));

void sdram_RAMTestFunc(void)
{
    uint16_t *pTest = NULL;
    volatile uint16_t temp = 0;
    uint16_t i, j, errCount = 0;
	uint32_t pAddr = 0 ,Addr = 0;    

	for(j = 0; j < 5; j++)
	{
	    if(j == 0)
	        pTest = testSdram0;
	    else if(j == 1)
	        pTest = testSdram1;
	    else if(j == 2)
	        pTest = testSdram2;
	    else if(j == 3)
	        pTest = testSdram3;
	    else
	        pTest = testSdram4;

	    printf("sdram wrtie start : j = %d, pTest = %p\r\n", j, pTest);
		for(i = 0; i < TEST_SZIE; i++)
		{
			pTest[i] = i;
		}

        printf("sdram read start : pTest = %p\r\n", pTest);
		for(i = 0; i < TEST_SZIE; i++)
		{
			temp = pTest[i];
			if(temp != i)
			{
			    printf("error : addr = %d, temp = %d\r\n", Addr, temp);
			    errCount++;
			}
			Addr++;
		}
		printf("sdram read done : errCount = %d\r\n", errCount);
	}
	
	printf("SDRAM read & write test finish\r\n");    
}
#endif

