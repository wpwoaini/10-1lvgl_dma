#ifndef CST816_H 
#define CST816_H

#include "stm32f4xx.h" 
#include "soft_i2c.h"

#define CST816_ADDR 0x15 // I2C 地址（7 位）

// 函数声明 
void CST816_Init(void); 
uint8_t CST816_Read_Reg(uint8_t reg); 
void CST816_Write_Reg(uint8_t reg, uint8_t value); 
void CST816_Get_Touch(uint16_t *x, uint16_t *y, uint8_t *gesture, uint8_t *points);

#endif