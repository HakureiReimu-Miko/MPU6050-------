#ifndef __MODBUS__H___
#define __MODBUS__H___
#include "sys.h"
typedef struct _mmodbus
{
	u8 SlaveAddr;		// 从机地址
	u8 mode;			// 这一帧的处理模式，0表示一直处理，一般用于读modbus，1表示flag==5a时处理，一般用于写
	u8 flag;			// 处理标记位，配合mode使用
	u8 Order;			// modbus命令
	u8 Length;			// 读写数据长度
	u8 reserved;		// 保留
	u16 waitTime;		// 本条指令执行时间
	u16 VPaddr;			// 本条指令VP起始地址，如果高字节是0XFF，低字节为曲线通道，则读取的数据直接写入曲线缓存区，曲线功能暂未处理
	u16 ModbusReg;		// 本条指令Modbus寄存器起始地址
	uint16_t *databuff; // 存储变量指针
} MMODBUS;

#define START_TREAT_LENGTH 5
#define READ_REGISTER 0x03
#define WRITE_REGISTER 0x10
#define UART485 UART4
#define PAGE_MAX_NUM 64	 // 每页20条数据,根据实际通过宏定义修改,将const.c里面的数据，拷贝到该缓存区
#define EMERTENCY_NUM 16 // 当有改变的时候，需要立即处理的时候，将需要执行的modbus配置写到缓存区中
#define MODBUS_TIMER 7

extern MMODBUS pageModbusReg[PAGE_MAX_NUM];	   // 将需要处理的数据，从const.c里面加载到这个缓存区，页面变化的时候加载一次
extern u8 modbusNum, modbusTreatID;			   // modbusTreatID,用来指示当前正在处理哪一条，modbusNum表示当前缓存区中一共有多少条
extern MMODBUS *emergencyTreat[EMERTENCY_NUM]; // 如果有写寄存器，则将某一帧数据缓存到紧急处理的数组，以免刷新出现比较大的演示
extern u8 emergencyTail, emergencyHead;		   // 循环队列头和尾

void modbusTreat(void);
void pushToEmergency(MMODBUS *modbusOrder);
u8 isEmergencyFull(void);
void clrEmergency(void);
#endif