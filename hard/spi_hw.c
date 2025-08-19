#include "spi_hw.h"
#include "string.h"

void HW_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 使能时钟：SPI1挂载在APB2，GPIOA（SCK/MOSI）和GPIOB（CS）在AHB1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // SPI1使用APB2时钟

    // 配置SCK（PA5）和MOSI（PA7）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin   = HW_SPI_SCK_PIN | HW_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速SPI
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(HW_SPI_SCK_PORT, &GPIO_InitStructure);

    // 配置CS（PB4）为普通推挽输出
    GPIO_InitStructure.GPIO_Pin   = HW_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(HW_SPI_CS_PORT, &GPIO_InitStructure);

    // 复用映射SPI1：PA5->SPI1_SCK，PA7->SPI1_MOSI
    GPIO_PinAFConfig(HW_SPI_SCK_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(HW_SPI_MOSI_PORT, GPIO_PinSource7, GPIO_AF_SPI1);

    // 配置SPI1参数（与ST7789匹配）
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; // 仅发送模式
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;        // 主机模式
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;        // 8位数据
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;           // 时钟极性：低电平空闲
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;         // 第1边沿采样
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;           // 软件控制片选
    // APB2时钟默认84MHz（可根据系统时钟调整），分频为2得到42MHz（ST7789支持更高速度）
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB; // 高位在前
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                // CRC多项式（无用，仅占位）
    SPI_Init(HW_SPI_PORT, &SPI_InitStructure);

    // 使能SPI1
    SPI_Cmd(HW_SPI_PORT, ENABLE);

    // 初始化片选状态（高电平无效）
    HW_SPI_CS_H();
}

// 单字节硬件SPI传输
uint8_t HW_SPI_Transfer(uint8_t byte)
{
    // 等待发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
    // 发送数据
    SPI_I2S_SendData(HW_SPI_PORT, byte);
    // 等待发送完成
    while (SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
    return 0; // ST7789无需接收，返回0
}

// 批量发送数据
void HW_SPI_Transmit(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0) return;
    for (uint32_t i = 0; i < len; i++) {
        HW_SPI_Transfer(data[i]);
    }
}
