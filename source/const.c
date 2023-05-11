#include "const.h"
#include "norflash.h"
#include "data.h"
#include "ui.h"
#include "debug.h"
#include "modbus.h"

void modbusCenerate(void)
{
    uint16_t num = 0;
    {                                        // �ۺϲ�����Ԫ
        pageModbusReg[num].SlaveAddr = 0x61; // 97
        pageModbusReg[num].mode = 0x00;
        pageModbusReg[num].flag = 0x00;
        pageModbusReg[num].Order = 0x03;
        pageModbusReg[num].Length = 0x12;
        pageModbusReg[num].reserved = 0x00;
        pageModbusReg[num].waitTime = 1000;
        pageModbusReg[num].VPaddr = 0;
        pageModbusReg[num].ModbusReg = 0x0000;
        pageModbusReg[num].databuff = (uint16_t *)&synthesisCollection;
        num++;
    }
    { // ֧·��Ե
        uint16_t i;
        for (i = 0; i < insulationSet.DC.Num; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                if (i == 0)
                {
                    pageModbusReg[num].SlaveAddr = 0x60;
                }
                else if (i >= 1)
                {
                    pageModbusReg[num].SlaveAddr = 0x60 + i + 1; // ����0x61��0x61Ϊ�ۺϲɼ���Ԫ
                }
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x21;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(DC_Insulation + i);
                num++;
            }
        }
    }
    { // ������
        uint16_t i;
        // DEBUGINFO("switchModuleSet.switchModule.switchNum = %d \n", (uint16_t)switchModuleSet.switchModule.switchNum);
        for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
        {                           // u8 SlaveAddr;u8 mode;u8 flag;u8 Order;u8 Length;u8 reserved;u16 waitTime;u16 VPaddr;u16 ModbusReg;
            if (num < PAGE_MAX_NUM) // Խ�籣��
            {
                pageModbusReg[num].SlaveAddr = 0xA0 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x03;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(switchModule + i);
                num++;
            }
        }
        // DEBUGINFO("switchModuleSet.switchModule.switchNum = %d \n", (uint16_t)switchModuleSet.switchModule.switchNum);
        // DEBUGINFO("%d \n", (uint16_t)num);
    }
    if (batterySet.batteryType == XJ24)
    { // XJ24
        uint16_t i;
        for (i = 0; i < batterySet.xj24.batteryModuleNum; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 0x70 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x19;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(battery_xj24 + i);
                num++;
            }
        }
    }
    else if (batterySet.batteryType == XJ55)
    { // XJ55
        uint16_t i;
        for (i = 0; i < batterySet.xj55.batteryModuleNum; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 0x70 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x37;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(battery_xj55 + i);
                num++;
            }
        }
    }

    { // DC4850ģ��
        uint16_t i;
        for (i = 0; i < DC_ConverterSet.Num; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 0x90 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x07;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(dc_4850_Module + i);
                num++;
            }
        }
    }

    { // Զ�ص�Ԫ
        uint16_t i;
        for (i = 0; i < switchModuleSet.synthesisCollection.remoteControlNum; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                if (switchModuleSet.synthesisCollection.remoteControlType == ARD)
                {
                    pageModbusReg[num].SlaveAddr = 0xD0 + i;
                    pageModbusReg[num].mode = 0x00;
                    pageModbusReg[num].flag = 0x00;
                    pageModbusReg[num].Order = 0x03;
                    pageModbusReg[num].Length = 0x01;
                    pageModbusReg[num].reserved = 0x00;
                    pageModbusReg[num].waitTime = 1000;
                    pageModbusReg[num].VPaddr = 0;
                    pageModbusReg[num].ModbusReg = 0x0002;
                    pageModbusReg[num].databuff = (uint16_t *)(remoteControlModule_ARD + i);
                    num++;
                }
                else if (switchModuleSet.synthesisCollection.remoteControlType == TH)
                {
                    pageModbusReg[num].SlaveAddr = 0xD0 + i;
                    pageModbusReg[num].mode = 0x00;
                    pageModbusReg[num].flag = 0x00;
                    pageModbusReg[num].Order = 0x03;
                    pageModbusReg[num].Length = 0x02;
                    pageModbusReg[num].reserved = 0x00;
                    pageModbusReg[num].waitTime = 1000;
                    pageModbusReg[num].VPaddr = 0;
                    pageModbusReg[num].ModbusReg = 0x0000;
                    pageModbusReg[num].databuff = (uint16_t *)(remoteControlModule_TH + i);
                    num++;
                }
            }
        }
    }

    { // ���ģ��
        uint16_t i;
        for (i = 0; i < chargeModuleSet.moduleNum; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 0x01 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x08;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(chargeModule + i);
                num++;
            }
        }
    }

    {
        uint16_t i;
        for (i = 0; i < UPS_INV_Set.UPS_Num; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 26 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 0x0B;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0000;
                pageModbusReg[num].databuff = (uint16_t *)(&ups[i].remoteMeasurement);
                num++;
            }
        }
    }
    {
        uint16_t i;
        for (i = 0; i < UPS_INV_Set.UPS_Num; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 26 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x02;
                pageModbusReg[num].Length = 0x10;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0300;
                pageModbusReg[num].databuff = (uint16_t *)(&ups[i].remoteSignal);
                num++;
            }
        }
    }
    {
        uint16_t i;
        for (i = 0; i < UPS_INV_Set.INV_Num; i++)
        {
            if (num < PAGE_MAX_NUM)
            {
                pageModbusReg[num].SlaveAddr = 0x80 + i;
                pageModbusReg[num].mode = 0x00;
                pageModbusReg[num].flag = 0x00;
                pageModbusReg[num].Order = 0x03;
                pageModbusReg[num].Length = 21;
                pageModbusReg[num].reserved = 0x00;
                pageModbusReg[num].waitTime = 1000;
                pageModbusReg[num].VPaddr = 0;
                pageModbusReg[num].ModbusReg = 0x0001;
                pageModbusReg[num].databuff = (uint16_t *)(&inv[i]);
                num++;
            }
        }
    }
    {
        if (num < PAGE_MAX_NUM)
        {
            pageModbusReg[num].SlaveAddr = 0x32;
            pageModbusReg[num].mode = 0x00;
            pageModbusReg[num].flag = 0x00;
            pageModbusReg[num].Order = 0x03;
            pageModbusReg[num].Length = 4;
            pageModbusReg[num].reserved = 0x00;
            pageModbusReg[num].waitTime = 1000;
            pageModbusReg[num].VPaddr = 0;
            pageModbusReg[num].ModbusReg = 0x0000;
            pageModbusReg[num].databuff = (uint16_t *)(&AC_Insulation);
            num++;
        }
    }
    modbusNum = num;
}

