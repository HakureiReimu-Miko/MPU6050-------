/******************************************************************************

				  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : uart.c
  版 本 号   :
  作    者   :
  生成日期   : 2019年9月2日
  功能描述   : 串口函数
  修改历史   :
  1.日    期   :
	作    者   :
	修改内容   :
******************************************************************************/

#include "sys.h"
#include "uart.h"
DATABUFF Uart_Struct[UARTBUFNUM];

/*****************************************************************************
 函 数 名  : void UART2_Init(u32 bdt)
 功能描述  : 串口2初始化
 输入参数  :	bdt 波特率
 输出参数  :
 修改历史  :
  1.日    期   : 2019年10月12日
	作    者   :chenmeishu
	修改内容   : 创建
*****************************************************************************/
// #define UART2485
#ifdef UART2485
sbit TR2 = P0 ^ 0;
#endif

void UART2_Init(u32 bdt) // 8N1 115200
{
	u16 tmp;
	tmp = 1024 - FOSC / 64 / bdt;
	MUX_SEL |= 0X40;
	SetPinOut(0, 4);
	SetPinIn(0, 5);
	P0 |= 0x30;
	ADCON = 0x80;
	SCON0 = 0x50;
	SREL0H = (u8)(tmp >> 8);
	SREL0L = (u8)tmp;
	IEN0 |= 0x10;
	ES0 = 1;
#ifdef UART2485
	SetPinOut(0, 0);
	TR2 = 0;
#endif
	//	Uart_Struct[UART2].tx_head = 0;
	//	Uart_Struct[UART2].tx_tail = 0;
	//	Uart_Struct[UART2].tx_flag = 0;
	//	Uart_Struct[UART2].rx_head = 0;
	//	Uart_Struct[UART2].rx_tail = 0;
	//	EA=1;
}
/*****************************************************************************
 函 数 名  : void UART3_Init(void)
 功能描述  : 串口3初始化
 输入参数  :
 输出参数  :
 修改历史  :
  1.日    期   : 2019年10月12日
	作    者   :chenmeishu
	修改内容   : 创建
*****************************************************************************/
// #define UART3485
#ifdef UART3485
sbit TR3 = P0 ^ 0;
#endif
void UART3_Init(u32 bdt) // 8N1 115200
{
	u16 tmp;
	tmp = 1024 - FOSC / 32 / bdt;

	MUX_SEL |= 0X20;

	SetPinOut(0, 6);
	SetPinIn(0, 7);
	P0 |= 0xC0;

	SCON1 = 0X90;
	SREL1H = (u8)(tmp >> 8);
	SREL1L = (u8)tmp;
	IEN2 |= 1;
#ifdef UART3485
	SetPinOut(0, 0);
	TR3 = 0;
#endif
	//	Uart_Struct[UART3].tx_head = 0;
	//	Uart_Struct[UART3].tx_tail = 0;
	//	Uart_Struct[UART3].tx_flag = 0;
	//	Uart_Struct[UART3].rx_head = 0;
	//	Uart_Struct[UART3].rx_tail = 0;
}
/*****************************************************************************
 函 数 名  : void UART4_Init(void)
 功能描述  : 串口4初始化
 输入参数  :
 输出参数  :
 修改历史  :
  1.日    期   : 2019年10月12日
	作    者   :chenmeishu
	修改内容   : 创建
*****************************************************************************/
#define UART4485
#ifdef UART4485
sbit TR4 = P0 ^ 0;
#endif
void UART4_Init(u32 bdt) // 8O1 115200
{
	u16 tmp;
	SCON2T = 0XC0;
	SCON2R = 0XC0;
	// SCON2T = 0X80;//8N1 115200
	// SCON2R = 0X80;
	tmp = FOSC / 8 / bdt;
	BODE2_DIV_H = (u8)(tmp >> 8);
	BODE2_DIV_L = (u8)tmp;

	IEN1 |= 0X0C;
#ifdef UART4485
	SetPinOut(0, 0);
	TR4 = 0;
#endif
	//	Uart_Struct[UART4].tx_head = 0;
	//	Uart_Struct[UART4].tx_tail = 0;
	//	Uart_Struct[UART4].tx_flag = 0;
	//	Uart_Struct[UART4].rx_head = 0;
	//	Uart_Struct[UART4].rx_tail = 0;
}
/*****************************************************************************
 函 数 名  : void UART5_Init(void)
 功能描述  : 串口5初始化
 输入参数  :
 输出参数  :
 修改历史  :
  1.日    期   : 2019年10月12日
	作    者   :chenmeishu
	修改内容   : 创建
*****************************************************************************/
#define UART5485
#ifdef UART5485
sbit TR5 = P0 ^ 1;
#endif
void UART5_Init(u32 bdt)
{
	u16 tmp;
	SCON3T = 0X80;
	SCON3R = 0X80;

	tmp = FOSC / 8 / bdt;
	BODE3_DIV_H = (u8)(tmp >> 8);
	BODE3_DIV_L = (u8)tmp;
	;
	IEN1 |= 0X30;
#ifdef UART5485
	SetPinOut(0, 1);
	TR5 = 0;
#endif
	//	Uart_Struct[UART5].tx_head = 0;
	//	Uart_Struct[UART5].tx_tail = 0;
	//	Uart_Struct[UART5].tx_flag = 0;
	//	Uart_Struct[UART5].rx_head = 0;
	//	Uart_Struct[UART5].rx_tail = 0;
}

