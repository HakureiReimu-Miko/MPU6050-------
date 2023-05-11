#ifndef __MODBUS__H___
#define __MODBUS__H___
#include "sys.h"
#include "alarm.h"
#include "const.h"
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
#define UART485RX UART5
#define PAGE_MAX_NUM 64	 // ÿҳ20������,����ʵ��ͨ���궨���޸�,��const.c��������ݣ��������û�����
#define EMERTENCY_NUM 16 // ���иı��ʱ����Ҫ���������ʱ�򣬽���Ҫִ�е�modbus����д����������
#define MODBUS_TIMER 7

extern MMODBUS pageModbusReg[PAGE_MAX_NUM];	   // ����Ҫ��������ݣ���const.c������ص������������ҳ��仯��ʱ�����һ��
extern u8 modbusNum, modbusTreatID;			   // modbusTreatID,����ָʾ��ǰ���ڴ�����һ����modbusNum��ʾ��ǰ��������һ���ж�����
extern MMODBUS *emergencyTreat[EMERTENCY_NUM]; // �����д�Ĵ�������ĳһ֡���ݻ��浽������������飬����ˢ�³��ֱȽϴ����ʾ
extern u8 emergencyTail, emergencyHead;		   // ѭ������ͷ��β


extern uint8_t synthesisCollection_Comm_Sta;
extern uint8_t DC_Insulation_Comm_Sta[DC_INSULATION_MAX];
extern uint8_t switchModule_Comm_Sta[SWITCH_MODULE_MAX];
extern uint8_t battery_Comm_Sta[5];
extern uint8_t dc_4850_Module_Comm_Sta[DC4850MODULE_MAX];
extern uint8_t remoteControlModule_Comm_Sta[REMOTE_CONTROL_MODULE_MAX];
extern uint8_t chargeModule_Comm_Sta[CHARGER_MODULE_MAX];
extern uint8_t ups_Comm_Sta[UPS_MAX];
extern uint8_t inv_Comm_Sta[INV_MAX];
extern uint8_t AC_Insulation_Comm_Sta;

void Uart485RxTreat(void);
void modbusTreat(void);
void pushToEmergency(MMODBUS *modbusOrder);
u8 isEmergencyFull(void);
void clrEmergency(void);
#endif