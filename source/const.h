#ifndef __CONST_H__
#define __CONST_H__
#include "sys.h"
#include "modbus.h"

#define SYNTHESIS_MODULE_VP 0x10000       // �ۺ�ģ��VP
#define BRANCH_INSULATION_VP 0x10030      // ֧·��ԵVP
#define BRANCH_INSULATION_OFFSET 35       // ��֧·��Եģ������ƫ��
#define BRANCH_INSULATION_MAX 2           // ֧·��Ե����
#define SWITCH_MODULE_VP 0x10200          // ������VP
#define SWITCH_MODULE_MAX 4               // ������ģ���������4
#define SWITCH_MODULE_OFFSET 5            // ��������ģ������ƫ��,���4��
#define BATTERY_XJ24_VP 0x10300           // ���Ѳ��XJ24 VP
#define BATTERY_XJ24_MAX 5                // ���Ѳ��XJ24�������5��
#define BATTERY_XJ24_OFFSET 0x19          // ���Ѳ��XJ24ģ������ƫ�ƣ����5��
#define BATTERY_XJ55_VP 0x10400           // ���Ѳ��XJ55 VP
#define BATTERY_XJ55_MAX 2                // ���Ѳ��XJ55ģ���������2��
#define BATTERY_XJ55_OFFSET 0x37          // ���Ѳ��XJ55ģ������ƫ�ƣ����2��
#define DC4850MODULE_VP 0x10600           // DC4850ģ��VP
#define DC4850MODULE_MAX 4                // DC4850ģ���������4��
#define DC4850MODULE_OFFSET 0x03          // DC4850ģ������ƫ�ƣ����4��
#define REMOTE_CONTROL_MODULE_VP 0x10700  // Զ�ص�ԪVP
#define REMOTE_CONTROL_MODULE_MAX 2       // Զ��ģ���������2��
#define REMOTE_CONTROL_MODULE_OFFSET 10 // Զ��ģ������ƫ�ƣ����2��
#define CHARGER_MODULE_VP 0x10800         // ���ģ��
#define CHARGER_MODULE_MAX 8              // ���ģ���������8��
#define CHARGER_MODULE_OFFSET 8           // ���ģ������ƫ�ƣ����8��
#define UPS_REMOTE_MEASUREMENT_VP 0x10900
#define UPS_MAX 2
#define UPS_REMOTE_MEASUREMENT_OFFSET 30
#define UPS_REMOTE_SIGNAL_VP 0x10A00
#define UPS_REMOTE_SIGNAL_OFFSET 20
#define INV_VP 0x10B00
#define INV_MAX 2
#define INV_OFFSET 30

void modbusCenerate(void);

extern struct
{
    uint16_t closeBusVolt;       // ��բĸ�ߵ�ѹ
    uint16_t controlBusVolt;     // ��ĸ��ѹ
    uint16_t busToGroundVolt;    // ĸ�߶Եص�ѹ
    uint16_t batteryVolt;        // ������ѹ
    uint16_t controlBusCurr;     // ��ĸ����
    uint16_t batteryCurr;        // ��ص���
    uint16_t AC_channel_1_Uab;   // ����1·Uab
    uint16_t AC_channel_1_Ubc;   // ����1·Ubc
    uint16_t AC_channel_1_Uca;   // ����1·Uca
    uint16_t AC_channel_2_Uab;   // ����2·Uab
    uint16_t AC_channel_2_Ubc;   // ����2·Ubc
    uint16_t AC_channel_2_Uca;   // ����2·Uca
    uint16_t AC_Curr;            // ��������
    uint16_t reserve1;           // ����
    uint16_t reserve2;           // ����
    uint16_t batteryTemperature; // ����¶�
    uint16_t sw_01to16;           // ������1��16·
    uint16_t sw_17to24;          // ������17��24·
} synthesisCollection;           // �ۺϲ���

extern struct
{
    uint16_t closeBusToGroundVolt;         // ��ĸ�Եص�ѹ
    uint16_t controlBusToGroundVolt;       // ����ĸ�Եص�ѹ
    uint16_t busToGroundVolt;              // ĸ�߶Եص�ѹ
    uint16_t res[30];                      // ��Ե��·ֵ30·
} branchInsulation[BRANCH_INSULATION_MAX]; // ֧·��Ե

extern struct
{
    uint16_t sw_u16[3];
} switchModule[SWITCH_MODULE_MAX]; // ������

extern struct
{
    int16_t volt[24];    // 24�ڵ�ص�ѹ
    int16_t temperature; // �¶�
} battery_xj24[BATTERY_XJ24_MAX];

extern struct
{
    int16_t volt[55]; // 55�ڵ�ص�ѹ
} battery_xj55[BATTERY_XJ55_MAX];

