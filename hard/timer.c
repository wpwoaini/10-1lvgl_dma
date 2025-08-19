#include "timer.h"
#include "lvgl.h"

void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能 TIM3 时钟（替换TIM2）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置 TIM3：1ms 中断
    TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;                        // 1ms中断间隔
    TIM_TimeBaseStructure.TIM_Prescaler     = (SystemCoreClock / 1000000) - 1; // 1us计数精度
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 初始化TIM3

    // 使能 TIM3 中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    // 配置 NVIC 中断（使用TIM3_IRQn）
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// TIM3中断服务函数
// void TIM3_IRQHandler(void)
// {
//     if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
//         TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//         lv_tick_inc(1); // 每1ms增加LVGL时间戳
//         // 注意：通常不建议在中断中直接调用lv_timer_handler()
//         // 可以只更新时间戳，在主循环中处理任务
//         // lv_timer_handler();
//     }
// }
