#include "timer.h"
#include "ui.h"
#include "uart.h"
#include "modbus.h"
#include "umath.h"
#include "debug.h"
#include "http.h"
#include "permission.h"
#include <STRING.H>
#include "norflash.h"
#include "rtc.h"
#include "data.h"
#include "const.h"
#include "alarm.h"


void main()
{
	T0_Init(); // 定时器0初始化
	init_rtc();
	DEBUGINIT();
	UartInit(UART485, 9600);
	UartInit(UART2, 115200);
	EA = 1;
	sysParameterRead();
	modbusCenerate(); // 需放到参数读取后边
	historyAlarmRead();
	DEBUGINFO("modbusNum = %d \n", (uint16_t)modbusNum);
	openScreenProtection();
	StartTimer(0, 200);

	DEBUGINFO("System start\n");

	while (1)
	{
		PageFunction();
		modbusTreat();
		if (GetTimeOutFlag(0)) // 200ms
		{
			rdtime();
			publicUI();
			alarmRecord();
			StartTimer(0, 200);
			// DEBUGINFO("AC_channel_1_Uab = %d\n",synthesisCollection.AC_channel_1_Uab);
			// DEBUGINFO("chargeModule = %d\n",chargeModule[0].currLimitPercentage);
		}
		batteryManage();
	}
}
