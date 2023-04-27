#include "ui.h"
#include "modbus.h"
#include <STRING.H>
#include "permission.h"
#include "debug.h"
#include "norflash.h"
#include <STDLIB.H>
#include "rtc.h"
#include "const.h"
#include "data.h"
#include "umath.h"
#include "timer.h"
#include "alarm.h"

#define testDis 0
void testDisplayNum(u16 addr, u16 num)
{
#if testDis

#else
	write_dgus_vp(addr, (u8 *)&num, 1);
#endif
}

// 读取按键0x1000的值
u8 getTouch(void)
{
	u8 tmp;
	// EA = 0;
	ADR_H = 0x00;
	ADR_M = 0x08; // 0x1000
	ADR_L = 0x00;
	ADR_INC = 1;
	RAMMODE = 0xAF;
	while (!APP_ACK)
		;
	APP_EN = 1;
	while (APP_EN)
		;
	// str[0] = DATA3;
	tmp = DATA2;
	// EA = 1;
	RAMMODE = 0;
	return tmp;
}

// 清除按键0x1000的值
void clrTouch(void)
{
	// EA = 0;
	ADR_H = 0x00;
	ADR_M = 0x08; // 0x1000
	ADR_L = 0x00;
	ADR_INC = 1;
	RAMMODE = 0x8C;
	while (!APP_ACK)
		;
	DATA3 = 0;
	DATA2 = 0;
	APP_EN = 1;
	while (APP_EN)
		;
	RAMMODE = 0;
	// EA = 1;
}

void P0Func(void)
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8000:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(SYS_FUNCTION_TABLE_PAGE);
				}
				else
				{
					passwordLogin(SYS_FUNCTION_TABLE_PAGE);
				}
			}
			break;
		case 0x8001:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(BATTERY_MANAGER_PAGE);
				}
				else
				{
					passwordLogin(BATTERY_MANAGER_PAGE);
				}
			}
			break;
		case 0x8006:
			if (variable[0] == 1)
			{
				if (chargeModuleSet.moduleNum > 0)
				{
					Page_Change(CHARGE_DATA_PAGE);
				}
			}
			break;
		case 0x8007:
			if (variable[0] == 1)
			{
				if (batterySet.batteryType == XJ24)
				{
					if (batterySet.xj24.batteryModuleNum > 0)
					{
						Page_Change(BATTERY_DATA_PAGE);
					}
				}
				else if (batterySet.batteryType == XJ55)
				{
					if (batterySet.xj55.batteryModuleNum > 0)
					{
						Page_Change(BATTERY_DATA_PAGE);
					}
				}
			}
			break;
		case 0x8008:
			if (variable[0] == 1)
			{
				if (switchModuleSet.switchModule.switchNum > 0)
				{
					Page_Change(SWITCH_MODULE_DATA_PAGE);
				}
			}
			break;
		case 0x800A:
			if (variable[0] == 1)
			{
				if (UPS_INV_Set.UPS_Num > 0)
				{
					Page_Change(UPS_DATA_PAGE);
				}
			}
			break;
		case 0x800B:
			if (variable[0] == 1)
			{
				if (DC_ConverterSet.Num > 0)
				{
					Page_Change(DC_CONVERTER_DATA_PAGE);
				}
			}
			break;
		case 0x800C:
			if (variable[0] == 1)
			{
				if (insulationSet.DC.Num > 0)
				{
					Page_Change(INSULATION_DATA_PAGE);
				}
				else
				{
					Page_Change(NON_INSULATION_DATA_PAGE);
				}
			}
			break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}

	if (manager.loginFlag)
	{
		manager.loginFlag = 0;
	}
	if (admin.loginFlag)
	{
		admin.loginFlag = 0;
	}
}

void P2Func(void)
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8020:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(SYS_INFO_PAGE);
				}
				else
				{
					passwordLogin(SYS_INFO_PAGE);
				}
			}
			break;
		case 0x8021:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(SWITCH_MODULE_SET_PAGE);
				}
				else
				{
					passwordLogin(SWITCH_MODULE_SET_PAGE);
				}
			}
			break;
		case 0x8022:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(SERIES_BATTERY_INSPECTION_DEVICE_SET_PAGE);
				}
				else
				{
					passwordLogin(SERIES_BATTERY_INSPECTION_DEVICE_SET_PAGE);
				}
			}
			break;
		case 0x8023:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(INSULATION_SET_PAGE);
				}
				else
				{
					passwordLogin(INSULATION_SET_PAGE);
				}
			}
			break;
		case 0x8024:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(CHARGER_MODULE_SET_PAGE);
				}
				else
				{
					passwordLogin(CHARGER_MODULE_SET_PAGE);
				}
			}
			break;
		case 0x8025:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(DC_MODULE_SET_PAGE);
				}
				else
				{
					passwordLogin(DC_MODULE_SET_PAGE);
				}
			}
			break;
		case 0x8026:
			if (variable[0] == 1)
			{
				if (manager.loginFlag || admin.loginFlag)
				{
					Page_Change(UPS_INV_SET_PAGE);
				}
				else
				{
					passwordLogin(UPS_INV_SET_PAGE);
				}
			}
			break;
		case 0x8027:
			if (variable[0] == 1)
			{
				if (admin.loginFlag)
				{
					Page_Change(RECOVERY_PAGE);
				}
				else
				{
					passwordLogin(RECOVERY_PAGE);
				}
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P11Func(void)
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8041:
			if (variable[0] == 1)
			{
				read_dgus_vp(SYS_INFO_VP, (uint8_t *)&sysInfoSet, SYS_INFO_SIZE);
				dgusToNorFlash(SYS_INFO_NORFLASH_ADDR, SYS_INFO_VP, SYS_INFO_SIZE);
				modbusCenerate();
				// DEBUGINFO("save sysInfo");
			}
			break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);

		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P12Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8060:
			if (variable[0] == 1)
			{
				read_dgus_vp(AC_INFO_VP, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);
				dgusToNorFlash(AC_INFO_NORFLASH_ADDR, AC_INFO_VP, AC_INFO_SIZE);
				modbusCenerate();
			}
			break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);

		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P18Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8060:
			if (variable[0] == 1)
			{
				read_dgus_vp(DC_INFO_VP, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);
				dgusToNorFlash(DC_INFO_NORFLASH_ADDR, DC_INFO_VP, DC_INFO_SIZE);
				modbusCenerate();
			}
			break;
		case 0x90B1:
		{
			if (sysInfoSet.siliconChain == 0)
			{
				write_dgus_vp(0x90A1, (uint8_t *)&variable, 1);
			}
			else
			{
				write_dgus_vp(0x90A5, (uint8_t *)&variable, 1);
			}
		}
		break;
		case 0x90B2:
		{
			if (sysInfoSet.siliconChain == 0)
			{
				write_dgus_vp(0x90A2, (uint8_t *)&variable, 1);
			}
			else
			{
				write_dgus_vp(0x90A6, (uint8_t *)&variable, 1);
			}
		}
		break;
		case 0x90B3:
		{
			if (sysInfoSet.siliconChain == 0)
			{
				write_dgus_vp(0x90A3, (uint8_t *)&variable, 1);
			}
			else
			{
				write_dgus_vp(0x90A7, (uint8_t *)&variable, 1);
			}
		}
		break;
		case 0x90B4:
		{
			if (sysInfoSet.siliconChain == 0)
			{
				write_dgus_vp(0x90A4, (uint8_t *)&variable, 1);
			}
			else
			{
				write_dgus_vp(0x90A8, (uint8_t *)&variable, 1);
			}
		}
		break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)variable, variableChangedIndication.len);

		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}

	{
		if (sysInfoSet.sysVoltageGrade == SYSTEM_220V)
		{
			uint16_t sp; // 描述指针
			sp = 0x90A1;
			write_dgus_vp(0x5000, (uint8_t *)&sp, 1);
			sp = 0x90A2;
			write_dgus_vp(0x5020, (uint8_t *)&sp, 1);
			sp = 0x90A3;
			write_dgus_vp(0x5040, (uint8_t *)&sp, 1);
			sp = 0x90A4;
			write_dgus_vp(0x5060, (uint8_t *)&sp, 1);
		}
		else if (sysInfoSet.sysVoltageGrade == SYSTEM_110V)
		{
			uint16_t sp; // 描述指针
			sp = 0x90A5;
			write_dgus_vp(0x5000, (uint8_t *)&sp, 1);
			sp = 0x90A6;
			write_dgus_vp(0x5020, (uint8_t *)&sp, 1);
			sp = 0x90A7;
			write_dgus_vp(0x5040, (uint8_t *)&sp, 1);
			sp = 0x90A8;
			write_dgus_vp(0x5060, (uint8_t *)&sp, 1);
		}
	}
}

