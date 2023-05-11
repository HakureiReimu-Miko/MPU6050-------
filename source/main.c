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
#include "norflash.h"
#include "data.h"

#define ALARM_TIMER 2
void main()
{
	T0_Init(); // 定时器0初始化
	init_rtc();
	rdtime();
	DEBUGINIT();
	UartInit(UART485, 9600);
	UartInit(UART485RX, 9600);
	UartInit(UART2, 115200);
	EA = 1;
	sysParameterRead();
	modbusCenerate(); // 需放到参数读取后边
	// DEBUGINFO("modbusCenerate\n");
	historyAlarmRead();

	// DEBUGINFO("modbusNum = %d \n", (uint16_t)modbusNum);
	StartTimer(0, 200);
	StartTimer(ALARM_TIMER, 1000);
	StartTimer(3, 1000);

	DEBUGINFO("System start\n");

	while (1)
	{
		PageFunction();
		modbusTreat();
		Uart485RxTreat();
		if (GetTimeOutFlag(0)) // 200ms
		{
			static uint8_t count = 0;
			rdtime();
			publicUI();
			if (count++ % 5 == 0)
			{
				alarmSoundPlay();
			}
			StartTimer(0, 200);
		}
		if (GetTimeOutFlag(ALARM_TIMER))
		{
			static uint8_t alarmActiveFlag = 0;
			if (!alarmActiveFlag)
			{
				alarmActiveFlag = 1;
			}
			if (alarmActiveFlag)
			{
				alarmTreat();
			}
			StartTimer(ALARM_TIMER, 1000);
		}
		batteryManage();
		screenProtection();
	}
}
