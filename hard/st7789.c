#include "st7789.h"
#include "spi_hw.h"
#include "delay.h"

// 替换为硬件 SPI 的片选控制宏
#define SPI_CS_H() HW_SPI_CS_H()
#define SPI_CS_L() HW_SPI_CS_L()

/**
 * @brief 发送 ST7789 命令
 * @param cmd 命令字节
 */
void ST7789_Write_Command(uint8_t cmd)
{
    SPI_CS_L();
    ST7789_DC_L(); // 命令模式
    HW_SPI_Transfer(cmd);
    SPI_CS_H();
}

/**
 * @brief 发送单个数据字节
 * @param data 数据字节
 */
void ST7789_Write_Data(uint8_t data)
{
    SPI_CS_L();
    ST7789_DC_H(); // 数据模式
    HW_SPI_Transfer(data);
    SPI_CS_H();
}

/**
 * @brief 使用 DMA 发送批量数据
 * @param data 数据缓冲区
 * @param len 数据长度（字节）
 */
void ST7789_Write_Data_Buffer(uint8_t *data, uint32_t len)
{
    SPI_CS_L();
    ST7789_DC_H();
    HW_SPI_Transmit_DMA(data, len);
    // CS 在 DMA 中断中拉高
}

/**
 * @brief 初始化 ST7789 显示屏
 */
void ST7789_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // 初始化 DC、RST 和 BLK 引脚
    GPIO_InitStructure.GPIO_Pin   = ST7789_DC_PIN | ST7789_RST_PIN | ST7789_BLK_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(ST7789_DC_PORT, &GPIO_InitStructure);

    Delay_Init();
    HW_SPI_Init();

    ST7789_BLK_ON(); // 打开背光

    // 硬件复位
    ST7789_RST_L();
    Delay_ms(10);
    ST7789_RST_H();
    Delay_ms(120);

    // ST7789 初始化序列
    ST7789_Write_Command(0x11); // 退出睡眠模式
    Delay_ms(120);

    ST7789_Write_Command(0x21); // 开启反显

    ST7789_Write_Command(0x36); // 内存访问控制
    ST7789_Write_Data(0x00);    // 正常方向

    ST7789_Write_Command(0x3A); // 像素格式
    ST7789_Write_Data(0x05);    // RGB565

    ST7789_Write_Command(0xB2); // Porch 设置
    ST7789_Write_Data(0x0C);
    ST7789_Write_Data(0x0C);
    ST7789_Write_Data(0x00);
    ST7789_Write_Data(0x33);
    ST7789_Write_Data(0x33);

    ST7789_Write_Command(0xB7); // 门控设置
    ST7789_Write_Data(0x35);

    ST7789_Write_Command(0xBB); // VCOM 设置
    ST7789_Write_Data(0x19);

    ST7789_Write_Command(0xC0); // 电源控制 1
    ST7789_Write_Data(0x2C);

    ST7789_Write_Command(0xC2); // VDV/VRH 使能
    ST7789_Write_Data(0x01);

    ST7789_Write_Command(0xC3); // VRH 设置
    ST7789_Write_Data(0x12);

    ST7789_Write_Command(0xC4); // VDV 设置
    ST7789_Write_Data(0x20);

    ST7789_Write_Command(0xC6); // 帧率
    ST7789_Write_Data(0x0F);

    ST7789_Write_Command(0xD0); // 电源控制 2
    ST7789_Write_Data(0xA4);
    ST7789_Write_Data(0xA1);

    ST7789_Write_Command(0xE0); // 正伽马校正
    ST7789_Write_Data(0xD0);
    ST7789_Write_Data(0x04);
    ST7789_Write_Data(0x0D);
    ST7789_Write_Data(0x11);
    ST7789_Write_Data(0x13);
    ST7789_Write_Data(0x2B);
    ST7789_Write_Data(0x3F);
    ST7789_Write_Data(0x54);
    ST7789_Write_Data(0x4C);
    ST7789_Write_Data(0x18);
    ST7789_Write_Data(0x0D);
    ST7789_Write_Data(0x0B);
    ST7789_Write_Data(0x1F);
    ST7789_Write_Data(0x23);

    ST7789_Write_Command(0xE1); // 负伽马校正
    ST7789_Write_Data(0xD0);
    ST7789_Write_Data(0x04);
    ST7789_Write_Data(0x0C);
    ST7789_Write_Data(0x11);
    ST7789_Write_Data(0x13);
    ST7789_Write_Data(0x2C);
    ST7789_Write_Data(0x3F);
    ST7789_Write_Data(0x44);
    ST7789_Write_Data(0x51);
    ST7789_Write_Data(0x2F);
    ST7789_Write_Data(0x1F);
    ST7789_Write_Data(0x1F);
    ST7789_Write_Data(0x0F);
    ST7789_Write_Data(0x03);

    ST7789_Write_Command(0x29); // 开启显示
}

/**
 * @brief 设置显示窗口
 * @param x_start 起始列
 * @param y_start 起始行
 * @param x_end 结束列
 * @param y_end 结束行
 */
void ST7789_Set_Window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    ST7789_Write_Command(0x2A); // 设置列地址
    ST7789_Write_Data(x_start >> 8);
    ST7789_Write_Data(x_start & 0xFF);
    ST7789_Write_Data(x_end >> 8);
    ST7789_Write_Data(x_end & 0xFF);

    ST7789_Write_Command(0x2B); // 设置行地址
    ST7789_Write_Data(y_start >> 8);
    ST7789_Write_Data(y_start & 0xFF);
    ST7789_Write_Data(y_end >> 8);
    ST7789_Write_Data(y_end & 0xFF);

    ST7789_Write_Command(0x2C); // 开始写内存
}

/**
 * @brief 填充整个屏幕为单一颜色
 * @param color 16 位 RGB565 颜色值
 */
void ST7789_Fill_Color(uint16_t color)
{
    ST7789_Set_Window(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    uint8_t color_buf[2] = {color >> 8, color & 0xFF};
    SPI_CS_L();
    ST7789_DC_H();
    for (uint32_t i = 0; i < ST7789_WIDTH * ST7789_HEIGHT; i++) {
        HW_SPI_Transfer(color_buf[0]);
        HW_SPI_Transfer(color_buf[1]);
    }
    SPI_CS_H();
}

/**
 * @brief 绘制单个像素
 * @param x 像素 x 坐标
 * @param y 像素 y 坐标
 * @param color 16 位 RGB565 颜色值
 */
void ST7789_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT) return;
    ST7789_Set_Window(x, y, x, y);
    SPI_CS_L();
    ST7789_DC_H();
    HW_SPI_Transfer(color >> 8);
    HW_SPI_Transfer(color & 0xFF);
    SPI_CS_H();
}

/**
 * @brief 填充矩形区域
 * @param x1 起始 x 坐标
 * @param y1 起始 y 坐标
 * @param x2 结束 x 坐标
 * @param y2 结束 y 坐标
 * @param px_map 像素数据缓冲区（RGB565 格式）
 */
void ST7789_Fill_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t *px_map)
{
    if (x1 > x2 || y1 > y2 || x2 >= ST7789_WIDTH || y2 >= ST7789_HEIGHT) return;
    ST7789_Set_Window(x1, y1, x2, y2);
    uint32_t len = (x2 - x1 + 1) * (y2 - y1 + 1) * 2; // 每个像素 2 字节（RGB565）
    ST7789_Write_Data_Buffer(px_map, len);
}
