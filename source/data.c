#include "sys.h"

struct
{
    uint16_t sysVoltageGrade; // 系统电压等级
    uint16_t language;
    uint16_t siliconChain;                        // 硅链配置
    uint16_t synthesisMeasurementRelayOutput[16]; // 综合测量继电器输出1-8
    // uint16_t synthesisMeasurementRelayOutput2; // 综合测量继电器输出2
    // uint16_t synthesisMeasurementRelayOutput3; // 综合测量继电器输出3
    // uint16_t synthesisMeasurementRelayOutput4; // 综合测量继电器输出4
    // uint16_t synthesisMeasurementRelayOutput5; // 综合测量继电器输出5
    // uint16_t synthesisMeasurementRelayOutput6; // 综合测量继电器输出6
    // uint16_t synthesisMeasurementRelayOutput7; // 综合测量继电器输出7
    // uint16_t synthesisMeasurementRelayOutput8; // 综合测量继电器输出8
    uint16_t synthesisMeasurementRelayInput22; // 综合测量继电器输入22
} sysInfoSet = {0};                            // 系统信息

struct
{
    uint16_t AC_InputChannel;                // 交流输入路数
    uint16_t threePhaseAC_VoltMax;           // 三相交流过压值
    uint16_t threePhaseAC_VoltMin;           // 三相交流欠压值
    uint16_t singlePhaseThreePhaseSelection; // 单/三相选择
    uint16_t singlePhaseAC_VoltMax;          // 单相交流过压值
    uint16_t singlePhaseAC_VoltMin;          // 单相交流欠压值
} AC_InfoSet = {0};                          // AC交流信息

struct
{
    uint16_t siliconChainsSet5or7; // 硅链设置5级或7级
    uint16_t closeBusVoltMax;      // 合母过压值
    uint16_t closeBusVoltMin;      // 合母过压值
    uint16_t controlBusVoltMax;    // 控母过压值
    uint16_t controlBusVoltMin;    // 控母欠压值
    uint16_t reserve[4];

    // uint16_t busVoltMax_220V;    // 母线过压值220
    // uint16_t busVoltMin_220V;    // 母线欠压值220
    // uint16_t moduleVoltMax_220V; // 模块过压值220
    // uint16_t moduleVoltMin_220V; // 模块欠压值220
    // uint16_t busVoltMax_110V;    // 母线过压值110
    // uint16_t busVoltMin_110V;    // 母线欠压值110
    // uint16_t moduleVoltMax_110V; // 模块过压值110
    // uint16_t moduleVoltMin_110V; // 模块欠压值110

    uint16_t loadHallRatio;    // 负载霍尔变换比率
    uint16_t batteryHallRatio; // 电池霍尔变换比率
    uint16_t reserve1;
} DC_InfoSet = {0};

struct
{
    uint16_t nominalCapacity;          // 标称容量
    uint16_t batteryVoltMax;           // 电池过压值
    uint16_t batteryVoltMin;           // 电池欠压值
    uint16_t equalizeChargeVolt;       // 均充电压值
    uint16_t floatChargeVolt;          // 浮充电压值
    uint16_t equalizeChargeHour;       // 均充时间
    uint16_t floatChargeDay;           // 浮充时间
    uint16_t chargingCurrentLimit;     // 充电电流限制值
    uint16_t floatToEqualizeChargeCur; // 浮充转均限流
    uint16_t equalizeToFloatChargeCur; // 均充转浮充限流
} batteryInfoSet = {0};

struct
{
    uint16_t synthesisMeasurementTempDisplay; // 综合测量温度显示
    uint16_t batteryTempDisplay;              // 电池温度显示
    // uint16_t closeBusVoltRectify;             // 合闸母线电压矫正
    // uint16_t controlBusVoltRectify;           // 控母电压矫正
    // uint16_t busToGroundVoltRectify;          // 母线对地电压矫正
    // uint16_t batteryVoltRectify;              // 电池电压矫正
    // uint16_t AC_1_Ua_Rectify;                 // 交流1路A相电压矫正
    // uint16_t AC_1_Ub_Rectify;                 // 交流1路A相电压矫正
    // uint16_t AC_1_Uc_Rectify;                 // 交流1路A相电压矫正
    // uint16_t AC_2_Ua_Rectify;                 // 交流2路A相电压矫正
    // uint16_t AC_2_Ub_Rectify;                 // 交流2路A相电压矫正
    // uint16_t AC_2_Uc_Rectify;                 // 交流2路A相电压矫正
} specialParaSet = {0};

