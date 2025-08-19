#include "spi_hw.h"
#include "string.h"
#include "lvgl.h"
// DMA 配置结构
static DMA_InitTypeDef DMA_InitStructure;

/**
 * @brief 初始化硬件 SPI1 和相关 GPIO 以及 DMA
 */
void HW_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 使能时钟：SPI1 挂载在 APB2，GPIOA（SCK/MOSI）和 GPIOB（CS）在 AHB1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // 使能 DMA2 时钟

    // 配置 SCK（PA5）和 MOSI（PA7）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin   = HW_SPI_SCK_PIN | HW_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;      // 复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  // 无上下拉
    GPIO_Init(HW_SPI_SCK_PORT, &GPIO_InitStructure);

    // 配置 CS（PB4）为普通推挽输出
    GPIO_InitStructure.GPIO_Pin   = HW_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT; // 普通输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(HW_SPI_CS_PORT, &GPIO_InitStructure);

    // 复用映射：PA5 -> SPI1_SCK，PA7 -> SPI1_MOSI
    GPIO_PinAFConfig(HW_SPI_SCK_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(HW_SPI_MOSI_PORT, GPIO_PinSource7, GPIO_AF_SPI1);

    // 配置 SPI1 参数（适配 ST7789）
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;  // 单向发送模式
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;         // 主机模式
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;         // 8 位数据
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;            // 时钟空闲低电平
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;          // 第一个时钟边沿采样
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;            // 软件控制片选
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // APB2 84MHz，分频 2，得 42MHz
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;        // 高位先行
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                       // CRC 多项式（占位）
    SPI_Init(HW_SPI_PORT, &SPI_InitStructure);

    // 使能 SPI1
    SPI_Cmd(HW_SPI_PORT, ENABLE);

    // 配置 DMA2 Stream3 Channel3 用于 SPI1 TX
    DMA_DeInit(DMA2_Stream3);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_3;                // SPI1 TX 通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(HW_SPI_PORT->DR); // SPI 数据寄存器
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;   // 内存到外设
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;    // 外设地址不增
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;         // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度：字节
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;      // 内存数据宽度：字节
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;              // 普通模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;        // 最高优先级
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         // 禁用 FIFO
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;       // 无意义（FIFO 禁用）
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;       // 单次传输
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;   // 单次传输
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);

    // 使能 DMA 传输完成中断
    DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA2_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 最高优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 初始化 CS 为高电平（无效状态）
    HW_SPI_CS_H();
}

/**
 * @brief 单字节 SPI 传输（用于命令或少量数据）
 * @param byte 要发送的字节
 * @return 固定返回 0（ST7789 无需接收数据）
 */
uint8_t HW_SPI_Transfer(uint8_t byte)
{
    // 等待发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
    // 发送数据
    SPI_I2S_SendData(HW_SPI_PORT, byte);
    // 等待发送完成
    while (SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
    return 0;
}

/**
 * @brief 使用 DMA 进行批量 SPI 传输
 * @param data 数据缓冲区
 * @param len 数据长度（字节）
 */
void HW_SPI_Transmit_DMA(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0) return;

    // 配置 DMA 内存地址和传输长度
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;
    DMA_InitStructure.DMA_BufferSize      = len;
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);

    // 使能 SPI1 DMA 发送
    SPI_I2S_DMACmd(HW_SPI_PORT, SPI_I2S_DMAReq_Tx, ENABLE);
    // 启动 DMA
    DMA_Cmd(DMA2_Stream3, ENABLE);
}

/**
 * @brief DMA2 Stream3 中断处理函数
 */
void DMA2_Stream3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3) == SET) {
        // 清除中断标志
        DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
        // 禁用 DMA 和 SPI DMA 请求
        DMA_Cmd(DMA2_Stream3, DISABLE);
        SPI_I2S_DMACmd(HW_SPI_PORT, SPI_I2S_DMAReq_Tx, DISABLE);
        // 拉高 CS，结束传输
        HW_SPI_CS_H();
        // 通知 LVGL 传输完成
        lv_display_flush_ready(lv_display_get_default());
    }
}