void UartInit(u8 UartPort, u32 bdt)
{
	switch (UartPort)
	{
	case UART2:
		UART2_Init(bdt);
		break;
	case UART3:
		UART3_Init(bdt);
		break;
	case UART4:
		UART4_Init(bdt);
		break;
	case UART5:
		UART5_Init(bdt);
		break;
	default:
		break;
	}
}

/*****************************************************************************
 函 数 名  : void Uart_Send_Data(u8 str,u8 len,const u8 *buf)
 功能描述  : 串口2-5发送数据
 输入参数  : 串口选择0-3对应串口2-5，发送长度len，发送数据缓存区*buf
 输出参数  :
 修改历史  :
  1.日    期   : 2020年5月1日
	作    者   :chenmeishu
	修改内容   : 创建
*****************************************************************************/
void Uart_Send_Data(u8 str, u16 len, const u8 *buf)
{
	u16 i;

	EA = 0;
	for (i = 0; i < len; i++)
	{
		Uart_Struct[str].tx_buf[Uart_Struct[str].tx_head] = *buf++;
		Uart_Struct[str].tx_head++;
		Uart_Struct[str].tx_head &= SERIAL_COUNT;
	}
	switch (str)
	{
	case UART2:
		if (Uart_Struct[UART2].tx_flag == 0)
		{
#ifdef UART2485
			TR2 = 1;
#endif
			Uart_Struct[UART2].tx_flag = 1;
			SBUF0 = Uart_Struct[UART2].tx_buf[Uart_Struct[UART2].tx_tail];
			Uart_Struct[UART2].tx_tail++;
			Uart_Struct[UART2].tx_tail &= SERIAL_COUNT;
		}
		break;
	case UART3:
		if (Uart_Struct[UART3].tx_flag == 0)
		{
#ifdef UART3485
			TR3 = 1;
#endif
			Uart_Struct[UART3].tx_flag = 1;
			SBUF1 = Uart_Struct[UART3].tx_buf[Uart_Struct[UART3].tx_tail];
			Uart_Struct[UART3].tx_tail++;
			Uart_Struct[UART3].tx_tail &= SERIAL_COUNT;
		}
		break;
	case UART4:
		if (Uart_Struct[UART4].tx_flag == 0)
		{
#ifdef UART4485
			TR4 = 1;
#endif
			Uart_Struct[UART4].tx_flag = 1;
			
			ACC = Uart_Struct[UART4].tx_buf[Uart_Struct[UART4].tx_tail]; // 奇校验
			P ? (SCON2T &= ~(0x01 << 5)) : (SCON2T |= (0x01 << 5));

			SBUF2_TX = Uart_Struct[UART4].tx_buf[Uart_Struct[UART4].tx_tail];
			Uart_Struct[UART4].tx_tail++;
			Uart_Struct[UART4].tx_tail &= SERIAL_COUNT;
		}
		break;
	case UART5:
		if (Uart_Struct[UART5].tx_flag == 0)
		{
#ifdef UART5485
			TR5 = 1;
#endif
			Uart_Struct[UART5].tx_flag = 1;
			SBUF3_TX = Uart_Struct[UART5].tx_buf[Uart_Struct[UART5].tx_tail];
			Uart_Struct[UART5].tx_tail++;
			Uart_Struct[UART5].tx_tail &= SERIAL_COUNT;
		}
		break;
	default:
		break;
	}
	EA = 1;
}

