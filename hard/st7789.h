#ifndef ST7789_H
#define ST7789_H

#include "stm32f4xx.h"
#include "spi_hw.h"
#include "lvgl.h"

// ST7789 引脚定义
#define ST7789_DC_PORT    GPIOB
#define ST7789_DC_PIN     GPIO_Pin_3 // 数据/命令控制引脚
#define ST7789_RST_PORT   GPIOB
#define ST7789_RST_PIN    GPIO_Pin_2 // 复位引脚
#define ST7789_BLK_PORT   GPIOB
#define ST7789_BLK_PIN    GPIO_Pin_0 // 背光控制引脚

// 引脚操作宏
#define ST7789_DC_H()     GPIO_SetBits(ST7789_DC_PORT, ST7789_DC_PIN) // DC 高（数据）
#define ST7789_DC_L()     GPIO_ResetBits(ST7789_DC_PORT, ST7789_DC_PIN) // DC 低（命令）
#define ST7789_RST_H()    GPIO_SetBits(ST7789_RST_PORT, ST7789_RST_PIN) // 复位高
#define ST7789_RST_L()    GPIO_ResetBits(ST7789_RST_PORT, ST7789_RST_PIN) // 复位低
#define ST7789_BLK_ON()   GPIO_SetBits(ST7789_BLK_PORT, ST7789_BLK_PIN) // 打开背光
#define ST7789_BLK_OFF()  GPIO_ResetBits(ST7789_BLK_PORT, ST7789_BLK_PIN) // 关闭背光

// 屏幕分辨率
#define ST7789_WIDTH      240 // 宽度
#define ST7789_HEIGHT     284 // 高度

// 函数声明
void ST7789_Init(void); // 初始化 ST7789
void ST7789_Write_Command(uint8_t cmd); // 发送命令
void ST7789_Write_Data(uint8_t data); // 发送单个数据字节
void ST7789_Write_Data_Buffer(uint8_t *data, uint32_t len); // 使用 DMA 发送批量数据
void ST7789_Set_Window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end); // 设置显示窗口
void ST7789_Fill_Color(uint16_t color); // 填充屏幕颜色
void ST7789_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color); // 绘制像素
void ST7789_Fill_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t *px_map); // 填充矩形

#endif
