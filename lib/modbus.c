#include "modbus.h"
#include "uart.h"
#include "umath.h"
#include "timer.h"
#include "debug.h"
#include <STRING.H>
#include "const.h"
#include "data.h"
#include "ui.h"
#include "alarm.h"

MMODBUS pageModbusReg[PAGE_MAX_NUM] = {0};
u8 modbusNum, modbusTreatID; // modbusTreatID,用来指示当前正在处理哪一条，modbusNum表示当前缓存区中一共有多少条
MMODBUS *emergencyTreat[EMERTENCY_NUM];
u8 emergencyTail, emergencyHead;
static MMODBUS *pNowOrder;
static u8 ModbusBusy; // 指示是否有MODBUS命令正在发送

uint8_t synthesisCollection_Comm_Sta;
uint8_t DC_Insulation_Comm_Sta[DC_INSULATION_MAX];
uint8_t switchModule_Comm_Sta[SWITCH_MODULE_MAX];
uint8_t battery_Comm_Sta[5];
uint8_t dc_4850_Module_Comm_Sta[DC4850MODULE_MAX];
uint8_t remoteControlModule_Comm_Sta[REMOTE_CONTROL_MODULE_MAX];
uint8_t chargeModule_Comm_Sta[CHARGER_MODULE_MAX];
uint8_t ups_Comm_Sta[UPS_MAX];
uint8_t inv_Comm_Sta[INV_MAX];
uint8_t AC_Insulation_Comm_Sta;

u8 isEmergencyFull(void) // 用于检查紧急处理是否是满的，满的不能再次装填数据，以免造成错乱
{
	u8 tmp;

	tmp = emergencyHead + 1;
	if (tmp >= EMERTENCY_NUM)
		tmp = 0;
	if (tmp == emergencyTail)
		return 1;
	else
		return 0;
}

u8 isEmergencyEmpty(void)
{
	if (emergencyHead == emergencyTail)
		return 1;
	else
		return 0;
}

void clrEmergency(void)
{
	emergencyHead = emergencyTail = 0;
}

void pushToEmergency(MMODBUS *modbusOrder) // 用于加载一个结构包指针到数组中
{
	if (isEmergencyFull())
		return;
	emergencyTreat[emergencyHead] = modbusOrder;
	emergencyHead++;
	if (emergencyHead >= EMERTENCY_NUM)
		emergencyHead = 0;
}

MMODBUS *popFromEmergency(void)
{
	u8 tmp;

	if (isEmergencyEmpty())
		return NULL;
	else
	{
		tmp = emergencyTail;
		emergencyTail++;
		if (emergencyTail >= EMERTENCY_NUM)
			emergencyTail = 0;
		return emergencyTreat[tmp];
	}
}


