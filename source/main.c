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
	T0_Init(); // ��ʱ��0��ʼ��
	init_rtc();
	DEBUGINIT();
	UartInit(UART485, 9600);
	UartInit(UART2, 115200);
	EA = 1;
	sysParameterRead();
	modbusCenerate(); // ��ŵ�������ȡ���
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
