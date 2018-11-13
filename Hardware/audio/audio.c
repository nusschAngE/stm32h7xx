
#include "stm32h7xx.h"

#include "delay.h"
#include "sys_i2c.h"
#include "audio_i2s.h"
#include "audio.h"



/* address */
#define WM8978_ADDR             (0x34)//without R/W bit

/* IIC secquence
*  START --- ADDR --- (REG<<1)|(VALUE[1]:bit8) --- VALUE[0]:bit7~0 --- STOP
*/

/* register */
#define REG_RESET               (0x00)
#define REG_PWR_MANAGE1         (0x01)
#define REG_PWR_MANAGE2         (0x02)
#define REG_PWR_MANAGE3         (0x03)
#define REG_AUDIN_CTRL          (0x04)
#define REG_COMP_CTRL           (0x05)
#define REG_CLOCK_CTRL          (0x06)
#define REG_ADDI_CTRL           (0x07)
#define REG_GPIO_CTRL           (0x08)
#define REG_JACKDET_CTRL        (0x09)
#define REG_DAC_CTRL            (0x0A)
#define REG_LDACVOL             (0x0B)
#define REG_RDACVOL             (0x0C)
#define REG_JACKEN              (0x0D)
#define REG_ADC_CTRL            (0x0E)
#define REG_LADCVOL             (0x0F)
#define REG_RADCVOL             (0x10)
#define REG_EQSHELF_LOW         (0x12)
#define REG_EQ_PEAK1            (0x13)
#define REG_EQ_PEAK2            (0x14)
#define REG_EQ_PEAK3            (0x15)
#define REG_EQSHELF_HIGH        (0x16)
#define REG_DAC_LIMIT1          (0x18)
#define REG_DAC_LIMIT2          (0x19)
#define REG_NOTCH1              (0x1B)
#define REG_NOTCH2              (0x1C)
#define REG_NOTCH3              (0x1D)
#define REG_NOTCH4              (0x1E)
#define REG_ALC_CTRL1           (0x20)
#define REG_ALC_CTRL2           (0x21)
#define REG_ALC_CTRL3           (0x22)
#define REG_NOISE_GATE          (0x23)
#define REG_PLLN                (0x24)
#define REG_PLLK1               (0x25)
#define REG_PLLK2               (0x26)
#define REG_PLLK3               (0x27)
#define REG_3D_CTRL             (0x29)
#define REG_BEEP_CTRL           (0x2B)
#define REG_INPUT_CTRL          (0x2C)
#define REG_LPGA_GAIN           (0x2D)
#define REG_RPGA_GAIN           (0x2E)
#define REG_LADC_BOOST          (0x2F)
#define REG_RADC_BOOST          (0x30)
#define REG_OUTPUT_CTRL         (0x31)
#define REG_LMIXER              (0x32)
#define REG_RMIXER              (0x33)
#define REG_LOUT1_VOL           (0x34) 
#define REG_ROUT1_VOL           (0x35)
#define REG_LOUT2_VOL           (0x36)
#define REG_ROUT2_VOL           (0x37)
#define REG_OUT3_MIXER          (0x38)
#define REG_OUT4_MIXER          (0x39)

/*  STATIC
*/
static uint8_t WM8978_SetRegister(uint8_t reg, uint8_t *val);
static uint8_t WM8978_SendBytes(uint8_t *pBuff, uint8_t size);

static uint8_t wm8978_CurrVolume = 0;
static uint8_t wm8978_CurrMute = 0;
static uint8_t wm8978_CurrEQ = 0;
static uint8_t wm8978_CurrChannel = 0;

/******************* PUBLIC FUNCTION *******************/
uint8_t Audio_Init(void)
{
    uint8_t InitParam[2];//[1]bit8, [0]bit7~0

    SYSI2C_Init();
    /* WM8978 Init */
    InitParam[1] = 0x00; InitParam[0] = 0x04;
    WM8978_SetRegister(0x01, InitParam);
    
    InitParam[1] = 0x01; InitParam[0] = 0x80;
    WM8978_SetRegister(0x02, InitParam);
    
    InitParam[1] = 0x00; InitParam[0] = 0x63;
    WM8978_SetRegister(0x03, InitParam);
    
    InitParam[1] = 0x00; InitParam[0] = 0x70;
    WM8978_SetRegister(0x04, InitParam);
    
    InitParam[1] = 0x00; InitParam[0] = 0x01;
    WM8978_SetRegister(0x08, InitParam);

    InitParam[1] = 0x00; InitParam[0] = 0x08;
    WM8978_SetRegister(0x0a, InitParam);

    /* param init */
    wm8978_CurrVolume = 0;
    wm8978_CurrMute = 0;
    wm8978_CurrEQ = 0;
    wm8978_CurrChannel = 0;

    return (1);
}

void Audio_SetMute(uint8_t chann, uint8_t mute)
{
    
}

void Audio_SetChannel(uint8_t chann)
{
    
}   

/* @para vol:0~255(0.5DB/STEP)
*/
void Audio_SetVolume(uint8_t vol)
{
    uint8_t temp[2];

    temp[1] = 0x00;
    temp[0] = vol;
    WM8978_SetRegister(REG_LDACVOL, temp);
    WM8978_SetRegister(REG_RDACVOL, temp);
}

void Audio_SetEQ(uint8_t eq)
{
    
}

void Audio_ConfigItface(uint32_t SampleRate)
{
    
}

void Audio_SetStart(void)
{
    
}

void Audio_FieldBuffer(uint8_t pBuff, uint16_t size)
{
    
}

void Audio_SetSleepON(void)
{
    
}

void Audio_SetSleepOFF(void)
/******************* PRIVATE FUNCTION *******************/
static uint8_t WM8978_SetRegister(uint8_t reg, uint8_t *val)
{
    uint8_t tmp, ret;

    /* addr */
    tmp = WM8978_ADDR;
    SYSI2C_SendByte(tmp);
    ret = SYSI2C_WaitACK();
    if(ret == 1)//nack
        return (0);
    /* control byte 1 */
    tmp = (reg << 1) | val[1];
    SYSI2C_SendByte(tmp);
    ret = SYSI2C_WaitACK();
    if(ret == 1)//nack
        return (0);
    /* control byte 0 */
    tmp = val[0];
    SYSI2C_SendByte(tmp);
    ret = SYSI2C_WaitACK();
    if(ret == 1)//nack
        return (0);
    /* Error None */
    return(1);
}

static uint8_t WM8978_SendBytes(uint8_t *pBuff, uint8_t size)
{
    uint8_t ret;

    /* addr */
    SYSI2C_SendByte(WM8978_ADDR);
    ret = SYSI2C_WaitACK();
    if(ret == 1)//nack
        return (0);

    /* data */
    while(size--)
    {
        SYSI2C_SendByte(*pBuff);
        ret = SYSI2C_WaitACK();
        if(ret == 1)//nack
        {
            return (0);
        }

        pBuff++;
    }
}