void uart2_Isr() interrupt 4
{
	EA = 0;
	if (RI0)
	{
		RI0 = 0;
		Uart_Struct[UART2].rx_buf[Uart_Struct[UART2].rx_head] = SBUF0;
		Uart_Struct[UART2].rx_head++;
		Uart_Struct[UART2].rx_head &= SERIAL_COUNT;
	}
	if (TI0)
	{
		TI0 = 0;
		if (Uart_Struct[UART2].tx_head != Uart_Struct[UART2].tx_tail)
		{
			SBUF0 = Uart_Struct[UART2].tx_buf[Uart_Struct[UART2].tx_tail];
			Uart_Struct[UART2].tx_tail++;
			Uart_Struct[UART2].tx_tail &= SERIAL_COUNT;
		}
		else
		{
			Uart_Struct[UART2].tx_flag = 0;
#ifdef UART2485
			TR2 = 0;
#endif
		}
	}
	EA = 1;
}

void uart4t_Isr() interrupt 10
{
	EA = 0;
	SCON2T &= 0XFE;
	if (Uart_Struct[UART4].tx_head != Uart_Struct[UART4].tx_tail)
	{
		ACC = Uart_Struct[UART4].tx_buf[Uart_Struct[UART4].tx_tail]; // 奇校验
		P ? (SCON2T &= ~(0x01 << 5)) : (SCON2T |= (0x01 << 5));

		SBUF2_TX = Uart_Struct[UART4].tx_buf[Uart_Struct[UART4].tx_tail];
		Uart_Struct[UART4].tx_tail++;
		Uart_Struct[UART4].tx_tail &= SERIAL_COUNT;
	}
	else
	{
		Uart_Struct[UART4].tx_flag = 0;
#ifdef UART4485
		TR4 = 0;
#endif
	}
	EA = 1;
}

void uart4r_Isr() interrupt 11
{
	EA = 0;
	SCON2R &= 0XFE;
	ACC = SBUF2_RX;
	if ((!SCON2R & (0x01 << 5)) == P) // 8O1
	{
		Uart_Struct[UART4].rx_buf[Uart_Struct[UART4].rx_head] = SBUF2_RX;
		Uart_Struct[UART4].rx_head++;
		Uart_Struct[UART4].rx_head &= SERIAL_COUNT;
	}
	EA = 1;
}

void uart5t_Isr() interrupt 12
{
	EA = 0;
	SCON3T &= 0XFE;
	if (Uart_Struct[UART5].tx_head != Uart_Struct[UART5].tx_tail)
	{
		SBUF3_TX = Uart_Struct[UART5].tx_buf[Uart_Struct[UART5].tx_tail];
		Uart_Struct[UART5].tx_tail++;
		Uart_Struct[UART5].tx_tail &= SERIAL_COUNT;
	}
	else
	{
		Uart_Struct[UART5].tx_flag = 0;
#ifdef UART5485
		TR5 = 0;
#endif
	}
	EA = 1;
}

void uart5r_Isr() interrupt 13
{
	EA = 0;
	SCON3R &= 0XFE;
	Uart_Struct[UART5].rx_buf[Uart_Struct[UART5].rx_head] = SBUF3_RX;
	Uart_Struct[UART5].rx_head++;
	Uart_Struct[UART5].rx_head &= SERIAL_COUNT;
	EA = 1;
}

void uart3_Isr() interrupt 16
{
	EA = 0;
	if ((SCON1 & 0X01))
	{
		SCON1 = 0X90;
		SCON1 &= 0XFE;
		Uart_Struct[UART3].rx_buf[Uart_Struct[UART3].rx_head] = SBUF1;
		Uart_Struct[UART3].rx_head++;
		Uart_Struct[UART3].rx_head &= SERIAL_COUNT;
	}
	if ((SCON1 & 0X02))
	{
		SCON1 = 0X90;
		SCON1 &= 0XFD;
		if (Uart_Struct[UART3].tx_head != Uart_Struct[UART3].tx_tail)
		{
			SBUF1 = Uart_Struct[UART3].tx_buf[Uart_Struct[UART3].tx_tail];
			Uart_Struct[UART3].tx_tail++;
			Uart_Struct[UART3].tx_tail &= SERIAL_COUNT;
		}
		else
		{
			Uart_Struct[UART3].tx_flag = 0;
#ifdef UART3485
			TR3 = 0;
#endif
		}
	}
	EA = 1;
}