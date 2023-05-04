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
    uint8_t message[50];
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
    struct node *next;
} AlarmTypeDef;

void alarmTreat();
void realTimeAlarmDisplay();
void historyAlarmDisplay();
void historyAlarmRead();

#endif