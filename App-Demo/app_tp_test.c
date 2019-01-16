
#include "app_demo.h"
#include "touch.h"

#include "delay.h"
#include "led.h"
#include "lcd.h"


#if (MODULE_TP_TSET)

void app_tp_test(void)
{
	uint8_t tpInt = FALSE;
	uint8_t tCount = 0;
	uint8_t tpPoint = 0;
	uint16_t xPos = 0, yPos = 0;

	printf("## touch test ##\r\n");

	if(TP_Init() != 0)
	{
		printf("touch init error!!\r\n");
		goto Touch_Test_End;
	}

	while(1)
	{
		tpPoint = TP_PointChecked();
		TP_ReadPoint(0, &xPos, &yPos);
		printf("tpPoint = %d,[%d-%d]\r\n", tpPoint, xPos, yPos);
		TP_ReadPoint(1, &xPos, &yPos);
		TP_ReadPoint(2, &xPos, &yPos);
		TP_ReadPoint(3, &xPos, &yPos);
		TP_ReadPoint(4, &xPos, &yPos);
		
		tpPoint = xPos = yPos = 0;

		/* delay */
		TimDelayMs(50);
		if(++tCount == 10)
		{
			tCount = 0;
			
			LED_Toggle(LED_GREEN);
		}
	}



Touch_Test_End:

	printf("## touch test end ##\r\n");
}







#endif


