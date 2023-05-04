#include "sys.h"

struct
{
    uint16_t sysVoltageGrade; // ϵͳ��ѹ�ȼ�
    uint16_t language;
    uint16_t siliconChain;                        // ��������
    uint16_t synthesisMeasurementRelayOutput[16]; // �ۺϲ����̵������1-8
    // uint16_t synthesisMeasurementRelayOutput2; // �ۺϲ����̵������2
    // uint16_t synthesisMeasurementRelayOutput3; // �ۺϲ����̵������3
    // uint16_t synthesisMeasurementRelayOutput4; // �ۺϲ����̵������4
    // uint16_t synthesisMeasurementRelayOutput5; // �ۺϲ����̵������5
    // uint16_t synthesisMeasurementRelayOutput6; // �ۺϲ����̵������6
    // uint16_t synthesisMeasurementRelayOutput7; // �ۺϲ����̵������7
    // uint16_t synthesisMeasurementRelayOutput8; // �ۺϲ����̵������8
    uint16_t synthesisMeasurementRelayInput22; // �ۺϲ����̵�������22
} sysInfoSet = {0};                            // ϵͳ��Ϣ

struct
{
    uint16_t AC_InputChannel;                // ��������·��
    uint16_t threePhaseAC_VoltMax;           // ���ཻ����ѹֵ
    uint16_t threePhaseAC_VoltMin;           // ���ཻ��Ƿѹֵ
    uint16_t singlePhaseThreePhaseSelection; // ��/����ѡ��
    uint16_t singlePhaseAC_VoltMax;          // ���ཻ����ѹֵ
    uint16_t singlePhaseAC_VoltMin;          // ���ཻ��Ƿѹֵ
} AC_InfoSet = {0};                          // AC������Ϣ

struct
{
    uint16_t siliconChainsSet5or7; // ��������5����7��
    uint16_t closeBusVoltMax;      // ��ĸ��ѹֵ
    uint16_t closeBusVoltMin;      // ��ĸ��ѹֵ
    uint16_t controlBusVoltMax;    // ��ĸ��ѹֵ
    uint16_t controlBusVoltMin;    // ��ĸǷѹֵ
    uint16_t reserve[4];

    // uint16_t busVoltMax_220V;    // ĸ�߹�ѹֵ220
    // uint16_t busVoltMin_220V;    // ĸ��Ƿѹֵ220
    // uint16_t moduleVoltMax_220V; // ģ���ѹֵ220
    // uint16_t moduleVoltMin_220V; // ģ��Ƿѹֵ220
    // uint16_t busVoltMax_110V;    // ĸ�߹�ѹֵ110
    // uint16_t busVoltMin_110V;    // ĸ��Ƿѹֵ110
    // uint16_t moduleVoltMax_110V; // ģ���ѹֵ110
    // uint16_t moduleVoltMin_110V; // ģ��Ƿѹֵ110

    uint16_t loadHallRatio;    // ���ػ����任����
    uint16_t batteryHallRatio; // ��ػ����任����
    uint16_t reserve1;
} DC_InfoSet = {0};

struct
{
    uint16_t nominalCapacity;          // �������
    uint16_t batteryVoltMax;           // ��ع�ѹֵ
    uint16_t batteryVoltMin;           // ���Ƿѹֵ
    uint16_t equalizeChargeVolt;       // �����ѹֵ
    uint16_t floatChargeVolt;          // �����ѹֵ
    uint16_t equalizeChargeHour;       // ����ʱ��
    uint16_t floatChargeDay;           // ����ʱ��
    uint16_t chargingCurrentLimit;     // ����������ֵ
    uint16_t floatToEqualizeChargeCur; // ����ת������
    uint16_t equalizeToFloatChargeCur; // ����ת��������
} batteryInfoSet = {0};

struct
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
} specialParaSet = {0};

uint16_t backLightTime[2] = {0}; // �������ʱ��

struct
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
        uint16_t switchNum;     // ����������
        uint16_t accessMode[4]; // 1#���ؽ���ģʽ
        // uint16_t sw_1_AccessMode;      // 1#���ؽ���ģʽ
        // uint16_t sw_2_AccessMode;      // 2#���ؽ���ģʽ
        // uint16_t sw_3_AccessMode;      // 3#���ؽ���ģʽ
        // uint16_t sw_4_AccessMode;      // 4#���ؽ���ģʽ
        uint16_t alarmChannelNum[4]; // 1#����·��
        // uint16_t sw_1_AlarmChannelNum; // 1#����·��
        // uint16_t sw_2_AlarmChannelNum; // 2#����·��
        // uint16_t sw_3_AlarmChannelNum; // 3#����·��
        // uint16_t sw_4_AlarmChannelNum; // 4#����·��
        uint16_t reserve;
    } switchModule;      // ������
} switchModuleSet = {0}; // ����������

struct
{
    uint16_t batteryType; // ���Ѳ������
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

struct
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

struct
{
    uint16_t moduleType; // ���Ĩ������
    uint16_t moduleNum;  // ���ģ������
    uint16_t ratedCurr;  // �����
    uint16_t MaxCurr;    // ģ���������
} chargeModuleSet;       // ���ģ������

struct
{
    uint16_t Num;           // ģ������
    uint16_t voltBroadcast; // �㲥��ѹ
    uint16_t currBroadcast; // �㲥����
    uint16_t reaserve;
} DC_ConverterSet; // DC�任��

struct
{
    uint16_t UPS_Num;
    uint16_t INV_Num;
} UPS_INV_Set;