uint16_t slaveID[2];//2字长用于flash存储
void Uart485RxTreat(void)
{
	u16 len, len1, i;
	u8 tmp[512];
	u16 headtmp;

	if (Uart_Struct[UART485RX].rx_tail != Uart_Struct[UART485RX].rx_head)
	{
		if (Uart_Struct[UART485RX].rx_buf[Uart_Struct[UART485RX].rx_tail] == slaveID[0])
		{
			EA = 0;
			headtmp = Uart_Struct[UART485RX].rx_head;
			EA = 1;
			if (headtmp < Uart_Struct[UART485RX].rx_tail)
			{
				len = (headtmp + SERIAL_SIZE) - Uart_Struct[UART485RX].rx_tail;
			}
			else
			{
				len = headtmp - Uart_Struct[UART485RX].rx_tail;
			}
			if (len >= START_TREAT_LENGTH)
			{
				if (Uart_Struct[UART485RX].rx_buf[(Uart_Struct[UART485RX].rx_tail + 1) & SERIAL_COUNT] == READ_REGISTER)
				{
					if (len >= 8)
					{
						for (i = 0; i < 8; i++)
						{
							tmp[i] = Uart_Struct[UART485RX].rx_buf[Uart_Struct[UART485RX].rx_tail];
							Uart_Struct[UART485RX].rx_tail++;
							Uart_Struct[UART485RX].rx_tail &= SERIAL_COUNT;
						}
						if (Calculate_CRC16(tmp, 8, 0) == 0)
						{
							uint16_t addr = *(uint16_t *)(tmp + 2);
							tmp[2] = *(uint16_t *)(tmp + 4) * 2;
							read_dgus_vp(addr, tmp + 3, tmp[2] / 2);
							Calculate_CRC16(tmp, tmp[2] + 3, 1);
							Uart_Send_Data(UART485RX, tmp[2] + 5, tmp);
						}
					}
				}
				else if (Uart_Struct[UART485RX].rx_buf[(Uart_Struct[UART485RX].rx_tail + 1) & SERIAL_COUNT] == WRITE_REGISTER)
				{
					len1 = Uart_Struct[UART485RX].rx_buf[(Uart_Struct[UART485RX].rx_tail + 6) & SERIAL_COUNT] + 9;
					if (len >= len1)
					{
						for (i = 0; i < len1; i++)
						{
							tmp[i] = Uart_Struct[UART485RX].rx_buf[Uart_Struct[UART485RX].rx_tail];
							Uart_Struct[UART485RX].rx_tail++;
							Uart_Struct[UART485RX].rx_tail &= SERIAL_COUNT;
						}
						if (Calculate_CRC16(tmp, len1, 0) == 0)
						{
							if (*(uint16_t *)(tmp + 2) >= 0x1000)
							{
								write_dgus_vp(*(uint16_t *)(tmp + 2), (uint8_t *)(tmp + 7), *(uint16_t *)(tmp + 4));
								Calculate_CRC16(tmp, 6, 1);
								Uart_Send_Data(UART485RX, 8, tmp);
							}
						}
					}
				}
				else
				{
					Uart_Struct[UART485RX].rx_tail++;
					Uart_Struct[UART485RX].rx_tail &= SERIAL_COUNT;
				}
			}
		}
		else
		{
			Uart_Struct[UART485RX].rx_tail++;
			Uart_Struct[UART485RX].rx_tail &= SERIAL_COUNT;
		}
	}
}

