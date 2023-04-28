#include "alarm.h"
#include "const.h"
#include "data.h"
#include <STDLIB.H>
#include <STRING.H>
#include "rtc.h"
#include "norflash.h"
#include "debug.h"
#include "ui.h"

#define P_HISTORY_ALARM_NORFLASH_ADDR 0x800 // 历史报警队列头尾指针储存位置
#define HISTORY_ALARM_NORFLASH_ADDR 0x810   // 历史报警norflash储存位置
#define HISTORY_ALARM_NORFLASH_OFFSET 0x50
#define HISTORY_ALARM_SIZE 1000

#define HISTORY_ALARM_DATA_VP 0x12000 // flash读取缓慢，使用VP作为历史报警数据缓冲区进行显示
struct
{
    uint16_t head;
    uint16_t tail;
} pHistoryAlarm = {0}; // 历史报警队列头尾指针

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
    Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
}

AlarmTypeDef realTimeAlarmHeader = {0};

/**
 * @brief 头插法
 *
 * @param list
 * @param node
 */
void listInsertHead(AlarmTypeDef *list, AlarmTypeDef *node)
{
    // DEBUGINFO("Insert %p,%p\n", node, node->next);
    node->next = list->next;
    list->next = node;
    // listdebug();
}

void listDelete(AlarmTypeDef *list, AlarmTypeDef *node)
{
    AlarmTypeDef *prevNode;
    prevNode = list;
    // DEBUGINFO("Delete %p,%p\n", node, node->next);
    while (prevNode->next && prevNode->next != node)
    {
        prevNode = prevNode->next;
    }
    if (prevNode->next == NULL)
    {
        return;
    }
    // DEBUGINFO("prevNode %p,%p\n", prevNode,prevNode->next);
    prevNode->next = node->next;
    node->next = NULL;
    // listdebug();
}

uint16_t getListLenth(AlarmTypeDef *list)
{
    AlarmTypeDef *temp = list;
    uint16_t i = 0;
    while (temp->next)
    {
        i++;
        temp = temp->next;
    }
    return i;
}

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

void saveStartAlarm(AlarmTypeDef alarm)
{
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 32);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 32);
    pHistoryAlarmMove();
    Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
}

void saveEndAlarm(AlarmTypeDef alarm)
{
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 32);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 32);
}

void alarmRecord()
{
    uint16_t i;
    {
        static AlarmTypeDef swAlarm[8] = {0};
        for (i = 0; i < 8; i++)
        {
            // DEBUGINFO("synthesisMeasurementRelayOutput = %d\n",sysInfoSet.synthesisMeasurementRelayOutput[i]);
            // DEBUGINFO("%s\n",outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);
            if (synthesisCollection.sw_01to16 & (0x0001 << i))
            {
                if (swAlarm[i].flag == NO_ALARM)
                {
                    listInsertHead(&realTimeAlarmHeader, &swAlarm[i]);
                    startTimeRecord(&swAlarm[i]);
                    swAlarm[i].flag = ALARM;
                    strncpy(swAlarm[i].message, outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]], 30);
                    swAlarm[i].saveSerialNum = pHistoryAlarm.tail;
                    memset(&swAlarm[i].endTime, 0, 6);

                    saveStartAlarm(swAlarm[i]);
                }
            }
            else
            {
                if (swAlarm[i].flag == ALARM)
                {
                    swAlarm[i].flag = NO_ALARM;
                    endTimeRecord(&swAlarm[i]);
                    saveEndAlarm(swAlarm[i]);
                    listDelete(&realTimeAlarmHeader, &swAlarm[i]);
                }
            }
            // DEBUGINFO("swAlarm[%d]flag = %d\n", i, swAlarm[i].flag);
        }
    }
}

