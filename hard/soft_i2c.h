#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include "stm32f4xx.h"

#define I2C_SCL_PORT    GPIOB
#define I2C_SCL_PIN     GPIO_Pin_15
#define I2C_SDA_PORT    GPIOB
#define I2C_SDA_PIN     GPIO_Pin_14

#define I2C_SCL_H()     GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_SCL_L()     GPIO_ResetBits(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_SDA_H()     GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_SDA_L()     GPIO_ResetBits(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_SDA_READ()  GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN)

void Soft_I2C_Init(void);
void Soft_I2C_Start(void);
void Soft_I2C_Stop(void);
uint8_t Soft_I2C_Write_Byte(uint8_t byte);
uint8_t Soft_I2C_Read_Byte(uint8_t ack);

#endif
