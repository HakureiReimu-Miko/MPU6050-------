#include "alarm.h"
#include "const.h"
#include "data.h"
#include <STDLIB.H>
#include <STRING.H>
#include "rtc.h"
#include "norflash.h"
#include "debug.h"

#define P_ALARM_NORFLASH_ADDR 0x800
#define HISTORY_ALARM_NORFLASH_ADDR 0x810
#define HISTORY_ALARM_NORFLASH_OFFSET 0x50
#define HISTORY_ALARM_SIZE 1000

struct
{
    uint16_t head;
    uint16_t tail;
} pHistoryAlarm = {0};

code uint8_t inputCodeTable[32][30] = {
    "屏蔽故障状态输出\xFF\xFF",
    "电池熔丝断\xFF\xFF",
    "电池开关分闸\xFF\xFF",
    "防雷器故障\xFF\xFF",
    "交流进线I开关分闸\xFF\xFF",
    "交流进线II开关分闸\xFF\xFF",
    "模块开关分闸\xFF\xFF",
    "交流开关分闸\xFF\xFF",
    "硅链故障\xFF\xFF",
    "绝缘故障\xFF\xFF",
    NULL,
};
code uint8_t outputCodeTable[32][40] = {
    "屏蔽故障状态输出\xFF\xFF",
    "充电模块通讯故障\xFF\xFF",
    "充电模块故障\xFF\xFF",
    "交流故障\xFF\xFF",
    "直流故障\xFF\xFF",
    "综合采样单元故障\xFF\xFF",
    "开关量故障通讯故障\xFF\xFF",
    "开关量故障\xFF\xFF",
    "单体电池通讯故障\xFF\xFF",
    "单体电池故障\xFF\xFF",
    "支路绝缘通讯故障\xFF\xFF",
    "支路绝缘故障\xFF\xFF",
    "系统故障告警\xFF\xFF",
    "INV逆变模块故障告警（含通信）\xFF\xFF",
    "DCDC模块故障告警（含通信）\xFF\xFF",
    "任一单元通信故障告警\xFF\xFF",
    "电池故障告警\xFF\xFF",
    "UPS故障告警（含通信）\xFF\xFF",
    "交流过欠压告警（含缺相告警）\xFF\xFF",
    "一路交流停电告警\xFF\xFF",
    "二路交流停电告警\xFF\xFF",
    "直流母线不平衡告警（母线绝缘）\xFF\xFF",
    "控母过欠压告警\xFF\xFF",
    "合母过欠压告警\xFF\xFF",
    "交流窜入直流告警\xFF\xFF",
    "ATS故障\xFF\xFF",
    "\xFF\xFF",
    "\xFF\xFF",
    "\xFF\xFF",
    "接触器控制(使用第1,2路开关量输出)\xFF\xFF",
    "硅链控制(使用第3,4,5路开关量输出)\xFF\xFF",
    NULL,
};

void pHistoryAlarmMove()
{
    pHistoryAlarm.tail = (pHistoryAlarm.tail + 1) % HISTORY_ALARM_SIZE;
    if (pHistoryAlarm.tail == pHistoryAlarm.head)
    {
        pHistoryAlarm.head = (pHistoryAlarm.head + 1) % HISTORY_ALARM_SIZE;
    }
}

#define realTimeAlarmMax 100
AlarmTypeDef realTimeAlarm[realTimeAlarmMax];
uint16_t realTimealarmTail = 0;

void startTimeRecord(AlarmTypeDef *node)
{
    memcpy(&node->startTime.year, RTCdata, 3);
    memcpy(&node->startTime.hour, RTCdata + 4, 3);
}
void endTimeRecord(AlarmTypeDef *node)
{
    memcpy(&node->endTime.year, RTCdata, 3);
    memcpy(&node->endTime.hour, RTCdata + 4, 3);
}

void alarmRecord()
{
    uint16_t i;
    {
        static AlarmTypeDef *pSwAlarm[8] = {0};
        for (i = 0; i < 8; i++)
        {
            // DEBUGINFO("synthesisMeasurementRelayOutput = %d\n",sysInfoSet.synthesisMeasurementRelayOutput[i]);
            // DEBUGINFO("%s\n",outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);
            if (synthesisCollection.sw_01to16 & (0x0001 << i))
            {
                if (pSwAlarm[i] == NULL)
                {
                    if (realTimealarmTail < realTimeAlarmMax)
                    {
                        pSwAlarm[i] = &realTimeAlarm[realTimealarmTail];
                        realTimealarmTail++;
                        // DEBUGINFO("%d\n", realTimealarmTail);
                        startTimeRecord(pSwAlarm[i]);
                        
                        strncpy(pSwAlarm[i]->message, outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]], 30);
                        pSwAlarm[i]->flashAddr = HISTORY_ALARM_NORFLASH_ADDR + pHistoryAlarm.tail * HISTORY_ALARM_NORFLASH_OFFSET;
                        Nor_Flash_write(pSwAlarm[i]->flashAddr, (uint8_t *)&pSwAlarm[i], 28);
                        pHistoryAlarmMove();
                    }
                }
            }
            else
            {
                if (pSwAlarm[i] != NULL)
                {
                    endTimeRecord(pSwAlarm[i]);
                    Nor_Flash_write(pSwAlarm[i]->flashAddr, (uint8_t *)&pSwAlarm[i], 28);
                    memmove(pSwAlarm[i], pSwAlarm[i] + 1, &realTimeAlarm[realTimealarmTail] - (pSwAlarm[i] + 1));
                    realTimealarmTail--;
                    pSwAlarm[i] = NULL;
                }
            }
        }
    }
}

#define ROW_NUM 14 // 表格行数
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // 表格当前页码
    uint16_t maxPage = realTimealarmTail / ROW_NUM;
    read_dgus_vp(0xA600, (uint8_t *)&curPage, 1);
    if (curPage > maxPage)
    {
        curPage = maxPage;
        write_dgus_vp(0xA600, (uint8_t *)&curPage, 1);
    }
    {
        uint16_t i;
        for (i = curPage * ROW_NUM; i < (curPage + 1) * ROW_NUM; i++)
        {
            struct
            {
                uint8_t date[0x20];
                uint8_t time[0x20];
                uint8_t message[50];
            } alarmBuff = {0};
            DEBUGINFO("%d\n",realTimealarmTail);
            if (i < realTimealarmTail)
            {
                sprintf(alarmBuff.date, "20%02bd-%02bd-%02bd\xFF\xFF", realTimeAlarm[i].startTime.year, realTimeAlarm[i].startTime.month, realTimeAlarm[i].startTime.day);
                sprintf(alarmBuff.time, "%02bd:%02bd%:%02bd\xFF\xFF", realTimeAlarm[i].startTime.hour, realTimeAlarm[i].startTime.minute, realTimeAlarm[i].startTime.second);
                strncpy(alarmBuff.message, realTimeAlarm[i].message, 50);
            }
            // DEBUGINFO("%d \n", i);
            // DEBUGINFO("%s \n", alarmBuff.date);
            // DEBUGINFO("%s \n", alarmBuff.time);
            DEBUGINFO("i = %d\n",i);
            write_dgus_vp(0xA100 + 0x50 * i, (uint8_t *)&alarmBuff, 45);
        }
    }
}
