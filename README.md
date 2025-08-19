# STM32F4 LVGL 项目 - README
## 项目概述
这是一个基于 STM32F4 微控制器的 LVGL 图形库项目，集成 ST7789 显示屏，使用 SPI DMA 优化显示刷新。项目目标是实现一个高效的图形界面，适用于嵌入式设备开发。当前版本在 EIDE 环境中通过 project.code-workspace 导入后构建，但运行调试时遇到了栈溢出问题。

- 创建日期: 2025年8月19日
- 更新时间: 2025年8月19日 下午2:59 (+04 时区)
- 硬件: STM32F4 系列（可能为 STM32F407 或 STM32F429）
- 软件: LVGL 图形库，EIDE 开发环境
- 显示: ST7789，240x284 分辨率，双缓冲区 (BUFFER_LINES = 40)
- [x] B站视频录制

## 安装与配置
### 前提条件

- 硬件: STM32F4 开发板，ST7789 显示模块，SPI 和 I2C 接口连接。
- 软件:
 - EIDE 环境 (Embedded Integrated Development Environment)
 - STM32CubeMX (可选，用于初始化配置)
 - 串口工具 (如 PuTTY 或 Tera Term) 用于调试输出


- 工具链: GCC ARM Embedded 编译器

## 安装步骤

1.下载项目:
 - 克隆或下载本仓库到本地。

 git clone https://github.com/wpwoaini/10-1lvgl_dma.git


2.打开 EIDE:
- 启动 [EIDE](https://marketplace.visualstudio.com/items?itemName=CL.eide)，点击“File” > “Open Workspace”。

 1.在vscode下载并且安装[EIDE](https://marketplace.visualstudio.com/items?itemName=CL.eide)：
![EIDE.png](https://imgg.we20040120.dpdns.org/file/1755612094470_image.png)

 2.打开项目：
 ![open.png](https://imgg.we20040120.dpdns.org/file/1755612337378_image.png)

 3.选择 project.code-workspace 文件，导入项目。
 ![work.png](https://imgg.we20040120.dpdns.org/file/1755612401914_image.png)



3.配置环境:
- 确保 STM32F4 固件库 (STM32F4xx.h) 已正确链接。
- 检查链接脚本 (.ld 文件)，确认 RAM 和 Heap/Stack 分配。


4.构建项目:
- 点击“Build”按钮，编译项目。


5.运行调试:
>**注意**：如果可以直接flash则不用此项，因为栈的问题，无法`启动`(HAL库没这个问题)

- 连接调试器 (如 ST-Link)，点击“Debug”运行。
 ![image.png](https://imgg.we20040120.dpdns.org/file/1755612475627_image.png)


## 硬件引脚配置
以下是项目使用的引脚定义，基于 GPIOB 端口：


|  功能   | 端口  |  引脚   | 描述  |
|  :----:  | :----:  |  :----:  | :----:  |
| SPI_SCK  | GPIOA | PA5  | SPI 时钟信号 |
| SPI_MOSI  | GPIOBA| PA7  | SPI 主输出从输入MOSI |
| SPI_MISO(未使用)  | x | x  | x |
| SPI_CS  | GPIOB | PB4  | SPI 片选信号 |
| ST7789_DC  | GPIOB | PB3  | 数据/命令控制引脚 |
| ST7789_RST  | GPIOB | PB2  | 复位引脚 |
| ST7789_BLK  | GPIOB | PB0  | 背光控制引脚 |
| I2C_SCL  | GPIOB | PB15  | I2C 时钟信号 |
| I2C_SDA  | GPIOB | PB14  | I2C 数据信号 |


#### 注意:

- 确保硬件连接与上述引脚定义一致。

## 已知问题 - 栈溢出 
- [x] 解决
### 问题描述
在 EIDE 导入`project.code-workspace`后构建并运行调试时，程序出现栈溢出 (Stack Overflow) 错误，导致调试中断或设备重启。问题可能发生在 mylvgl_init() 初始化期间。
### 可能原因

- 栈空间不足:
双缓冲区占用 38.4KB SRAM (BUFFER_LINES = 40)，可能超出 STM32F4 SRAM 容量 (如 STM32F407 的 128KB)。
默认栈大小 (1-4KB) 不足以支持 LVGL 初始化或中断处理。


- 内存分配冲突:
静态缓冲区 (LV_ATTRIBUTE_MEM_ALIGN) 和堆栈可能重叠，链接脚本配置错误。


- 中断堆栈溢出:
TIM3 中断或 DMA 中断中调用 lv_timer_handler() 导致栈溢出。


- 代码优化问题:
EIDE 优化级别过高，可能导致栈使用异常。



### 排查步骤

1.检查 SRAM 使用:
- 打开 .ld 文件，确认 RAM 区域大小。
- 示例配置:
```
MEMORY
{
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
}
_estack = ORIGIN(RAM) + LENGTH(RAM); /* 栈顶 */
_Min_Stack_Size = 0x800; /* 2KB 栈 */
_Min_Heap_Size = 0x400; /* 1KB 堆 */
```

- 确保 `BUFFER_LINES * MY_DISP_HOR_RES * BYTE_PER_PIXEL * 2 `加上栈和堆不超过 SRAM。


2.添加调试信息:
在 `lv_port_disp_init() `中添加串口输出:
```
void lv_port_disp_init(void)
{
    printf("Starting lv_port_disp_init...\n");
    disp_init();
    printf("disp_init completed.\n");
    // 其他初始化代码
}
```

- 运行后检查串口输出，定位崩溃点。


3.调整缓冲区大小:
- 减小 BUFFER_LINES (如从 40 减到 20)，缓冲区降至 19.2KB:
```
#define BUFFER_LINES 20
```




4.修改栈大小:
- 在 .ld 文件中增加 _Min_Stack_Size (如 4KB):
```
_Min_Stack_Size = 0x1000; /* 4KB 栈 */
```



5.检查中断配置:
- 确保 TIM3 中断优先级低于 DMA (如 TIM3 设为 1，DMA 设为 0):
```
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TIM3
```



6.验证硬件:
- 检查 SPI 引脚 (PB5, PB6, PB7, PB4) 和 ST7789 连接。
- 降低 SPI 频率 (如从 42MHz 降至 21MHz):
```
SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
```




## 解决方案

- 临时解决:
 - 减小 BUFFER_LINES 到 20，增加栈大小到 4KB，重新构建并调试。


- 长期优化:
 - 使用调试器 (如 Keil 或 STM32CubeIDE) 监控栈使用。
 - 移除中断中的 lv_timer_handler()，在主循环中调用:
 ```
 while (1) {
    if (sys_time - last_task_time >= 5) {
        lv_timer_handler();
        last_task_time = sys_time;
    }
    Delay_ms(5);
}
```

- 确认 STM32F4 型号 (如 F407 或 F429)，调整内存分配。



## 贡献
欢迎贡献代码或修复问题：

1.Fork 本仓库。

2.创建新分支 (git checkout -b feature-branch)。

3.提交更改 (git commit -m "描述")。

4.推送并提交 Pull Request。

## 许可证
本项目采用 MIT 许可证。详情见 LICENSE 文件。
联系方式

日期: 2025年8月19日 下午2:59 (+04)

支持: 通过 GitHub Issues 联系。
