#ifndef __CONST_H__
#define __CONST_H__
#include "sys.h"
#include "modbus.h"

#define SYNTHESIS_MODULE_VP 0x10000       // 综合模块VP
#define BRANCH_INSULATION_VP 0x10030      // 支路绝缘VP
#define BRANCH_INSULATION_OFFSET 35       // 各支路绝缘模块数据偏移
#define BRANCH_INSULATION_MAX 2           // 支路绝缘数量
#define SWITCH_MODULE_VP 0x10200          // 开关量VP
#define SWITCH_MODULE_MAX 4               // 开关量模块数量最大4
#define SWITCH_MODULE_OFFSET 5            // 各开关量模块数据偏移,最大4个
#define BATTERY_XJ24_VP 0x10300           // 电池巡检XJ24 VP
#define BATTERY_XJ24_MAX 5                // 电池巡检XJ24数量最大5个
#define BATTERY_XJ24_OFFSET 0x19          // 电池巡检XJ24模块数据偏移，最大5个
#define BATTERY_XJ55_VP 0x10400           // 电池巡检XJ55 VP
#define BATTERY_XJ55_MAX 2                // 电池巡检XJ55模块数量最大2个
#define BATTERY_XJ55_OFFSET 0x37          // 电池巡检XJ55模块数据偏移，最大2个
#define DC4850MODULE_VP 0x10600           // DC4850模块VP
#define DC4850MODULE_MAX 4                // DC4850模块数量最大4个
#define DC4850MODULE_OFFSET 0x03          // DC4850模块数据偏移，最大4个
#define REMOTE_CONTROL_MODULE_VP 0x10700  // 远控单元VP
#define REMOTE_CONTROL_MODULE_MAX 2       // 远控模块数量最大2个
#define REMOTE_CONTROL_MODULE_OFFSET 10 // 远控模块数据偏移，最大2个
#define CHARGER_MODULE_VP 0x10800         // 充电模块
#define CHARGER_MODULE_MAX 8              // 充电模块数量最大8个
#define CHARGER_MODULE_OFFSET 8           // 充电模块数据偏移，最大8个
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
    uint16_t closeBusVolt;       // 合闸母线电压
    uint16_t controlBusVolt;     // 控母电压
    uint16_t busToGroundVolt;    // 母线对地电压
    uint16_t batteryVolt;        // 电池组电压
    uint16_t controlBusCurr;     // 控母电流
    uint16_t batteryCurr;        // 电池电流
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
    uint16_t sw_01to16;           // 开关量1到16路
    uint16_t sw_17to24;          // 开关量17到24路
} synthesisCollection;           // 综合采样

extern struct
{
    uint16_t closeBusToGroundVolt;         // 合母对地电压
    uint16_t controlBusToGroundVolt;       // 控制母对地电压
    uint16_t busToGroundVolt;              // 母线对地电压
    uint16_t res[30];                      // 绝缘电路值30路
} branchInsulation[BRANCH_INSULATION_MAX]; // 支路绝缘

extern struct
{
    uint16_t sw_u16[3];
} switchModule[SWITCH_MODULE_MAX]; // 开关量

extern struct
{
    int16_t volt[24];    // 24节电池电压
    int16_t temperature; // 温度
} battery_xj24[BATTERY_XJ24_MAX];

extern struct
{
    int16_t volt[55]; // 55节电池电压
} battery_xj55[BATTERY_XJ55_MAX];

extern struct
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

extern struct 
{
    uint16_t sw1to16;//开入状态1-16
}remoteControlModule_ARD[REMOTE_CONTROL_MODULE_MAX];

extern struct
{
    uint16_t channel1to16State; // 1-16路状态
    uint16_t sw1to16;           // 1-16路继电器状态
} remoteControlModule_TH[REMOTE_CONTROL_MODULE_MAX];

extern struct
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

extern struct
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
        uint8_t bypassOut : 1;          // 旁路输出
        uint8_t rectifyInverterOut : 1; // 整流逆变输出
        uint8_t DC_InverterOut : 1;     // 直流逆变输出
        uint8_t overTemperature : 1;    // 过温
        uint8_t overload : 1;           // 过载
        uint8_t bypassFault : 1;        // 旁路故障
        uint8_t mainFault : 1;          // 主路故障
        uint8_t batteryHighVolt : 1;    // 电池高压
        uint8_t battertLowVolt : 1;     // 电池低压
        uint8_t reserve : 7;
    } remoteSignal; // 遥信
} ups[UPS_MAX];

extern struct
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
    uint16_t DI;                // 模块开关状态量DI
    uint16_t config;            // 配置寄寄存器
    uint16_t rectify;           // 校准寄存器
    uint16_t mainsVolt;         // 市电电压
    uint16_t mainsFreq;         // 市电频率
    uint16_t mainsCurr;         // 市电电流
} inv[INV_MAX];

#endif