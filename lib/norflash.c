#include "norflash.h"
#include <string.h>
#include "ui.h"
#include "debug.h"
#include "data.h"

xdata u8 norflash_cmd[8];

// 写入到norflash中
// 向T5L内部自带的nor flash写入数据
//  addr:nor flash的地址,必须为偶数,范围为:0x000000-0x027FFE,然后一个地址对应2个字节,即总容量为320KB
//  buff:需要写入的缓冲区,长度至少大于len*2
//  len:写入的字数,必须为偶数,而且写入的最大长度也有限制,这个跟CACHE_ADDR的值有关系
void Nor_Flash_write(u32 addr, u8 *buff, u16 len)
{
	write_dgus_vp(CACHE_ADDR, buff, len); // 将数据从缓冲区写到CACHE_ADDR中
	norflash_cmd[0] = NORFLASH_WRITE;	  // 操作模式

	norflash_cmd[1] = (u8)(addr >> 16); // norflash地址
	norflash_cmd[2] = (u8)(addr >> 8);
	norflash_cmd[3] = (u8)addr;

	norflash_cmd[4] = (u8)(CACHE_ADDR >> 8); // dugs变量数据地址
	norflash_cmd[5] = (u8)CACHE_ADDR;

	norflash_cmd[6] = (u8)(len >> 8); // 读写字节长度
	norflash_cmd[7] = (u8)(len);

	write_dgus_vp(NORFLASH_ADDR, norflash_cmd, 4); // 写入系统变量接口地址0x0008中
	while (1)									   // 等待清零
	{
		read_dgus_vp(NORFLASH_ADDR, norflash_cmd, 2); // 读取操作模式位 0x5A=读 0xA5=写， CPU 操作完清零。
		if (norflash_cmd[0] == 0)					  // 清零
		{
			break;
		}
		delay_ms(50);
	}
}
// 从norflash中读取
void Nor_Flash_read(u32 addr, u8 *buff, u16 len)
{
	norflash_cmd[0] = NORFLASH_READ; // 操作模式

	norflash_cmd[1] = (u8)(addr >> 16); // norflash地址
	norflash_cmd[2] = (u8)(addr >> 8);
	norflash_cmd[3] = (u8)addr;

	norflash_cmd[4] = (u8)(CACHE_ADDR >> 8); // dugs变量数据地址
	norflash_cmd[5] = (u8)CACHE_ADDR;

	norflash_cmd[6] = (u8)(len >> 8); // 读写字节长度
	norflash_cmd[7] = (u8)(len);

	write_dgus_vp(NORFLASH_ADDR, norflash_cmd, 4);
	while (1)
	{
		read_dgus_vp(NORFLASH_ADDR, norflash_cmd, 2);
		if (norflash_cmd[0] == 0)
		{
			break;
		}
		delay_ms(1);
	}
	read_dgus_vp(CACHE_ADDR, buff, len); // 将数据从CACHE_ADDR中读到buff中
}

// 向T5L内部自带的nor flash写入数据
//  addrForNorFlash:nor flash的地址,必须为偶数,范围为:0x000000-0x027FFE,然后一个地址对应2个字节,即总容量为320KB
//  addrInDgus:需要写入的Dgus VP缓冲区,长度至少大于len*2
//  len:写入的字数,必须为偶数,而且写入的最大长度也有限制,这个跟CACHE_ADDR的值有关系
void dgusToNorFlash(u32 addrForNorFlash, u32 addrInDgus, u16 len)
{
	u8 norFlash_buff[8];

	norFlash_buff[0] = NORFLASH_WRITE;				   // 写操作
	norFlash_buff[1] = (addrForNorFlash >> 16) & 0xff; // nor flash地址
	norFlash_buff[2] = (addrForNorFlash >> 8) & 0xff;
	norFlash_buff[3] = addrForNorFlash & 0xff;
	norFlash_buff[4] = (addrInDgus >> 8) & 0xff; // dgusii数据地址
	norFlash_buff[5] = addrInDgus & 0xff;
	norFlash_buff[6] = (len >> 8) & 0xff; // 写入的数据长度
	norFlash_buff[7] = len & 0xff;
	write_dgus_vp(NORFLASH_ADDR, norFlash_buff, 4);

	while (1)
	{
		read_dgus_vp(NORFLASH_ADDR, norFlash_buff, 2); // 只需读取0x0008变量的前2个字,然后判断D7是否为0
		if (norFlash_buff[0] == 0)
			break;
		delay_ms(50); // 这个延时必须加,可以防止莫名其妙的错误
	}
}