struct
{
    uint16_t closeBusVolt;       // ��բĸ�ߵ�ѹ
    uint16_t controlBusVolt;     // ��ĸ��ѹ
    uint16_t busToGroundVolt;    // ĸ�߶Եص�ѹ
    uint16_t batteryVolt;        // ������ѹ
    uint16_t controlBusCurr;     // ��ĸ����
    int16_t batteryCurr;         // ��ص���
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
    uint16_t sw_01to16;          // ������1��16·
    uint16_t sw_17to24;          // ������17��24·
    // uint16_t sw1 : 1;            // ��������1·
    // uint16_t sw2 : 1;            // ��������2·
    // uint16_t sw3 : 1;            // ��������3·
    // uint16_t sw4 : 1;            // ��������4·
    // uint16_t sw5 : 1;            // ��������5·
    // uint16_t sw6 : 1;            // ��������6·
    // uint16_t sw7 : 1;            // ��������7·
    // uint16_t sw8 : 1;
    // uint16_t sw9 : 1;
    // uint16_t sw10 : 1;
    // uint16_t sw11 : 1;
    // uint16_t sw12 : 1;
    // uint16_t sw13 : 1;
    // uint16_t sw14 : 1;
    // uint16_t sw15 : 1;
    // uint16_t sw16 : 1;
    // uint16_t sw17 : 1;
    // uint16_t sw18 : 1;
    // uint16_t sw19 : 1;
    // uint16_t sw20 : 1;
    // uint16_t sw21 : 1;
    // uint16_t sw22 : 1;
    // uint16_t sw23 : 1;
    // uint16_t sw24 : 1;
    // uint16_t reserve3 : 8;
} synthesisCollection; // �ۺϲ���

