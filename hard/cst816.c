#include "cst816.h"
#include <stdio.h> // 用于调试输出，需实现串口
#include "delay.h" // 用于延时函数，需实现 Delay_ms 函数
// 初始化 CST816 触摸控制器
void CST816_Init(void)
{                                 // 禁用自动休眠
    CST816_Write_Reg(0xFE, 0x01); // DisAutoSleep = 1，禁止自动低功耗

    // 配置中断控制
    CST816_Write_Reg(0xFA, 0x40); // EnTouch = 1，检测触摸时触发数据更新

    // 配置手势检测
    CST816_Write_Reg(0xEC, 0x07); // 使能连续滑动和双击（EnConLR, EnConUD, EnDClick）

    // 设置中断脉冲宽度
    CST816_Write_Reg(0xED, 0x0A); // IrqPluseWidth = 10（1ms）

    // 设置正常扫描周期
    CST816_Write_Reg(0xEE, 0x01); // NorScanPer = 1（10ms）

    // 等待初始化完成
    Delay_ms(100);
}

// 读取寄存器
uint8_t CST816_Read_Reg(uint8_t reg)
{
    uint8_t value;

    Soft_I2C_Start();
    Soft_I2C_Write_Byte((CST816_ADDR << 1) | 0); // 写地址
    Soft_I2C_Write_Byte(reg);                    // 寄存器地址
    Soft_I2C_Start();
    Soft_I2C_Write_Byte((CST816_ADDR << 1) | 1); // 读地址
    value = Soft_I2C_Read_Byte(0);               // 读取数据（NACK）
    Soft_I2C_Stop();

    return value;
}

// 写入寄存器
void CST816_Write_Reg(uint8_t reg, uint8_t value)
{
    Soft_I2C_Start();
    Soft_I2C_Write_Byte((CST816_ADDR << 1) | 0); // 写地址
    Soft_I2C_Write_Byte(reg);                    // 寄存器地址
    Soft_I2C_Write_Byte(value);                  // 写入数据
    Soft_I2C_Stop();
}

// 获取触摸数据
void CST816_Get_Touch(uint16_t *x, uint16_t *y, uint8_t *gesture, uint8_t *points)
{
    uint8_t buf[6];

    Soft_I2C_Start();
    Soft_I2C_Write_Byte((CST816_ADDR << 1) | 0); // 写地址
    Soft_I2C_Write_Byte(0x01);                   // 从寄存器 0x01 开始
    Soft_I2C_Start();
    Soft_I2C_Write_Byte((CST816_ADDR << 1) | 1); // 读地址
    buf[0] = Soft_I2C_Read_Byte(1);              // GestureID
    buf[1] = Soft_I2C_Read_Byte(1);              // FingerNum
    buf[2] = Soft_I2C_Read_Byte(1);              // XposH
    buf[3] = Soft_I2C_Read_Byte(1);              // XposL
    buf[4] = Soft_I2C_Read_Byte(1);              // YposH
    buf[5] = Soft_I2C_Read_Byte(0);              // YposL
    Soft_I2C_Stop();

    *gesture = buf[0];
    *points  = buf[1] & 0x0F;                   // 触摸点数（低 4 位）
    *x       = ((buf[2] & 0x0F) << 8) | buf[3]; // X 坐标（12 位）
    *y       = ((buf[4] & 0x0F) << 8) | buf[5]; // Y 坐标（12 位）

    // 调试输出
    // if (*points > 0) {
    //     printf("Touch: x=%d, y=%d, points=%d, gesture=%d\n", *x, *y, *points, *gesture);
    // }
}
