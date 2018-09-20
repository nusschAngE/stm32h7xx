# stm32h7xx
apollo stm32h7xx development board

# resources on board
**Resource** | **IC Model** | **Ctrl IO** | **Features**
------------ | ------------ | ----------- | ------------
 MCU | stm32h743 | none | 400mhz, 1Mbyte SRAM, 2MByte Flash
 SDRAM | W9825G6KH | FMC SDRAM | 32MByte, 133mhz
 SPI Flash | W25Q256 | QSPI | 32MByte, 104mhz
 LCD | NT35510 | MPU(FMC SRAM) | 565 color, 800x480
 SD Card | none | SDIO | none
 KEY | none | GPIO | KEY1->wakeup/normal, KEY234->normal
 LED | none | GPIO | red, green
 