struct
{
    uint16_t closeBusToGroundVolt;   // ��ĸ�Եص�ѹ
    uint16_t controlBusToGroundVolt; // ����ĸ�Եص�ѹ
    uint16_t busToGroundVolt;        // ĸ�߶Եص�ѹ
    uint16_t res[30];                // ��Ե��·ֵ30·
    // uint16_t channel_1_Res;                // ��1·��Ե©����ֵ
    // uint16_t channel_2_Res;                // ��2·��Ե©����ֵ
    // uint16_t channel_3_Res;                // ��3·��Ե©����ֵ
    // uint16_t channel_4_Res;                // ��4·��Ե©����ֵ
    // uint16_t channel_5_Res;                // ��5·��Ե©����ֵ
    // uint16_t channel_6_Res;                // ��6·��Ե©����ֵ
    // uint16_t channel_7_Res;                // ��7·��Ե©����ֵ
    // uint16_t channel_8_Res;                // ��8·��Ե©����ֵ
    // uint16_t channel_9_Res;                // ��9·��Ե©����ֵ
    // uint16_t channel_10_Res;               // ��10·��Ե©����ֵ
    // uint16_t channel_11_Res;               // ��11·��Ե©����ֵ
    // uint16_t channel_12_Res;               // ��12·��Ե©����ֵ
    // uint16_t channel_13_Res;               // ��13·��Ե©����ֵ
    // uint16_t channel_14_Res;               // ��14·��Ե©����ֵ
    // uint16_t channel_15_Res;               // ��15·��Ե©����ֵ
    // uint16_t channel_16_Res;               // ��16·��Ե©����ֵ
    // uint16_t channel_17_Res;               // ��17·��Ե©����ֵ
    // uint16_t channel_18_Res;               // ��18·��Ե©����ֵ
    // uint16_t channel_19_Res;               // ��19·��Ե©����ֵ
    // uint16_t channel_20_Res;               // ��20·��Ե©����ֵ
    // uint16_t channel_21_Res;               // ��21·��Ե©����ֵ
    // uint16_t channel_22_Res;               // ��22·��Ե©����ֵ
    // uint16_t channel_23_Res;               // ��23·��Ե©����ֵ
    // uint16_t channel_24_Res;               // ��24·��Ե©����ֵ
    // uint16_t channel_25_Res;               // ��25·��Ե©����ֵ
    // uint16_t channel_26_Res;               // ��26·��Ե©����ֵ
    // uint16_t channel_27_Res;               // ��27·��Ե©����ֵ
    // uint16_t channel_28_Res;               // ��28·��Ե©����ֵ
    // uint16_t channel_29_Res;               // ��29·��Ե©����ֵ
    // uint16_t channel_30_Res;               // ��30·��Ե©����ֵ
} DC_Insulation[DC_INSULATION_MAX]; // DC֧·��Ե

struct
{
    uint16_t sw_u16[3];
} switchModule[SWITCH_MODULE_MAX]; // ������

struct
{
    int16_t volt[24];    // 24�ڵ�ص�ѹ
    int16_t temperature; // �¶�
    // uint16_t No_1_Volt;  // ��1�ڵ�ص�ѹ
    // uint16_t No_2_Volt;  // ��2�ڵ�ص�ѹ
    // uint16_t No_3_Volt;  // ��3�ڵ�ص�ѹ
    // uint16_t No_4_Volt;  // ��4�ڵ�ص�ѹ
    // uint16_t No_5_Volt;  // ��5�ڵ�ص�ѹ
    // uint16_t No_6_Volt;  // ��6�ڵ�ص�ѹ
    // uint16_t No_7_Volt;  // ��7�ڵ�ص�ѹ
    // uint16_t No_8_Volt;  // ��8�ڵ�ص�ѹ
    // uint16_t No_9_Volt;  // ��9�ڵ�ص�ѹ
    // uint16_t No_10_Volt; // ��10�ڵ�ص�ѹ
    // uint16_t No_11_Volt; // ��11�ڵ�ص�ѹ
    // uint16_t No_12_Volt; // ��12�ڵ�ص�ѹ
    // uint16_t No_13_Volt; // ��13�ڵ�ص�ѹ
    // uint16_t No_14_Volt; // ��14�ڵ�ص�ѹ
    // uint16_t No_15_Volt; // ��15�ڵ�ص�ѹ
    // uint16_t No_16_Volt; // ��16�ڵ�ص�ѹ
    // uint16_t No_17_Volt; // ��17�ڵ�ص�ѹ
    // uint16_t No_18_Volt; // ��18�ڵ�ص�ѹ
    // uint16_t No_19_Volt; // ��19�ڵ�ص�ѹ
    // uint16_t No_20_Volt; // ��20�ڵ�ص�ѹ
    // uint16_t No_21_Volt; // ��21�ڵ�ص�ѹ
    // uint16_t No_22_Volt; // ��22�ڵ�ص�ѹ
    // uint16_t No_23_Volt; // ��23�ڵ�ص�ѹ
    // uint16_t No_24_Volt; // ��24�ڵ�ص�ѹ
} battery_xj24[BATTERY_XJ24_MAX];

