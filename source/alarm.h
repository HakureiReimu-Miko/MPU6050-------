#ifndef __ALARM_H
#define __ALARM_H

#include "sys.h"

// typedef struct
// {
//     struct
//     {
//         uint8_t year;
//         uint8_t month;
//         uint8_t day;
//         uint8_t hour;
//         uint8_t minute;
//         uint8_t second;
//     } startTime;
//     uint8_t message[50];

//     uint8_t alarmFlag;
//     uint16_t flashAddr;
// } realTimeAlarmTypeDef;

#define ALARM 1
#define NO_ALARM 0

typedef struct node
{
    struct
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } startTime;
    uint8_t message[40];
    struct
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } endTime;
    uint16_t flag;
    uint16_t saveSerialNum; // 保存的序号，用于记录当前储存的位置
    uint32_t curVP;
    uint32_t nextVP;
    // struct node *next;
} AlarmTypeDef;

extern uint16_t alarmTabFlag[31];

#define NO_OUTPUT_ALARM 0
#define CHARGE_MODULE_COMMUNICATION_FAULT 1
#define CHARGE_MODULE_FAULT 2
#define AC_FAULT 3
#define DC_FAULT 4
#define SYNTHESIS_COLLECTION_MODULE_FAULT 5
#define SWITCH_MODULE_COMMUNICATION_FAULT 6
#define SWITCH_MODULE_FAULT 7
#define SINGLE_CELL_COMMUNICATION_FAULT 8
#define SINGLE_CELL_FAULT 9
#define DC_INSULATION_COMMUNICATION_FAULT 10
#define DC_INSULATION_FAULT 11
#define SYSTEM_FAULT 12
#define INV_FAULT 13
#define DCDC_MODULE_FAULT 14
#define ANY_COMMUCATION_FAULT 15
#define BATTERY_FAULT 16
#define UPS_FAULT 17
#define AC_OVER_VOLT_OR_UNDER_VOLT 18
#define CHANNEL_1_POWER_FAILURE 19
#define CHANNEL_2_POWER_FAILURE 20
#define DC_BUS_INSULATION 21
#define CONTROL_BUS_UNDER_VOLT 22
#define CLOSE_BUS_UNDER_VOLT 23
#define AC_INTO_DC 24
#define ATS_FAULT 25

#define P_HISTORY_ALARM_NORFLASH_ADDR 0x800 // 历史报警队列头尾指针储存位置
#define HISTORY_ALARM_NORFLASH_ADDR 0x810   // 历史报警norflash储存位置
#define ALARM_SIZE (sizeof(AlarmTypeDef) / 2 + sizeof(AlarmTypeDef) % 2)
#define HISTORY_ALARM_NUM 1000

#define HISTORY_ALARM_DATA_VP 0x10000 // flash读取缓慢，使用VP作为历史报警数据缓冲区进行显示
#define REAL_TIME_ALARM_DATA_VP 0x1A000

void alarmTreat();
void realTimeAlarmDisplay();
void historyAlarmDisplay();
void historyAlarmRead();

#endif