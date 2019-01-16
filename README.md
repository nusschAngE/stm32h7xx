# 正点原子STM32H7阿波罗开发板
  4个GPIO按键，已调试。
  2个LED灯，已调试。
  LCD[800*480/NT35510/FMC]，已调试。
  TP[GT9147/IIC]，已调试。
  SDRAM[W9825G6KH]，已调试。
  FLASH[W25Q256/QSPI]，已调试。
  IO扩展[PCF8547/IIC]，已调试。
  SD卡[SDMMC]，已调试。
  音频编解码器[W8978G/IIC]，已调试解码功能。
  USB-HOST，已调试USB-MSC。

# 挂在系统
  UCOS-II：
  	基本点灯、按键Q。
  FATFS：
  	中文支持，字库在QSPIFlash。
  	SD卡、U盘。

# 基本例程[app_demo.c/app_demo.h]
  malloc，包括SDRAM。
  hzk字体。
  LCD，SDRAM，DMA传输。
  WAV播放器，SD/U盘。
  ESP8266简单操作。