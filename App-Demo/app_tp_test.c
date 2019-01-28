
#include "app_demo.h"
#include "touch.h"

#include "delay.h"
#include "led.h"
#include "mytext.h"


#if (MODULE_TP_TSET)

void Touch_Test(void)
{
	uint8_t tpInt = FALSE;
	uint8_t tCount = 0;
	uint8_t tpPoint = 0;
	uint16_t xPos = 0, yPos = 0, color = 0;

    ShowTextLineAscii(10, 10, "##Touch Test##", FontPreset(2));

	if(TP_Init() != 0)
	{
		printf("touch init error!!\r\n");
		goto Touch_Test_End;
	}

	while(1)
	{
		tpPoint = TP_PointChecked();
		TP_ReadPoint(0, &xPos, &yPos);
		//TP_ReadPoint(1, &xPos, &yPos);
		//TP_ReadPoint(2, &xPos, &yPos);
		//TP_ReadPoint(3, &xPos, &yPos);
		//TP_ReadPoint(4, &xPos, &yPos);
        
		if(tpPoint) 
		{
            TFTLCD_FastDrawPoint(xPos, yPos, xPos+yPos);
            color = TFTLCD_ReadPoint(xPos, yPos);

            printf("Point[%d-%d], dColor=0x%04x, rColor=0x%04x\r\n", xPos, yPos, xPos+yPos, color);
		}
		color = tpPoint = xPos = yPos = 0;

		/* delay */
		TimDelayMs(50);
		if(++tCount == 10) 
		{
			tCount = 0;
			LED_Toggle(LED_GREEN);
		}
	}



Touch_Test_End:

	printf("touch test end.\r\n");
}







#endif


