
#include "stm32h7xx.h"

#include "usb_otg.h"
#include "delay.h"

/*  
*/
static HCD_HandleTypeDef HCD_Handle;

static uint8_t usbType = 0;

/******************* PUBLIC FUNCTION *********************/
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_Init.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF10_OTG1_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);
}

uint8_t USB_OTG_Init(void)
{
    //return USB_ConfigAsHost();
}

/*
*   usb host
*/

uint8_t USB_ConfigAsHost(void)
{
    uint32_t ret = 0;

    HCD_Handle.Instance = USB2_OTG_FS;
    //HCD_Handle.Init.dev_endpoints = 1;
    //HCD_Handle.Init.Host_channels = 1;
    HCD_Handle.Init.speed = USB_OTG_SPEED_FULL;
    HCD_Handle.Init.dma_enable = ENABLE;
    HCD_Handle.Init.ep0_mps = DEP0CTL_MPS_64;
    HCD_Handle.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
    HCD_Handle.Init.Sof_enable = DISABLE;
    HCD_Handle.Init.low_power_enable = DISABLE;
    HCD_Handle.Init.battery_charging_enable = DISABLE;
    HCD_Handle.Init.lpm_enable = DISABLE;
    HCD_Handle.Init.vbus_sensing_enable = DISABLE;
    HCD_Handle.Init.use_dedicated_ep1 = ENABLE;
    HCD_Handle.Init.use_external_vbus = ENABLE;
    ret = HAL_HCD_Init(&HCD_Handle);
    if(ret == HAL_OK)
    {
        usbType = USB_TYPE_HOST;
        HAL_HCD_Start(&HCD_Handle);
        return (1);
    }

    return (0);
}

void USB_HostStop(void)
{
    if(usbType == USB_TYPE_HOST)
    {
        HAL_HCD_Stop(&HCD_Handle);
    }
}

void USB_HostManage(uint8_t channel, bool connect)
{
    
}

/*
*   usb device
*/

uint8_t USB_ConfigAsDevice(void)
{
#if 0
    uint32_t ret = 0;

    HCD_Handle.Instance = USB2_OTG_FS;
    //HCD_Handle.Init.dev_endpoints = 1;
    //HCD_Handle.Init.Host_channels = 1;
    HCD_Handle.Init.speed = USB_OTG_SPEED_FULL;
    HCD_Handle.Init.dma_enable = ENABLE;
    HCD_Handle.Init.ep0_mps = DEP0CTL_MPS_64;
    HCD_Handle.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
    HCD_Handle.Init.Sof_enable = DISABLE;
    HCD_Handle.Init.low_power_enable = DISABLE;
    HCD_Handle.Init.battery_charging_enable = DISABLE;
    HCD_Handle.Init.lpm_enable = DISABLE;
    HCD_Handle.Init.vbus_sensing_enable = DISABLE;
    HCD_Handle.Init.use_dedicated_ep1 = ENABLE;
    HCD_Handle.Init.use_external_vbus = ENABLE;
    ret = HAL_PCD_Init(&HCD_Handle);
    if(ret == HAL_OK)
    {
        usbType = USB_TYPE_DEVICE;
        return (1);
    }
#endif
    return (0);    
}


