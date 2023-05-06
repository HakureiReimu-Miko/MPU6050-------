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

void main()
{
	T0_Init(); // 定时器0初始化
	init_rtc();
	rdtime();
	DEBUGINIT();
	UartInit(UART485, 9600);
	UartInit(UART2, 115200);
	EA = 1;
	sysParameterRead();
	modbusCenerate(); // 需放到参数读取后边
	// DEBUGINFO("modbusCenerate\n");
	historyAlarmRead();

	// DEBUGINFO("modbusNum = %d \n", (uint16_t)modbusNum);
	openScreenProtection();
	StartTimer(0, 200);

	DEBUGINFO("System start\n");
	// {
	// 	AlarmTypeDef temp;
	// 	AlarmTypeDef temp2;
	// 	temp.flag = 1;
	// 	write_dgus_vp(0x1C030,&temp,sizeof(AlarmTypeDef));
	// 	read_dgus_vp(0x1c030,&temp2,sizeof(AlarmTypeDef));
	// 	DEBUGINFO("temp2flag = %d\n",temp2.flag);
	// }

	while (1)
	{
		PageFunction();
		modbusTreat();
		if (GetTimeOutFlag(0)) // 200ms
		{
			rdtime();
			publicUI();
			alarmTreat();
			StartTimer(0, 200);
		}
		batteryManage();
	}
}
