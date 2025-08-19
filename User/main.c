#include "stm32f4xx.h"
#include "usart.h"
#include "timer.h"
// #include "lvgl_port.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"
#include "Delay.h"
#include "stdio.h"
#include "CST816.h"

int main(void)
{
    // 初始化硬件接口
    Delay_Init();  // 延时函数
    USART1_Init(); // 串口调试
    // 不需要定时器为LVGL提供心跳，注释或删除定时器初始化
    Timer_Init();

    // 初始化 LV
    mylvgl_init();
    // 运行 LVGL 示例
    lv_demo_widgets();

    // uint16_t x, y;
    // uint8_t gesture, points;

    // 主循环
    while (1) {
        // 获取触摸数据
        // CST816_Get_Touch(&x, &y, &gesture, &points);
        // if (points > 1) {
        //     printf("X:%d Y:%d\n", x, y);
        // }

        // 在主循环中处理LVGL任务并提供时间戳（非中断方式）
        // lv_tick_inc(5);    // 每5ms增加LVGL时间戳
        // lv_task_handler(); // 处理LVGL任务
        lv_timer_handler(); // 不再使用中断处理任务
        Delay_ms(5);        // 延时5ms，控制循环周期
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        lv_tick_inc(1); // 每1ms增加LVGL时间戳
        // 注意：通常不建议在中断中直接调用lv_timer_handler()
        // 可以只更新时间戳，在主循环中处理任务
        // lv_timer_handler();
    }
}
