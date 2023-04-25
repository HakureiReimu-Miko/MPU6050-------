/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年4月30日
  功能描述   : 主函数，外设和参数初始化，主循环中主要功能函数入口。
  修改历史   :
  1.日    期   :
    作    者   :
    修改内容   :
******************************************************************************/

#include "t5los8051.h"
#include "sys.h"
#include "uart.h"
#include "config.h"
#include "control.h"
#include "modbus.h"
#include "iic.h"
#include "uart.h"
#include "rtc.h"
#include "modbus.h"
#include "vcnl4000.h"
#include "IR.h"
#include "timer2.h"

void Analyze_8283_5(void)
{
    u16 regAddr, regNumber;
    // u16 tmp;

    P_S_UART uart = &uart5;
    u8* pBuf = uart5RxBuf;
    void (*pSendFuc)(u8*, u16) = Uart5_Sendstring;

    if ((pBuf[0] != 0x5A) || (pBuf[1] != 0xA5))
    {
        // tmp = 72;
        // write_dgus_vp(0x7702, (u8 *)&tmp, 1);
        return;
    }
    if (pBuf[3] == 0x83)
    {
        if (uart->rxLen != 7)
            return;
        if (pBuf[2] != 0x04)
            return;

        regAddr = ((u16)pBuf[4] << 8) | (u16)pBuf[5];
        regNumber = (u16)pBuf[6];
        read_dgus_vp(regAddr, &pBuf[7], regNumber);
        pBuf[2] = pBuf[2] + (regNumber << 1);
        pSendFuc(pBuf, uart->rxLen + (regNumber << 1));
    }
    else if (pBuf[3] == 0x82)
    {
        regAddr = ((u16)pBuf[4] << 8) | (u16)pBuf[5];
        write_dgus_vp(regAddr, (u8*)&pBuf[6], (pBuf[2] - 3) / 2);
        pBuf[2] = 3;
        pBuf[4] = 0x4F;
        pBuf[5] = 0x4B;
        pSendFuc(pBuf, 6);
    }
}
#if APDS9900
void ScreenSaver_Update(void)
{
    u8 TouchDataFlag[2];
    u8 j[2];
    u16 i;
    APDS_Read_Byte(0x19, j);
    APDS_Read_Byte(0x18, j + 1);
    i = (j[0] << 8) | j[1];
    write_dgus_vp(0x8000, (u8*)&i, 1);

    read_dgus_vp(0x16, (u8*)TouchDataFlag, 1);
    if (TouchDataFlag[0] == 0x5A || i >= 0x3FF)
    {
        u16 brightness = 0x6400;
        sysTick3(20000);
        TouchDataFlag[0] = 0;
        write_dgus_vp(0x16, (u8*)TouchDataFlag, 1);
        write_dgus_vp(0x82, (u8*)brightness, 1);
    }
    if (SysTick3_Done == 1)
    {
        u16 brightness = 0xa00;
        write_dgus_vp(0x82, (u8*)brightness, 1);
    }
}
#endif
int main(void)
{
#if VCNL4200
    u16 result;
#endif
    INIT_CPU();
    PORT_Init();
    POWER_ON();
    T0_Init();
    T1_Init();
    timer2_Init();
	Timer2Cmd(ENABLE);
//    T2_Init();
#ifdef TICK_RTC
    Tick_RTC_Init(); //时间初始化，防止出现不正确的时间
#else
    init_rtc();
#endif
    Uart2_Init(115200, MODE_8N1);
    Uart5_Init(115200, MODE_8N1);
    // Modbus_Init();
    System_Parm_Init();
#if APDS9900
    APDS_9900_Init();
#endif
#if VCNL4200
    VCNL4200_Init();
#endif
    WDT_ON(); //打开开门狗
    sysTick2(300);
    // {
    //     u16 a0[2] = {0x0401, 0x4000};
    //     write_dgus_vp(0xA0, (u8 *)a0, 2); //蜂鸣器
    // }


	while (1)
	{
		IR_Display();

			RTC_Update();
			Get_Temperature();
        WDT_RST();
#ifdef TICK_RTC
        Tick_Time_Update(); //更新时间
#else
        RTC_Update();
#endif
        Parm_Set_Function(); //设置函数入口
        Control_Function();  //数据采集和逻辑控制函数入口

        // Modbus_Salve_Handler();
        // modbus_master_send();
        // modbus_data_parsing();
        data_upload();
        uartRun(&uart5, Analyze_8283_5);
        uartRun(&uart2, Analyze_8283);
#if APDS9900
        if (SysTick2_Done == 1)
        {
            ScreenSaver_Update();
            sysTick2(300);
        }
#endif
#if VCNL4200
        VCNL4200_MeasurementResult((u8*)&result);
        // write_dgus_vp(0x7001, (u8 *)&result, 1);
#endif
    }
}
