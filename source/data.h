#ifndef __DATA_H
#define __DATA_H
#include "sys.h"

extern struct
{
    uint16_t sysVoltageGrade; // ϵͳ��ѹ�ȼ�
    uint16_t language;
    uint16_t siliconChain;                        // ��������
    uint16_t synthesisMeasurementRelayOutput[16]; // �ۺϲ����̵������1-8
    uint16_t synthesisMeasurementRelayInput22;    // �ۺϲ����̵�������22
} sysInfoSet;                                     // ϵͳ��Ϣ

extern struct
{
    uint16_t AC_InputChannel;                // ��������·��
    uint16_t threePhaseAC_VolMax;            // ���ཻ����ѹֵ
    uint16_t threePhaseAC_VolMin;            // ���ཻ��Ƿѹֵ
    uint16_t singlePhaseThreePhaseSelection; // ��/����ѡ��
    uint16_t singlePhaseAC_VolMax;           // ���ཻ����ѹֵ
    uint16_t singlePhaseAC_VolMin;           // ���ཻ��Ƿѹֵ
} AC_InfoSet;                                // AC������Ϣ

extern struct
{
    uint16_t siliconChainsSet5or7; // ��������5����7��
    uint16_t closeBusVoltMax;      // ��ĸ��ѹֵ
    uint16_t closeBusVoltMin;      // ��ĸ��ѹֵ
    uint16_t controlBusVoltMax;    // ��ĸ��ѹֵ
    uint16_t controlBusVoltMin;    // ��ĸǷѹֵ
    uint16_t reserve[4];
    uint16_t loadHallRatio;    // ���ػ����任����
    uint16_t batteryHallRatio; // ��ػ����任����
    uint16_t reserve1;         // �ճ�ż���ֳ���
} DC_InfoSet;

extern struct
{
    uint16_t nominalCapacity;          // �������
    uint16_t batteryVoltMax;           // ��ع�ѹֵ
    uint16_t batteryVoltMin;           // ���Ƿѹֵ
    uint16_t equalizeChargeVolt;       // �����ѹֵ
    uint16_t floatChargeVolt;          // �����ѹֵ
    uint16_t equalizeChargeHour;       // ����ʱ�䣬Сʱ
    uint16_t floatChargeDay;           // ����ʱ��,��
    uint16_t chargingCurrentLimit;     // ����������ֵ
    uint16_t floatToEqualizeChargeCur; // ����ת������
    uint16_t equalizeToFloatChargeCur; // ����ת��������
} batteryInfoSet;

extern struct
{
    uint16_t synthesisMeasurementTempDisplay; // �ۺϲ����¶���ʾ
    uint16_t batteryTempDisplay;              // ����¶���ʾ
    // uint16_t closeBusVoltRectify;             // ��բĸ�ߵ�ѹ����
    // uint16_t controlBusVoltRectify;           // ��ĸ��ѹ����
    // uint16_t busToGroundVoltRectify;          // ĸ�߶Եص�ѹ����
    // uint16_t batteryVoltRectify;              // ��ص�ѹ����
    // uint16_t AC_1_Ua_Rectify;                 // ����1·A���ѹ����
    // uint16_t AC_1_Ub_Rectify;                 // ����1·A���ѹ����
    // uint16_t AC_1_Uc_Rectify;                 // ����1·A���ѹ����
    // uint16_t AC_2_Ua_Rectify;                 // ����2·A���ѹ����
    // uint16_t AC_2_Ub_Rectify;                 // ����2·A���ѹ����
    // uint16_t AC_2_Uc_Rectify;                 // ����2·A���ѹ����
} specialParaSet;

extern uint16_t backLightTime[2];

extern struct
{
    struct
    {
        uint16_t switchAccessMode;  // ���ؽ���ģʽ
        uint16_t alarmChannelNum;   // ����·��
        uint16_t remoteControlType; // Զ�ص�Ԫ����
        uint16_t remoteControlNum;  // Զ�ص�Ԫ����
        uint16_t reserve[0x10 - 4];
    } synthesisCollection; // �ۺϼ��
    struct
    {
        uint16_t switchNum;          // ����������
        uint16_t accessMode[4];      // 1#���ؽ���ģʽ
        uint16_t alarmChannelNum[4]; // 1#����·��
        uint16_t reserve;
    } switchModule; // ������
} switchModuleSet;  // ����������

extern struct
{
    uint16_t batteryType; // ���Ѳ������ :XJ24/XJ55
    uint16_t reserve[0x10 - 1];
    struct
    {
        uint16_t batteryModuleNum;  // ���Ѳ������
        uint16_t cellNum;           // ��ؽ���
        uint16_t singleCellVoltMax; // ��ֻ��ع�ѹֵ
        uint16_t singleCellVoltMin; // ��ֻ���Ƿѹֵ
        uint16_t endCellNum;        // ĩ�˵������
        uint16_t endCellVoltMax;    // ĩ�˵�ع�ѹֵ
        uint16_t endCellVoltMin;    // ĩ�˵���Ƿѹֵ
        uint16_t reserve[0x10 - 7];
    } xj24;
    struct
    {
        uint16_t batteryModuleNum;  // ���Ѳ������
        uint16_t cellNum;           // ��ؽ���
        uint16_t singleCellVoltMax; // ��ֻ��ع�ѹֵ
        uint16_t singleCellVoltMin; // ��ֻ���Ƿѹֵ
        uint16_t endCellNum;        // ĩ�˵������
        uint16_t endCellVoltMax;    // ĩ�˵�ع�ѹֵ
        uint16_t endCellVoltMin;    // ĩ�˵���Ƿѹֵ
        uint16_t reserve;
    } xj55;
} batterySet; // �������

