#ifndef __MODBUS__H___
#define __MODBUS__H___
#include "sys.h"
typedef struct _mmodbus
{
	u8 SlaveAddr;		// �ӻ���ַ
	u8 mode;			// ��һ֡�Ĵ���ģʽ��0��ʾһֱ����һ�����ڶ�modbus��1��ʾflag==5aʱ����һ������д
	u8 flag;			// ������λ�����modeʹ��
	u8 Order;			// modbus����
	u8 Length;			// ��д���ݳ���
	u8 reserved;		// ����
	u16 waitTime;		// ����ָ��ִ��ʱ��
	u16 VPaddr;			// ����ָ��VP��ʼ��ַ��������ֽ���0XFF�����ֽ�Ϊ����ͨ�������ȡ������ֱ��д�����߻����������߹�����δ����
	u16 ModbusReg;		// ����ָ��Modbus�Ĵ�����ʼ��ַ
	uint16_t *databuff; // �洢����ָ��
} MMODBUS;

#define START_TREAT_LENGTH 5
#define READ_REGISTER 0x03
#define WRITE_REGISTER 0x10
#define UART485 UART4
#define PAGE_MAX_NUM 64	 // ÿҳ20������,����ʵ��ͨ���궨���޸�,��const.c��������ݣ��������û�����
#define EMERTENCY_NUM 16 // ���иı��ʱ����Ҫ���������ʱ�򣬽���Ҫִ�е�modbus����д����������
#define MODBUS_TIMER 7

extern MMODBUS pageModbusReg[PAGE_MAX_NUM];	   // ����Ҫ��������ݣ���const.c������ص������������ҳ��仯��ʱ�����һ��
extern u8 modbusNum, modbusTreatID;			   // modbusTreatID,����ָʾ��ǰ���ڴ�����һ����modbusNum��ʾ��ǰ��������һ���ж�����
extern MMODBUS *emergencyTreat[EMERTENCY_NUM]; // �����д�Ĵ�������ĳһ֡���ݻ��浽������������飬����ˢ�³��ֱȽϴ����ʾ
extern u8 emergencyTail, emergencyHead;		   // ѭ������ͷ��β

void modbusTreat(void);
void pushToEmergency(MMODBUS *modbusOrder);
u8 isEmergencyFull(void);
void clrEmergency(void);
#endif