#if 0 // 屏幕做从机
void Uart485RxTreat(void)
{
	u16 len,len1,i;
	u8 tmp[512];
	u16 headtmp;

	if(Uart_Struct[UART485].rx_tail != Uart_Struct[UART485].rx_head)
	{
		if(Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail]==0)
		{
			EA = 0;
			headtmp = Uart_Struct[UART485].rx_head;
			EA = 1;
            if(headtmp < Uart_Struct[UART485].rx_tail)
            {
                len = (headtmp+SERIAL_SIZE) - Uart_Struct[UART485].rx_tail;
            }
            else
            {
                len = headtmp - Uart_Struct[UART485].rx_tail;
            }
			if(len >= START_TREAT_LENGTH)
			{
				if(Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+1)&SERIAL_COUNT] == READ_REGISTER)
				{
					if(len>=8)
					{
						for(i=0;i<8;i++)
						{
							tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
						}
						if(Calculate_CRC16(tmp,8)==0)
						{
							
						}
					}
				}
				else if(Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+1)&SERIAL_COUNT] == WRITE_REGISTER)
				{
					len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail+6)&SERIAL_COUNT]+9;
					if(len >= len1)
					{
						for(i=0;i<len1;i++)
						{
							tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
						}
						if(Calculate_CRC16(tmp,len1)==0)
						{
								Calculate_CRC16(tmp,6);
								Uatr_Send_Data(UART485,8,tmp);
						}
					}
				}
				else
				{
					Uart_Struct[UART485].rx_tail++;
					Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
				}
			}
		}
		else
		{
			Uart_Struct[UART485].rx_tail++;
			Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
		}
	}
}
#else // 屏幕做主机
void modbusRxTreat(void)
{
	u16 len, len1, i, lentmp;
	u8 tmp[512];
	u16 headtmp;
	u8 tmp8;

	EA = 0;
	headtmp = Uart_Struct[UART485].rx_head;
	EA = 1;
	if (Uart_Struct[UART485].rx_tail != headtmp)
	{
		if (Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail] == pNowOrder->SlaveAddr) // 查找当前正在发送命令的modbus地址
		{
			if (headtmp < Uart_Struct[UART485].rx_tail)
			{
				len = (headtmp + SERIAL_SIZE) - Uart_Struct[UART485].rx_tail;
			}
			else
			{
				len = headtmp - Uart_Struct[UART485].rx_tail;
			}
			if (len >= START_TREAT_LENGTH) // 缓存到一定长度后才开始处理数据
			{
				if (Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail + 1) & SERIAL_COUNT] == pNowOrder->Order) // 查找到了地址，并且后面是响应的指令
				{
					if (pNowOrder->Order == 0x01 || pNowOrder->Order == 0x02)
					{
						len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail + 2) & SERIAL_COUNT];
						lentmp = pNowOrder->Length / 8;
						if ((pNowOrder->Length % 8) != 0)
							lentmp++;
						if (len1 == lentmp)
						{
							if (len >= len1 + 5)
							{
								for (i = 0; i < len1 + 5; i++)
								{
									tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
									Uart_Struct[UART485].rx_tail++;
									Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
								}
								if (Calculate_CRC16(tmp, len1 + 5, 0) == 0) // 校验正确
								{
									ModbusBusy = 0; // 标志位变成空闲，可以继续发送下一包数据
									for (i = 0; i < pNowOrder->Length; i++)
									{
										tmp8 = 0x01 << (i % 8);
										if (tmp[3 + i / 8] & tmp8)
											headtmp = 1;
										else
											headtmp = 0;
										write_dgus_vp(pNowOrder->VPaddr + i, (u8 *)&headtmp, 1);
									}
									if (pNowOrder->databuff != NULL)
									{
										memcpy(pNowOrder->databuff, tmp + 3, tmp[2]);
									}
								}
							}
						}
						else
						{
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT; // 继续搜索
						}
					}
					else if (0x03 == pNowOrder->Order)
					{
						len1 = Uart_Struct[UART485].rx_buf[(Uart_Struct[UART485].rx_tail + 2) & SERIAL_COUNT];
						lentmp = pNowOrder->Length << 1;
						if (len1 == lentmp)
						{
							if (len >= len1 + 5)
							{
								for (i = 0; i < len1 + 5; i++)
								{
									tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
									Uart_Struct[UART485].rx_tail++;
									Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
								}
								if (Calculate_CRC16(tmp, len1 + 5, 0) == 0) // 校验正确
								{
									ModbusBusy = 0; // 标志位变成空闲，可以继续发送下一包数据
									if (pNowOrder->VPaddr != 0)
									{
										write_dgus_vp(pNowOrder->VPaddr, &tmp[3], pNowOrder->Length);
									}
									if (pNowOrder->databuff != NULL)
									{
										memcpy(pNowOrder->databuff, tmp + 3, tmp[2]);
									}

									{
										uint16_t i;
										if (pNowOrder->SlaveAddr == 0x61)
										{
											synthesisCollection_Comm_Sta = 1;
										}
										if (pNowOrder->SlaveAddr == 0x60)
										{
											DC_Insulation_Comm_Sta[0] = 1;
										}
										if (pNowOrder->SlaveAddr == 0x62)
										{
											DC_Insulation_Comm_Sta[1] = 1;
										}
										for (i = 0; i < SWITCH_MODULE_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 0xA0 + i)
											{
												switchModule_Comm_Sta[i] = 1;
											}
										}
										for (i = 0; i < SWITCH_MODULE_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 0x70 + i)
											{
												battery_Comm_Sta[i] = 1;
											}
										}
										for (i = 0; i < DC4850MODULE_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 0x90 + i)
											{
												dc_4850_Module_Comm_Sta[i] = 1;
											}
										}
										for (i = 0; i < CHARGER_MODULE_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 0x01 + i)
											{
												chargeModule_Comm_Sta[i] = 1;
											}
										}
										for (i = 0; i < UPS_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 26 + i)
											{
												ups_Comm_Sta[i] = 1;
											}
										}
										for (i = 0; i < INV_MAX; i++)
										{
											if (pNowOrder->SlaveAddr == 0x80 + i)
											{
												inv_Comm_Sta[i] = 1;
											}
										}
										if (pNowOrder->SlaveAddr == 0x32)
										{
											alarmTabFlag[AC_FAULT] = 1;
										}
									}
								}
							}
						}
						else
						{
							Uart_Struct[UART485].rx_tail++;
							Uart_Struct[UART485].rx_tail &= SERIAL_COUNT; // 继续搜索
						}
					}
					else if ((0x05 == pNowOrder->Order) || (0x06 == pNowOrder->Order) || (0x10 == pNowOrder->Order))
					{
						if (len >= 8)
						{
							for (i = 0; i < 8; i++)
							{
								tmp[i] = Uart_Struct[UART485].rx_buf[Uart_Struct[UART485].rx_tail];
								Uart_Struct[UART485].rx_tail++;
								Uart_Struct[UART485].rx_tail &= SERIAL_COUNT;
							}
							if (Calculate_CRC16(tmp, 8, 0) == 0) // 校验正确
							{
								if (*(u16 *)&tmp[2] == pNowOrder->ModbusReg)
								{
									ModbusBusy = 0; // 标志位变成空闲，可以继续发送下一包数据
								}
							}
						}
					}
					else
					{
						Uart_Struct[UART485].rx_tail++;
						Uart_Struct[UART485].rx_tail &= SERIAL_COUNT; // 继续搜索
					}
				}
				else
				{
					Uart_Struct[UART485].rx_tail++;
					Uart_Struct[UART485].rx_tail &= SERIAL_COUNT; // 继续搜索
				}
			}
		}
		else
		{
			Uart_Struct[UART485].rx_tail++;
			Uart_Struct[UART485].rx_tail &= SERIAL_COUNT; // 继续搜索
		}
	}
}

