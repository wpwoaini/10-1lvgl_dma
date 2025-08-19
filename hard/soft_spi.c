#include "soft_spi.h"
#include "Delay.h"

void Soft_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN | SPI_MOSI_PIN | SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

    SPI_SCK_L();
    SPI_CS_H();
}

uint8_t Soft_SPI_Transfer(uint8_t byte)
{
    uint8_t i, rx = 0;
    for (i = 0; i < 8; i++) {
        if (byte & 0x80)
            SPI_MOSI_H();
        else
            SPI_MOSI_L();
        byte <<= 1;
        SPI_SCK_H();
        Delay_us(1);
        rx <<= 1;
        if (SPI_MISO_READ()) rx |= 0x01;
        SPI_SCK_L();
        Delay_us(1);
    }
    return rx;
}