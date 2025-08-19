#include "soft_i2c.h"
#include "Delay.h"
void Soft_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);

    I2C_SCL_H();
    I2C_SDA_H();
}

void Soft_I2C_Start(void)
{
    I2C_SDA_H();
    I2C_SCL_H();
    Delay_us(5);
    I2C_SDA_L();
    Delay_us(5);
    I2C_SCL_L();
    Delay_us(5);
}

void Soft_I2C_Stop(void)
{
    I2C_SDA_L();
    I2C_SCL_H();
    Delay_us(5);
    I2C_SDA_H();
    Delay_us(5);
}

uint8_t Soft_I2C_Write_Byte(uint8_t byte)
{
    uint8_t i, ack;
    for (i = 0; i < 8; i++) {
        if (byte & 0x80)
            I2C_SDA_H();
        else
            I2C_SDA_L();
        Delay_us(5);
        I2C_SCL_H();
        Delay_us(5);
        I2C_SCL_L();
        byte <<= 1;
    }
    I2C_SDA_H(); // Release SDA
    Delay_us(5);
    I2C_SCL_H();
    Delay_us(5);
    ack = I2C_SDA_READ();
    I2C_SCL_L();
    Delay_us(5);
    return ack;
}

uint8_t Soft_I2C_Read_Byte(uint8_t ack)
{
    uint8_t i, byte = 0;
    I2C_SDA_H(); // Release SDA
    for (i = 0; i < 8; i++) {
        byte <<= 1;
        I2C_SCL_H();
        Delay_us(5);
        if (I2C_SDA_READ()) byte |= 0x01;
        I2C_SCL_L();
        Delay_us(5);
    }
    if (ack)
        I2C_SDA_L();
    else
        I2C_SDA_H();
    Delay_us(5);
    I2C_SCL_H();
    Delay_us(5);
    I2C_SCL_L();
    Delay_us(5);
    I2C_SDA_H();
    return byte;
}