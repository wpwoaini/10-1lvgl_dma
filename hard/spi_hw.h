#ifndef SPI_HW_H
#define SPI_HW_H

#include "stm32f4xx.h"

// 硬件SPI1引脚定义（SPI1通常挂载在APB2，引脚常用PA5/PA7）
#define HW_SPI_PORT       SPI1
#define HW_SPI_SCK_PORT   GPIOA   // SPI1_SCK -> PA5
#define HW_SPI_SCK_PIN    GPIO_Pin_5
#define HW_SPI_MOSI_PORT  GPIOA   // SPI1_MOSI -> PA7
#define HW_SPI_MOSI_PIN   GPIO_Pin_7
#define HW_SPI_CS_PORT    GPIOB   // 保持原片选引脚（可根据需要修改）
#define HW_SPI_CS_PIN     GPIO_Pin_4

// 片选控制宏（保持原逻辑）
#define HW_SPI_CS_H()     GPIO_SetBits(HW_SPI_CS_PORT, HW_SPI_CS_PIN)
#define HW_SPI_CS_L()     GPIO_ResetBits(HW_SPI_CS_PORT, HW_SPI_CS_PIN)

// 函数声明
void HW_SPI_Init(void);
uint8_t HW_SPI_Transfer(uint8_t byte);
void HW_SPI_Transmit(uint8_t *data, uint32_t len);

#endif