struct
{
    int16_t volt[55]; // 55�ڵ�ص�ѹ
    // uint16_t No_1_Volt;  // ��1�ڵ�ص�ѹ
    // uint16_t No_2_Volt;  // ��2�ڵ�ص�ѹ
    // uint16_t No_3_Volt;  // ��3�ڵ�ص�ѹ
    // uint16_t No_4_Volt;  // ��4�ڵ�ص�ѹ
    // uint16_t No_5_Volt;  // ��5�ڵ�ص�ѹ
    // uint16_t No_6_Volt;  // ��6�ڵ�ص�ѹ
    // uint16_t No_7_Volt;  // ��7�ڵ�ص�ѹ
    // uint16_t No_8_Volt;  // ��8�ڵ�ص�ѹ
    // uint16_t No_9_Volt;  // ��9�ڵ�ص�ѹ
    // uint16_t No_10_Volt; // ��10�ڵ�ص�ѹ
    // uint16_t No_11_Volt; // ��11�ڵ�ص�ѹ
    // uint16_t No_12_Volt; // ��12�ڵ�ص�ѹ
    // uint16_t No_13_Volt; // ��13�ڵ�ص�ѹ
    // uint16_t No_14_Volt; // ��14�ڵ�ص�ѹ
    // uint16_t No_15_Volt; // ��15�ڵ�ص�ѹ
    // uint16_t No_16_Volt; // ��16�ڵ�ص�ѹ
    // uint16_t No_17_Volt; // ��17�ڵ�ص�ѹ
    // uint16_t No_18_Volt; // ��18�ڵ�ص�ѹ
    // uint16_t No_19_Volt; // ��19�ڵ�ص�ѹ
    // uint16_t No_20_Volt; // ��20�ڵ�ص�ѹ
    // uint16_t No_21_Volt; // ��21�ڵ�ص�ѹ
    // uint16_t No_22_Volt; // ��22�ڵ�ص�ѹ
    // uint16_t No_23_Volt; // ��23�ڵ�ص�ѹ
    // uint16_t No_24_Volt; // ��24�ڵ�ص�ѹ
    // uint16_t No_25_Volt; // ��25�ڵ�ص�ѹ
    // uint16_t No_26_Volt; // ��26�ڵ�ص�ѹ
    // uint16_t No_27_Volt; // ��27�ڵ�ص�ѹ
    // uint16_t No_28_Volt; // ��28�ڵ�ص�ѹ
    // uint16_t No_29_Volt; // ��29�ڵ�ص�ѹ
    // uint16_t No_30_Volt; // ��30�ڵ�ص�ѹ
    // uint16_t No_31_Volt; // ��31�ڵ�ص�ѹ
    // uint16_t No_32_Volt; // ��32�ڵ�ص�ѹ
    // uint16_t No_33_Volt; // ��33�ڵ�ص�ѹ
    // uint16_t No_34_Volt; // ��34�ڵ�ص�ѹ
    // uint16_t No_35_Volt; // ��35�ڵ�ص�ѹ
    // uint16_t No_36_Volt; // ��36�ڵ�ص�ѹ
    // uint16_t No_37_Volt; // ��37�ڵ�ص�ѹ
    // uint16_t No_38_Volt; // ��38�ڵ�ص�ѹ
    // uint16_t No_39_Volt; // ��39�ڵ�ص�ѹ
    // uint16_t No_40_Volt; // ��40�ڵ�ص�ѹ
    // uint16_t No_41_Volt; // ��41�ڵ�ص�ѹ
    // uint16_t No_42_Volt; // ��42�ڵ�ص�ѹ
    // uint16_t No_43_Volt; // ��43�ڵ�ص�ѹ
    // uint16_t No_44_Volt; // ��44�ڵ�ص�ѹ
    // uint16_t No_45_Volt; // ��45�ڵ�ص�ѹ
    // uint16_t No_46_Volt; // ��46�ڵ�ص�ѹ
    // uint16_t No_47_Volt; // ��47�ڵ�ص�ѹ
    // uint16_t No_48_Volt; // ��48�ڵ�ص�ѹ
    // uint16_t No_49_Volt; // ��49�ڵ�ص�ѹ
    // uint16_t No_50_Volt; // ��50�ڵ�ص�ѹ
    // uint16_t No_51_Volt; // ��51�ڵ�ص�ѹ
    // uint16_t No_52_Volt; // ��52�ڵ�ص�ѹ
    // uint16_t No_53_Volt; // ��53�ڵ�ص�ѹ
    // uint16_t No_54_Volt; // ��54�ڵ�ص�ѹ
    // uint16_t No_55_Volt; // ��55�ڵ�ص�ѹ
} battery_xj55[BATTERY_XJ55_MAX];

