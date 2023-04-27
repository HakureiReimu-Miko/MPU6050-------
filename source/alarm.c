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
    "���ι���״̬���\xFF\xFF",
    "�����˿��\xFF\xFF",
    "��ؿ��ط�բ\xFF\xFF",
    "����������\xFF\xFF",
    "��������I���ط�բ\xFF\xFF",
    "��������II���ط�բ\xFF\xFF",
    "ģ�鿪�ط�բ\xFF\xFF",
    "�������ط�բ\xFF\xFF",
    "��������\xFF\xFF",
    "��Ե����\xFF\xFF",
    NULL,
};
code uint8_t outputCodeTable[32][40] = {
    "���ι���״̬���\xFF\xFF",
    "���ģ��ͨѶ����\xFF\xFF",
    "���ģ�����\xFF\xFF",
    "��������\xFF\xFF",
    "ֱ������\xFF\xFF",
    "�ۺϲ�����Ԫ����\xFF\xFF",
    "����������ͨѶ����\xFF\xFF",
    "����������\xFF\xFF",
    "������ͨѶ����\xFF\xFF",
    "�����ع���\xFF\xFF",
    "֧·��ԵͨѶ����\xFF\xFF",
    "֧·��Ե����\xFF\xFF",
    "ϵͳ���ϸ澯\xFF\xFF",
    "INV���ģ����ϸ澯����ͨ�ţ�\xFF\xFF",
    "DCDCģ����ϸ澯����ͨ�ţ�\xFF\xFF",
    "��һ��Ԫͨ�Ź��ϸ澯\xFF\xFF",
    "��ع��ϸ澯\xFF\xFF",
    "UPS���ϸ澯����ͨ�ţ�\xFF\xFF",
    "������Ƿѹ�澯����ȱ��澯��\xFF\xFF",
    "һ·����ͣ��澯\xFF\xFF",
    "��·����ͣ��澯\xFF\xFF",
    "ֱ��ĸ�߲�ƽ��澯��ĸ�߾�Ե��\xFF\xFF",
    "��ĸ��Ƿѹ�澯\xFF\xFF",
    "��ĸ��Ƿѹ�澯\xFF\xFF",
    "��������ֱ���澯\xFF\xFF",
    "ATS����\xFF\xFF",
    "\xFF\xFF",
    "\xFF\xFF",
    "\xFF\xFF",
    "�Ӵ�������(ʹ�õ�1,2·���������)\xFF\xFF",
    "��������(ʹ�õ�3,4,5·���������)\xFF\xFF",
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

#define ROW_NUM 14 // �������
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // ���ǰҳ��
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
