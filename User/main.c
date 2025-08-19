#include "stm32f4xx.h"
#include "usart.h"
#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"
#include "Delay.h"
#include "stdio.h"
#include "CST816.h"

// 全局系统时间计数器（由 TIM3 中断递增）
static volatile uint32_t sys_time = 0;

/**
 * @brief 主函数
 */
int main(void)
{
    // 初始化硬件接口
    Delay_Init();      // 延时函数
    USART1_Init();     // 串口调试
    Timer_Init();      // 初始化 TIM3，提供 1ms 时间戳
    mylvgl_init();     // 初始化 LVGL
    lv_demo_widgets(); // 运行 LVGL 示例

    uint16_t x, y;
    uint8_t gesture, points;
    uint32_t last_task_time  = 0;
    uint32_t last_touch_time = 0;

    // 主循环
    while (1) {
        // 每 5ms 调用 lv_timer_handler
        if (sys_time - last_task_time >= 5) {
            lv_timer_handler(); // 处理 LVGL 任务
            last_task_time = sys_time;
        }

        // // 每 10ms 采样触摸数据
        // if (sys_time - last_touch_time >= 10) {
        //     CST816_Get_Touch(&x, &y, &gesture, &points);
        //     if (points > 1) {
        //         printf("X:%d Y:%d\n", x, y);
        //     }
        //     last_touch_time = sys_time;
        // }
    }
}

/**
 * @brief TIM3 中断服务函数，每 1ms 调用
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        sys_time++;     // 递增系统时间
        lv_tick_inc(1); // 每 1ms 增加 LVGL 时间戳
    }
}