#define ROW_NUM 14 // 表格行数
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // 表格当前页码
    uint16_t curPageDisplay;
    uint16_t alarmNum;
    uint16_t maxPage;
    alarmNum = getListLenth(&realTimeAlarmHeader);
    maxPage = alarmNum ? (alarmNum - 1) / ROW_NUM : 0;
    // DEBUGINFO("%d\n", maxPage);
    read_dgus_vp(0xA600, (uint8_t *)&curPage, 1);
    if (curPage > maxPage)
    {
        curPage = maxPage;
        write_dgus_vp(0xA600, (uint8_t *)&curPage, 1);
    }
    curPageDisplay = curPage + 1;
    write_dgus_vp(0xA601, (uint8_t *)&curPageDisplay, 1);
    {
        uint16_t i = 0;
        AlarmTypeDef *tempNode = realTimeAlarmHeader.next;
        while (i < curPage * 14)
        {
            tempNode = tempNode->next;
            i++;
        }

        for (i = 0; i < 14; i++)
        {
            struct
            {
                uint8_t date[0x20];
                uint8_t time[0x20];
                uint8_t message[50];
            } alarmBuff = {0};
            if (tempNode)
            {
                sprintf(alarmBuff.date, "20%02bd-%02bd-%02bd\xFF\xFF", tempNode->startTime.year, tempNode->startTime.month, tempNode->startTime.day);
                sprintf(alarmBuff.time, "%02bd:%02bd:%02bd\xFF\xFF", tempNode->startTime.hour, tempNode->startTime.minute, tempNode->startTime.second);
                strncpy(alarmBuff.message, tempNode->message, 50);
                tempNode = tempNode->next;
            }
            write_dgus_vp(0xA100 + 0x50 * i, (uint8_t *)&alarmBuff, 45);
        }
    }
}