extern struct
{
    struct
    {
        uint16_t Num;               // ��Ե����
        uint16_t voltToGroundAlarm; // �����Եظ澯ֵ
        uint16_t reserve[0x10 - 2];
    } AC;
    struct
    {
        uint16_t Num;                   // ��Ե����
        uint16_t busVoltDifference;     // ĸ��ѹ��
        uint16_t resAlarm;              // ��Ե�澯ֵ
        uint16_t closeBus_1_ChannelNum; // 1#��ĸ·��
        uint16_t closeBus_2_ChannelNum; // 2#��ĸ·��
        uint16_t reserve;
    } DC;
} insulationSet; // ��Ե����

extern struct
{
    uint16_t moduleType; // ���ģ������
    uint16_t moduleNum;  // ���ģ������
    uint16_t ratedCurr;  // �����
    uint16_t MaxCurr;    // ģ���������
} chargeModuleSet;       // ���ģ������

extern struct
{
    uint16_t Num;           // ģ������
    uint16_t voltBroadcast; // �㲥��ѹ
    uint16_t currBroadcast; // �㲥����
    uint16_t reaserve;
} DC_ConverterSet; // DC�任��

extern struct
{
    uint16_t UPS_Num;
    uint16_t INV_Num;
} UPS_INV_Set;

extern uint16_t alarmSoundOnOff[2];

// ҳ��
#define HOME_PAGE 0
#define SYS_FUNCTION_TABLE_PAGE 2                    // ϵͳ���ܱ�ҳ��
#define SYS_INFO_PAGE 11                             // ϵͳ����ҳ��
#define BATTERY_MANAGER_PAGE 22                      // ��ع���
#define SWITCH_MODULE_SET_PAGE 35                    // ����������ҳ��
#define SERIES_BATTERY_INSPECTION_DEVICE_SET_PAGE 39 // ���Ѳ�죨����ǣ�����ҳ��
#define INSULATION_SET_PAGE 42                       // ��Ե����
#define CHARGER_MODULE_SET_PAGE 44                   // ���ģ������
#define DC_MODULE_SET_PAGE 48
#define UPS_INV_SET_PAGE 50
#define RECOVERY_PAGE 52
#define REAL_TIME_ALARM_PAGE 59     // ʵʱ����ҳ��
#define NON_INSULATION_DATA_PAGE 63 // �޾�Ե��Ϣ����
#define CHARGE_DATA_PAGE 70         // ���ģ������
#define SWITCH_MODULE_DATA_PAGE 72
#define DC_BUS_DATA_PAGE 80 // ֱ��ĸ����Ϣҳ��
#define UPS_DATA_PAGE 82
#define DC_CONVERTER_DATA_PAGE 86
#define INSULATION_DATA_PAGE 87 // ��Ե��Ϣ����
#define BATTERY_DATA_PAGE 94
#define PASSWORD_LOGIN_PAGE 100
#define SCREEN_PROTECTION_PAGE 116

// flash�洢��ַ�볤��
#define SYS_INFO_NORFLASH_ADDR 0x0100
#define SYS_INFO_SIZE 12 // �ֳ���,����Ϊż��
#define AC_INFO_NORFLASH_ADDR 0x150
#define AC_INFO_SIZE 6
#define DC_INFO_NORFLASH_ADDR 0x1A0
#define DC_INFO_SIZE 12
#define BATTERY_INFO_NORFLASH_ADDR 0x200
#define BATTERY_INFO_SIZE 10
#define SPECIAL_PARA_NORFLASH_ADDR 0x250
#define SPECIAL_PARA_SIZE 2
#define BACKLIGHT_TIME_NORFLASH_ADDR 0x2A0
#define SWITCH_MODULE_NORFLASH_ADDR 0x300
#define SWITCH_MODULE_SIZE (sizeof(switchModuleSet) / 2 + sizeof(switchModuleSet) / 2 % 2)
#define BATTERY_SET_NORFLASH_ADDR 0x350
#define BATTERY_SET_SIZE (sizeof(batterySet) / 2 + sizeof(batterySet) / 2 % 2)
#define INSULATION_SET_NORFLASH_ADDR 0x3A0
#define INSULATION_SET_SIZE 0x16
#define CHARGE_MODULE_SET_NORFLASH_ADDR 0x400
#define CHARGE_MODULE_SET_SIZE 6
#define DC_CONVERTER_SET_NORFLASH_ADDR 0x450
#define DC_CONVERTER_SET_SIZE 4
#define UPS_INV_SET_NORFLASH_ADDR 0x4A0
#define UPS_INV_SET_SIZE 2
#define ALARM_SOUND_ONOFF_NORFLASH_ADDR 0x500
#define SLAVE_ID_NORFLASH_ADDR 0x550

// VP
#define SYS_INFO_VP 0x9000
#define AC_INFO_VP 0x9050
#define DC_INFO_VP 0x90A0
#define BATTERY_INFO_VP 0x9100
#define SPECIAL_PARA_VP 0x91A0
#define BACKLIGHT_TIME_SET_VP 0x91F0
#define SWITCH_MODULE_SET_VP 0x9200
#define BATTERY_SET_VP 0x9250
#define INSULATION_SET_VP 0x92A0
#define CHARGE_MODULE_SET_VP 0x9300
#define DC_CONVERTER_SET_VP 0x9350
#define UPS_INV_SET_VP 0x93A0

#endif