uint16_t backLightTime[2] = {0}; // 背光持续时间

struct
{
    struct
    {
        uint16_t switchAccessMode;  // 开关接入模式
        uint16_t alarmChannelNum;   // 报警路数
        uint16_t remoteControlType; // 远控单元类型
        uint16_t remoteControlNum;  // 远控单元数量
        uint16_t reserve[0x10 - 4];
    } synthesisCollection; // 综合检测
    struct
    {
        uint16_t switchNum;     // 开关量数量
        uint16_t accessMode[4]; // 1#开关接入模式
        // uint16_t sw_1_AccessMode;      // 1#开关接入模式
        // uint16_t sw_2_AccessMode;      // 2#开关接入模式
        // uint16_t sw_3_AccessMode;      // 3#开关接入模式
        // uint16_t sw_4_AccessMode;      // 4#开关接入模式
        uint16_t alarmChannelNum[4]; // 1#报警路数
        // uint16_t sw_1_AlarmChannelNum; // 1#报警路数
        // uint16_t sw_2_AlarmChannelNum; // 2#报警路数
        // uint16_t sw_3_AlarmChannelNum; // 3#报警路数
        // uint16_t sw_4_AlarmChannelNum; // 4#报警路数
        uint16_t reserve;
    } switchModule;      // 开关量
} switchModuleSet = {0}; // 开关量配置

struct
{
    uint16_t batteryType; // 电池巡检类型
    uint16_t reserve[0x10 - 1];
    struct
    {
        uint16_t batteryModuleNum;  // 电池巡检数量
        uint16_t cellNum;           // 电池节数
        uint16_t singleCellVoltMax; // 单只电池过压值
        uint16_t singleCellVoltMin; // 单只电池欠压值
        uint16_t endCellNum;        // 末端电池数量
        uint16_t endCellVoltMax;    // 末端电池过压值
        uint16_t endCellVoltMin;    // 末端电视欠压值
        uint16_t reserve[0x10 - 7];
    } xj24;
    struct
    {
        uint16_t batteryModuleNum;  // 电池巡检数量
        uint16_t cellNum;           // 电池节数
        uint16_t singleCellVoltMax; // 单只电池过压值
        uint16_t singleCellVoltMin; // 单只电池欠压值
        uint16_t endCellNum;        // 末端电池数量
        uint16_t endCellVoltMax;    // 末端电池过压值
        uint16_t endCellVoltMin;    // 末端电视欠压值
        uint16_t reserve;
    } xj55;
} batterySet; // 电池配置

struct
{
    struct
    {
        uint16_t Num;               // 绝缘数量
        uint16_t voltToGroundAlarm; // 交流对地告警值
        uint16_t reserve[0x10 - 2];
    } AC;
    struct
    {
        uint16_t Num;                   // 绝缘数量
        uint16_t busVoltDifference;     // 母线压差
        uint16_t resAlarm;              // 绝缘告警值
        uint16_t closeBus_1_ChannelNum; // 1#合母路数
        uint16_t closeBus_2_ChannelNum; // 2#合母路数
        uint16_t reserve;
    } DC;
} insulationSet; // 绝缘配置

struct
{
    uint16_t moduleType; // 充电抹开类型
    uint16_t moduleNum;  // 充电模块数量
    uint16_t ratedCurr;  // 额定电流
    uint16_t MaxCurr;    // 模块最大限流
} chargeModuleSet;       // 充电模块配置

struct
{
    uint16_t Num;           // 模块数量
    uint16_t voltBroadcast; // 广播电压
    uint16_t currBroadcast; // 广播电流
    uint16_t reaserve;
} DC_ConverterSet; // DC变换器

struct
{
    uint16_t UPS_Num;
    uint16_t INV_Num;
} UPS_INV_Set;