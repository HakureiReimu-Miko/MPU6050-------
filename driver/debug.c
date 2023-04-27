#include "debug.h"
#include "uart.h"

void initDebugUart(void)
{
    u16 tmp;

    tmp = 1024 - FOSC / 32 / 115200;
    MUX_SEL |= 0X20;
    SetPinOut(0, 6);
    SetPinIn(0, 7);
    P0 |= 0xC0;

    SCON1 = 0X90;
    SREL1H = (u8)(tmp >> 8);
    SREL1L = (u8)tmp;
    SBUF1 = '\r';
}

// char putchar (char c)  {

//   while ((SCON1&0X02)==0);
//   SCON1 &= 0XFC;
//   SCON1 &= 0XFC;
//   return (SBUF1 = c);
// }

char putchar(char c)
{
    Uart_Send_Data(UART2, 1, &c);
    return c;
}