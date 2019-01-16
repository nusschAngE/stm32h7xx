
#include "stm32h7xx.h"
#include "lcd.h"
#include "delay.h"

/* LCD backlight control gpio */
#define LCD_BK_GPIO     GPIOB
#define LCD_BK_PIN      GPIO_PIN_5

/* LCD scan direction */
typedef enum
{
    L2R_U2D = 0, 	
    L2R_D2U,	
    R2L_U2D,
    R2L_D2U,

    U2D_L2R,
    U2D_R2L,
    D2U_L2R,
    D2U_R2L,

    SCANDIR_NUM
}LCDScanDirection;

#define DEFAULT_SCANDIR  L2R_U2D 

/* LCD command defines */
#define NT35510_SLEEP_IN    ((uint16_t)0x1000)
#define NT35510_SLEEP_OUT   ((uint16_t)0x1100)
#define NT35510_DISP_OFF    ((uint16_t)0x2800)
#define NT35510_DISP_ON     ((uint16_t)0x2900)
#define NT35510_SET_COLUMN  ((uint16_t)0x2a00)
#define NT35510_SET_ROW     ((uint16_t)0x2b00)
#define NT35510_FLUSH_RAM   ((uint16_t)0x2c00)
#define NT35510_MADCTL		((uint16_t)0x3600)

/* LCD controller */
SRAM_HandleTypeDef LCDIO_Handle;
/* LCD device struct */
LCD_Device lcdDev;

/* STATIC 
*/
static void NT35510_write_reg(uint16_t reg);
static void NT35510_write_ram(uint16_t data);
static void NT35510_set_register(uint16_t reg, uint16_t data);
static void NT35510_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
static void NT35510_flush_color(LCD_COLOR color, uint32_t size);
static void NT35510_flush_data(const LCD_COLOR *pData, uint32_t size);

/******************* PUBLIC FUNCTION ***********************/
/*
*/
uint8_t LCD_SRAM_IOInit(void)
{
	GPIO_InitTypeDef GPIO_Init;	
	FMC_NORSRAM_TimingTypeDef ReadTiming;
	FMC_NORSRAM_TimingTypeDef WriteTiming;

    /* Peripheral clock enable */
	__HAL_RCC_FMC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_Init.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
							|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14 
							|GPIO_PIN_15;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOE, &GPIO_Init);

	GPIO_Init.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_13 
							|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1 
							|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOD, &GPIO_Init);
#if 0
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOD, &GPIO_Init); 
#endif
    /* lcd backlight control pin */
    GPIO_Init.Pin = LCD_BK_PIN;        
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;  
    GPIO_Init.Pull = GPIO_PULLUP;        
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(LCD_BK_GPIO, &GPIO_Init); 

	/** Perform the SRAM1 memory initialization sequence
	*/
	LCDIO_Handle.Instance = FMC_NORSRAM_DEVICE;
	LCDIO_Handle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
	/* hsram1.Init */
	LCDIO_Handle.Init.NSBank = FMC_NORSRAM_BANK1;
	LCDIO_Handle.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
	LCDIO_Handle.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
	LCDIO_Handle.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
	LCDIO_Handle.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
	LCDIO_Handle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
	LCDIO_Handle.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
	LCDIO_Handle.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
	LCDIO_Handle.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
	LCDIO_Handle.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
	LCDIO_Handle.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
	LCDIO_Handle.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
	LCDIO_Handle.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
	//LCDIO_Handle.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
	//LCDIO_Handle.Init.PageSize = FMC_PAGE_SIZE_NONE;

	/* current FMC clock frequecy is 200mhz[5ns]
	*/

	/* ReadTiming 
	*/
	ReadTiming.AddressSetupTime = 15;
	ReadTiming.AddressHoldTime = 1;
	ReadTiming.DataSetupTime = 10;
	//ReadTiming.BusTurnAroundDuration = 15;
	//Timing.CLKDivision = 16;
	//ReadTiming.DataLatency = 3;
	ReadTiming.AccessMode = FMC_ACCESS_MODE_A;

	/* WriteTiming 
	*/
	WriteTiming.AddressSetupTime = 3;
	WriteTiming.AddressHoldTime = 0;
	WriteTiming.DataSetupTime = 2;
	//WriteTiming.BusTurnAroundDuration = 15;
	//Timing.CLKDivision = 16;
	//WriteTiming.DataLatency = 3;
	WriteTiming.AccessMode = FMC_ACCESS_MODE_A;

	if (HAL_SRAM_Init(&LCDIO_Handle, &ReadTiming, &WriteTiming) != HAL_OK)
	{
		return (1);//init error
	}	 

	return (0);//init done
}

void LCD_SRAM_MPUInit(void)
{
	MPU_Region_InitTypeDef MPU_Initure;

	HAL_MPU_Disable();					
	
	/* LCD : FMC SRAM Bank1 */
	MPU_Initure.Enable = MPU_REGION_ENABLE;	
	MPU_Initure.Number = MPU_REGION_NUMBER1;	
	MPU_Initure.BaseAddress = (uint32_t)0x60000000;
	MPU_Initure.Size = MPU_REGION_SIZE_32MB;		
	MPU_Initure.SubRegionDisable = 0x00;
	MPU_Initure.TypeExtField = MPU_TEX_LEVEL1;
	MPU_Initure.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_Initure.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_Initure.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_Initure.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_Initure.IsBufferable = MPU_ACCESS_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_Initure);
	
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT); 	
}