extern struct
{
    uint16_t outputVolt;          // ģ�������ѹ
    uint16_t outputCurr;          // ģ���������
    uint16_t currLimitPercentage; // ģ��������ٷ���
    uint16_t voltMax;             // ģ�������ѹ����
    uint16_t voltMin;             // ģ�������ѹ����
    struct
    {
        uint16_t onOffState : 1;   // 1���ػ�״̬��0������״̬
        uint16_t autoOrManual : 1; // 1���ֶ���0���Զ�
        uint16_t protectState : 1; // 1��������0������
        uint16_t faultState : 1;   // 1�����ϣ�0������
        uint16_t reserve : 12;
    } DI;                        // ģ�鿪��״̬��DI
    uint16_t floatChargeVolt;    // ģ�鸡���ѹ
    uint16_t equalizeChargeVolt; // ģ������ѹ
} dc_4850_Module[DC4850MODULE_MAX];

extern struct 
{
    uint16_t sw1to16;//����״̬1-16
}remoteControlModule_ARD[REMOTE_CONTROL_MODULE_MAX];

extern struct
{
    uint16_t channel1to16State; // 1-16·״̬
    uint16_t sw1to16;           // 1-16·�̵���״̬
} remoteControlModule_TH[REMOTE_CONTROL_MODULE_MAX];

extern struct
{
    uint16_t outputVolt;          // ģ�������ѹ
    uint16_t outputCurr;          // ģ���������
    uint16_t currLimitPercentage; // ģ��������ٷ���
    uint16_t voltMax;             // ģ�������ѹ����
    uint16_t voltMin;             // ģ�������ѹ����
    struct
    {
        uint16_t onOffState : 1;   // 1���ػ�״̬��0������״̬
        uint16_t autoOrManual : 1; // 1���ֶ���0���Զ�
        uint16_t protectState : 1; // 1��������0������
        uint16_t faultState : 1;   // 1�����ϣ�0������
        uint16_t reserve : 12;
    } DI;                        // ģ�鿪��״̬��DI
    uint16_t floatChargeVolt;    // ģ�鸡���ѹ
    uint16_t equalizeChargeVolt; // ģ������ѹ
} chargeModule[CHARGER_MODULE_MAX];

extern struct
{
    struct
    {
        uint16_t interval_Uab;      // ��·AB�ߵ�ѹ���������룩/��·��ѹ���������룩
        uint16_t interval_Ubc;      // ��·BC�ߵ�ѹ���������룩
        uint16_t interval_Uca;      // ��·CA�ߵ�ѹ���������룩
        uint16_t interval_Freq;     // ��·����Ƶ��
        uint16_t bypassVolt;        // ��·�����ѹ
        uint16_t bypassFreq;        // ��·����Ƶ��
        uint16_t outputVolt;        // �����ѹ
        uint16_t outputFreq;        // ���Ƶ��
        uint16_t DC_Volt;           // ֱ����ѹ
        uint16_t outputCurr;        // �������
        uint16_t insideTemperature; // �����¶�
    } remoteMeasurement;            // ң��
    struct
    {
        uint8_t bypassOut : 1;          // ��·���
        uint8_t rectifyInverterOut : 1; // ����������
        uint8_t DC_InverterOut : 1;     // ֱ��������
        uint8_t overTemperature : 1;    // ����
        uint8_t overload : 1;           // ����
        uint8_t bypassFault : 1;        // ��·����
        uint8_t mainFault : 1;          // ��·����
        uint8_t batteryHighVolt : 1;    // ��ظ�ѹ
        uint8_t battertLowVolt : 1;     // ��ص�ѹ
        uint8_t reserve : 7;
    } remoteSignal; // ң��
} ups[UPS_MAX];

extern struct
{
    uint16_t outVolt;           // ���ģ��ģ�������ѹ
    uint16_t outCurr;           // ���ģ��ģ���������
    uint16_t outFreq;           // ���ģ�����Ƶ��
    uint16_t outPowerFactor;    // �����������
    uint16_t inverterVolt;      // ���ģ������ѹ
    uint16_t bypassInputVolt;   // ��·�����ѹ
    uint16_t bypassInputFreq;   // ��·����Ƶ��
    uint16_t batteryInputVolt;  // ��������ѹ
    uint16_t outActivePower;    // ���ģ������й�����
    uint16_t outApparentPowper; // ������ڹ���
    uint16_t outLoadRate;       // ���ģ�����������
    uint16_t temperature;       // ģ���¶�
    uint16_t capacityPercent;   // ģ�����������
    uint16_t bypassMaxVolt;     // ��·��ѹ����
    uint16_t bypassMinVolt;     // ��·��ѹ����
    uint16_t DI;                // ģ�鿪��״̬��DI
    uint16_t config;            // ���üļĴ���
    uint16_t rectify;           // У׼�Ĵ���
    uint16_t mainsVolt;         // �е��ѹ
    uint16_t mainsFreq;         // �е�Ƶ��
    uint16_t mainsCurr;         // �е����
} inv[INV_MAX];

#endif