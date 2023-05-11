#ifndef __DATA_H
#define __DATA_H
#include "sys.h"

extern struct
{
    uint16_t sysVoltageGrade; // 系统电压等级
    uint16_t language;
    uint16_t siliconChain;                        // 硅链配置
    uint16_t synthesisMeasurementRelayOutput[16]; // 综合测量继电器输出1-8
    uint16_t synthesisMeasurementRelayInput22;    // 综合测量继电器输入22
} sysInfoSet;                                     // 系统信息

extern struct
{
    uint16_t AC_InputChannel;                // 交流输入路数
    uint16_t threePhaseAC_VolMax;            // 三相交流过压值
    uint16_t threePhaseAC_VolMin;            // 三相交流欠压值
    uint16_t singlePhaseThreePhaseSelection; // 单/三相选择
    uint16_t singlePhaseAC_VolMax;           // 单相交流过压值
    uint16_t singlePhaseAC_VolMin;           // 单相交流欠压值
} AC_InfoSet;                                // AC交流信息

extern struct
{
    uint16_t siliconChainsSet5or7; // 硅链设置5级或7级
    uint16_t closeBusVoltMax;      // 合母过压值
    uint16_t closeBusVoltMin;      // 合母过压值
    uint16_t controlBusVoltMax;    // 控母过压值
    uint16_t controlBusVoltMin;    // 控母欠压值
    uint16_t reserve[4];
    uint16_t loadHallRatio;    // 负载霍尔变换比率
    uint16_t batteryHallRatio; // 电池霍尔变换比率
    uint16_t reserve1;         // 凑成偶数字长度
} DC_InfoSet;

extern struct
{
    uint16_t nominalCapacity;          // 标称容量
    uint16_t batteryVoltMax;           // 电池过压值
    uint16_t batteryVoltMin;           // 电池欠压值
    uint16_t equalizeChargeVolt;       // 均充电压值
    uint16_t floatChargeVolt;          // 浮充电压值
    uint16_t equalizeChargeHour;       // 均充时间，小时
    uint16_t floatChargeDay;           // 浮充时间,天
    uint16_t chargingCurrentLimit;     // 充电电流限制值
    uint16_t floatToEqualizeChargeCur; // 浮充转均限流
    uint16_t equalizeToFloatChargeCur; // 均充转浮充限流
} batteryInfoSet;

extern struct
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
} specialParaSet;

extern uint16_t backLightTime[2];

extern struct
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
        uint16_t switchNum;          // 开关量数量
        uint16_t accessMode[4];      // 1#开关接入模式
        uint16_t alarmChannelNum[4]; // 1#报警路数
        uint16_t reserve;
    } switchModule; // 开关量
} switchModuleSet;  // 开关量配置

extern struct
{
    uint16_t batteryType; // 电池巡检类型 :XJ24/XJ55
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

extern struct
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

extern struct
{
    uint16_t moduleType; // 充电模块类型
    uint16_t moduleNum;  // 充电模块数量
    uint16_t ratedCurr;  // 额定电流
    uint16_t MaxCurr;    // 模块最大限流
} chargeModuleSet;       // 充电模块配置

extern struct
{
    uint16_t Num;           // 模块数量
    uint16_t voltBroadcast; // 广播电压
    uint16_t currBroadcast; // 广播电流
    uint16_t reaserve;
} DC_ConverterSet; // DC变换器

extern struct
{
    uint16_t UPS_Num;
    uint16_t INV_Num;
} UPS_INV_Set;

extern uint16_t alarmSoundOnOff[2];

// 页码
#define HOME_PAGE 0
#define SYS_FUNCTION_TABLE_PAGE 2                    // 系统功能表页码
#define SYS_INFO_PAGE 11                             // 系统参数页码
#define BATTERY_MANAGER_PAGE 22                      // 电池管理
#define SWITCH_MODULE_SET_PAGE 35                    // 开关量配置页码
#define SERIES_BATTERY_INSPECTION_DEVICE_SET_PAGE 39 // 电池巡检（电池仪）配置页面
#define INSULATION_SET_PAGE 42                       // 绝缘配置
#define CHARGER_MODULE_SET_PAGE 44                   // 充电模块配置
#define DC_MODULE_SET_PAGE 48
#define UPS_INV_SET_PAGE 50
#define RECOVERY_PAGE 52
#define REAL_TIME_ALARM_PAGE 59     // 实时报警页面
#define NON_INSULATION_DATA_PAGE 63 // 无绝缘信息界面
#define CHARGE_DATA_PAGE 70         // 充电模块数据
#define SWITCH_MODULE_DATA_PAGE 72
#define DC_BUS_DATA_PAGE 80 // 直流母线信息页面
#define UPS_DATA_PAGE 82
#define DC_CONVERTER_DATA_PAGE 86
#define INSULATION_DATA_PAGE 87 // 绝缘信息界面
#define BATTERY_DATA_PAGE 94
#define PASSWORD_LOGIN_PAGE 100
#define SCREEN_PROTECTION_PAGE 116

// flash存储地址与长度
#define SYS_INFO_NORFLASH_ADDR 0x0100
#define SYS_INFO_SIZE 12 // 字长度,必须为偶数
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