#endif

u8 AnalysisMosbusOrder(u8 *pBuf) // 根据当前指令的格式将要发送的数据放到Pbuf里面，并返回发送长度
{
	u8 len;
	u8 tmp[4];

	if (pNowOrder->mode == 0)
	{
	}
	else if (pNowOrder->mode == 1)
	{
		if (pNowOrder->flag == 0x5a)
		{
			pNowOrder->flag = 0x00;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	len = 0;
	switch (pNowOrder->Order)
	{
	case 0x01: // 读位状态寄存器
		if (pNowOrder->Length)
		{
			pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
			pBuf[1] = 0x01;					// 功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg); // 位寄存器起始地址
			pBuf[4] = 0;
			pBuf[5] = pNowOrder->Length; // 位寄存器长度，最大支持255
			len = 6;
		}
		else
		{
			len = 0;
		}
		break;
	case 0x02: // 读位状态寄存器
		if (pNowOrder->Length)
		{
			pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
			pBuf[1] = 0x02;					// 功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg); // 位寄存器起始地址
			pBuf[4] = 0;
			pBuf[5] = pNowOrder->Length; // 位寄存器长度，最大支持255
			len = 6;
		}
		else
		{
			len = 0;
		}
		break;
	case 0x03: // 读保持寄存器
		if (pNowOrder->Length)
		{
			pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
			pBuf[1] = 0x03;					// 功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg); // 寄存器起始地址
			pBuf[4] = 0;
			pBuf[5] = pNowOrder->Length; // 位寄存器长度，最大支持255
			len = 6;
		}
		else
		{
			len = 0;
		}
		break;
	case 0x05:							// 写单个位寄存器
		pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
		pBuf[1] = 0x05;					// 功能码
		pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
		pBuf[3] = (u8)(pNowOrder->ModbusReg); // 位寄存器起始地址
		if (pNowOrder->databuff != NULL)
		{
			memcpy(tmp, (uint8_t *)pNowOrder->databuff, 2);
		}
		else if (pNowOrder->VPaddr != 0)
		{
			read_dgus_vp(pNowOrder->VPaddr, tmp, 1);
		}
		if (tmp[1])
			pBuf[4] = 0xff; // 位转态为ON
		else
			pBuf[4] = 0; // 位状态为OFF
		pBuf[5] = 0x00;
		len = 6;
		break;
	case 0x06:							// 写单个寄存器
		pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
		pBuf[1] = 0x06;					// 功能码
		pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
		pBuf[3] = (u8)(pNowOrder->ModbusReg); // 寄存器起始地址
		if (pNowOrder->databuff != NULL)
		{
			memcpy(&pBuf[4], (uint8_t *)pNowOrder->databuff, 2);
		}
		else if (pNowOrder->VPaddr != 0)
		{
			read_dgus_vp(pNowOrder->VPaddr, &pBuf[4], 1);
		}
		len = 6;
		break;
	case 0x10: // 写多个寄存器
		if ((pNowOrder->Length > 0) && (pNowOrder->Length < 0x7b))
		{
			pBuf[0] = pNowOrder->SlaveAddr; // 从机地址
			pBuf[1] = 0x10;					// 功能码
			pBuf[2] = (u8)(pNowOrder->ModbusReg >> 8);
			pBuf[3] = (u8)(pNowOrder->ModbusReg); // 寄存器起始地址
			pBuf[4] = 0;
			pBuf[5] = pNowOrder->Length;	  // 位寄存器长度，最大支持0x7b
			pBuf[6] = pNowOrder->Length << 1; // 写入寄存器字节数
			len = pBuf[6] + 7;
			if (pNowOrder->databuff != NULL)
			{
				memcpy(&pBuf[7], (uint8_t *)pNowOrder->databuff, pNowOrder->Length * 2);
			}
			else if (pNowOrder->VPaddr != 0)
			{
				read_dgus_vp(pNowOrder->VPaddr, &pBuf[7], pNowOrder->Length); // 实际数据
			}
		}
		else
		{
			len = 0;
		}
		break;
	default:
		break;
	}
	if (len)
	{
		Calculate_CRC16(pBuf, len, 1);
		return len + 2; // 返回发送长度+2字节CRC
	}
	return 0; // 0表示本条指令无效
}

void modbusTxTreat(void)
{
	u8 sendbuf[512];
	u8 len;

	if (ModbusBusy) // 有数据正在处理的时候，不处理发送
		return;
	pNowOrder = popFromEmergency();
	if (pNowOrder != NULL)
	{
		len = AnalysisMosbusOrder(sendbuf);
		if (len)
		{
			ModbusBusy = 1;
			Uart_Send_Data(UART485, len, sendbuf);
			StartTimer(MODBUS_TIMER, pNowOrder->waitTime);
		}
	}
	else
	{
		if (modbusNum == 0) // 当前页的寄存器个数为0的时候，不处理发送
		{
			modbusTreatID = 0;
			return;
		}
		pNowOrder = &pageModbusReg[modbusTreatID];
		modbusTreatID++;
		if (modbusTreatID >= modbusNum)
			modbusTreatID = 0;
		len = AnalysisMosbusOrder(sendbuf);
		if (len)
		{
			ModbusBusy = 1;
			Uart_Send_Data(UART485, len, sendbuf);
			StartTimer(MODBUS_TIMER, pNowOrder->waitTime);
		}
	}
}

void modbusTreat(void)
{
	if (ModbusBusy)
	{
		if (GetTimeOutFlag(MODBUS_TIMER)) // 超时后，强制处理下一包
		{
			ModbusBusy = 0;
			EA = 0;
			Uart_Struct[UART485].rx_tail = Uart_Struct[UART485].rx_head;
			{
				uint16_t i;
				if (pNowOrder->SlaveAddr == 0x61)
				{
					synthesisCollection_Comm_Sta = 0;
				}
				if (pNowOrder->SlaveAddr == 0x60)
				{
					DC_Insulation_Comm_Sta[0] = 0;
				}
				if (pNowOrder->SlaveAddr == 0x62)
				{
					DC_Insulation_Comm_Sta[1] = 0;
				}
				for (i = 0; i < SWITCH_MODULE_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 0xA0 + i)
					{
						switchModule_Comm_Sta[i] = 0;
					}
				}
				for (i = 0; i < SWITCH_MODULE_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 0x70 + i)
					{
						battery_Comm_Sta[i] = 0;
					}
				}
				for (i = 0; i < DC4850MODULE_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 0x90 + i)
					{
						dc_4850_Module_Comm_Sta[i] = 0;
					}
				}
				for (i = 0; i < CHARGER_MODULE_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 0x01 + i)
					{
						chargeModule_Comm_Sta[i] = 0;
					}
				}
				for (i = 0; i < UPS_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 26 + i)
					{
						ups_Comm_Sta[i] = 0;
					}
				}
				for (i = 0; i < INV_MAX; i++)
				{
					if (pNowOrder->SlaveAddr == 0x80 + i)
					{
						inv_Comm_Sta[i] = 0;
					}
				}
				if (pNowOrder->SlaveAddr == 0x32)
				{
					alarmTabFlag[AC_FAULT] = 0;
				}
				EA = 1;
			}
		}
	}
	modbusTxTreat();
	modbusRxTreat();
}
