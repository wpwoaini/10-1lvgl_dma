#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"

void USART1_Init(void);
void USART1_SendByte(uint8_t data);
void USART1_SendString(char *str);
#endif