void P22Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8080:
			if (variable[0] == 1)
			{
				read_dgus_vp(BATTERY_INFO_VP, (uint8_t *)&batteryInfoSet, BATTERY_SET_SIZE);
				dgusToNorFlash(BATTERY_INFO_NORFLASH_ADDR, BATTERY_INFO_VP, BATTERY_SET_SIZE);
				modbusCenerate();
			}
			break;
		case 0x9120:
			if (variable[0] == 1)
			{
				chargeModeSwitch(FLOAT_CHARGE);
			}
			break;
		case 0x9121:
			if (variable[0] == 1)
			{
				chargeModeSwitch(EQUALIZE_CHARGE);
			}
			break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);

		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P26Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x9150:
			if (variable[0] == 1)
			{
				passwordLogin(26);
			}
			break;
		case 0x9152:
			if (variable[0] == 1)
			{
				manager.loginFlag = 0;
				admin.loginFlag = 0;
			}
			break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}

	{ // 当前登录用户显示
		int16_t userNum;
		if (admin.loginFlag)
		{
			userNum = 2;
		}
		else if (manager.loginFlag)
		{
			userNum = 1;
		}
		else
		{
			userNum = 0;
		}
		write_dgus_vp(0x9160, (uint8_t *)&userNum, 1);
	}
}

