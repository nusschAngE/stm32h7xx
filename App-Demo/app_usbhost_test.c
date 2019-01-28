
#include "stm32h7xx.h"
#include "app_demo.h"
#include "led.h"
#include "delay.h"
#include "io_expand.h"
#include "usb_host.h"
#include "mytext.h"


#if (MODULE_USBHOST_TEST)

void USB_DPDM_Test(void)
{
	GPIO_InitTypeDef GPIO_Init;
	uint32_t porta = 0;

	__HAL_RCC_GPIOA_CLK_ENABLE();
    /**USB_OTG_FS GPIO Configuration    
        PA11     ------> USB_OTG_FS_DM
        PA12     ------> USB_OTG_FS_DP 
    */
    GPIO_Init.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //GPIO_Init.Alternate = GPIO_AF10_OTG1_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

	IOEx_SetPin(IOEX_USB_PWR, 0);
    porta = GPIOA->IDR;
    while(1)
	{
		if(porta != GPIOA->IDR)
		{
			porta = GPIOA->IDR;
			printf("porta = 0x%08x\r\n", porta);
		}

		LED_Toggle(LED_RED);
      	TimDelayMs(200);
    }
}

uint8_t USBH_User_App(void)
{
	return 0;
}

void USB_Host_Test(void)
{
	uint32_t dmdp = 0, Count = 0;

	ShowTextLineAscii(10, 10, "##USB Host Test##", FontPreset(2));
	//USB_DPDM_Test();
    MX_USB_HOST_Init();
    while(1)
    {
        MX_USB_HOST_Process();
        
        TimDelayMs(1);
        if(++Count == 200)
        {
        	Count = 0;
        	//IOEx_IntCheck();
        	LED_Toggle(LED_RED);
        }
    }

USB_Host_Test_End:

    printf("USB Host Test end.\r\n");
}




#endif

