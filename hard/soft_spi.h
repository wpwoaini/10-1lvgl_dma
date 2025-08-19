#ifndef SOFT_SPI_H
#define SOFT_SPI_H

#include "stm32f4xx.h"

#define SPI_SCK_PORT    GPIOB
#define SPI_SCK_PIN     GPIO_Pin_5
#define SPI_MOSI_PORT   GPIOB
#define SPI_MOSI_PIN    GPIO_Pin_6
#define SPI_MISO_PORT   GPIOB
#define SPI_MISO_PIN    GPIO_Pin_7
#define SPI_CS_PORT     GPIOB
#define SPI_CS_PIN      GPIO_Pin_4  // For ST7789 CS

#define SPI_SCK_H()     GPIO_SetBits(SPI_SCK_PORT, SPI_SCK_PIN)
#define SPI_SCK_L()     GPIO_ResetBits(SPI_SCK_PORT, SPI_SCK_PIN)
#define SPI_MOSI_H()    GPIO_SetBits(SPI_MOSI_PORT, SPI_MOSI_PIN)
#define SPI_MOSI_L()    GPIO_ResetBits(SPI_MOSI_PORT, SPI_MOSI_PIN)
#define SPI_CS_H()      GPIO_SetBits(SPI_CS_PORT, SPI_CS_PIN)
#define SPI_CS_L()      GPIO_ResetBits(SPI_CS_PORT, SPI_CS_PIN)
#define SPI_MISO_READ() GPIO_ReadInputDataBit(SPI_MISO_PORT, SPI_MISO_PIN)

void Soft_SPI_Init(void);
uint8_t Soft_SPI_Transfer(uint8_t byte);

#endif