void norFlashToDgus(u32 addrForNorFlash, u32 addrInDgus, u16 len)
{
	u8 norFlash_buff[8];

	norFlash_buff[0] = NORFLASH_READ;				   // 写操作
	norFlash_buff[1] = (addrForNorFlash >> 16) & 0xff; // nor flash地址
	norFlash_buff[2] = (addrForNorFlash >> 8) & 0xff;
	norFlash_buff[3] = addrForNorFlash & 0xff;
	norFlash_buff[4] = (addrInDgus >> 8) & 0xff; // dgusii数据地址
	norFlash_buff[5] = addrInDgus & 0xff;
	norFlash_buff[6] = (len >> 8) & 0xff; // 写入的数据长度
	norFlash_buff[7] = len & 0xff;
	write_dgus_vp(NORFLASH_ADDR, norFlash_buff, 4);

	while (1)
	{
		read_dgus_vp(NORFLASH_ADDR, norFlash_buff, 2); // 只需读取0x0008变量的前2个字,然后判断D7是否为0
		if (norFlash_buff[0] == 0)
			break;
		delay_ms(50); // 这个延时必须加,可以防止莫名其妙的错误
	}
}

void sysParameterRead(void)
{
	// DEBUGINFO("sysParameterRead");
	Nor_Flash_read(SYS_INFO_NORFLASH_ADDR, (uint8_t *)&sysInfoSet, SYS_INFO_SIZE);
	write_dgus_vp(SYS_INFO_VP, (uint8_t *)&sysInfoSet, SYS_INFO_SIZE);

	Nor_Flash_read(AC_INFO_NORFLASH_ADDR, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);
	write_dgus_vp(AC_INFO_VP, (uint8_t *)&AC_InfoSet, AC_INFO_SIZE);

	Nor_Flash_read(DC_INFO_NORFLASH_ADDR, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);
	write_dgus_vp(DC_INFO_VP, (uint8_t *)&DC_InfoSet, DC_INFO_SIZE);

	Nor_Flash_read(BATTERY_INFO_NORFLASH_ADDR, (uint8_t *)&batteryInfoSet, BATTERY_INFO_SIZE);
	write_dgus_vp(BATTERY_INFO_VP, (uint8_t *)&batteryInfoSet, BATTERY_INFO_SIZE);

	Nor_Flash_read(SPECIAL_PARA_NORFLASH_ADDR, (uint8_t *)&specialParaSet, SPECIAL_PARA_SIZE);
	write_dgus_vp(SPECIAL_PARA_VP, (uint8_t *)&specialParaSet, SPECIAL_PARA_SIZE);

	Nor_Flash_read(BACKLIGHT_TIME_NORFLASH_ADDR, (uint8_t *)&backLightTime, 2);
	if (backLightTime[0] < 10)
	{
		backLightTime[0] = 10;
	}
	write_dgus_vp(BACKLIGHT_TIME_SET_VP, (uint8_t *)&backLightTime, 1);
	write_dgus_vp(0x83, (uint8_t *)&backLightTime, 1);

	// DEBUGINFO("BATTERY_SET_NORFLASH_ADDR");
	Nor_Flash_read(SWITCH_MODULE_NORFLASH_ADDR, (uint8_t *)&switchModuleSet, SWITCH_MODULE_SIZE);
	write_dgus_vp(SWITCH_MODULE_SET_VP, (uint8_t *)&switchModuleSet, SWITCH_MODULE_SIZE);

	Nor_Flash_read(BATTERY_SET_NORFLASH_ADDR, (uint8_t *)&batterySet, BATTERY_SET_SIZE);
	write_dgus_vp(BATTERY_SET_VP, (uint8_t *)&batterySet, BATTERY_SET_SIZE);

	Nor_Flash_read(INSULATION_SET_NORFLASH_ADDR, (uint8_t *)&insulationSet, INSULATION_SET_SIZE);
	write_dgus_vp(INSULATION_SET_VP, (uint8_t *)&insulationSet, INSULATION_SET_SIZE);

	Nor_Flash_read(CHARGE_MODULE_SET_NORFLASH_ADDR, (uint8_t *)&chargeModuleSet, CHARGE_MODULE_SET_SIZE);
	write_dgus_vp(CHARGE_MODULE_SET_VP, (uint8_t *)&chargeModuleSet, CHARGE_MODULE_SET_SIZE);

	Nor_Flash_read(DC_CONVERTER_SET_NORFLASH_ADDR, (uint8_t *)&DC_ConverterSet, DC_CONVERTER_SET_SIZE);
	write_dgus_vp(DC_CONVERTER_SET_VP, (uint8_t *)&DC_ConverterSet, DC_CONVERTER_SET_SIZE);

	Nor_Flash_read(UPS_INV_SET_NORFLASH_ADDR, (uint8_t *)&UPS_INV_Set, UPS_INV_SET_SIZE);
	write_dgus_vp(UPS_INV_SET_VP, (uint8_t *)&UPS_INV_Set, UPS_INV_SET_SIZE);

	Nor_Flash_read(ALARM_SOUND_ONOFF_NORFLASH_ADDR, (uint8_t *)alarmSoundOnOff, 2);
	write_dgus_vp(0xB040, (uint8_t *)alarmSoundOnOff, 1);

	{
		extern uint16_t slaveID[2];
		Nor_Flash_read(SLAVE_ID_NORFLASH_ADDR, (uint8_t *)slaveID, 2);
		write_dgus_vp(0x9151, (uint8_t *)slaveID, 1);
	}
}