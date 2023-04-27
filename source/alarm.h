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

typedef struct
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

    uint16_t flashAddr;//flash地址，用于记录结束时间
} AlarmTypeDef;



void alarmRecord();
void realTimeAlarmDisplay();

#endif