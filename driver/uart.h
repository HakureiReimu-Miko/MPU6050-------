

/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : net.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#ifndef __UART_H__
#define __UART_H__
#include "sys.h"
#define SERIAL_SIZE 0x400
#define SERIAL_COUNT 0x3ff
#define UART2 0
#define UART3 1
#define UART4 2
#define UART5 3
#define UARTBUFNUM 4


typedef struct databuff 
{
	u8 tx_buf[SERIAL_SIZE];
	u8 rx_buf[SERIAL_SIZE];	 
	u16 tx_head,tx_tail;
	u16 rx_head,rx_tail;
	u8 tx_flag;
} DATABUFF;

extern DATABUFF Uart_Struct[];

void Uart_Send_Data(u8 str,u16 len,const u8 *buf);
void UartInit(u8 UartPort, u32 bdt);
#endif