void historyAlarmDisplay()
{
    uint16_t curPage = 0;    // 表格当前页码
    uint16_t curPageDisplay; // 当前页显示用
    uint16_t alarmNum;
    uint16_t maxPage;
    uint16_t i;
    alarmNum = pHistoryAlarm.head < pHistoryAlarm.tail ? pHistoryAlarm.tail - pHistoryAlarm.head : pHistoryAlarm.tail + HISTORY_ALARM_SIZE - pHistoryAlarm.head;
    maxPage = alarmNum ? (alarmNum - 1) / ROW_NUM : 0;
    DEBUGINFO("maxPage = %d\n", maxPage);
    read_dgus_vp(0xAC00, (uint8_t *)&curPage, 1);
    if (curPage > maxPage)
    {
        curPage = maxPage;
        write_dgus_vp(0xAC00, (uint8_t *)&curPage, 1);
    }
    curPageDisplay = curPage + 1;
    DEBUGINFO("%d\n", curPageDisplay);
    write_dgus_vp(0xAC01, (uint8_t *)&curPageDisplay, 1);
    {
        AlarmTypeDef alarmData = {0};
        int16_t dispalyNum;

        if (pHistoryAlarm.head <= pHistoryAlarm.tail)
        {
            for (i = 0; i < 14; i++)
            {
                struct
                {
                    uint8_t date[0x20];
                    uint8_t time[0x20];
                    uint8_t message[0x40];
                    uint8_t endTime[0x20];
                } alarmTxt = {0};
                dispalyNum = pHistoryAlarm.tail - 1 - i - curPage * 14;
                if (((int16_t)pHistoryAlarm.head <= (int16_t)dispalyNum))
                {
                    read_dgus_vp(HISTORY_ALARM_DATA_VP + dispalyNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarmData, HISTORY_ALARM_NORFLASH_OFFSET);
                    sprintf(alarmTxt.date, "20%02bd-%02bd-%02bd\xFF\xFF", alarmData.startTime.year, alarmData.startTime.month, alarmData.startTime.day);
                    sprintf(alarmTxt.time, "%02bd:%02bd:%02bd\xFF\xFF", alarmData.startTime.hour, alarmData.startTime.minute, alarmData.startTime.second);
                    strncpy(alarmTxt.message, alarmData.message, 50);
                    if (memcmp(&alarmData.endTime, "\x00\x00\x00\x00\x00\x00", 6) != 0) // 有结束时间显示结束时间
                    {
                        sprintf(alarmTxt.endTime, "20%02bd-%02bd-%02bd %02bd:%02bd:%02bd\xFF\xFF", alarmData.endTime.year, alarmData.endTime.month,
                                alarmData.endTime.day, alarmData.endTime.hour, alarmData.endTime.minute, alarmData.endTime.second);
                    }
                }
                write_dgus_vp(0xA700 + 0x50 * i, (uint8_t *)&alarmTxt, 0x50);
            }
        }
        else if (pHistoryAlarm.head > pHistoryAlarm.tail)
        {
            for (i = 0; i < 14; i++)
            {
                struct
                {
                    uint8_t date[0x20];
                    uint8_t time[0x20];
                    uint8_t message[0x40];
                    uint8_t endTime[0x20];
                } alarmTxt = {0};
                dispalyNum = (pHistoryAlarm.tail - 1 - i - curPage * 14 + HISTORY_ALARM_SIZE) % HISTORY_ALARM_SIZE;
                if ((0 <= (int16_t)dispalyNum && (int16_t)dispalyNum <= pHistoryAlarm.tail) ||
                    (pHistoryAlarm.head <= dispalyNum && dispalyNum <= HISTORY_ALARM_SIZE - 1))
                {
                    read_dgus_vp(HISTORY_ALARM_DATA_VP + dispalyNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarmData, HISTORY_ALARM_NORFLASH_OFFSET);
                    sprintf(alarmTxt.date, "20%02bd-%02bd-%02bd\xFF\xFF", alarmData.startTime.year, alarmData.startTime.month, alarmData.startTime.day);
                    sprintf(alarmTxt.time, "%02bd:%02bd:%02bd\xFF\xFF", alarmData.startTime.hour, alarmData.startTime.minute, alarmData.startTime.second);
                    strncpy(alarmTxt.message, alarmData.message, 50);
                    if (memcmp(&alarmData.endTime, "\x00\x00\x00\x00\x00\x00", 6) != 0) // 有结束时间显示结束时间
                    {
                        sprintf(alarmTxt.endTime, "20%02bd-%02bd-%02bd %02bd:%02bd:%02bd\xFF\xFF", alarmData.endTime.year, alarmData.endTime.month,
                                alarmData.endTime.day, alarmData.endTime.hour, alarmData.endTime.minute, alarmData.endTime.second);
                    }
                }
                write_dgus_vp(0xA700 + 0x50 * i, (uint8_t *)&alarmTxt, 0x50);
            }
        }
    }

    {
        VariableChangedIndicationTypeDef variableChangedIndication;
        read_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 2);
        if (variableChangedIndication.flag == 0x5A)
        {
            uint16_t variable[5];
            read_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);
            switch (variableChangedIndication.addr)
            {
            case 0xAC02:
            {
                variable[0] -= 1;
                write_dgus_vp(0xAC00, (uint8_t *)&variable[0], 1);
            }
            break;

            case 0xAC05://清楚历史报警
            pHistoryAlarm.head = pHistoryAlarm.tail = 0;
            Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR,(uint8_t*)&pHistoryAlarm,2);
            break;
            default:
                break;
            }
            memset(variable, 0, 5);
            write_dgus_vp(variableChangedIndication.addr, (uint8_t *)&variable, variableChangedIndication.len);

            variableChangedIndication.flag = 0;
            write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
        }
    }
}

void historyAlarmRead()
{
    Nor_Flash_read(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
    if (pHistoryAlarm.head <= pHistoryAlarm.tail)
    {
        norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
                       HISTORY_ALARM_DATA_VP + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
                       (pHistoryAlarm.tail - pHistoryAlarm.head) * HISTORY_ALARM_NORFLASH_OFFSET);
    }
    else if (pHistoryAlarm.head > pHistoryAlarm.tail)
    {
        norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
                       HISTORY_ALARM_DATA_VP + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
                       (HISTORY_ALARM_SIZE - 1 - pHistoryAlarm.head) * HISTORY_ALARM_NORFLASH_OFFSET);

        norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR,
                       HISTORY_ALARM_DATA_VP,
                       pHistoryAlarm.tail * HISTORY_ALARM_NORFLASH_OFFSET);
    }
}