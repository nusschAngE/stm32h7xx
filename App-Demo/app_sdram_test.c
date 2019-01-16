
#include "sdram.h"
#include "app_demo.h"
#include "delay.h"



#if (MODULE_SDRAM_TEST)
static uint8_t buffer1[1024] __attribute__((section(".ExtSDRAM")));
static uint16_t buffer2[1024] __attribute__((section(".ExtSDRAM")));
static uint32_t buffer3[1024] __attribute__((section(".ExtSDRAM")));

//static uint8_t buffer1[1024] __attribute__((section(".ARM.__at_0xc0000000")));
//static uint16_t buffer2[1024] __attribute__((section(".ARM.__at_0xc0001000")));
//static uint32_t buffer3[1024] __attribute__((section(".ARM.__at_0xc0010000")));


void SDRAM_Test(void)
{
	uint32_t i = 0;

	printf("buffer1 = %p, buffer2 = %p, buffer3 = %p\r\n", buffer1, buffer2, buffer3);
#if 0
	for(i = 0; i < 1024; i++)
	{
		buffer1[i] = i & 0xff;
	}

	printf("buffer1, init!!\r\n");
	for(i = 0; i < 1024; i++)
	{
		printf("%d, ", buffer1[i]);
		if((i) && (i%24 == 0))
			printf("\r\n");
	}
	printf("\r\n");

	for(i = 0; i < 1024; i++)
	{
		buffer2[i] = i & 0xff;
	}
#endif	
#if 0
	for(i = 0; i < 1024; i++)
	{
		buffer2[i] = i;
	}
	printf("buffer2, init!!\r\n");
	for(i = 0; i < 1024; i++)
	{
		printf("%d, ", buffer2[i]);
		if((i) && (i%16 == 0))
			printf("\r\n");
	}
	printf("\r\n");
#endif	
#if 1
	for(i = 0; i < 1024; i++)
	{
		buffer3[i] = i;
	}
	printf("buffer3, init!!\r\n");
	for(i = 0; i < 1024; i++)
	{
		printf("%d, ", buffer3[i]);
		if((i) && (i%16 == 0))
			printf("\r\n");
	}
	printf("\r\n");
#endif

}

#endif //MODULE_SDRAM_TEST

