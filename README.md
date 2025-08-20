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

## 解决方案

![image.png](https://imgg.we20040120.dpdns.org/file/1755695717859_image.png)

问题是`printf("Touch: x=%d, y=%d, points=%d, gesture=%d\n", *x, *y, *points, *gesture);`代码栈溢出问题，虽然进行了串口初始化但是，进行`printf`会瞬间加大栈的消耗，导致启动失败。

1. 嵌入式系统的栈空间限制
在 STM32 等嵌入式系统中，栈（Stack）的大小是固定且有限的（通常在链接脚本.ld中配置，例如_Min_Stack_Size = 0x400;即 1KB）。栈用于存储：

- 函数调用的返回地址
- 局部变量
- 函数参数
- 函数执行过程中的临时数据（如寄存器压栈）

如果程序中某个操作消耗的栈空间超过配置的栈大小，就会发生栈溢出（Stack Overflow），导致程序崩溃、运行异常等问题。

2. printf 函数的栈消耗特性
printf（尤其是带格式化字符串的调用）在嵌入式系统中是高栈消耗操作，原因如下：

- 格式化字符串处理：printf("Touch: x=%d, y=%d, points=%d, gesture=%d\n", ...)需要将整数（x、y等）转换为字符串，这个过程会在栈上临时分配缓冲区（用于存储转换后的字符串）。
- 参数压栈：多个参数（4 个整数 + 字符串指针）需要依次压入栈中，增加栈的瞬时占用。
- 库函数内部调用：printf会调用底层的fputc、字符串处理函数等，这些函数也会消耗额外的栈空间。
3. 代码中的触发条件
从代码逻辑看，被注释的printf位于触摸采样逻辑中：

- 触摸芯片CST816会周期性（原代码计划每 10ms）采样数据，若检测到触摸（*points > 0），则触发printf。
- 当触摸事件频繁发生（例如手指滑动时），printf会被高频调用（每 10ms 内多次执行），导致栈空间被反复、快速占用。

此时，若 LVGL 库（lv_task_handler）本身也在消耗栈空间（例如 UI 绘制时的临时变量），两者叠加可能超过系统配置的栈大小，引发栈溢出。
4. 注释后问题消失的原因
注释printf后，消除了高栈消耗的操作：

- 不再有格式化字符串的临时缓冲区分配。
- 不再有频繁的参数压栈和库函数调用。
- 栈空间仅需满足lv_task_handler、触摸采样等基础操作，未超过配置上限，因此栈溢出问题消失。

```
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
```

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
