#ifndef SPI_HW_H
#define SPI_HW_H

#include "stm32f4xx.h"

// 硬件 SPI1 引脚定义
#define HW_SPI_PORT       SPI1
#define HW_SPI_SCK_PORT   GPIOA   // SPI1_SCK -> PA5
#define HW_SPI_SCK_PIN    GPIO_Pin_5
#define HW_SPI_MOSI_PORT  GPIOA   // SPI1_MOSI -> PA7
#define HW_SPI_MOSI_PIN   GPIO_Pin_7
#define HW_SPI_CS_PORT    GPIOB   // 片选引脚 -> PB4
#define HW_SPI_CS_PIN     GPIO_Pin_4

// 片选控制宏
#define HW_SPI_CS_H()     GPIO_SetBits(HW_SPI_CS_PORT, HW_SPI_CS_PIN) // CS 拉高（无效）
#define HW_SPI_CS_L()     GPIO_ResetBits(HW_SPI_CS_PORT, HW_SPI_CS_PIN) // CS 拉低（有效）

// 函数声明
void HW_SPI_Init(void); // 初始化 SPI 和 DMA
uint8_t HW_SPI_Transfer(uint8_t byte); // 单字节 SPI 传输
void HW_SPI_Transmit_DMA(uint8_t *data, uint32_t len); // DMA 批量传输

#endif
