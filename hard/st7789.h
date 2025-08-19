#ifndef ST7789_H
#define ST7789_H

#include "stm32f4xx.h"
#include "spi_hw.h"  // 改用硬件SPI
#include "lvgl.h"

// ST7789 引脚定义
#define ST7789_DC_PORT    GPIOB
#define ST7789_DC_PIN     GPIO_Pin_3
#define ST7789_RST_PORT   GPIOB
#define ST7789_RST_PIN    GPIO_Pin_2
#define ST7789_BLK_PORT   GPIOB
#define ST7789_BLK_PIN    GPIO_Pin_0

// 引脚操作宏
#define ST7789_DC_H()     GPIO_SetBits(ST7789_DC_PORT, ST7789_DC_PIN)
#define ST7789_DC_L()     GPIO_ResetBits(ST7789_DC_PORT, ST7789_DC_PIN)
#define ST7789_RST_H()    GPIO_SetBits(ST7789_RST_PORT, ST7789_RST_PIN)
#define ST7789_RST_L()    GPIO_ResetBits(ST7789_RST_PORT, ST7789_RST_PIN)
#define ST7789_BLK_ON()   GPIO_SetBits(ST7789_BLK_PORT, ST7789_BLK_PIN)
#define ST7789_BLK_OFF()  GPIO_ResetBits(ST7789_BLK_PORT, ST7789_BLK_PIN)

// 屏幕参数
#define ST7789_WIDTH      240
#define ST7789_HEIGHT     284

// 函数声明
void ST7789_Init(void);
void ST7789_Write_Command(uint8_t cmd);
void ST7789_Write_Data(uint8_t data);
void ST7789_Write_Data_Buffer(uint8_t *data, uint32_t len);  // 批量写数据
void ST7789_Set_Window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
void ST7789_Fill_Color(uint16_t color);
void ST7789_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_Fill_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t *px_map);

#endif