uint8_t LCD_Init(void)
{
    LCD_SRAM_IOInit();
	/* SRAM MPU Init */
    LCD_SRAM_MPUInit();
    /*delay*/
    TimDelayMs(20);

 	NT35510_set_register(0xF000,0x55);
	NT35510_set_register(0xF001,0xAA);
	NT35510_set_register(0xF002,0x52);
	NT35510_set_register(0xF003,0x08);
	NT35510_set_register(0xF004,0x01);
    //AVDD Set AVDD 5.2V
	NT35510_set_register(0xB000,0x0D);
	NT35510_set_register(0xB001,0x0D);
	NT35510_set_register(0xB002,0x0D);
	//AVDD ratio
	NT35510_set_register(0xB600,0x34);
	NT35510_set_register(0xB601,0x34);
	NT35510_set_register(0xB602,0x34);
    //AVEE -5.2V
	NT35510_set_register(0xB100,0x0D);
	NT35510_set_register(0xB101,0x0D);
	NT35510_set_register(0xB102,0x0D);
    //AVEE ratio
	NT35510_set_register(0xB700,0x34);
	NT35510_set_register(0xB701,0x34);
	NT35510_set_register(0xB702,0x34);
    //VCL -2.5V
	NT35510_set_register(0xB200,0x00);
	NT35510_set_register(0xB201,0x00);
	NT35510_set_register(0xB202,0x00);
    //VCL ratio
	NT35510_set_register(0xB800,0x24);
	NT35510_set_register(0xB801,0x24);
	NT35510_set_register(0xB802,0x24);
    //VGH 15V (Free pump)
	NT35510_set_register(0xBF00,0x01);
	NT35510_set_register(0xB300,0x0F);
	NT35510_set_register(0xB301,0x0F);
	NT35510_set_register(0xB302,0x0F);
    //VGH ratio
	NT35510_set_register(0xB900,0x34);
	NT35510_set_register(0xB901,0x34);
	NT35510_set_register(0xB902,0x34);
    //VGL_REG -10V
	NT35510_set_register(0xB500,0x08);
	NT35510_set_register(0xB501,0x08);
	NT35510_set_register(0xB502,0x08);
	NT35510_set_register(0xC200,0x03);
	//VGLX ratio
	NT35510_set_register(0xBA00,0x24);
	NT35510_set_register(0xBA01,0x24);
	NT35510_set_register(0xBA02,0x24);
	//VGMP/VGSP 4.5V/0V
	NT35510_set_register(0xBC00,0x00);
	NT35510_set_register(0xBC01,0x78);
	NT35510_set_register(0xBC02,0x00);
	//VGMN/VGSN -4.5V/0V
	NT35510_set_register(0xBD00,0x00);
	NT35510_set_register(0xBD01,0x78);
	NT35510_set_register(0xBD02,0x00);
	//VCOM
	NT35510_set_register(0xBE00,0x00);
	NT35510_set_register(0xBE01,0x64);
	//Gamma Setting
	NT35510_set_register(0xD100,0x00);
	NT35510_set_register(0xD101,0x33);
	NT35510_set_register(0xD102,0x00);
	NT35510_set_register(0xD103,0x34);
	NT35510_set_register(0xD104,0x00);
	NT35510_set_register(0xD105,0x3A);
	NT35510_set_register(0xD106,0x00);
	NT35510_set_register(0xD107,0x4A);
	NT35510_set_register(0xD108,0x00);
	NT35510_set_register(0xD109,0x5C);
	NT35510_set_register(0xD10A,0x00);
	NT35510_set_register(0xD10B,0x81);
	NT35510_set_register(0xD10C,0x00);
	NT35510_set_register(0xD10D,0xA6);
	NT35510_set_register(0xD10E,0x00);
	NT35510_set_register(0xD10F,0xE5);
	NT35510_set_register(0xD110,0x01);
	NT35510_set_register(0xD111,0x13);
	NT35510_set_register(0xD112,0x01);
	NT35510_set_register(0xD113,0x54);
	NT35510_set_register(0xD114,0x01);
	NT35510_set_register(0xD115,0x82);
	NT35510_set_register(0xD116,0x01);
	NT35510_set_register(0xD117,0xCA);
	NT35510_set_register(0xD118,0x02);
	NT35510_set_register(0xD119,0x00);
	NT35510_set_register(0xD11A,0x02);
	NT35510_set_register(0xD11B,0x01);
	NT35510_set_register(0xD11C,0x02);
	NT35510_set_register(0xD11D,0x34);
	NT35510_set_register(0xD11E,0x02);
	NT35510_set_register(0xD11F,0x67);
	NT35510_set_register(0xD120,0x02);
	NT35510_set_register(0xD121,0x84);
	NT35510_set_register(0xD122,0x02);
	NT35510_set_register(0xD123,0xA4);
	NT35510_set_register(0xD124,0x02);
	NT35510_set_register(0xD125,0xB7);
	NT35510_set_register(0xD126,0x02);
	NT35510_set_register(0xD127,0xCF);
	NT35510_set_register(0xD128,0x02);
	NT35510_set_register(0xD129,0xDE);
	NT35510_set_register(0xD12A,0x02);
	NT35510_set_register(0xD12B,0xF2);
	NT35510_set_register(0xD12C,0x02);
	NT35510_set_register(0xD12D,0xFE);
	NT35510_set_register(0xD12E,0x03);
	NT35510_set_register(0xD12F,0x10);
	NT35510_set_register(0xD130,0x03);
	NT35510_set_register(0xD131,0x33);
	NT35510_set_register(0xD132,0x03);
	NT35510_set_register(0xD133,0x6D);
	NT35510_set_register(0xD200,0x00);
	NT35510_set_register(0xD201,0x33);
	NT35510_set_register(0xD202,0x00);
	NT35510_set_register(0xD203,0x34);
	NT35510_set_register(0xD204,0x00);
	NT35510_set_register(0xD205,0x3A);
	NT35510_set_register(0xD206,0x00);
	NT35510_set_register(0xD207,0x4A);
	NT35510_set_register(0xD208,0x00);
	NT35510_set_register(0xD209,0x5C);
	NT35510_set_register(0xD20A,0x00);

	NT35510_set_register(0xD20B,0x81);
	NT35510_set_register(0xD20C,0x00);
	NT35510_set_register(0xD20D,0xA6);
	NT35510_set_register(0xD20E,0x00);
	NT35510_set_register(0xD20F,0xE5);
	NT35510_set_register(0xD210,0x01);
	NT35510_set_register(0xD211,0x13);
	NT35510_set_register(0xD212,0x01);
	NT35510_set_register(0xD213,0x54);
	NT35510_set_register(0xD214,0x01);
	NT35510_set_register(0xD215,0x82);
	NT35510_set_register(0xD216,0x01);
	NT35510_set_register(0xD217,0xCA);
	NT35510_set_register(0xD218,0x02);
	NT35510_set_register(0xD219,0x00);
	NT35510_set_register(0xD21A,0x02);
	NT35510_set_register(0xD21B,0x01);
	NT35510_set_register(0xD21C,0x02);
	NT35510_set_register(0xD21D,0x34);
	NT35510_set_register(0xD21E,0x02);
	NT35510_set_register(0xD21F,0x67);
	NT35510_set_register(0xD220,0x02);
	NT35510_set_register(0xD221,0x84);
	NT35510_set_register(0xD222,0x02);
	NT35510_set_register(0xD223,0xA4);
	NT35510_set_register(0xD224,0x02);
	NT35510_set_register(0xD225,0xB7);
	NT35510_set_register(0xD226,0x02);
	NT35510_set_register(0xD227,0xCF);
	NT35510_set_register(0xD228,0x02);
	NT35510_set_register(0xD229,0xDE);
	NT35510_set_register(0xD22A,0x02);
	NT35510_set_register(0xD22B,0xF2);
	NT35510_set_register(0xD22C,0x02);
	NT35510_set_register(0xD22D,0xFE);
	NT35510_set_register(0xD22E,0x03);
	NT35510_set_register(0xD22F,0x10);
	NT35510_set_register(0xD230,0x03);
	NT35510_set_register(0xD231,0x33);
	NT35510_set_register(0xD232,0x03);
	NT35510_set_register(0xD233,0x6D);
	NT35510_set_register(0xD300,0x00);
	NT35510_set_register(0xD301,0x33);
	NT35510_set_register(0xD302,0x00);
	NT35510_set_register(0xD303,0x34);
	NT35510_set_register(0xD304,0x00);
	NT35510_set_register(0xD305,0x3A);
	NT35510_set_register(0xD306,0x00);
	NT35510_set_register(0xD307,0x4A);
	NT35510_set_register(0xD308,0x00);
	NT35510_set_register(0xD309,0x5C);
	NT35510_set_register(0xD30A,0x00);

	NT35510_set_register(0xD30B,0x81);
	NT35510_set_register(0xD30C,0x00);
	NT35510_set_register(0xD30D,0xA6);
	NT35510_set_register(0xD30E,0x00);
	NT35510_set_register(0xD30F,0xE5);
	NT35510_set_register(0xD310,0x01);
	NT35510_set_register(0xD311,0x13);
	NT35510_set_register(0xD312,0x01);
	NT35510_set_register(0xD313,0x54);
	NT35510_set_register(0xD314,0x01);
	NT35510_set_register(0xD315,0x82);
	NT35510_set_register(0xD316,0x01);
	NT35510_set_register(0xD317,0xCA);
	NT35510_set_register(0xD318,0x02);
	NT35510_set_register(0xD319,0x00);
	NT35510_set_register(0xD31A,0x02);
	NT35510_set_register(0xD31B,0x01);
	NT35510_set_register(0xD31C,0x02);
	NT35510_set_register(0xD31D,0x34);
	NT35510_set_register(0xD31E,0x02);
	NT35510_set_register(0xD31F,0x67);
	NT35510_set_register(0xD320,0x02);
	NT35510_set_register(0xD321,0x84);
	NT35510_set_register(0xD322,0x02);
	NT35510_set_register(0xD323,0xA4);
	NT35510_set_register(0xD324,0x02);
	NT35510_set_register(0xD325,0xB7);
	NT35510_set_register(0xD326,0x02);
	NT35510_set_register(0xD327,0xCF);
	NT35510_set_register(0xD328,0x02);
	NT35510_set_register(0xD329,0xDE);
	NT35510_set_register(0xD32A,0x02);
	NT35510_set_register(0xD32B,0xF2);
	NT35510_set_register(0xD32C,0x02);
	NT35510_set_register(0xD32D,0xFE);
	NT35510_set_register(0xD32E,0x03);
	NT35510_set_register(0xD32F,0x10);
	NT35510_set_register(0xD330,0x03);
	NT35510_set_register(0xD331,0x33);
	NT35510_set_register(0xD332,0x03);
	NT35510_set_register(0xD333,0x6D);
	NT35510_set_register(0xD400,0x00);
	NT35510_set_register(0xD401,0x33);
	NT35510_set_register(0xD402,0x00);
	NT35510_set_register(0xD403,0x34);
	NT35510_set_register(0xD404,0x00);
	NT35510_set_register(0xD405,0x3A);
	NT35510_set_register(0xD406,0x00);
	NT35510_set_register(0xD407,0x4A);
	NT35510_set_register(0xD408,0x00);
	NT35510_set_register(0xD409,0x5C);
	NT35510_set_register(0xD40A,0x00);
	NT35510_set_register(0xD40B,0x81);

	NT35510_set_register(0xD40C,0x00);
	NT35510_set_register(0xD40D,0xA6);
	NT35510_set_register(0xD40E,0x00);
	NT35510_set_register(0xD40F,0xE5);
	NT35510_set_register(0xD410,0x01);
	NT35510_set_register(0xD411,0x13);
	NT35510_set_register(0xD412,0x01);
	NT35510_set_register(0xD413,0x54);
	NT35510_set_register(0xD414,0x01);
	NT35510_set_register(0xD415,0x82);
	NT35510_set_register(0xD416,0x01);
	NT35510_set_register(0xD417,0xCA);
	NT35510_set_register(0xD418,0x02);
	NT35510_set_register(0xD419,0x00);
	NT35510_set_register(0xD41A,0x02);
	NT35510_set_register(0xD41B,0x01);
	NT35510_set_register(0xD41C,0x02);
	NT35510_set_register(0xD41D,0x34);
	NT35510_set_register(0xD41E,0x02);
	NT35510_set_register(0xD41F,0x67);
	NT35510_set_register(0xD420,0x02);
	NT35510_set_register(0xD421,0x84);
	NT35510_set_register(0xD422,0x02);
	NT35510_set_register(0xD423,0xA4);
	NT35510_set_register(0xD424,0x02);
	NT35510_set_register(0xD425,0xB7);
	NT35510_set_register(0xD426,0x02);
	NT35510_set_register(0xD427,0xCF);
	NT35510_set_register(0xD428,0x02);
	NT35510_set_register(0xD429,0xDE);
	NT35510_set_register(0xD42A,0x02);
	NT35510_set_register(0xD42B,0xF2);
	NT35510_set_register(0xD42C,0x02);
	NT35510_set_register(0xD42D,0xFE);
	NT35510_set_register(0xD42E,0x03);
	NT35510_set_register(0xD42F,0x10);
	NT35510_set_register(0xD430,0x03);
	NT35510_set_register(0xD431,0x33);
	NT35510_set_register(0xD432,0x03);
	NT35510_set_register(0xD433,0x6D);
	NT35510_set_register(0xD500,0x00);
	NT35510_set_register(0xD501,0x33);
	NT35510_set_register(0xD502,0x00);
	NT35510_set_register(0xD503,0x34);
	NT35510_set_register(0xD504,0x00);
	NT35510_set_register(0xD505,0x3A);
	NT35510_set_register(0xD506,0x00);
	NT35510_set_register(0xD507,0x4A);
	NT35510_set_register(0xD508,0x00);
	NT35510_set_register(0xD509,0x5C);
	NT35510_set_register(0xD50A,0x00);
	NT35510_set_register(0xD50B,0x81);

	NT35510_set_register(0xD50C,0x00);
	NT35510_set_register(0xD50D,0xA6);
	NT35510_set_register(0xD50E,0x00);
	NT35510_set_register(0xD50F,0xE5);
	NT35510_set_register(0xD510,0x01);
	NT35510_set_register(0xD511,0x13);
	NT35510_set_register(0xD512,0x01);
	NT35510_set_register(0xD513,0x54);
	NT35510_set_register(0xD514,0x01);
	NT35510_set_register(0xD515,0x82);
	NT35510_set_register(0xD516,0x01);
	NT35510_set_register(0xD517,0xCA);
	NT35510_set_register(0xD518,0x02);
	NT35510_set_register(0xD519,0x00);
	NT35510_set_register(0xD51A,0x02);
	NT35510_set_register(0xD51B,0x01);
	NT35510_set_register(0xD51C,0x02);
	NT35510_set_register(0xD51D,0x34);
	NT35510_set_register(0xD51E,0x02);
	NT35510_set_register(0xD51F,0x67);
	NT35510_set_register(0xD520,0x02);
	NT35510_set_register(0xD521,0x84);
	NT35510_set_register(0xD522,0x02);
	NT35510_set_register(0xD523,0xA4);
	NT35510_set_register(0xD524,0x02);
	NT35510_set_register(0xD525,0xB7);
	NT35510_set_register(0xD526,0x02);
	NT35510_set_register(0xD527,0xCF);
	NT35510_set_register(0xD528,0x02);
	NT35510_set_register(0xD529,0xDE);
	NT35510_set_register(0xD52A,0x02);
	NT35510_set_register(0xD52B,0xF2);
	NT35510_set_register(0xD52C,0x02);
	NT35510_set_register(0xD52D,0xFE);
	NT35510_set_register(0xD52E,0x03);
	NT35510_set_register(0xD52F,0x10);
	NT35510_set_register(0xD530,0x03);
	NT35510_set_register(0xD531,0x33);
	NT35510_set_register(0xD532,0x03);
	NT35510_set_register(0xD533,0x6D);
	NT35510_set_register(0xD600,0x00);
	NT35510_set_register(0xD601,0x33);
	NT35510_set_register(0xD602,0x00);
	NT35510_set_register(0xD603,0x34);
	NT35510_set_register(0xD604,0x00);
	NT35510_set_register(0xD605,0x3A);
	NT35510_set_register(0xD606,0x00);
	NT35510_set_register(0xD607,0x4A);
	NT35510_set_register(0xD608,0x00);
	NT35510_set_register(0xD609,0x5C);
	NT35510_set_register(0xD60A,0x00);
	NT35510_set_register(0xD60B,0x81);

	NT35510_set_register(0xD60C,0x00);
	NT35510_set_register(0xD60D,0xA6);
	NT35510_set_register(0xD60E,0x00);
	NT35510_set_register(0xD60F,0xE5);
	NT35510_set_register(0xD610,0x01);
	NT35510_set_register(0xD611,0x13);
	NT35510_set_register(0xD612,0x01);
	NT35510_set_register(0xD613,0x54);
	NT35510_set_register(0xD614,0x01);
	NT35510_set_register(0xD615,0x82);
	NT35510_set_register(0xD616,0x01);
	NT35510_set_register(0xD617,0xCA);
	NT35510_set_register(0xD618,0x02);
	NT35510_set_register(0xD619,0x00);
	NT35510_set_register(0xD61A,0x02);
	NT35510_set_register(0xD61B,0x01);
	NT35510_set_register(0xD61C,0x02);
	NT35510_set_register(0xD61D,0x34);
	NT35510_set_register(0xD61E,0x02);
	NT35510_set_register(0xD61F,0x67);
	NT35510_set_register(0xD620,0x02);
	NT35510_set_register(0xD621,0x84);
	NT35510_set_register(0xD622,0x02);
	NT35510_set_register(0xD623,0xA4);
	NT35510_set_register(0xD624,0x02);
	NT35510_set_register(0xD625,0xB7);
	NT35510_set_register(0xD626,0x02);
	NT35510_set_register(0xD627,0xCF);
	NT35510_set_register(0xD628,0x02);
	NT35510_set_register(0xD629,0xDE);
	NT35510_set_register(0xD62A,0x02);
	NT35510_set_register(0xD62B,0xF2);
	NT35510_set_register(0xD62C,0x02);
	NT35510_set_register(0xD62D,0xFE);
	NT35510_set_register(0xD62E,0x03);
	NT35510_set_register(0xD62F,0x10);
	NT35510_set_register(0xD630,0x03);
	NT35510_set_register(0xD631,0x33);
	NT35510_set_register(0xD632,0x03);
	NT35510_set_register(0xD633,0x6D);
	//LV2 Page 0 enable
	NT35510_set_register(0xF000,0x55);
	NT35510_set_register(0xF001,0xAA);
	NT35510_set_register(0xF002,0x52);
	NT35510_set_register(0xF003,0x08);
	NT35510_set_register(0xF004,0x00);
	//Display control
	NT35510_set_register(0xB100, 0xCC);
	NT35510_set_register(0xB101, 0x00);
	//Source hold time
	NT35510_set_register(0xB600,0x05);
	//Gate EQ control
	NT35510_set_register(0xB700,0x70);
	NT35510_set_register(0xB701,0x70);
	//Source EQ control (Mode 2)
	NT35510_set_register(0xB800,0x01);
	NT35510_set_register(0xB801,0x03);
	NT35510_set_register(0xB802,0x03);
	NT35510_set_register(0xB803,0x03);
	//Inversion mode (2-dot)
	NT35510_set_register(0xBC00,0x02);
	NT35510_set_register(0xBC01,0x00);
	NT35510_set_register(0xBC02,0x00);
	//Timing control 4H w/ 4-delay
	NT35510_set_register(0xC900,0xD0);
	NT35510_set_register(0xC901,0x02);
	NT35510_set_register(0xC902,0x50);
	NT35510_set_register(0xC903,0x50);
	NT35510_set_register(0xC904,0x50);
	NT35510_set_register(0x3500,0x00);
	//NT35510_set_register(0x3600,0x00);
	NT35510_set_register(0x3A00,0x55);  //16-bit/pixel
	NT35510_write_reg(0x1100); //sleep out
    TimDelayMs(50);
	NT35510_write_reg(0x2800); //display off

	lcdDev.id = 35510;
	lcdDev.dir = 0;
	lcdDev.width = DEFAULT_LCD_WIDTH;
	lcdDev.height = DEFAULT_LCD_HEIGHT;

    /* lcd device set on */
    LCD_SetScanDirection(DEFAULT_SCANDIR);
    LCD_SetBkliteOnOff(TRUE);
    LCD_SetDispOnOff(TRUE);
    //LCD_Clear(0x0000);//black
    LCD_FillBlock(0, 0 , lcdDev.width , lcdDev.height, 0x0000);

    return (0);
}

