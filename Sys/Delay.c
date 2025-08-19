#include "delay.h"

// 假设系统时钟为168MHz，TIM2时钟为84MHz（APB1时钟）
#define TIM_US_PRESCALER (84 - 1) // 84MHz/84 = 1MHz，1us计数一次
#define TIM_US_PERIOD    0xFFFF   // 自动重装载值最大值

void Delay_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 定时器基础配置
    TIM_TimeBaseStructure.TIM_Period        = TIM_US_PERIOD;      // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler     = TIM_US_PRESCALER;   // 预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // 时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 禁止预装载寄存器
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

void Delay_us(uint32_t us)
{
    uint32_t start = TIM2->CNT;
    uint32_t elapsed;
    uint32_t ticks = us; // 因为定时器配置为1us计数一次

    do {
        elapsed = TIM2->CNT - start;
    } while (elapsed < ticks);
}

void Delay_ms(uint32_t ms)
{
    while (ms--) {
        Delay_us(1000);
    }
}

void Delay_s(uint32_t s)
{
    while (s--) {
        Delay_ms(1000);
    }
}
