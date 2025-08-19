#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"

/**
  * @brief  初始化延时函数（必须先调用）
  * @note   配置SysTick时钟源并初始化系统时钟
  * @param  无
  * @retval 无
  */
void Delay_Init(void);

/**
  * @brief  微秒级延时
  * @param  us：延时时长，范围：0~7989（超出会自动截断）
  * @note   基于SysTick定时器，时钟源为HCLK/8（21MHz@168MHz系统时钟）
  * @retval 无
  */
void Delay_us(uint32_t us);

/**
  * @brief  毫秒级延时
  * @param  ms：延时时长，范围：0~4294967295（无上限）
  * @note   基于微秒延时实现，自动分段处理长延时
  * @retval 无
  */
void Delay_ms(uint32_t ms);

/**
  * @brief  秒级延时
  * @param  s：延时时长，范围：0~4294967295（无上限）
  * @note   基于毫秒延时实现
  * @retval 无
  */
void Delay_s(uint32_t s);

#endif /* __DELAY_H */