void LCD_SetDispOnOff(bool on)
{
    NT35510_write_reg(on ? NT35510_DISP_ON : NT35510_DISP_OFF);
}

void LCD_SetBkliteOnOff(bool on)
{
    HAL_GPIO_WritePin(LCD_BK_GPIO, LCD_BK_PIN, (on ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

void LCD_SetScanDirection(uint8_t dirt)
{
	uint16_t regval = 0;

	switch(dirt)
	{
		case L2R_U2D:
			lcdDev.dir = L2R_U2D;
	        lcdDev.width = DEFAULT_LCD_WIDTH;
	        lcdDev.height = DEFAULT_LCD_HEIGHT;
	        regval |= (0<<7)|(0<<6)|(0<<5);
			break;
		case L2R_D2U:
		    lcdDev.dir = L2R_D2U;
	        lcdDev.width = DEFAULT_LCD_WIDTH;
	        lcdDev.height = DEFAULT_LCD_HEIGHT;
			regval |= (1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D:
		    lcdDev.dir = R2L_U2D;
	        lcdDev.width = DEFAULT_LCD_WIDTH;
	        lcdDev.height = DEFAULT_LCD_HEIGHT;
			regval |= (0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U:
		    lcdDev.dir = R2L_D2U;
	        lcdDev.width = DEFAULT_LCD_WIDTH;
	        lcdDev.height = DEFAULT_LCD_HEIGHT;
			regval |= (1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R:
		    lcdDev.dir = U2D_L2R;
	        lcdDev.width = DEFAULT_LCD_HEIGHT;
	        lcdDev.height = DEFAULT_LCD_WIDTH;
			regval |= (0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L:
		    lcdDev.dir = U2D_R2L;
	        lcdDev.width = DEFAULT_LCD_HEIGHT;
	        lcdDev.height = DEFAULT_LCD_WIDTH;
			regval |= (0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R:
		    lcdDev.dir = D2U_L2R;
	        lcdDev.width = DEFAULT_LCD_HEIGHT;
	        lcdDev.height = DEFAULT_LCD_WIDTH;
			regval |= (1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L:
		    lcdDev.dir = D2U_R2L;
	        lcdDev.width = DEFAULT_LCD_HEIGHT;
	        lcdDev.height = DEFAULT_LCD_WIDTH;
			regval |= (1<<7)|(1<<6)|(1<<5); 
            break;
        default:
	        lcdDev.dir = L2R_U2D;
	        lcdDev.width = DEFAULT_LCD_WIDTH;
	        lcdDev.height = DEFAULT_LCD_HEIGHT;
	        regval |= (0<<7)|(0<<6)|(0<<5);    
			break;	 
	}

	NT35510_set_register(NT35510_MADCTL, regval);
}

void LCD_SetWorkRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint16_t tmp = 0;

    if((x >= lcdDev.width) || (y >= lcdDev.height))
    {
        return ;
    }

    /* start column */
    NT35510_set_register(NT35510_SET_COLUMN,     x >> 8);
    NT35510_set_register(NT35510_SET_COLUMN + 1, x & 0xff);
    /* end column */
    tmp = x + w - 1;
    NT35510_set_register(NT35510_SET_COLUMN + 2, tmp >> 8);
    NT35510_set_register(NT35510_SET_COLUMN + 3, tmp & 0xff);

    /* start row */
    NT35510_set_register(NT35510_SET_ROW,     y >> 8);
    NT35510_set_register(NT35510_SET_ROW + 1, y & 0xff);   
    /* end row */
    tmp = y + h - 1;
    NT35510_set_register(NT35510_SET_ROW + 2, tmp >> 8);
    NT35510_set_register(NT35510_SET_ROW + 3, tmp & 0xff);
}

void LCD_StartDataSession(void)
{
    NT35510_write_reg(NT35510_FLUSH_RAM);
}

void LCD_FlushData(LCD_COLOR *pData, uint32_t size)
{
    NT35510_write_reg(NT35510_FLUSH_RAM);

    while(size--)
    {
        LCD->LCD_RAM = *pData++;
        ///* picture */
        //pData++;
    }    
}

void LCD_Clear(const LCD_COLOR color)
{
    uint32_t wSize = DEFAULT_LCD_WIDTH * DEFAULT_LCD_HEIGHT;

    NT35510_set_window(0, 0, lcdDev.width, lcdDev.height);
    NT35510_flush_color(color, wSize);
}

void LCD_DrawPoint(uint16_t x, uint16_t y, LCD_COLOR color)
{
    if(x >= lcdDev.width || y >= lcdDev.height)
    {
        return ;
    }

    /* draw one point ,set start position only */
    NT35510_set_window(x, y, 1, 1);
    /* write to lcd */
    NT35510_write_reg(NT35510_FLUSH_RAM);
    NT35510_write_ram(color);
}

void LCD_FillBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR color)
{
    uint32_t wSize = width * height;

    if(x >= lcdDev.width || y >= lcdDev.height)
    {
        return ;
    }

    NT35510_set_window(x, y, width, height);
    NT35510_flush_color(color, wSize);
}

void LCD_FillPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR *pic)
{
    uint32_t wSize = width * height;

    if(x >= lcdDev.width || y >= lcdDev.height || wSize == 0)
    {
        return ;
    }

    /* write to lcd */    
    //printf("sp, x=%d, y=%d, width=%d, height=%d, size=%d\r\n", x, y, width, height, wSize);
    NT35510_set_window(x, y, width, height);
    NT35510_flush_data(pic, wSize);
}

/* LCD DMA Flush data */
#define DISP_DMA            DMA2_Stream0
#define DISP_DMAIRQn        DMA2_Stream0_IRQn 
#define DMA_MAXTX           0xFFFF

static DMA_HandleTypeDef DispDMA_Handle;
static XsferCplt XsferCpltCallback = NULL;
static uint32_t DispDMAXsferRegion = 0;
static bool DispDMAXsferCplt = TRUE;
static int DispDMARemainNbr = 0;
static int DispDMAXsferNbr = 0;

uint8_t LCD_DMAChannelInit(XsferCplt xsferCpltFunc)
{
    uint8_t ret = 0;
    
    /* DMA M2M Init */
    __HAL_RCC_DMA2_CLK_ENABLE(); 
    DispDMA_Handle.Instance = DISP_DMA;
	HAL_DMA_DeInit(&DispDMA_Handle);
	DispDMA_Handle.Init.Request = DMA_REQUEST_MEM2MEM;
	DispDMA_Handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
	DispDMA_Handle.Init.PeriphInc = DMA_PINC_ENABLE;
	DispDMA_Handle.Init.MemInc = DMA_MINC_DISABLE;
	DispDMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DispDMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DispDMA_Handle.Init.Mode = DMA_NORMAL;
	DispDMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
	DispDMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DispDMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
	DispDMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
	DispDMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
	ret = HAL_DMA_Init(&DispDMA_Handle);
	if(ret != HAL_OK) {return (1);}
    /* DMA IRQ */
    HAL_NVIC_SetPriority(DISP_DMAIRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DISP_DMAIRQn);
    /* callback function */
    XsferCpltCallback = xsferCpltFunc;
    
    return (0);
}

void LCD_DMAXsferStart(LCD_COLOR *pData, uint32_t size)
{
    uint8_t ret = 0;
    uint32_t xsferSize = 0;
    
    __HAL_DMA_CLEAR_FLAG(&DispDMA_Handle, DMA_FLAG_TCIF0_4|DMA_FLAG_TEIF0_4);
    __HAL_DMA_ENABLE_IT(&DispDMA_Handle, DMA_IT_TC|DMA_IT_TE);
    xsferSize = (size>DMA_MAXTX) ? DMA_MAXTX : size;
    ret = HAL_DMA_Start(&DispDMA_Handle, (uint32_t)pData, (uint32_t)&(LCD->LCD_RAM), xsferSize);
    if(ret == HAL_OK)
    {
    	//__HAL_DMA_ENABLE(&DispDMA_Handle);
        DispDMAXsferRegion = (uint32_t)pData;
        DispDMAXsferCplt = FALSE;
        DispDMARemainNbr = size;
        DispDMAXsferNbr = xsferSize;
    }
    else
    {
        __HAL_DMA_DISABLE_IT(&DispDMA_Handle, DMA_IT_TC|DMA_IT_TE);
        __HAL_DMA_DISABLE(&DispDMA_Handle);
        DispDMAXsferRegion = 0;
        DispDMAXsferCplt = TRUE;
        DispDMARemainNbr = 0;
        DispDMAXsferNbr = 0;
        /* mark as transfer completed */
        DispDMA_Handle.State = HAL_DMA_STATE_READY;
        if(XsferCpltCallback) {XsferCpltCallback();}
    }
}

void LCD_DMAXsferStop(void)
{
    __HAL_DMA_DISABLE_IT(&DispDMA_Handle, DMA_IT_TC|DMA_IT_TE);
    __HAL_DMA_DISABLE(&DispDMA_Handle);
    DispDMAXsferRegion = 0;
    DispDMAXsferCplt = TRUE;
    DispDMARemainNbr = 0;
    DispDMAXsferNbr = 0;
    /* mark as transfer completed */
    DispDMA_Handle.State = HAL_DMA_STATE_READY;
    if(XsferCpltCallback) {XsferCpltCallback();}
}

static void LCD_DMAXsferCplt(void)
{
    uint8_t ret = 0;
    uint32_t xsferSize = 0;

    DispDMARemainNbr -= DispDMAXsferNbr;
    DispDMAXsferRegion += DispDMAXsferNbr*2;
    DispDMA_Handle.State = HAL_DMA_STATE_READY;
    __HAL_UNLOCK(&DispDMA_Handle);
    
    if(DispDMARemainNbr > 0)//not completed
    {
        xsferSize = (DispDMARemainNbr>DMA_MAXTX) ? DMA_MAXTX : DispDMARemainNbr;
        ret = HAL_DMA_Start(&DispDMA_Handle, DispDMAXsferRegion, (uint32_t)&(LCD->LCD_RAM), xsferSize);
        if(ret == HAL_OK)
        {
            DispDMAXsferNbr = xsferSize;
        }
        else
        {
            __HAL_DMA_DISABLE_IT(&DispDMA_Handle, DMA_IT_TC|DMA_IT_TE);
            __HAL_DMA_DISABLE(&DispDMA_Handle);
            DispDMAXsferRegion = 0;
            DispDMAXsferCplt = TRUE;
            DispDMARemainNbr = 0;
            DispDMAXsferNbr = 0;
            /* mark as transfer completed */
            if(XsferCpltCallback) {XsferCpltCallback();}
        }
    }
    else //completed!!
    {
        __HAL_DMA_DISABLE_IT(&DispDMA_Handle, DMA_IT_TC|DMA_IT_TE);
        __HAL_DMA_DISABLE(&DispDMA_Handle);
        DispDMAXsferRegion = 0;
        DispDMAXsferCplt = TRUE;
        DispDMARemainNbr = 0;
        DispDMAXsferNbr = 0;
        /* mark as transfer completed */
        if(XsferCpltCallback) {XsferCpltCallback();}
    }
}

void LCD_DMAXsferError(void)
{
    __HAL_DMA_DISABLE(&DispDMA_Handle);
	DispDMAXsferRegion = 0;
    DispDMAXsferCplt = TRUE;
    DispDMARemainNbr = 0;
    DispDMAXsferNbr = 0;
    /* mark as transfer completed */
    DispDMA_Handle.State = HAL_DMA_STATE_READY;
    __HAL_UNLOCK(&DispDMA_Handle);
    
    if(XsferCpltCallback) {XsferCpltCallback();}
}

void DMA2_Stream0_IRQHandler(void)
{
    /* Tx Cplt */
    if(__HAL_DMA_GET_FLAG(&DispDMA_Handle, DMA_FLAG_TCIF0_4) != RESET)
	{
		__HAL_DMA_CLEAR_FLAG(&DispDMA_Handle, DMA_FLAG_TCIF0_4);
		
        LCD_DMAXsferCplt();
    }
    /* Tx Error */
    if(__HAL_DMA_GET_FLAG(&DispDMA_Handle, DMA_FLAG_TEIF0_4) != RESET)
	{
		__HAL_DMA_CLEAR_FLAG(&DispDMA_Handle, DMA_FLAG_TEIF0_4);

		LCD_DMAXsferError();
    }
}

/**************** PRIVATE FUNCTION *****************/

static void NT35510_write_reg(uint16_t reg)
{
    reg = reg;
    LCD->LCD_REG = reg;
}

static void NT35510_write_ram(uint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}

static void NT35510_set_register(uint16_t reg, uint16_t data)
{
    reg = reg;
    LCD->LCD_REG = reg;
    data = data;
    LCD->LCD_RAM = data;
}

#if 0
static void NT35510_set_cursor(uint16_t x, uint16_t y)
{
    NT35510_set_register(NT35510_SET_COLUMN,     x >> 8);
    NT35510_set_register(NT35510_SET_COLUMN + 1, x & 0xff);

    NT35510_set_register(NT35510_SET_ROW,     y >> 8);
    NT35510_set_register(NT35510_SET_ROW + 1, y & 0xff);
}
#endif

static void NT35510_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t tmp = 0;

    /* start column */
    NT35510_set_register(NT35510_SET_COLUMN,     x >> 8);
    NT35510_set_register(NT35510_SET_COLUMN + 1, x & 0xff);
    /* end column */
    tmp = x + width - 1;
    NT35510_set_register(NT35510_SET_COLUMN + 2, tmp >> 8);
    NT35510_set_register(NT35510_SET_COLUMN + 3, tmp & 0xff);

    /* start row */
    NT35510_set_register(NT35510_SET_ROW,     y >> 8);
    NT35510_set_register(NT35510_SET_ROW + 1, y & 0xff);   
    /* end row */
    tmp = y + height - 1;
    NT35510_set_register(NT35510_SET_ROW + 2, tmp >> 8);
    NT35510_set_register(NT35510_SET_ROW + 3, tmp & 0xff);
}

static void NT35510_flush_color(LCD_COLOR color, uint32_t size)
{
    NT35510_write_reg(NT35510_FLUSH_RAM);

    while(size--)
    {
        //color = color;
        LCD->LCD_RAM = color;
    }
}

static void NT35510_flush_data(const LCD_COLOR *pData, uint32_t size)
{
    NT35510_write_reg(NT35510_FLUSH_RAM);

    while(size--)
    {
        LCD->LCD_RAM = *pData++;
    }
}


