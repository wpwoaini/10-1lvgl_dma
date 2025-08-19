#include "stm32f4xx.h"                  // Device header
#include "OLED_Font.h"

/*引脚配置*/
#define OLED_W_D0(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)(x))
#define OLED_W_D1(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)(x))
#define OLED_W_RES(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)(x))
#define OLED_W_DC(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)(x))
#define OLED_W_CS(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)(x))

/*引脚初始化*/
void OLED_SPI_Init(void)
{
    // 第一步：使能GPIOB时钟（必须添加！）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    // 复位时钟（可选，一般不需要）
    // RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    // RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE); // 复位后需关闭复位
	
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 补充上拉/下拉配置（重要）
    // 批量配置引脚（简化代码）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    // 初始化引脚状态
    OLED_W_D0(0);
    OLED_W_D1(1);
    OLED_W_RES(1);
    OLED_W_DC(1);
    OLED_W_CS(1);
}

/**
  * @brief  SPI发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_SPI_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_D1(!!(Byte & (0x80 >> i)));
		OLED_W_D0(1);
		OLED_W_D0(0);
	}
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_W_CS(0);
	OLED_W_DC(0);
	OLED_SPI_SendByte(Command);
	OLED_W_CS(1);
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_W_CS(0);
	OLED_W_DC(1);
	OLED_SPI_SendByte(Data);
	OLED_W_CS(1);
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，固定宽度显示）
  * @param  Line 起始行位置
  * @param  Column 起始列位置
  * @param  Number 要显示的数字（支持int32_t范围）
  * @param  Width 显示宽度（至少1）
  * @param  ShowSign 正数是否显示+号（0=不显示，1=显示）
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Width, uint8_t ShowSign)
{
    uint8_t i;  // 仅保留使用的循环变量i
    uint32_t temp;
    uint8_t digits[10] = {0}; // 存储数字的每一位
    uint8_t digitCount = 0;   // 实际数字位数
    
    // 处理符号
    if (Number < 0) {
        OLED_ShowChar(Line, Column, '-'); // 显示负号
        temp = (uint32_t)(-Number);       // 转为正数处理
        Column++;                         // 列位置右移
    } else {
        temp = (uint32_t)Number;
        if (ShowSign) {
            OLED_ShowChar(Line, Column, '+'); // 显示正号
            Column++;                         // 列位置右移
        }
    }
    
    // 计算数字的每一位（分解为数组）
    if (temp == 0) {
        digits[digitCount++] = 0; // 0的特殊处理
    } else {
        while (temp > 0 && digitCount < 10) {
            digits[digitCount++] = temp % 10; // 取个位
            temp /= 10;                       // 移除个位
        }
    }
    
    // 计算需要补位的数量（宽度不足时补位）
    uint8_t padding = Width > digitCount ? Width - digitCount : 0;
    
    // 补位字符：正数补空格，负数补0
    char padChar = (Number < 0) ? '0' : '0';
    
    // 显示补位字符
    for (i = 0; i < padding; i++) {
        OLED_ShowChar(Line, Column++, padChar);
    }
    
    // 倒序显示数字（从高位到低位）
    for (i = digitCount; i > 0; i--) {
        OLED_ShowChar(Line, Column++, digits[i-1] + '0');
    }
}
/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_SPI_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
