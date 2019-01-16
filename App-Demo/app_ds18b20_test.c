
#include "stm32h7xx.h"
#include "app_demo.h"

#include "delay.h"
#include "ds18b20.h"


#if (MODULE_DS18B20_TEST)
void DS18B20_Test(void)
{
    uint16_t value = 0;
    float temp;
    uint8_t ret;

    printf("--- DS18B20 test start ----------------\r\n");
    /* init DS18B20 */
    ret = DS18B20_Init(0x2f, 0x2f, DS_TEMP_9BIT);
    if(ret == 0)
    {
        printf("DS18B20_Init() error\r\n");
        goto DS18B20_Test_End;
    }
    //TimDelayMs(5);
    /* start convert process */
    ret = DS18B20_StartConvert();
    if(ret == 0)
    {
        printf("DS18B20_StartConvert() error\r\n");
        goto DS18B20_Test_End;
    }
#if 0
	TimDelayMs(100);
#else
    /* wait convert */
    value = 0x0fff;
    while((value--) && (DS18B20_CheckConvert() == 0))
    {
        TimDelayMs(10);
    }
    if(DS18B20_CheckConvert() == 0)//convert error
    {
        printf("DS18B20_CheckConvert() return 1\r\n");
        goto DS18B20_Test_End;
    }
#endif    
    /* read temp */
    ret = DS18B20_ReadTemp(&value);
    if(ret == 0)
    {
        printf("DS18B20_ReadTemp() error\r\n");
        goto DS18B20_Test_End;
    }
    /* caculate temp */
    temp = DS18B20_GetTempFloat(value);
    printf("DS18B20 read = %f\r\n", temp);

DS18B20_Test_End:    
    printf("--- DS18B20 test end ----------------\r\n");    
}

#endif

