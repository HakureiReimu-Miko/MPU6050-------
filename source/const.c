#include "const.h"
#include "norflash.h"
#include "data.h"
#include "ui.h"
#include "debug.h"
#include "modbus.h"

void modbusCenerate(void)
{
    uint16_t num = 0;
    {                                        // 综合采样单元
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
    { // 支路绝缘
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
                    pageModbusReg[num].SlaveAddr = 0x60 + i + 1; // 跳过0x61，0x61为综合采集单元
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
    { // 开关量
        uint16_t i;
        // DEBUGINFO("switchModuleSet.switchModule.switchNum = %d \n", (uint16_t)switchModuleSet.switchModule.switchNum);
        for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
        {                           // u8 SlaveAddr;u8 mode;u8 flag;u8 Order;u8 Length;u8 reserved;u16 waitTime;u16 VPaddr;u16 ModbusReg;
            if (num < PAGE_MAX_NUM) // 越界保护
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

    { // DC4850模块
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

    { // 远控单元
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

    { // 充电模块
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
    uint16_t closeBusVolt;       // 合闸母线电压
    uint16_t controlBusVolt;     // 控母电压
    uint16_t busToGroundVolt;    // 母线对地电压
    uint16_t batteryVolt;        // 电池组电压
    uint16_t controlBusCurr;     // 控母电流
    int16_t batteryCurr;         // 电池电流
    uint16_t AC_channel_1_Uab;   // 交流1路Uab
    uint16_t AC_channel_1_Ubc;   // 交流1路Ubc
    uint16_t AC_channel_1_Uca;   // 交流1路Uca
    uint16_t AC_channel_2_Uab;   // 交流2路Uab
    uint16_t AC_channel_2_Ubc;   // 交流2路Ubc
    uint16_t AC_channel_2_Uca;   // 交流2路Uca
    uint16_t AC_Curr;            // 交流电流
    uint16_t reserve1;           // 保留
    uint16_t reserve2;           // 保留
    uint16_t batteryTemperature; // 电池温度
    uint16_t sw_01to16;          // 开关量1到16路
    uint16_t sw_17to24;          // 开关量17到24路
    // uint16_t sw1 : 1;            // 开关量第1路
    // uint16_t sw2 : 1;            // 开关量第2路
    // uint16_t sw3 : 1;            // 开关量第3路
    // uint16_t sw4 : 1;            // 开关量第4路
    // uint16_t sw5 : 1;            // 开关量第5路
    // uint16_t sw6 : 1;            // 开关量第6路
    // uint16_t sw7 : 1;            // 开关量第7路
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
} synthesisCollection; // 综合采样

struct
{
    uint16_t closeBusToGroundVolt;   // 合母对地电压
    uint16_t controlBusToGroundVolt; // 控制母对地电压
    uint16_t busToGroundVolt;        // 母线对地电压
    uint16_t res[30];                // 绝缘电路值30路
    // uint16_t channel_1_Res;                // 第1路绝缘漏电阻值
    // uint16_t channel_2_Res;                // 第2路绝缘漏电阻值
    // uint16_t channel_3_Res;                // 第3路绝缘漏电阻值
    // uint16_t channel_4_Res;                // 第4路绝缘漏电阻值
    // uint16_t channel_5_Res;                // 第5路绝缘漏电阻值
    // uint16_t channel_6_Res;                // 第6路绝缘漏电阻值
    // uint16_t channel_7_Res;                // 第7路绝缘漏电阻值
    // uint16_t channel_8_Res;                // 第8路绝缘漏电阻值
    // uint16_t channel_9_Res;                // 第9路绝缘漏电阻值
    // uint16_t channel_10_Res;               // 第10路绝缘漏电阻值
    // uint16_t channel_11_Res;               // 第11路绝缘漏电阻值
    // uint16_t channel_12_Res;               // 第12路绝缘漏电阻值
    // uint16_t channel_13_Res;               // 第13路绝缘漏电阻值
    // uint16_t channel_14_Res;               // 第14路绝缘漏电阻值
    // uint16_t channel_15_Res;               // 第15路绝缘漏电阻值
    // uint16_t channel_16_Res;               // 第16路绝缘漏电阻值
    // uint16_t channel_17_Res;               // 第17路绝缘漏电阻值
    // uint16_t channel_18_Res;               // 第18路绝缘漏电阻值
    // uint16_t channel_19_Res;               // 第19路绝缘漏电阻值
    // uint16_t channel_20_Res;               // 第20路绝缘漏电阻值
    // uint16_t channel_21_Res;               // 第21路绝缘漏电阻值
    // uint16_t channel_22_Res;               // 第22路绝缘漏电阻值
    // uint16_t channel_23_Res;               // 第23路绝缘漏电阻值
    // uint16_t channel_24_Res;               // 第24路绝缘漏电阻值
    // uint16_t channel_25_Res;               // 第25路绝缘漏电阻值
    // uint16_t channel_26_Res;               // 第26路绝缘漏电阻值
    // uint16_t channel_27_Res;               // 第27路绝缘漏电阻值
    // uint16_t channel_28_Res;               // 第28路绝缘漏电阻值
    // uint16_t channel_29_Res;               // 第29路绝缘漏电阻值
    // uint16_t channel_30_Res;               // 第30路绝缘漏电阻值
} DC_Insulation[DC_INSULATION_MAX]; // DC支路绝缘

struct
{
    uint16_t sw_u16[3];
} switchModule[SWITCH_MODULE_MAX]; // 开关量

struct
{
    int16_t volt[24];    // 24节电池电压
    int16_t temperature; // 温度
    // uint16_t No_1_Volt;  // 第1节电池电压
    // uint16_t No_2_Volt;  // 第2节电池电压
    // uint16_t No_3_Volt;  // 第3节电池电压
    // uint16_t No_4_Volt;  // 第4节电池电压
    // uint16_t No_5_Volt;  // 第5节电池电压
    // uint16_t No_6_Volt;  // 第6节电池电压
    // uint16_t No_7_Volt;  // 第7节电池电压
    // uint16_t No_8_Volt;  // 第8节电池电压
    // uint16_t No_9_Volt;  // 第9节电池电压
    // uint16_t No_10_Volt; // 第10节电池电压
    // uint16_t No_11_Volt; // 第11节电池电压
    // uint16_t No_12_Volt; // 第12节电池电压
    // uint16_t No_13_Volt; // 第13节电池电压
    // uint16_t No_14_Volt; // 第14节电池电压
    // uint16_t No_15_Volt; // 第15节电池电压
    // uint16_t No_16_Volt; // 第16节电池电压
    // uint16_t No_17_Volt; // 第17节电池电压
    // uint16_t No_18_Volt; // 第18节电池电压
    // uint16_t No_19_Volt; // 第19节电池电压
    // uint16_t No_20_Volt; // 第20节电池电压
    // uint16_t No_21_Volt; // 第21节电池电压
    // uint16_t No_22_Volt; // 第22节电池电压
    // uint16_t No_23_Volt; // 第23节电池电压
    // uint16_t No_24_Volt; // 第24节电池电压
} battery_xj24[BATTERY_XJ24_MAX];

struct
{
    int16_t volt[55]; // 55节电池电压
    // uint16_t No_1_Volt;  // 第1节电池电压
    // uint16_t No_2_Volt;  // 第2节电池电压
    // uint16_t No_3_Volt;  // 第3节电池电压
    // uint16_t No_4_Volt;  // 第4节电池电压
    // uint16_t No_5_Volt;  // 第5节电池电压
    // uint16_t No_6_Volt;  // 第6节电池电压
    // uint16_t No_7_Volt;  // 第7节电池电压
    // uint16_t No_8_Volt;  // 第8节电池电压
    // uint16_t No_9_Volt;  // 第9节电池电压
    // uint16_t No_10_Volt; // 第10节电池电压
    // uint16_t No_11_Volt; // 第11节电池电压
    // uint16_t No_12_Volt; // 第12节电池电压
    // uint16_t No_13_Volt; // 第13节电池电压
    // uint16_t No_14_Volt; // 第14节电池电压
    // uint16_t No_15_Volt; // 第15节电池电压
    // uint16_t No_16_Volt; // 第16节电池电压
    // uint16_t No_17_Volt; // 第17节电池电压
    // uint16_t No_18_Volt; // 第18节电池电压
    // uint16_t No_19_Volt; // 第19节电池电压
    // uint16_t No_20_Volt; // 第20节电池电压
    // uint16_t No_21_Volt; // 第21节电池电压
    // uint16_t No_22_Volt; // 第22节电池电压
    // uint16_t No_23_Volt; // 第23节电池电压
    // uint16_t No_24_Volt; // 第24节电池电压
    // uint16_t No_25_Volt; // 第25节电池电压
    // uint16_t No_26_Volt; // 第26节电池电压
    // uint16_t No_27_Volt; // 第27节电池电压
    // uint16_t No_28_Volt; // 第28节电池电压
    // uint16_t No_29_Volt; // 第29节电池电压
    // uint16_t No_30_Volt; // 第30节电池电压
    // uint16_t No_31_Volt; // 第31节电池电压
    // uint16_t No_32_Volt; // 第32节电池电压
    // uint16_t No_33_Volt; // 第33节电池电压
    // uint16_t No_34_Volt; // 第34节电池电压
    // uint16_t No_35_Volt; // 第35节电池电压
    // uint16_t No_36_Volt; // 第36节电池电压
    // uint16_t No_37_Volt; // 第37节电池电压
    // uint16_t No_38_Volt; // 第38节电池电压
    // uint16_t No_39_Volt; // 第39节电池电压
    // uint16_t No_40_Volt; // 第40节电池电压
    // uint16_t No_41_Volt; // 第41节电池电压
    // uint16_t No_42_Volt; // 第42节电池电压
    // uint16_t No_43_Volt; // 第43节电池电压
    // uint16_t No_44_Volt; // 第44节电池电压
    // uint16_t No_45_Volt; // 第45节电池电压
    // uint16_t No_46_Volt; // 第46节电池电压
    // uint16_t No_47_Volt; // 第47节电池电压
    // uint16_t No_48_Volt; // 第48节电池电压
    // uint16_t No_49_Volt; // 第49节电池电压
    // uint16_t No_50_Volt; // 第50节电池电压
    // uint16_t No_51_Volt; // 第51节电池电压
    // uint16_t No_52_Volt; // 第52节电池电压
    // uint16_t No_53_Volt; // 第53节电池电压
    // uint16_t No_54_Volt; // 第54节电池电压
    // uint16_t No_55_Volt; // 第55节电池电压
} battery_xj55[BATTERY_XJ55_MAX];

struct
{
    uint16_t outputVolt;          // 模块输出电压
    uint16_t outputCurr;          // 模块输出电流
    uint16_t currLimitPercentage; // 模块限流点百分数
    uint16_t voltMax;             // 模块输出电压上限
    uint16_t voltMin;             // 模块输出电压下限
    struct
    {
        uint16_t onOffState : 1;   // 1：关机状态，0：开机状态
        uint16_t autoOrManual : 1; // 1：手动，0：自动
        uint16_t protectState : 1; // 1：保护，0：正常
        uint16_t faultState : 1;   // 1：故障，0：正常
        uint16_t reserve : 12;
    } DI;                        // 模块开关状态量DI
    uint16_t floatChargeVolt;    // 模块浮充电压
    uint16_t equalizeChargeVolt; // 模块均充电压
} dc_4850_Module[DC4850MODULE_MAX];

struct
{
    uint16_t sw1to16; // 开入状态1-16
} remoteControlModule_ARD[REMOTE_CONTROL_MODULE_MAX];

struct
{
    uint16_t channel1to16State; // 1-16路状态
    uint16_t sw1to16;           // 1-16路继电器状态
} remoteControlModule_TH[REMOTE_CONTROL_MODULE_MAX];

struct
{
    uint16_t outputVolt;          // 模块输出电压
    uint16_t outputCurr;          // 模块输出电流
    uint16_t currLimitPercentage; // 模块限流点百分数
    uint16_t voltMax;             // 模块输出电压上限
    uint16_t voltMin;             // 模块输出电压下限
    struct
    {
        uint16_t onOffState : 1;   // 1：关机状态，0：开机状态
        uint16_t autoOrManual : 1; // 1：手动，0：自动
        uint16_t protectState : 1; // 1：保护，0：正常
        uint16_t faultState : 1;   // 1：故障，0：正常
        uint16_t reserve : 12;
    } DI;                        // 模块开关状态量DI
    uint16_t floatChargeVolt;    // 模块浮充电压
    uint16_t equalizeChargeVolt; // 模块均充电压
} chargeModule[CHARGER_MODULE_MAX];

struct
{
    struct
    {
        uint16_t interval_Uab;      // 主路AB线电压（三相输入）/主路电压（单相输入）
        uint16_t interval_Ubc;      // 主路BC线电压（三相输入）
        uint16_t interval_Uca;      // 主路CA线电压（三相输入）
        uint16_t interval_Freq;     // 主路输入频率
        uint16_t bypassVolt;        // 旁路输入电压
        uint16_t bypassFreq;        // 旁路输入频率
        uint16_t outputVolt;        // 输出电压
        uint16_t outputFreq;        // 输出频率
        uint16_t DC_Volt;           // 直流电压
        uint16_t outputCurr;        // 输出电流
        uint16_t insideTemperature; // 机内温度
    } remoteMeasurement;            // 遥测
    struct
    {
        uint16_t bypassOut : 1;          // 旁路输出
        uint16_t rectifyInverterOut : 1; // 整流逆变输出
        uint16_t DC_InverterOut : 1;     // 直流逆变输出
        uint16_t overTemperature : 1;    // 过温
        uint16_t overload : 1;           // 过载
        uint16_t bypassFault : 1;        // 旁路故障
        uint16_t mainFault : 1;          // 主路故障
        uint16_t batteryHighVolt : 1;    // 电池高压
        uint16_t battertLowVolt : 1;     // 电池低压
        uint16_t reserve : 7;
    } remoteSignal; // 遥信
} ups[UPS_MAX];

struct
{
    uint16_t outVolt;           // 逆变模块模块输出电压
    uint16_t outCurr;           // 逆变模块模块输出电流
    uint16_t outFreq;           // 逆变模块输出频率
    uint16_t outPowerFactor;    // 输出功率因数
    uint16_t inverterVolt;      // 逆变模块逆变电压
    uint16_t bypassInputVolt;   // 旁路输入电压
    uint16_t bypassInputFreq;   // 旁路输入频率
    uint16_t batteryInputVolt;  // 电池输入电压
    uint16_t outActivePower;    // 逆变模块输出有功功率
    uint16_t outApparentPowper; // 输出视在功率
    uint16_t outLoadRate;       // 逆变模块输出负载率
    uint16_t temperature;       // 模块温度
    uint16_t capacityPercent;   // 模块容量降额比
    uint16_t bypassMaxVolt;     // 旁路电压上限
    uint16_t bypassMinVolt;     // 旁路电压下限
    struct
    {
        uint16_t onOffState : 1;      // 1：关机状态，0：开机状态
        uint16_t workMode : 1;        // 逆变器工作方式
        uint16_t faultState : 1;      // 1：故障，0：正常
        uint16_t overload : 1;        // 过载状态
        uint16_t overTemperature : 1; // 过温
        uint16_t battertLowVolt : 1;  // 电池低压
        uint16_t bypassState : 1;     // 旁路输入状态
        uint16_t outputMode : 1;      // 输出方式
        uint16_t reserve : 8;
    } DI;               // 模块开关状态量DI
    uint16_t config;    // 配置寄寄存器
    uint16_t rectify;   // 校准寄存器
    uint16_t mainsVolt; // 市电电压
    uint16_t mainsFreq; // 市电频率
    uint16_t mainsCurr; // 市电电流
} inv[INV_MAX];

struct
{
    int16_t positiveBusToGroundVolt; // 正母线交流对地电压
    int16_t negativeBusToGroundVolt; // 负母线交流对地电压
    int16_t positiveBusAlarm : 1;    // 正母线告警
    int16_t Reserve : 7;
    int16_t negativeBusAlarm : 1; // 负母线告警
    int16_t Reserve1 : 7;
    int16_t alarmValue; // 母线对地交流电压告警设置值
} AC_Insulation;        // AC绝缘