void P29Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x80C0:
			if (variable[0] == 1)
			{
				read_dgus_vp(SPECIAL_PARA_VP, (uint8_t *)&specialParaSet, SPECIAL_PARA_SIZE);
				dgusToNorFlash(SPECIAL_PARA_NORFLASH_ADDR, SPECIAL_PARA_VP, SPECIAL_PARA_SIZE);
				modbusCenerate();
			}
			break;
		// case 0x80C1:
		// 	if (variable[0] == 1)
		// 	{
		// 		manager.loginFlag = 0;
		// 		admin.loginFlag = 0;
		// 	}
		// 	break;
		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P33Func()
{
	static uint8_t timeModifyFlag = 0; // 时间修改标记
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x009D:
		case 0x009E:
		case 0x009F:
			timeModifyFlag = 1;
			break;

		case 0x80E0:
			if (variable[0] == 1)
			{
				if (timeModifyFlag == 1) // 由于时间会不断变化，故仅在修改时间的情况下保存时间，避免修改屏保时写入时间
				{
					uint8_t rtcSetData[6];
					read_dgus_vp(0x9D, rtcSetData, 4);
					Write_RTC(rtcSetData);
					timeModifyFlag = 0;
				}
				read_dgus_vp(BACKLIGHT_TIME_SET_VP, (uint8_t *)&backLightTime, 1);
				Nor_Flash_write(BACKLIGHT_TIME_NORFLASH_ADDR, (uint8_t *)&backLightTime, 2);
				write_dgus_vp(0x83, (uint8_t *)&backLightTime, 1);
			}
			break;

		default:
			break;
		}
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P35Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8100:
			if (variable[0] == 1)
			{
				read_dgus_vp(SWITCH_MODULE_SET_VP, (uint8_t *)&switchModuleSet, SWITCH_MODULE_SIZE);
				dgusToNorFlash(SWITCH_MODULE_NORFLASH_ADDR, SWITCH_MODULE_SET_VP, SWITCH_MODULE_SIZE);
				modbusCenerate();
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void P39Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8120:
			if (variable[0] == 1)
			{
				read_dgus_vp(BATTERY_SET_VP, (uint8_t *)&batterySet, BATTERY_SET_SIZE);
				dgusToNorFlash(BATTERY_SET_NORFLASH_ADDR, BATTERY_SET_VP, BATTERY_SET_SIZE);
				modbusCenerate();
			}
			break;
		case 0x9250:
			read_dgus_vp(BATTERY_SET_VP, (uint8_t *)&batterySet.batteryType, 1);
			break;
		case 0x9280:
		{
			if (batterySet.batteryType == XJ24)
			{
				if (0 <= variable[0] && variable[0] <= 5)
				{
					uint16_t batteryModuleNum = variable[0];
					uint16_t cellNum;
					write_dgus_vp(0x9260, (uint8_t *)&batteryModuleNum, 1);

					// 根据巡检数量设置电池节数上限
					read_dgus_vp(0x9261, (uint8_t *)&cellNum, 1);
					if (cellNum > batteryModuleNum * 24)
					{
						cellNum = batteryModuleNum * 24;
					}
					write_dgus_vp(0x9261, (uint8_t *)&cellNum, 1);
				}
			}
			else if (batterySet.batteryType == XJ55)
			{
				if (0 <= variable[0] && variable[0] <= 2)
				{
					uint16_t batteryModuleNum = variable[0];
					uint16_t cellNum;
					write_dgus_vp(0x9270, (uint8_t *)&batteryModuleNum, 1);

					// 根据巡检数量设置电池节数上限
					read_dgus_vp(0x9271, (uint8_t *)&cellNum, 1);
					if (cellNum > batteryModuleNum * 55)
					{
						cellNum = batteryModuleNum * 55;
					}
					write_dgus_vp(0x9271, (uint8_t *)&cellNum, 1);
				}
			}
		}
		break;
		case 0x9281:
		{
			if (batterySet.batteryType == XJ24)
			{
				uint16_t batteryModuleNum;
				uint16_t cellNum = variable[0];
				read_dgus_vp(0x9260, (uint8_t *)&batteryModuleNum, 1);
				if (cellNum > batteryModuleNum * 24)
				{
					cellNum = batteryModuleNum * 24;
				}
				write_dgus_vp(0x9261, (uint8_t *)&cellNum, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				uint16_t batteryModuleNum;
				uint16_t cellNum = variable[0];
				read_dgus_vp(0x9270, (uint8_t *)&batteryModuleNum, 1);
				if (cellNum > batteryModuleNum * 55)
				{
					cellNum = batteryModuleNum * 55;
				}
				write_dgus_vp(0x9271, (uint8_t *)&cellNum, 1);
			}
		}
		break;
		case 0x9282:
		{
			if (batterySet.batteryType == XJ24)
			{
				write_dgus_vp(0x9262, (uint8_t *)variable, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				write_dgus_vp(0x9272, (uint8_t *)variable, 1);
			}
		}
		break;
		case 0x9283:
		{
			if (batterySet.batteryType == XJ24)
			{
				write_dgus_vp(0x9263, (uint8_t *)variable, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				write_dgus_vp(0x9273, (uint8_t *)variable, 1);
			}
		}
		break;
		case 0x9284:
		{
			if (batterySet.batteryType == XJ24)
			{
				write_dgus_vp(0x9264, (uint8_t *)variable, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				write_dgus_vp(0x9274, (uint8_t *)variable, 1);
			}
		}
		break;
		case 0x9285:
		{
			if (batterySet.batteryType == XJ24)
			{
				write_dgus_vp(0x9265, (uint8_t *)variable, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				write_dgus_vp(0x9275, (uint8_t *)variable, 1);
			}
		}
		break;
		case 0x9286:
		{
			if (batterySet.batteryType == XJ24)
			{
				write_dgus_vp(0x9266, (uint8_t *)variable, 1);
			}
			else if (batterySet.batteryType == XJ55)
			{
				write_dgus_vp(0x9276, (uint8_t *)variable, 1);
			}
		}
		break;
		default:
			break;
		}
		// memset(variable, 0, 5);
		// write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
	{
		if (batterySet.batteryType == XJ24)
		{
			uint16_t sp; // 描述指针
			sp = 0x9260;
			write_dgus_vp(0x5200, (uint8_t *)&sp, 1);
			sp = 0x9261;
			write_dgus_vp(0x5220, (uint8_t *)&sp, 1);
			sp = 0x9262;
			write_dgus_vp(0x5240, (uint8_t *)&sp, 1);
			sp = 0x9263;
			write_dgus_vp(0x5260, (uint8_t *)&sp, 1);
			sp = 0x9264;
			write_dgus_vp(0x5280, (uint8_t *)&sp, 1);
			sp = 0x9265;
			write_dgus_vp(0x52A0, (uint8_t *)&sp, 1);
			sp = 0x9266;
			write_dgus_vp(0x52C0, (uint8_t *)&sp, 1);
		}
		else if (batterySet.batteryType == XJ55)
		{
			uint16_t sp; // 描述指针
			sp = 0x9270;
			write_dgus_vp(0x5200, (uint8_t *)&sp, 1);
			sp = 0x9271;
			write_dgus_vp(0x5220, (uint8_t *)&sp, 1);
			sp = 0x9272;
			write_dgus_vp(0x5240, (uint8_t *)&sp, 1);
			sp = 0x9273;
			write_dgus_vp(0x5260, (uint8_t *)&sp, 1);
			sp = 0x9274;
			write_dgus_vp(0x5280, (uint8_t *)&sp, 1);
			sp = 0x9275;
			write_dgus_vp(0x52A0, (uint8_t *)&sp, 1);
			sp = 0x9276;
			write_dgus_vp(0x52C0, (uint8_t *)&sp, 1);
		}
	}
}

void p42Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8140:
			if (variable[0] == 1)
			{
				read_dgus_vp(INSULATION_SET_VP, (uint8_t *)&insulationSet, INSULATION_SET_SIZE);
				dgusToNorFlash(INSULATION_SET_NORFLASH_ADDR, INSULATION_SET_VP, INSULATION_SET_SIZE);
				modbusCenerate();
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void p44Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8160:
			if (variable[0] == 1)
			{
				read_dgus_vp(CHARGE_MODULE_SET_VP, (uint8_t *)&chargeModuleSet, CHARGE_MODULE_SET_SIZE);
				dgusToNorFlash(CHARGE_MODULE_SET_NORFLASH_ADDR, CHARGE_MODULE_SET_VP, CHARGE_MODULE_SET_SIZE);
				modbusCenerate();
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void p48Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x8180:
			if (variable[0] == 1)
			{
				read_dgus_vp(DC_CONVERTER_SET_VP, (uint8_t *)&DC_ConverterSet, DC_CONVERTER_SET_SIZE);
				dgusToNorFlash(DC_CONVERTER_SET_NORFLASH_ADDR, DC_CONVERTER_SET_VP, DC_CONVERTER_SET_SIZE);
				modbusCenerate();
			}
			break;
		case 0x9360:
			if (variable[0] == 1)
			{
				if (DC_ConverterSet.Num > 0)
				{
					static MMODBUS DC_moduleOnOffCmd;
					static uint16_t DC_module_DI;
					dc_4850_Module[0].DI.onOffState = !dc_4850_Module[0].DI.onOffState;
					DC_module_DI = dc_4850_Module[0].DI.onOffState;
					DC_moduleOnOffCmd.SlaveAddr = 0xFE;
					DC_moduleOnOffCmd.mode = 0x00;
					DC_moduleOnOffCmd.flag = 0x00;
					DC_moduleOnOffCmd.Order = 0x06;
					DC_moduleOnOffCmd.Length = 1;
					DC_moduleOnOffCmd.reserved = 0x00;
					DC_moduleOnOffCmd.waitTime = 1000;
					DC_moduleOnOffCmd.VPaddr = 0;
					DC_moduleOnOffCmd.ModbusReg = 0x0005;
					DC_moduleOnOffCmd.databuff = &DC_module_DI;
					pushToEmergency(&DC_moduleOnOffCmd);
				}
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
	{
		uint16_t DC_Module_OnOffState = dc_4850_Module[0].DI.onOffState ? 1 : 0;
		write_dgus_vp(0x9361, (uint8_t *)&DC_Module_OnOffState, 1);
	}
}

void p50Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x81A0:
			if (variable[0] == 1)
			{
				read_dgus_vp(UPS_INV_SET_VP, (uint8_t *)&UPS_INV_Set, UPS_INV_SET_SIZE);
				dgusToNorFlash(UPS_INV_SET_NORFLASH_ADDR, UPS_INV_SET_VP, UPS_INV_SET_SIZE);
				modbusCenerate();
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void recovery(void)
{
	AC_InfoSet.AC_InputChannel = 1;
	AC_InfoSet.threePhaseAC_VolMax = 4500;
	AC_InfoSet.threePhaseAC_VolMin = 3230;
	AC_InfoSet.singlePhaseAC_VolMax = 2800;
	AC_InfoSet.singlePhaseAC_VolMin = 1870;
	Nor_Flash_write(AC_INFO_NORFLASH_ADDR, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);
	write_dgus_vp(AC_INFO_VP, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);

	DC_InfoSet.busVoltMax_220V = 2600;
	DC_InfoSet.busVoltMin_220V = 2000;
	DC_InfoSet.moduleVoltMax_220V = 2420;
	DC_InfoSet.moduleVoltMin_220V = 2000;
	DC_InfoSet.busVoltMax_110V = 1300;
	DC_InfoSet.busVoltMin_110V = 1000;
	DC_InfoSet.moduleVoltMax_110V = 1200;
	DC_InfoSet.moduleVoltMin_110V = 1000;
	DC_InfoSet.loadHallRatio = 1000;
	DC_InfoSet.batteryHallRatio = 1000;
	Nor_Flash_write(DC_INFO_NORFLASH_ADDR, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);
	write_dgus_vp(DC_INFO_VP, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);
}

void p52Func()
{
	VariableChangedIndicationTypeDef variableChangedIndication;
	read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
	if (variableChangedIndication.flag == 0x5A)
	{
		uint16_t variable[5];
		read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		switch (variableChangedIndication.addr)
		{
		case 0x9400:
			if (variable[0] == 1)
			{
				recovery();
			}
			break;
		case 0x9401:
			if (variable[0] == 1)
			{
				// 清楚记录
			}
			break;
		case 0x9402:
			if (variable[0] == 1)
			{
				// 重启触摸屏
				write_dgus_vp(0x04, (uint8_t *)"\x55\xAA\x5A\xA5", 2);
			}
			break;

		default:
			break;
		}
		memset(variable, 0, 5);
		write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
		variableChangedIndication.flag = 0;
		write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
	}
}

void p68Func(void)
{

	if (AC_InfoSet.singlePhaseThreePhaseSelection == THREE_PHASE)
	{
		uint16_t sp;
		sp = 0x94A0;
		write_dgus_vp(0x5400, (uint8_t *)&sp, 1);
		// sp = 0x94A0;
		// write_dgus_vp(0x5420, (uint8_t *)&sp, 1);
		sp = 0x94A1;
		write_dgus_vp(0x5440, (uint8_t *)&sp, 1);
		sp = 0x94A3;
		write_dgus_vp(0x5460, (uint8_t *)&sp, 1);
		// sp = 0x94A4;
		// write_dgus_vp(0x5480, (uint8_t *)&sp, 1);
		sp = 0x94A5;
		write_dgus_vp(0x54A0, (uint8_t *)&sp, 1);

		write_dgus_vp(0x94A0, (uint8_t *)&synthesisCollection.AC_channel_1_Uab, 6);
	}
	else if (AC_InfoSet.singlePhaseThreePhaseSelection == SINGLE_PHASE)
	{
		uint16_t sp;
		sp = 0xFFFF;
		write_dgus_vp(0x5400, (uint8_t *)&sp, 1);
		// sp = 0x94A0;
		// write_dgus_vp(0x5420, (uint8_t *)&sp, 1);
		sp = 0xFFFF;
		write_dgus_vp(0x5440, (uint8_t *)&sp, 1);
		sp = 0xFFFF;
		write_dgus_vp(0x5460, (uint8_t *)&sp, 1);
		// sp = 0x94A4;
		// write_dgus_vp(0x5480, (uint8_t *)&sp, 1);
		sp = 0xFFFF;
		write_dgus_vp(0x54A0, (uint8_t *)&sp, 1);

		write_dgus_vp(0x94A1, (uint8_t *)&synthesisCollection.AC_channel_1_Uab, 1);
		write_dgus_vp(0x94A4, (uint8_t *)&synthesisCollection.AC_channel_1_Uab, 1);
	}
}

void p70Func(void)
{
	{
		VariableChangedIndicationTypeDef variableChangedIndication;
		read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
		if (variableChangedIndication.flag == 0x5A)
		{
			uint16_t variable[5];
			read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			switch (variableChangedIndication.addr)
			{
			case 0x9530:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 1)
					{
						static MMODBUS chargeModuleOnOff = {0x01, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9540, 0x06, NULL};
						chargeModule[0].DI.onOffState = !chargeModule[0].DI.onOffState;
						write_dgus_vp(0x9540, (uint8_t *)&chargeModule[0].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9531:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 2)
					{
						static MMODBUS chargeModuleOnOff = {0x02, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9541, 0x06, NULL};
						chargeModule[1].DI.onOffState = !chargeModule[1].DI.onOffState;
						write_dgus_vp(0x9541, (uint8_t *)&chargeModule[1].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9532:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 3)
					{
						static MMODBUS chargeModuleOnOff = {0x03, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9542, 0x06, NULL};
						chargeModule[2].DI.onOffState = !chargeModule[2].DI.onOffState;
						write_dgus_vp(0x9542, (uint8_t *)&chargeModule[2].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9533:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 4)
					{
						static MMODBUS chargeModuleOnOff = {0x04, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9543, 0x06, NULL};
						chargeModule[3].DI.onOffState = !chargeModule[3].DI.onOffState;
						write_dgus_vp(0x9543, (uint8_t *)&chargeModule[3].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9534:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 5)
					{
						static MMODBUS chargeModuleOnOff = {0x05, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9544, 0x06, NULL};
						chargeModule[4].DI.onOffState = !chargeModule[4].DI.onOffState;
						write_dgus_vp(0x9544, (uint8_t *)&chargeModule[4].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9535:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 6)
					{
						static MMODBUS chargeModuleOnOff = {0x06, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9545, 0x06, NULL};
						chargeModule[5].DI.onOffState = !chargeModule[5].DI.onOffState;
						write_dgus_vp(0x9545, (uint8_t *)&chargeModule[5].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9536:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 7)
					{
						static MMODBUS chargeModuleOnOff = {0x07, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9546, 0x06, NULL};
						chargeModule[6].DI.onOffState = !chargeModule[6].DI.onOffState;
						write_dgus_vp(0x9546, (uint8_t *)&chargeModule[6].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;
			case 0x9537:
				if (variable[0] == 1)
				{
					if (chargeModuleSet.moduleNum >= 8)
					{
						static MMODBUS chargeModuleOnOff = {0x08, 0x00, 0x00, 0x05, 0x01, 0x00, 1000, 0x9547, 0x06, NULL};
						chargeModule[7].DI.onOffState = !chargeModule[7].DI.onOffState;
						write_dgus_vp(0x9547, (uint8_t *)&chargeModule[7].DI, 1);
						pushToEmergency(&chargeModuleOnOff);
					}
				}
				break;

			default:
				break;
			}
			memset(variable, 0, 5);
			write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			variableChangedIndication.flag = 0;
			write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
		}
	}
	{
		if (chargeModuleSet.moduleNum >= 1)
		{
			write_dgus_vp(0x9500, (uint8_t *)&chargeModule[0].outputVolt, 3);
			write_dgus_vp(0x9503, (uint8_t *)&chargeModule[0].DI, 1);
			// {
			// 	uint16_t temp = chargeModule.onOffState;
			// 	write_dgus_vp(0x9503, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9520, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9520, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 2)
		{
			write_dgus_vp(0x9504, (uint8_t *)&chargeModule[1].outputVolt, 3);
			write_dgus_vp(0x9507, (uint8_t *)&chargeModule[1].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[1].DI.onOffState;
			// 	write_dgus_vp(0x9507, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9521, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9521, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 3)
		{
			write_dgus_vp(0x9508, (uint8_t *)&chargeModule[2].outputVolt, 3);
			write_dgus_vp(0x950B, (uint8_t *)&chargeModule[2].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[2].DI.onOffState;
			// 	write_dgus_vp(0x950B, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9522, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9522, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 4)
		{
			write_dgus_vp(0x950C, (uint8_t *)&chargeModule[3].outputVolt, 3);
			write_dgus_vp(0x950F, (uint8_t *)&chargeModule[3].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[3].DI.onOffState;
			// 	write_dgus_vp(0x950F, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9523, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9523, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 5)
		{
			write_dgus_vp(0x9510, (uint8_t *)&chargeModule[4].outputVolt, 3);
			write_dgus_vp(0x9513, (uint8_t *)&chargeModule[4].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[4].DI.onOffState;
			// 	write_dgus_vp(0x9513, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9524, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9524, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 6)
		{
			write_dgus_vp(0x9514, (uint8_t *)&chargeModule[5].outputVolt, 3);
			write_dgus_vp(0x9517, (uint8_t *)&chargeModule[5].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[5].DI.onOffState;
			// 	write_dgus_vp(0x9517, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9525, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9525, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 7)
		{
			write_dgus_vp(0x9518, (uint8_t *)&chargeModule[6].outputVolt, 3);
			write_dgus_vp(0x951B, (uint8_t *)&chargeModule[6].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[6].DI.onOffState;
			// 	write_dgus_vp(0x951B, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9526, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9526, (uint8_t *)&temp, 1);
		}
		if (chargeModuleSet.moduleNum >= 8)
		{
			write_dgus_vp(0x951C, (uint8_t *)&chargeModule[7].outputVolt, 3);
			write_dgus_vp(0x951F, (uint8_t *)&chargeModule[7].DI, 1);
			// {
			// 	uint16_t temp = chargeModule[7].DI.onOffState;
			// 	write_dgus_vp(0x951F, (uint8_t *)&temp, 1);
			// }
			{
				uint16_t temp = 0;
				write_dgus_vp(0x9527, (uint8_t *)&temp, 1);
			}
		}
		else
		{
			uint16_t temp = 1;
			write_dgus_vp(0x9527, (uint8_t *)&temp, 1);
		}
	}
}

void p72Func(void)
{
	uint16_t swState;
	uint16_t i;
	for (i = 0; i < switchModuleSet.synthesisCollection.alarmChannelNum; i++)
	{
		if (((synthesisCollection.sw_01to16 >> i) & 0x0001) == switchModuleSet.synthesisCollection.switchAccessMode)
		{
			swState = FAULT;
		}
		else
		{
			swState = NORMAL;
		}
		write_dgus_vp(0x95A0 + i, (uint8_t *)&swState, 1);
	}
	for (; i < 12; i++)
	{
		if ((synthesisCollection.sw_01to16 >> i) & 0x0001)
		{
			swState = CLOSING;
		}
		else
		{
			swState = OPENING;
		}
		write_dgus_vp(0x95A0 + i, (uint8_t *)&swState, 1);
	}
	for (; i < 13; i++)
	{
		if (((synthesisCollection.sw_01to16 >> i) & 0x0001) == switchModuleSet.synthesisCollection.switchAccessMode)
		{
			swState = FAULT;
		}
		else
		{
			swState = NORMAL;
		}
		write_dgus_vp(0x95A0 + i, (uint8_t *)&swState, 1);
	}
	for (; i < 15; i++)
	{
		if ((synthesisCollection.sw_01to16 >> i) & 0x0001)
		{
			swState = CLOSING;
		}
		else
		{
			swState = OPENING;
		}
		write_dgus_vp(0x95A0 + i, (uint8_t *)&swState, 1);
	}
}

void p75Func(void)
{
	if (switchModuleSet.synthesisCollection.remoteControlType == ARD)
	{
		uint16_t i;
		for (i = 0; i < 16; i++)
		{
			uint16_t swState;
			swState = (remoteControlModule_ARD[0].sw1to16 >> i) & 0x01;
			write_dgus_vp(0x9750 + i, (uint8_t *)&swState, 1);
		}
	}
	else if (switchModuleSet.synthesisCollection.remoteControlType == TH)
	{
		uint16_t i;
		for (i = 0; i < 16; i++)
		{
			uint16_t swState;
			swState = (remoteControlModule_TH[0].sw1to16 >> i) & 0x01;
			write_dgus_vp(0x9750 + i, (uint8_t *)&swState, 1);
		}
	}
	{
		VariableChangedIndicationTypeDef variableChangedIndication;
		read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
		if (variableChangedIndication.flag == 0x5A)
		{
			uint16_t variable[5];
			read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			switch (variableChangedIndication.addr)
			{
			case 0x9760:
			case 0x9761:
			case 0x9762:
			case 0x9763:
			case 0x9764:
			case 0x9765:
			case 0x9766:
			case 0x9767:
			case 0x9768:
			case 0x9769:
			case 0x976A:
			case 0x976B:
			case 0x976C:
			case 0x976D:
			case 0x976E:
			case 0x976F:
			{
				if (switchModuleSet.synthesisCollection.remoteControlType == ARD)
				{
					// 根据销售描述，开关为0x02寄存器，感觉不大对，可能需要重构
					if (variable[0] == 0)
					{
						static MMODBUS rtuSwOff[16];
						uint16_t offset = (variableChangedIndication.addr - 0x9760);
						remoteControlModule_ARD[0].sw1to16 &= ~(0x0001 << offset);
						rtuSwOff[offset].SlaveAddr = 0xD0;
						rtuSwOff[offset].mode = 0x00;
						rtuSwOff[offset].flag = 0x00;
						rtuSwOff[offset].Order = 0x06;
						rtuSwOff[offset].Length = 0x01;
						rtuSwOff[offset].reserved = 0x00;
						rtuSwOff[offset].waitTime = 1000;
						rtuSwOff[offset].VPaddr = 0x9780 + offset;
						rtuSwOff[offset].ModbusReg = 0x0002;
						rtuSwOff[offset].databuff = NULL;
						write_dgus_vp(0x9780 + offset, (uint8_t *)&remoteControlModule_ARD[0].sw1to16, 1);
						pushToEmergency(rtuSwOff + offset);
					}
					else if (variable[0] == 1)
					{
						static MMODBUS rtuSwOn[16];
						uint16_t offset = (variableChangedIndication.addr - 0x9760);
						remoteControlModule_ARD[0].sw1to16 |= 0x0001 << offset;
						rtuSwOn[offset].SlaveAddr = 0xD0;
						rtuSwOn[offset].mode = 0x00;
						rtuSwOn[offset].flag = 0x00;
						rtuSwOn[offset].Order = 0x06;
						rtuSwOn[offset].Length = 0x01;
						rtuSwOn[offset].reserved = 0x00;
						rtuSwOn[offset].waitTime = 1000;
						rtuSwOn[offset].VPaddr = 0x9780 + offset;
						rtuSwOn[offset].ModbusReg = 0x0002;
						rtuSwOn[offset].databuff = NULL;
						write_dgus_vp(0x9780 + offset, (uint8_t *)&remoteControlModule_ARD[0].sw1to16, 1);
						pushToEmergency(rtuSwOn + offset);
					}
				}
				else if (switchModuleSet.synthesisCollection.remoteControlType == TH)
				{
					if (variable[0] == 0)
					{
						static MMODBUS rtuSwOff[16];
						uint16_t OffVal = 0x0000;
						uint16_t offset = (variableChangedIndication.addr - 0x9760);
						rtuSwOff[offset].SlaveAddr = 0xD0;
						rtuSwOff[offset].mode = 0x00;
						rtuSwOff[offset].flag = 0x00;
						rtuSwOff[offset].Order = 0x06;
						rtuSwOff[offset].Length = 0x01;
						rtuSwOff[offset].reserved = 0x00;
						rtuSwOff[offset].waitTime = 1000;
						rtuSwOff[offset].VPaddr = 0x9780 + offset;
						rtuSwOff[offset].ModbusReg = 0x0002 + offset;
						rtuSwOff[offset].databuff = NULL;
						write_dgus_vp(0x9780 + offset, (uint8_t *)&OffVal, 1);
						pushToEmergency(rtuSwOff + offset);
					}
					else if (variable[0] == 1)
					{
						static MMODBUS rtuSwOn[16];
						uint16_t OnVal = 0xFFFF;
						uint16_t offset = (variableChangedIndication.addr - 0x9760);
						rtuSwOn[offset].SlaveAddr = 0xD0;
						rtuSwOn[offset].mode = 0x00;
						rtuSwOn[offset].flag = 0x00;
						rtuSwOn[offset].Order = 0x06;
						rtuSwOn[offset].Length = 0x01;
						rtuSwOn[offset].reserved = 0x00;
						rtuSwOn[offset].waitTime = 1000;
						rtuSwOn[offset].VPaddr = 0x9780 + offset;
						rtuSwOn[offset].ModbusReg = 0x0002 + offset;
						rtuSwOn[offset].databuff = NULL;
						write_dgus_vp(0x9780 + offset, (uint8_t *)&OnVal, 1);
						pushToEmergency(rtuSwOn + offset);
					}
				}
			}
			break;

			default:
				break;
			}
			memset(variable, 0, 5);
			write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			variableChangedIndication.flag = 0;
			write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
		}
	}
}

void p76Func(void)
{
	uint16_t swState;
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		if (i < switchModuleSet.switchModule.sw_1_AlarmChannelNum)
		{
			if ((switchModule[0].sw_u16[i / 16] >> (i % 16) & 0x0001) == switchModuleSet.switchModule.sw_1_AccessMode)
			{
				swState = FAULT;
			}
			else
			{
				swState = NORMAL;
			}
			write_dgus_vp(0x9600 + i, (uint8_t *)&swState, 1);
		}
		else
		{
			if (switchModule[0].sw_u16[i / 16] >> (i % 16) & 0x0001)
			{
				swState = CLOSING;
			}
			else
			{
				swState = OPENING;
			}
			write_dgus_vp(0x9600 + i, (uint8_t *)&swState, 1);
		}
	}
}
void p77Func(void)
{
	uint16_t swState;
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		if (i < switchModuleSet.switchModule.sw_2_AlarmChannelNum)
		{

			if ((switchModule[1].sw_u16[i / 16] >> (i % 16) & 0x0001) == switchModuleSet.switchModule.sw_2_AccessMode)
			{
				swState = FAULT;
			}
			else
			{
				swState = NORMAL;
			}
			write_dgus_vp(0x9650 + i, (uint8_t *)&swState, 1);
		}
		else
		{
			if (switchModule[1].sw_u16[i / 16] >> (i % 16) & 0x0001)
			{
				swState = CLOSING;
			}
			else
			{
				swState = OPENING;
			}
			write_dgus_vp(0x9650 + i, (uint8_t *)&swState, 1);
		}
	}
}
void p78Func(void)
{
	uint16_t swState;
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		if (i < switchModuleSet.switchModule.sw_3_AlarmChannelNum)
		{

			if ((switchModule[2].sw_u16[i / 16] >> (i % 16) & 0x0001) == switchModuleSet.switchModule.sw_3_AccessMode)
			{
				swState = FAULT;
			}
			else
			{
				swState = NORMAL;
			}
			write_dgus_vp(0x96A0 + i, (uint8_t *)&swState, 1);
		}
		else
		{
			if (switchModule[2].sw_u16[i / 16] >> (i % 16) & 0x0001)
			{
				swState = CLOSING;
			}
			else
			{
				swState = OPENING;
			}
			write_dgus_vp(0x96A0 + i, (uint8_t *)&swState, 1);
		}
	}
}
void p79Func(void)
{
	uint16_t swState;
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		if (i < switchModuleSet.switchModule.sw_4_AlarmChannelNum)
		{

			if ((switchModule[3].sw_u16[i / 16] >> (i % 16) & 0x0001) == switchModuleSet.switchModule.sw_4_AccessMode)
			{
				swState = FAULT;
			}
			else
			{
				swState = NORMAL;
			}
			write_dgus_vp(0x9700 + i, (uint8_t *)&swState, 1);
		}
		else
		{
			if (switchModule[3].sw_u16[i / 16] >> (i % 16) & 0x0001)
			{
				swState = CLOSING;
			}
			else
			{
				swState = OPENING;
			}
			write_dgus_vp(0x9700 + i, (uint8_t *)&swState, 1);
		}
	}
}

void p80Func(void)
{
	write_dgus_vp(0x9800, (uint8_t *)&synthesisCollection.closeBusVolt, 1);

	write_dgus_vp(0x9801, (uint8_t *)&synthesisCollection.controlBusVolt, 1);

	write_dgus_vp(0x9802, (uint8_t *)&synthesisCollection.controlBusCurr, 1);

	write_dgus_vp(0x9803, (uint8_t *)&synthesisCollection.batteryVolt, 1);

	write_dgus_vp(0x9804, (uint8_t *)&synthesisCollection.batteryCurr, 1);
}

void p82Func(void)
{
	if (UPS_INV_Set.UPS_Num >= 1)
	{
		write_dgus_vp(0x9850, (uint8_t *)&ups[0].remoteMeasurement, 11);
		write_dgus_vp(0x985B, (uint8_t *)&ups[0].remoteSignal, 1);
	}
	if (UPS_INV_Set.UPS_Num >= 2)
	{
		write_dgus_vp(0x9860, (uint8_t *)&ups[1].remoteMeasurement, 11);
		write_dgus_vp(0x986B, (uint8_t *)&ups[1].remoteSignal, 1);
	}
	{
		uint16_t ups2Hide;
		ups2Hide = (UPS_INV_Set.UPS_Num < 2) ? 1 : 0;
		write_dgus_vp(0x9880, (uint8_t *)&ups2Hide, 1);
	}
}

void p85Func(void)
{
	if (UPS_INV_Set.INV_Num >= 1)
	{
		write_dgus_vp(0x98A0, (uint8_t *)&inv[0], 21);
	}
	if (UPS_INV_Set.INV_Num >= 2)
	{
		write_dgus_vp(0x98C0, (uint8_t *)&inv[1], 21);
	}
	{
		uint16_t inv2Hide;
		inv2Hide = (UPS_INV_Set.INV_Num < 2) ? 1 : 0;
		write_dgus_vp(0x98E0, (uint8_t *)&inv2Hide, 1);
	}
}

void p86Func(void)
{
	uint16_t i;
	for (i = 0; i < DC_ConverterSet.Num; i++)
	{
		uint16_t faultSta;
		uint16_t protectSta;
		write_dgus_vp(0x9900 + i, (uint8_t *)&dc_4850_Module[i].outputVolt, 1);
		write_dgus_vp(0x9910 + i, (uint8_t *)&dc_4850_Module[i].outputCurr, 1);
		faultSta = dc_4850_Module[i].DI.faultState;
		write_dgus_vp(0x9920 + i, (uint8_t *)&faultSta, 1);
		protectSta = dc_4850_Module[i].DI.protectState;
		write_dgus_vp(0x9930 + i, (uint8_t *)&protectSta, 1);
	}
	for (i = 0; i < DC4850MODULE_MAX; i++)
	{
		uint16_t DC_hide;
		DC_hide = (i >= DC_ConverterSet.Num) ? 1 : 0;
		write_dgus_vp(0x9940 + i, (uint8_t *)&DC_hide, 1);
	}
}

void p87Func(void)
{
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		int16_t insulationRes;
		insulationRes = branchInsulation[0].res[i];
		insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // 如果为合母路数，数值加100.0
		write_dgus_vp(0x9950 + i, (uint8_t *)&insulationRes, 1);
	}

	if (sysInfoSet.siliconChain == 1)
	{
		int16_t temp;
		temp = branchInsulation[0].closeBusToGroundVolt - branchInsulation[0].busToGroundVolt;
		write_dgus_vp(0x9970, (uint8_t *)&temp, 1);

		temp = branchInsulation[0].controlBusToGroundVolt - branchInsulation[0].busToGroundVolt;
		write_dgus_vp(0x9971, (uint8_t *)&temp, 1);
	}
	else if (sysInfoSet.siliconChain == 0)
	{
		int32_t batteryVoltSum;	  // 电池巡检电压总和
		int32_t chargeModuleVolt; // 充电模块电压
		int16_t displayVolt;
		uint16_t i;

		// if (batterySet.batteryType == XJ24)
		// {
		// 	batteryVoltSum = 0;
		// 	for (i = 0; i < batterySet.xj24.cellNum; i++)
		// 	{
		// 		batteryVoltSum += battery_xj24[i / 24].volt[i % 24];
		// 	}
		// }
		// else if (batterySet.batteryType == XJ55)
		// {
		// 	batteryVoltSum = 0;
		// 	for (i = 0; i < batterySet.xj55.cellNum; i++)
		// 	{
		// 		batteryVoltSum += battery_xj55[i / 55].volt[i % 55];
		// 	}
		// }
		read_dgus_vp(0xb020, (uint8_t *)&batteryVoltSum, 1);

		chargeModuleVolt = 0;
		for (i = 0; i < chargeModuleSet.moduleNum; i++)
		{
			if (chargeModuleVolt < chargeModule[i].outputVolt)
			{
				chargeModuleVolt = chargeModule[i].outputVolt;
			}
		}

		displayVolt = MAX(batteryVoltSum, chargeModuleVolt) - branchInsulation[0].controlBusToGroundVolt;
		write_dgus_vp(0x9970, (uint8_t *)&displayVolt, 1);
		write_dgus_vp(0x9971, (uint8_t *)&displayVolt, 1);
	}
	write_dgus_vp(0x9972, (uint8_t *)&synthesisCollection.busToGroundVolt, 1);
}

void p88Func(void)
{
	uint16_t i;
	for (i = 0; i < 30; i++)
	{
		uint16_t insulationRes;
		insulationRes = branchInsulation[1].res[i];
		insulationRes += i < insulationSet.DC.closeBus_2_ChannelNum ? 1000 : 0; // 如果为合母路数，数值加100.0
		write_dgus_vp(0x99A0 + i, (uint8_t *)&insulationRes, 1);
	}

	if (sysInfoSet.siliconChain == 1)
	{
		int16_t temp;
		temp = branchInsulation[0].closeBusToGroundVolt - branchInsulation[0].closeBusToGroundVolt;
		write_dgus_vp(0x9970, (uint8_t *)&temp, 1);

		temp = branchInsulation[0].controlBusToGroundVolt - branchInsulation[0].controlBusToGroundVolt;
		write_dgus_vp(0x9971, (uint8_t *)&temp, 1);
	}
	else if (sysInfoSet.siliconChain == 0)
	{
		int16_t batteryVoltSum;	  // 电池巡检电压总和
		int16_t chargeModuleVolt; // 充电模块电压
		int16_t displayVolt;
		uint16_t i;
		// if (batterySet.batteryType == XJ24)
		// {
		// 	batteryVoltSum = 0;
		// 	for (i = 0; i < batterySet.xj24.cellNum; i++)
		// 	{
		// 		batteryVoltSum += battery_xj24[i / 24].volt[i % 24];
		// 	}
		// }
		// else if (batterySet.batteryType == XJ55)
		// {
		// 	batteryVoltSum = 0;
		// 	for (i = 0; i < batterySet.xj55.cellNum; i++)
		// 	{
		// 		batteryVoltSum += battery_xj55[i / 55].volt[i % 55];
		// 	}
		// }
		read_dgus_vp(0xb020, (uint8_t *)&batteryVoltSum, 1);

		chargeModuleVolt = 0;
		for (i = 0; i < chargeModuleSet.moduleNum; i++)
		{
			if (chargeModuleVolt < chargeModule[i].outputVolt)
			{
				chargeModuleVolt = chargeModule[i].outputVolt;
			}
		}

		displayVolt = MAX(batteryVoltSum, chargeModuleVolt) - branchInsulation[0].controlBusToGroundVolt;
		write_dgus_vp(0x9970, (uint8_t *)&displayVolt, 1);
		write_dgus_vp(0x9971, (uint8_t *)&displayVolt, 1);
	}
	write_dgus_vp(0x9972, (uint8_t *)&synthesisCollection.busToGroundVolt, 1);
}

void p94Func(void)
{
	uint16_t i;
	if (batterySet.batteryType == XJ24)
	{
		for (i = 0; i < 60; i++)
		{
			if (i < batterySet.xj24.cellNum)
			{
				write_dgus_vp(0xA000 + i, (uint8_t *)&battery_xj24[i / 24].volt[i % 24], 1);
			}
			else
			{
				uint16_t temp = 0;
				write_dgus_vp(0xA000 + i, (uint8_t *)&temp, 1);
			}
		}
	}
	else if (batterySet.batteryType == XJ55)
	{
		for (i = 0; i < 60; i++)
		{
			if (i < batterySet.xj24.cellNum)
			{
				write_dgus_vp(0xA000 + i, (uint8_t *)&battery_xj55[i / 55].volt[i % 55], 1);
			}
			else
			{
				uint16_t temp = 0;
				write_dgus_vp(0xA000 + i, (uint8_t *)&temp, 1);
			}
		}
	}
}

void p113Func(void)
{
	uint16_t i;
	if (batterySet.batteryType == XJ24)
	{
		for (i = 60; i < 120; i++)
		{
			if (i < batterySet.xj24.cellNum)
			{
				write_dgus_vp(0xA050 + (i - 60), (uint8_t *)&battery_xj24[i / 24].volt[i % 24], 1);
			}
			else
			{
				uint16_t temp = 0;
				write_dgus_vp(0xA050 + (i - 60), (uint8_t *)&temp, 1);
			}
		}
	}
	else if (batterySet.batteryType == XJ55)
	{
		for (i = 60; i < 120; i++)
		{
			if (i < batterySet.xj24.cellNum)
			{
				write_dgus_vp(0xA050 + (i - 60), (uint8_t *)&battery_xj55[i / 55].volt[i % 55], 1);
			}
			else
			{
				uint16_t temp = 0;
				write_dgus_vp(0xA050 + (i - 60), (uint8_t *)&temp, 1);
			}
		}
	}
}

void p114Func()
{
	if (switchModuleSet.synthesisCollection.remoteControlType == ARD)
	{
		uint16_t i;
		for (i = 0; i < 16; i++)
		{
			uint16_t swState;
			swState = (remoteControlModule_ARD[1].sw1to16 >> i) & 0x01;
			write_dgus_vp(0x97A0 + i, (uint8_t *)&swState, 1);
		}
	}
	else if (switchModuleSet.synthesisCollection.remoteControlType == TH)
	{
		uint16_t i;
		for (i = 0; i < 16; i++)
		{
			uint16_t swState;
			swState = (remoteControlModule_TH[1].sw1to16 >> i) & 0x01;
			write_dgus_vp(0x97A0 + i, (uint8_t *)&swState, 1);
		}
	}
	{
		VariableChangedIndicationTypeDef variableChangedIndication;
		read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
		if (variableChangedIndication.flag == 0x5A)
		{
			uint16_t variable[5];
			read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			switch (variableChangedIndication.addr)
			{
			case 0x97B0:
			case 0x97B1:
			case 0x97B2:
			case 0x97B3:
			case 0x97B4:
			case 0x97B5:
			case 0x97B6:
			case 0x97B7:
			case 0x97B8:
			case 0x97B9:
			case 0x97BA:
			case 0x97BB:
			case 0x97BC:
			case 0x97BD:
			case 0x97BE:
			case 0x97BF:
			{
				if (switchModuleSet.synthesisCollection.remoteControlType == ARD)
				{
					// 根据销售描述，开关为0x02寄存器，感觉不大对，可能需要重构
					if (variable[0] == 0)
					{
						static MMODBUS rtuSwOff[16];
						uint16_t offset = (variableChangedIndication.addr - 0x97B0);
						remoteControlModule_ARD[0].sw1to16 &= ~(0x0001 << offset);
						rtuSwOff[offset].SlaveAddr = 0xD1;
						rtuSwOff[offset].mode = 0x00;
						rtuSwOff[offset].flag = 0x00;
						rtuSwOff[offset].Order = 0x06;
						rtuSwOff[offset].Length = 0x01;
						rtuSwOff[offset].reserved = 0x00;
						rtuSwOff[offset].waitTime = 1000;
						rtuSwOff[offset].VPaddr = 0x97D0 + offset;
						rtuSwOff[offset].ModbusReg = 0x0002;
						rtuSwOff[offset].databuff = NULL;
						write_dgus_vp(0x97D0 + offset, (uint8_t *)&remoteControlModule_ARD[1].sw1to16, 1);
						pushToEmergency(rtuSwOff + offset);
					}
					else if (variable[0] == 1)
					{
						static MMODBUS rtuSwOn[16];
						uint16_t offset = (variableChangedIndication.addr - 0x97B0);
						remoteControlModule_ARD[0].sw1to16 |= 0x0001 << offset;
						rtuSwOn[offset].SlaveAddr = 0xD1;
						rtuSwOn[offset].mode = 0x00;
						rtuSwOn[offset].flag = 0x00;
						rtuSwOn[offset].Order = 0x06;
						rtuSwOn[offset].Length = 0x01;
						rtuSwOn[offset].reserved = 0x00;
						rtuSwOn[offset].waitTime = 1000;
						rtuSwOn[offset].VPaddr = 0x97D0 + offset;
						rtuSwOn[offset].ModbusReg = 0x0002;
						rtuSwOn[offset].databuff = NULL;
						write_dgus_vp(0x97D0 + offset, (uint8_t *)&remoteControlModule_ARD[1].sw1to16, 1);
						pushToEmergency(rtuSwOn + offset);
					}
				}
				else if (switchModuleSet.synthesisCollection.remoteControlType == TH)
				{
					if (variable[0] == 0)
					{
						static MMODBUS rtuSwOff[16];
						uint16_t OffVal = 0x0000;
						uint16_t offset = (variableChangedIndication.addr - 0x97B0);
						rtuSwOff[offset].SlaveAddr = 0xD1;
						rtuSwOff[offset].mode = 0x00;
						rtuSwOff[offset].flag = 0x00;
						rtuSwOff[offset].Order = 0x06;
						rtuSwOff[offset].Length = 0x01;
						rtuSwOff[offset].reserved = 0x00;
						rtuSwOff[offset].waitTime = 1000;
						rtuSwOff[offset].VPaddr = 0x97D0 + offset;
						rtuSwOff[offset].ModbusReg = 0x0002 + offset;
						rtuSwOff[offset].databuff = NULL;
						write_dgus_vp(0x97D0 + offset, (uint8_t *)&OffVal, 1);
						pushToEmergency(rtuSwOff + offset);
					}
					else if (variable[0] == 1)
					{
						static MMODBUS rtuSwOn[16];
						uint16_t OnVal = 0xFFFF;
						uint16_t offset = (variableChangedIndication.addr - 0x97B0);
						rtuSwOn[offset].SlaveAddr = 0xD1;
						rtuSwOn[offset].mode = 0x00;
						rtuSwOn[offset].flag = 0x00;
						rtuSwOn[offset].Order = 0x06;
						rtuSwOn[offset].Length = 0x01;
						rtuSwOn[offset].reserved = 0x00;
						rtuSwOn[offset].waitTime = 1000;
						rtuSwOn[offset].VPaddr = 0x97D0 + offset;
						rtuSwOn[offset].ModbusReg = 0x0002 + offset;
						rtuSwOn[offset].databuff = NULL;
						write_dgus_vp(0x97D0 + offset, (uint8_t *)&OnVal, 1);
						pushToEmergency(rtuSwOn + offset);
					}
				}
			}
			break;

			default:
				break;
			}
			memset(variable, 0, 5);
			write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
			variableChangedIndication.flag = 0;
			write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
		}
	}
}

code void (*PagePointer[])(void) = {
	P0Func,
	NULL,
	P2Func,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	P11Func,
	P12Func,
	NULL,				  // 13
	NULL,				  // 14
	NULL,				  // 15
	NULL,				  // 16
	NULL,				  // 17
	P18Func,			  // 18
	NULL,				  // 19
	NULL,				  // 20
	NULL,				  // 21
	P22Func,			  // 22
	NULL,				  // 23
	NULL,				  // 24
	NULL,				  // 25
	P26Func,			  // 26
	NULL,				  // 27
	NULL,				  // 28
	P29Func,			  // 29
	NULL,				  // 30
	NULL,				  // 31
	NULL,				  // 32
	P33Func,			  // 33
	NULL,				  // 34
	P35Func,			  // 35
	NULL,				  // 36
	NULL,				  // 37
	NULL,				  // 38
	P39Func,			  // 39
	NULL,				  // 40
	NULL,				  // 41
	p42Func,			  // 42
	NULL,				  // 43
	p44Func,			  // 44
	NULL,				  // 45
	NULL,				  // 46
	NULL,				  // 47
	p48Func,			  // 48
	NULL,				  // 49
	p50Func,			  // 50
	NULL,				  // 51
	p52Func,			  // 52
	NULL,				  // 53
	NULL,				  // 54
	NULL,				  // 55
	NULL,				  // 56
	NULL,				  // 57
	NULL,				  // 58
	realTimeAlarmDisplay, // 59
	NULL,				  // 60
	NULL,				  // 61
	NULL,				  // 62
	NULL,				  // 63
	NULL,				  // 64
	NULL,				  // 65
	NULL,				  // 66
	NULL,				  // 67
	p68Func,			  // 68
	NULL,				  // 69
	p70Func,			  // 70
	NULL,				  // 71
	p72Func,			  // 72
	NULL,				  // 73
	NULL,				  // 74
	p75Func,			  // 75
	p76Func,			  // 76
	p77Func,			  // 77
	p78Func,			  // 78
	p79Func,			  // 79
	p80Func,			  // 80
	NULL,				  // 81
	p82Func,			  // 82
	NULL,				  // 83
	NULL,				  // 84
	p85Func,			  // 85
	p86Func,			  // 86
	p87Func,			  // 87
	p88Func,			  // 88
	NULL,				  // 89
	NULL,				  // 90
	NULL,				  // 91
	NULL,				  // 92
	NULL,				  // 93
	p94Func,			  // 94
	NULL,				  // 95
	NULL,				  // 96
	NULL,				  // 97
	NULL,				  // 98
	NULL,				  // 99
	permissionLogin,	  // 100
	NULL,				  // 101
	passwordModify,		  // 102
	NULL,				  // 103
	NULL,				  // 104
	NULL,				  // 105
	NULL,				  // 106
	NULL,				  // 107
	NULL,				  // 108
	NULL,				  // 109
	NULL,				  // 110
	NULL,				  // 111
	NULL,				  // 112
	p113Func,			  // 113
	p114Func,			  // 114
	NULL,				  // 115s
};

// code void (*pageOpenTreat[])(void) =
// {

// }

/**
 * @brief 打开某页时执行的功能，主要为更新一次当前页的数据
 *
 * @param page
 */
void pageOpenTreat(uint16_t page)
{
	switch (page)
	{
	case 11:
		write_dgus_vp(SYS_INFO_VP, (uint8_t *)&sysInfoSet, SYS_INFO_SIZE); // 进入页面时刷新数据
		break;
	case 12:
		write_dgus_vp(AC_INFO_VP, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);
		break;
	case 18:
		write_dgus_vp(DC_INFO_VP, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);
		break;
	case 22:
		write_dgus_vp(BATTERY_INFO_VP, (uint8_t *)&batteryInfoSet, BATTERY_SET_SIZE);
		break;
	case 29:
		write_dgus_vp(SPECIAL_PARA_VP, (uint8_t *)&specialParaSet, BATTERY_SET_SIZE);
		break;
	case 33:
	{
		uint8_t rtc[6];
		memcpy(rtc, RTCdata, 3);
		memcpy(rtc + 3, RTCdata + 4, 3);
		write_dgus_vp(0x9D, (uint8_t *)&rtc, 3);
		write_dgus_vp(BACKLIGHT_TIME_SET_VP, (uint8_t *)&backLightTime, 1);
	}
	break;
	case 35:
		write_dgus_vp(SWITCH_MODULE_SET_VP, (uint8_t *)&switchModuleSet, SWITCH_MODULE_SIZE);
		break;
	case 39:
		write_dgus_vp(BATTERY_SET_VP, (uint8_t *)&batterySet, BATTERY_SET_SIZE);
		break;
	case 42:
		write_dgus_vp(INSULATION_SET_VP, (uint8_t *)&insulationSet, INSULATION_SET_SIZE);
		break;
	case 44:
		write_dgus_vp(CHARGE_MODULE_SET_VP, (uint8_t *)&chargeModuleSet, CHARGE_MODULE_SET_SIZE);
		break;
	case 48:
		write_dgus_vp(DC_CONVERTER_SET_VP, (uint8_t *)&DC_ConverterSet, DC_CONVERTER_SET_SIZE);
		break;

	default:
		break;
	}
}
void PageFunction(void)
{
	u16 pageid;

	static uint16_t oldPageid = 0;

	// EA = 0;
	ADR_H = 0x00;
	ADR_M = 0x00;
	ADR_L = 0x0a;
	ADR_INC = 1;
	RAMMODE = 0xAF;
	while (!APP_ACK)
		;
	APP_EN = 1;
	while (APP_EN)
		;
	pageid = DATA3;
	pageid <<= 8;
	pageid |= DATA2;
	RAMMODE = 0;
	EA = 1;
	if (PagePointer[pageid] != 0)
	{
		PagePointer[pageid]();
	}

	if (pageid != oldPageid)
	{
		pageOpenTreat(pageid);
		oldPageid = pageid;
	}
}

/**
 * @brief 打开屏保，CFG未设置屏保的情况下，可以通过该代码设置
 *
 */
void openScreenProtection(void)
{
	// 打开屏保
	uint16_t data_0x80[2];
	data_0x80[0] = 0x5A00;
	read_dgus_vp(0x81, (uint8_t *)(data_0x80 + 1), 1);
	data_0x80[1] |= 0x01 << 2;
	write_dgus_vp(0x80, (uint8_t *)data_0x80, 2);

	{ // 设置屏保亮度
		struct
		{
			uint8_t OpenLuminance;		 // 开启亮度
			uint8_t protectionLuminance; // 屏保亮度
		} screenLuminance;				 // 屏幕亮度
		read_dgus_vp(0x82, (uint8_t *)&screenLuminance, 1);
		screenLuminance.protectionLuminance = 0;
		write_dgus_vp(0x82, (uint8_t *)&screenLuminance, 1);
	}
}

void weekDisplay(void)
{
	uint16_t weekday = RTCdata[3];
	write_dgus_vp(0xB000, (uint8_t *)&weekday, 1);
}

/**
 * @brief 充电模式切换为浮充或均充
 *
 * @param chargeMode ：FLOAT_CHARGE/EQUALIZE_CHARGE
 */
void chargeModeSwitch(uint16_t chargeMode)
{
	static MMODBUS chargeCmd;
	if (chargeMode == FLOAT_CHARGE)
	{
		chargeCmd.SlaveAddr = 0xFF; // 广播
		chargeCmd.mode = 0x00;
		chargeCmd.flag = 0x00;
		chargeCmd.Order = 0x06;
		chargeCmd.Length = 1;
		chargeCmd.reserved = 0x00;
		chargeCmd.waitTime = 1000;
		chargeCmd.VPaddr = 0;
		chargeCmd.ModbusReg = 0X0000;
		chargeCmd.databuff = &batteryInfoSet.floatChargeVolt;
		synthesisCollection.closeBusVolt = batteryInfoSet.floatChargeVolt; // 防止重复发送数据
	}
	else if (chargeMode == EQUALIZE_CHARGE)
	{
		chargeCmd.SlaveAddr = 0xFF; // 广播
		chargeCmd.mode = 0x00;
		chargeCmd.flag = 0x00;
		chargeCmd.Order = 0x06;
		chargeCmd.Length = 1;
		chargeCmd.reserved = 0x00;
		chargeCmd.waitTime = 1000;
		chargeCmd.VPaddr = 0;
		chargeCmd.ModbusReg = 0X0000;
		chargeCmd.databuff = &batteryInfoSet.equalizeChargeVolt;
		synthesisCollection.closeBusVolt = batteryInfoSet.equalizeChargeVolt;
	}
	pushToEmergency(&chargeCmd);
}
void batteryManage(void)
{
	static int32_t totalSecond = 0;
	static uint16_t chargeState = FLOAT_CHARGE; // 充电状态：均充/浮充
	int16_t batteryCurr;						// 霍尔变比后电池组电流

	batteryCurr = (int32_t)synthesisCollection.batteryCurr * (int32_t)DC_InfoSet.batteryHallRatio / 100;

	{
		if (batteryCurr < 0)
		{
			chargeState = DISCHARGE;
			KillTimer(CHARGE_TIMER);
			totalSecond = 0;
		}
		else if (batteryCurr >= 0)
		{
			if (synthesisCollection.closeBusVolt >= batteryInfoSet.equalizeChargeVolt)
			{ // 根据电压判断均充浮充
				if (chargeState != EQUALIZE_CHARGE)
				{
					chargeState = EQUALIZE_CHARGE;
					StartTimer(CHARGE_TIMER, 1000);
					totalSecond = (int32_t)batteryInfoSet.equalizeChargeHour * 60 * 60;
				}
			}
			else if (synthesisCollection.closeBusVolt <= batteryInfoSet.floatChargeVolt)
			{
				if (chargeState != FLOAT_CHARGE)
				{
					chargeState = FLOAT_CHARGE;
					StartTimer(CHARGE_TIMER, 1000);
					totalSecond = (int32_t)batteryInfoSet.floatChargeDay * 24 * 60 * 60;
				}
			}

			{
				int32_t equalizeCurr; // 均充电流
				int32_t floatCurr;	  // 浮充电流
				equalizeCurr = (int32_t)batteryInfoSet.nominalCapacity * batteryInfoSet.floatToEqualizeChargeCur / 1000;
				floatCurr = (int32_t)batteryInfoSet.nominalCapacity * batteryInfoSet.equalizeToFloatChargeCur / 1000;
				if (batteryCurr >= equalizeCurr)
				{
					if (chargeState != EQUALIZE_CHARGE)
					{
						chargeModeSwitch(EQUALIZE_CHARGE);
					}
				}
				else if (batteryCurr <= floatCurr)
				{
					if (chargeState != FLOAT_CHARGE)
					{
						chargeModeSwitch(FLOAT_CHARGE);
					}
				}
			}

			if (GetTimeOutFlag(CHARGE_TIMER))
			{
				totalSecond--;
				if (totalSecond <= 0)
				{
					if (chargeState == FLOAT_CHARGE)
					{
						chargeModeSwitch(EQUALIZE_CHARGE);
					}
					else if (chargeState == EQUALIZE_CHARGE)
					{
						chargeModeSwitch(FLOAT_CHARGE);
					}
				}
				StartTimer(CHARGE_TIMER, 1000);
				{
					timeTypeDef chargeTime;
					chargeTime.day = totalSecond / 60 / 60 / 24;
					chargeTime.hour = totalSecond / 60 / 60 % 24;
					chargeTime.minite = totalSecond / 60 % 60;
					chargeTime.second = totalSecond % 60;
					write_dgus_vp(0xB010, (uint8_t *)&chargeTime, 10);
				}
			}
		}
		write_dgus_vp(0xB002, (uint8_t *)&chargeState, 1);
	}
}

void batteryVoltDisplay()
{
	int16_t batteryVoltSum;
	uint16_t i;
	if (batterySet.xj24.cellNum > 0)
	{
		if (batterySet.batteryType == XJ24)
		{
			batteryVoltSum = 0;
			for (i = 0; i < batterySet.xj24.cellNum; i++)
			{
				batteryVoltSum += battery_xj24[i / 24].volt[i % 24];
			}
		}
		else if (batterySet.batteryType == XJ55)
		{
			batteryVoltSum = 0;
			for (i = 0; i < batterySet.xj55.cellNum; i++)
			{
				batteryVoltSum += battery_xj55[i / 55].volt[i % 55];
			}
		}
	}
	else if (batterySet.xj24.cellNum == 0)
	{
		batteryVoltSum = synthesisCollection.batteryVolt;
	}
	write_dgus_vp(0xB020, (uint8_t *)&batteryVoltSum, 1);
}

void batteryCurrDisplay(void)
{
	write_dgus_vp(0xB022, (uint8_t *)&synthesisCollection.batteryCurr, 1);
}

void publicUI(void)
{
	weekDisplay();
	batteryVoltDisplay();
	batteryCurrDisplay();
}