struct
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

struct
{
    uint16_t sw1to16; // ����״̬1-16
} remoteControlModule_ARD[REMOTE_CONTROL_MODULE_MAX];

struct
{
    uint16_t channel1to16State; // 1-16·״̬
    uint16_t sw1to16;           // 1-16·�̵���״̬
} remoteControlModule_TH[REMOTE_CONTROL_MODULE_MAX];

struct
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

struct
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
        uint16_t bypassOut : 1;          // ��·���
        uint16_t rectifyInverterOut : 1; // ����������
        uint16_t DC_InverterOut : 1;     // ֱ��������
        uint16_t overTemperature : 1;    // ����
        uint16_t overload : 1;           // ����
        uint16_t bypassFault : 1;        // ��·����
        uint16_t mainFault : 1;          // ��·����
        uint16_t batteryHighVolt : 1;    // ��ظ�ѹ
        uint16_t battertLowVolt : 1;     // ��ص�ѹ
        uint16_t reserve : 7;
    } remoteSignal; // ң��
} ups[UPS_MAX];

struct
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
    struct
    {
        uint16_t onOffState : 1;      // 1���ػ�״̬��0������״̬
        uint16_t workMode : 1;        // �����������ʽ
        uint16_t faultState : 1;      // 1�����ϣ�0������
        uint16_t overload : 1;        // ����״̬
        uint16_t overTemperature : 1; // ����
        uint16_t battertLowVolt : 1;  // ��ص�ѹ
        uint16_t bypassState : 1;     // ��·����״̬
        uint16_t outputMode : 1;      // �����ʽ
        uint16_t reserve : 8;
    } DI;               // ģ�鿪��״̬��DI
    uint16_t config;    // ���üļĴ���
    uint16_t rectify;   // У׼�Ĵ���
    uint16_t mainsVolt; // �е��ѹ
    uint16_t mainsFreq; // �е�Ƶ��
    uint16_t mainsCurr; // �е����
} inv[INV_MAX];

struct
{
    int16_t positiveBusToGroundVolt; // ��ĸ�߽����Եص�ѹ
    int16_t negativeBusToGroundVolt; // ��ĸ�߽����Եص�ѹ
    int16_t positiveBusAlarm : 1;    // ��ĸ�߸澯
    int16_t Reserve : 7;
    int16_t negativeBusAlarm : 1; // ��ĸ�߸澯
    int16_t Reserve1 : 7;
    int16_t alarmValue; // ĸ�߶Եؽ�����ѹ�澯����ֵ
} AC_Insulation;        // AC��Ե
