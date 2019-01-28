
#include "sdram.h"
#include "app_demo.h"
#include "delay.h"
#include "mytext.h"


#if (MODULE_SDRAM_TEST)

#define SDRAM_TEST_SIZE		(0x200000)

void SDRAM_Test(void)
{
    uint32_t i = 0, cnt = 5;
	uint8_t *u8Ptr = NULL;
	uint16_t *u16Ptr = NULL;
	uint32_t *u32Ptr = NULL;
	uint64_t *u64Ptr = NULL;

    ShowTextLineAscii(10, 10, "##SDRAM Test##", FontPreset(2));
#if 0
    u8Ptr = (uint8_t *)0xC0000000;
    for(i=0; i<512; i++) {
        u8Ptr[i] = i/2;
    }
    printf("u8Ptr : \r\n");
    for(i=0; i<512; i++) {
        printf("%d,", u8Ptr[i]);
    }
    printf("\r\n");

    u16Ptr = (uint16_t *)0xC0800000;
    for(i=0; i<512; i++) {
        u16Ptr[i] = i;
    }
    printf("u16Ptr : \r\n");
    for(i=0; i<512; i++) {
        printf("%d,", u16Ptr[i]);
    }
    printf("\r\n");
#endif
    u64Ptr = (uint64_t *)0xC0000000;
    while(cnt--)
    {
	    for(i=0; i<SDRAM_TEST_SIZE/8; i++) {
	        u32Ptr[i] = i;
	    }
	    printf("u32Ptr : \r\n");
	    for(i=0; i<SDRAM_TEST_SIZE/8; i++) {
	    	if(u32Ptr[i] != i) {
	        	printf("%d,", u32Ptr[i]);
			}
	    }
    }
    //printf("\r\n");

    printf("SDRAM test end.\r\n");
}

#endif //MODULE_SDRAM_TEST

