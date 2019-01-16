
#include "stm32h7xx.h"
#include "ucos_ii.h"
#include "fatfs_ex.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "usbh_conf.h"
#include "usb_host.h"
#include "usb_host_track.h"


/********** PRIVATE FUNCTION ************/

static void USB_HostProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    /* USER CODE BEGIN CALL_BACK_1 */
    switch(id)
    {
    case HOST_USER_SELECT_CONFIGURATION:
    printf("USB Configuration!!\r\n");
    break;

    case HOST_USER_DISCONNECTION:
    {
        FRESULT fret = 0;
        
        Appli_state = APPLICATION_DISCONNECT;
        printf("USB Disconnected process!!\r\n");

        f_unmount(UsbFsDrv);
        IsUSBMount = FALSE;
    }
    break;

    case HOST_USER_CLASS_ACTIVE:
    {
        FRESULT fret = 0;
    
        printf("USB Active process!!\r\n");

        fret = f_mount(&USBFatFS, UsbFsDrv, 1);
        if(fret == FR_OK)
        {
            printf("f_mount() success!!\r\n");
            IsUSBMount = TRUE;
        }
        else 
        {
            printf("f_mount() error, fret = %d!!\r\n", fret);
            IsUSBMount = FALSE;
        }
        
        Appli_state = APPLICATION_READY;
    }
    break;

    case HOST_USER_CONNECTION:
    printf("USB Connected process!!\r\n");
    Appli_state = APPLICATION_START;
    break;

    default:
    break;
    }
    /* USER CODE END CALL_BACK_1 */    
}

/****** PUBLIC FUNCTION *********/

void USB_HostTrackInit(void)
{  
    /* Init host Library, add supported class and start the library. */
    USBH_Init(&hUsbHostFS, USB_HostProcess, HOST_FS);

    //USBH_RegisterClass(&hUsbHostFS, USBH_AUDIO_CLASS);
    //USBH_RegisterClass(&hUsbHostFS, USBH_CDC_CLASS);
    USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);
    //USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS);
    //USBH_RegisterClass(&hUsbHostFS, USBH_MTP_CLASS);

    USBH_Start(&hUsbHostFS);
	HAL_PWREx_EnableUSBVoltageDetector();
}

/**** USB Track task body *****/


void USB_HostTrackTask(void *p_arg)
{
    (void)p_arg;

    while(1)
    {
        USBH_Process(&hUsbHostFS);
        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}






