/**
 * @file lv_port_disp.c
 * @brief LVGL 显示驱动移植文件
 */

#if 1

#include "lv_port_disp.h"
#include <stdbool.h>
#include "st7789.h"
#include "lv_port_indev.h"

// 定义屏幕分辨率
#ifndef MY_DISP_HOR_RES
#define MY_DISP_HOR_RES 240 // 水平分辨率
#endif

#ifndef MY_DISP_VER_RES
#define MY_DISP_VER_RES 284 // 垂直分辨率
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) // RGB565 每像素 2 字节
#define BUFFER_LINES   40                                                 // 缓冲区行数，优化内存与刷新频率平衡

// 函数原型
static void disp_init(void);                                                        // 初始化显示设备
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map); // 刷新显示区域

/**
 * @brief 初始化 LVGL 显示驱动
 */
void lv_port_disp_init(void)
{
    disp_init(); // 初始化显示设备

    // 创建显示对象并设置刷新回调
    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    // 定义两个缓冲区，用于部分渲染（双缓冲）
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_1[MY_DISP_HOR_RES * BUFFER_LINES * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_2[MY_DISP_HOR_RES * BUFFER_LINES * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/**
 * @brief 初始化显示硬件
 */
static void disp_init(void)
{
    ST7789_Init(); // 初始化 ST7789 显示屏
}

volatile bool disp_flush_enabled = true; // 控制刷新使能标志

/**
 * @brief 使能屏幕刷新
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/**
 * @brief 禁用屏幕刷新
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/**
 * @brief LVGL 刷新回调，将缓冲区内容传输到显示屏
 * @param disp 显示对象
 * @param area 待刷新区域
 * @param px_map 像素数据（RGB565 格式）
 */
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (disp_flush_enabled) {
        // 交换 RGB565 字节序（适配 ST7789）
        lv_draw_sw_rgb565_swap(px_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
        // 使用 DMA 传输像素数据到显示屏
        ST7789_Fill_Rect(area->x1, area->y1, area->x2, area->y2, px_map);
    }
    // 注意：lv_display_flush_ready 在 DMA 中断中调用
}

/**
 * @brief 初始化 LVGL 及相关驱动
 */
void mylvgl_init(void)
{
    lv_init();            // 初始化 LVGL 库
    lv_port_disp_init();  // 初始化显示驱动
    lv_port_indev_init(); // 初始化输入设备
    // 在主程序中初始化 LVGL 定时器（如 10ms 调用 lv_timer_handler）
}

#else
typedef int keep_pedantic_happy;
#endif
