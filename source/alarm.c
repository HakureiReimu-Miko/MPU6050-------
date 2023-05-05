#include "alarm.h"
#include "const.h"
#include "data.h"
#include <STDLIB.H>
#include <STRING.H>
#include "rtc.h"
#include "norflash.h"
#include "debug.h"
#include "ui.h"

#define P_HISTORY_ALARM_NORFLASH_ADDR 0x800 // ��ʷ��������ͷβָ�봢��λ��
#define HISTORY_ALARM_NORFLASH_ADDR 0x810   // ��ʷ����norflash����λ��
#define HISTORY_ALARM_NORFLASH_OFFSET 0x30
#define HISTORY_ALARM_SIZE 1000

#define HISTORY_ALARM_DATA_VP 0x10000 // flash��ȡ������ʹ��VP��Ϊ��ʷ�������ݻ�����������ʾ
#define REAL_TIME_ALARM_DATA_VP 0x1C000
struct
{
    uint16_t head;
    uint16_t tail;
} pHistoryAlarm = {0}; // ��ʷ��������ͷβָ��

// code uint8_t inputCodeTable[32][30] = {
//     "���ι���״̬���\xFF\xFF",
//     "�����˿��\xFF\xFF",
//     "��ؿ��ط�բ\xFF\xFF",
//     "����������\xFF\xFF",
//     "��������I���ط�բ\xFF\xFF",
//     "��������II���ط�բ\xFF\xFF",
//     "ģ�鿪�ط�բ\xFF\xFF",
//     "�������ط�բ\xFF\xFF",
//     "��������\xFF\xFF",
//     "��Ե����\xFF\xFF",
//     NULL,
// };
// code uint8_t outputCodeTable[32][40] = {
//     "���ι���״̬���\xFF\xFF",
//     "���ģ��ͨѶ����\xFF\xFF",
//     "���ģ�����\xFF\xFF",
//     "��������\xFF\xFF",
//     "ֱ������\xFF\xFF",
//     "�ۺϲ�����Ԫ����\xFF\xFF",
//     "����������ͨѶ����\xFF\xFF",
//     "����������\xFF\xFF",
//     "������ͨѶ����\xFF\xFF",
//     "�����ع���\xFF\xFF",
//     "֧·��ԵͨѶ����\xFF\xFF",
//     "֧·��Ե����\xFF\xFF",
//     "ϵͳ���ϸ澯\xFF\xFF",
//     "INV���ģ����ϸ澯����ͨ�ţ�\xFF\xFF",
//     "DCDCģ����ϸ澯����ͨ�ţ�\xFF\xFF",
//     "��һ��Ԫͨ�Ź��ϸ澯\xFF\xFF",
//     "��ع��ϸ澯\xFF\xFF",
//     "UPS���ϸ澯����ͨ�ţ�\xFF\xFF",
//     "������Ƿѹ�澯����ȱ��澯��\xFF\xFF",
//     "һ·����ͣ��澯\xFF\xFF",
//     "��·����ͣ��澯\xFF\xFF",
//     "ֱ��ĸ�߲�ƽ��澯��ĸ�߾�Ե��\xFF\xFF",
//     "��ĸ��Ƿѹ�澯\xFF\xFF",
//     "��ĸ��Ƿѹ�澯\xFF\xFF",
//     "��������ֱ���澯\xFF\xFF",
//     "ATS����\xFF\xFF",
//     "\xFF\xFF",
//     "\xFF\xFF",
//     "\xFF\xFF",
//     "�Ӵ�������(ʹ�õ�1,2·���������)\xFF\xFF",
//     "��������(ʹ�õ�3,4,5·���������)\xFF\xFF",
//     NULL,
// };

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
 * @brief ͷ�巨
 *
 * @param list
 * @param node
 */
void listInsertHead(/* AlarmTypeDef *list */ AlarmTypeDef *node)
{
    node->nextVP = realTimeAlarmHeader.nextVP;
    realTimeAlarmHeader.nextVP = node->curVP;
    DEBUGINFO("realTimeAlarmHeader.nextVP = %ld,node->nextVP = %ld\n",realTimeAlarmHeader.nextVP,node->nextVP);
    DEBUGINFO("node->flag = %d\n",node->flag);
    DEBUGINFO("sizeof(AlarmTypeDef) = %d\n",sizeof(AlarmTypeDef));
    DEBUGINFO("sizeof(uint16) = %d\n",sizeof(uint16_t));
    
    write_dgus_vp(node->curVP, (uint8_t *)&node, sizeof(AlarmTypeDef));

    // DEBUGINFO("Insert %p,%p\n", node, node->next);
    // node->next = list->next;
    // list->next = node;
    // listdebug();
}

void listDelete(/* AlarmTypeDef *list */ uint32_t VP /* , AlarmTypeDef *node */)
{
    AlarmTypeDef prevNode;
    AlarmTypeDef curNode;
    prevNode = realTimeAlarmHeader;
    while (prevNode.nextVP != VP && prevNode.nextVP != 0)
    {
        read_dgus_vp(prevNode.nextVP, (uint8_t *)&prevNode, sizeof(AlarmTypeDef));
    }
    if (prevNode.nextVP == 0)
    {
        return;
    }
    read_dgus_vp(VP, (uint8_t *)&curNode, sizeof(AlarmTypeDef));
    prevNode.nextVP = curNode.nextVP;
    write_dgus_vp(prevNode.curVP, (uint8_t *)&prevNode, sizeof(AlarmTypeDef));
    curNode.nextVP = 0;
    write_dgus_vp(curNode.curVP, (uint8_t *)&curNode, sizeof(AlarmTypeDef));

    // AlarmTypeDef *prevNode;
    // prevNode = list;
    // // DEBUGINFO("Delete %p,%p\n", node, node->next);
    // while (prevNode->next && prevNode->next != node)
    // {
    //     prevNode = prevNode->next;
    // }
    // if (prevNode->next == NULL)
    // {
    //     return;
    // }
    // // DEBUGINFO("prevNode %p,%p\n", prevNode,prevNode->next);
    // prevNode->next = node->next;
    // node->next = NULL;
    // listdebug();
}

uint16_t getListLenth(/* AlarmTypeDef *list */)
{
    AlarmTypeDef temp;
    uint16_t i = 0;
    temp = realTimeAlarmHeader;
    while (temp.nextVP != 0)
    {
        DEBUGINFO("temp.nextVP = %d\n",temp.nextVP);
        read_dgus_vp(temp.nextVP, (uint8_t *)&temp, sizeof(AlarmTypeDef));
        i++;
    }
    return i;
    // AlarmTypeDef *temp = list;
    // uint16_t i = 0;
    // while (temp->next)
    // {
    //     i++;
    //     temp = temp->next;
    // }
    // return i;
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
    memset(&alarm.endTime, 0, 6);
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 30);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 30);
    pHistoryAlarmMove();
    Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
}

void saveEndAlarm(AlarmTypeDef alarm)
{
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 30);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * HISTORY_ALARM_NORFLASH_OFFSET, (uint8_t *)&alarm, 30);
}

void alarmStart(AlarmTypeDef *alarmData, uint32_t VP, uint8_t *alarmMessage)
{
    alarmData->flag = ALARM;
    DEBUGINFO("alarmData->flag = %d",alarmData->flag);
    alarmData->curVP = VP;
    startTimeRecord(alarmData);
    strncpy(alarmData->message, alarmMessage, 40);
    listInsertHead(alarmData);
    alarmData->saveSerialNum = pHistoryAlarm.tail;
    saveStartAlarm(*alarmData);
}

void alarmEnd(AlarmTypeDef *alarmData)
{
    alarmData->flag = NO_ALARM;
    endTimeRecord(alarmData);
    saveEndAlarm(*alarmData);
    listDelete(alarmData->curVP);
}
/**
 * @brief ��װ�������ܣ���ʡ�ڴ�
 *
 * @param condition ������������
 * @param VP �������ݻ����ַ
 * @param alarmMessage ������Ϣ
 */
void alarm(uint16_t condition, uint32_t VP, uint8_t *alarmMessage)
{
    AlarmTypeDef alarmData;
    read_dgus_vp(VP, (uint8_t *)&alarmData, sizeof(AlarmTypeDef));
    DEBUGINFO("alarmData.flag = %d\n",alarmData.flag);
    DEBUGINFO("VP = %ld\n",VP);
    if (condition)
    {
        if (alarmData.flag != ALARM)
        {
            alarmStart(&alarmData, VP, alarmMessage);
        }
    }
    else
    {
        if (alarmData.flag == ALARM)
        {
            alarmEnd(&alarmData);
        }
    }
}
// /**
//  * @brief ��װ�������ܣ���ʡ�ڴ�
//  *
//  * @param condition ������������
//  * @param alarmData �������ݽṹ��ָ��
//  * @param alarmMessage ������Ϣ
//  */
// void alarm(uint16_t condition, AlarmTypeDef *alarmData, uint8_t *alarmMessage)
// {
//     if (condition)
//     {
//         if (alarmData->flag != ALARM)
//         {
//             alarmData->flag = ALARM;
//             alarmStart(alarmData, alarmMessage);
//         }
//     }
//     else
//     {
//         if (alarmData->flag == ALARM)
//         {
//             alarmData->flag = NO_ALARM;
//             alarmEnd(alarmData);
//         }
//     }
// }

void alarmTreat()
{
    uint16_t i, j;
    uint16_t num = 0;

    { // ���൥�ཻ����ѹ������page12
        if (AC_InfoSet.AC_InputChannel == SINGLE_PHASE)
        {
            if (AC_InfoSet.AC_InputChannel >= 1) // 1·
            {
                {
                    alarm(synthesisCollection.AC_channel_1_Uab > AC_InfoSet.singlePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "����1·������\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "����1·����Ƿѹ\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2·
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.singlePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "����2·������\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "����2·����Ƿѹ\xFF\xFF");
                }
            }
        }

        else if (AC_InfoSet.AC_InputChannel == THREE_PHASE) // ����
        {
            if (AC_InfoSet.AC_InputChannel >= 1) // 1·
            {
                {
                    alarm(synthesisCollection.AC_channel_1_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·A���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·B���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·C���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·A��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·B��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��1·C��Ƿѹ\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2·
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·A���ѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·B���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·C���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·A��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·B��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "���ཻ��2·C��Ƿѹ\xFF\xFF");
                }
            }
        }
    }

    // {
    //     if (sysInfoSet.siliconChain == 0) // �޹���
    //     {
    //         {
    //             alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "ĸ�߹�\xFDѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "ĸ��Ƿѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "ģ���\xFDѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "ģ��Ƿѹ\xFF\xFF");
    //         }
    //     }
    //     else if (sysInfoSet.siliconChain == 1) // �޹���
    //     {
    //         {
    //             alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "��ĸ��\xFDѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "��ĸǷѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "��ĸ��\xFDѹ\xFF\xFF");
    //         }
    //         {
    //             alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "��ĸǷѹ\xFF\xFF");
    //         }
    //     }
    // }

    // {
    //     extern int16_t batteryVoltSum;
    //     {
    //         alarm(batteryVoltSum > batteryInfoSet.batteryVoltMax,
    //               REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "������\xFDѹ\xFF\xFF");
    //     }
    //     {
    //         alarm(batteryVoltSum < batteryInfoSet.batteryVoltMin,
    //               REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "�����Ƿѹ\xFF\xFF");
    //     }
    // }
    // { // �ۺϲ����̵������
    //     for (i = 0; i < switchModuleSet.synthesisCollection.alarmChannelNum; i++)
    //     {
    //         uint8_t message[30];
    //         sprintf(message, "�ۺϲɼ�����%d����\xFF\xFF", i + 1);
    //         alarm((synthesisCollection.sw_01to16 >> i) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
    //               REAL_TIME_ALARM_DATA_VP + 0x30 * num++, message);
    //     }
    //     {
    //         alarm((synthesisCollection.sw_01to16 >> (13 - 1)) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
    //               REAL_TIME_ALARM_DATA_VP + 0x30 * num++, "�ۺϲɼ�����13����\xFF\xFF");
    //     }
    // }
    // {
    //     for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
    //     {
    //         for (j = 0; j < switchModuleSet.switchModule.alarmChannelNum[i]; i++)
    //         {
    //             uint8_t message[20];
    //             sprintf(message, "%d#���ص�%d·����\xFF\xFF", i + 1, j + 1);
    //             alarm(switchModule[i].sw_u16[j / 16] >> j % 16 != switchModuleSet.switchModule.accessMode[i],
    //                   REAL_TIME_ALARM_DATA_VP + 0x30 * num++, message);
    //         }
    //     }
    // }

    // if (batterySet.batteryType == XJ24)
    // {
    //     for (i = 0; i < batterySet.xj24.cellNum - batterySet.xj24.endCellNum; i++)
    //     {
    //         uint8_t message[20];
    //         sprintf(message, "������%d#Ƿѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.singleCellVoltMin,
    //               &alarmData[i], message);
    //         sprintf(message, "������%d#��\xFDѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.singleCellVoltMax,
    //               &alarmData[i], message);
    //     }
    //     for (; i < batterySet.xj24.cellNum; i++)
    //     {
    //         uint8_t message[20];
    //         sprintf(message, "ĩ�˵��%d#Ƿѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.endCellVoltMin,
    //               &alarmData[i], message);
    //         sprintf(message, "ĩ�˵��%d#��\xFDѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.endCellVoltMax,
    //               &alarmData[i], message);
    //     }
    // }
    // else if (batterySet.batteryType == XJ55)
    // {
    //     for (i = 0; i < batterySet.xj55.cellNum - batterySet.xj55.endCellNum; i++)
    //     {
    //         uint8_t message[20];
    //         sprintf(message, "������%d#Ƿѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.singleCellVoltMin,
    //               &alarmData[i], message);
    //         sprintf(message, "������%d#��\xFDѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj55[i / 55].volt[i % 55] > batterySet.xj55.singleCellVoltMax,
    //               &alarmData[i], message);
    //     }
    //     for (; i < batterySet.xj55.cellNum; i++)
    //     {
    //         uint8_t message[20];
    //         sprintf(message, "ĩ�˵��%d#Ƿѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.endCellVoltMin,
    //               &alarmData[i], message);
    //         sprintf(message, "ĩ�˵��%d#��\xFDѹ\xFF\xFF", i + 1);
    //         alarm(battery_xj55[i / 55].volt[i % 55] > batterySet.xj55.endCellVoltMax,
    //               &alarmData[i], message);
    //     }
    // }

    // // { // �ۺϲ����̵������
    // //     static AlarmTypeDef swAlarm[8] = {0};
    // //     static MMODBUS swAlarmCmd = {0};
    // //     for (i = 0; i < switchModuleSet.synthesisCollection.alarmChannelNum; i++)
    // //     {
    // //         // DEBUGINFO("synthesisMeasurementRelayOutput = %d\n",sysInfoSet.synthesisMeasurementRelayOutput[i]);
    // //         // DEBUGINFO("%s\n",outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);
    // //         if ((synthesisCollection.sw_01to16 >> i) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode)
    // //         {
    // //             if (swAlarm[i].flag == NO_ALARM)
    // //             {
    // //                 swAlarm[i].flag = ALARM;
    // //                 alarmStart(&swAlarm[i], outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);

    // //                 // �����·��̵���
    // //                 swAlarmCmd.SlaveAddr = 0x61;
    // //                 swAlarmCmd.mode = 0x00;
    // //                 swAlarmCmd.flag = 0x00;
    // //                 swAlarmCmd.Order = 0x06;
    // //                 swAlarmCmd.Length = 1;
    // //                 swAlarmCmd.reserved = 0x00;
    // //                 swAlarmCmd.waitTime = 1000;
    // //                 swAlarmCmd.VPaddr = 0;
    // //                 swAlarmCmd.ModbusReg = 0x0300;
    // //                 swAlarmCmd.databuff = &synthesisCollection.sw_01to16;
    // //                 pushToEmergency(&swAlarmCmd);
    // //             }
    // //         }
    // //         else
    // //         {
    // //             if (swAlarm[i].flag == ALARM)
    // //             {
    // //                 swAlarm[i].flag = NO_ALARM;
    // //                 alarmEnd(&swAlarm[i]);

    // //                 // �����·��̵���
    // //                 swAlarmCmd.SlaveAddr = 0x61;
    // //                 swAlarmCmd.mode = 0x00;
    // //                 swAlarmCmd.flag = 0x00;
    // //                 swAlarmCmd.Order = 0x06;
    // //                 swAlarmCmd.Length = 1;
    // //                 swAlarmCmd.reserved = 0x00;
    // //                 swAlarmCmd.waitTime = 1000;
    // //                 swAlarmCmd.VPaddr = 0;
    // //                 swAlarmCmd.ModbusReg = 0x0300;
    // //                 swAlarmCmd.databuff = &synthesisCollection.sw_01to16;
    // //                 pushToEmergency(&swAlarmCmd);
    // //             }
    // //         }
    // //         // DEBUGINFO("swAlarm[%d]flag = %d\n", i, swAlarm[i].flag);
    // //     }
    // // }

    // // { // �ۺϲ����̵�������
    // //

    // //     alarm((synthesisCollection.sw_17to24 >> (22 - 1) % 16) & 0x0001,
    // //           &alarmData, inputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);
    // // }
}

#define ROW_NUM 14 // �������
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // ���ǰҳ��
    uint16_t curPageDisplay;
    uint16_t alarmNum;
    uint16_t maxPage;
    DEBUGINFO("realTimeAlarmDisplay");
    alarmNum = getListLenth();
    DEBUGINFO("getListLenth");
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
        AlarmTypeDef tempNode;
        tempNode.nextVP = realTimeAlarmHeader.nextVP;
        while (i < curPage * 14)
        {
            read_dgus_vp(tempNode.nextVP, (uint8_t *)&tempNode, sizeof(AlarmTypeDef));
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
            if (tempNode.curVP != 0)
            {
                sprintf(alarmBuff.date, "20%02bd-%02bd-%02bd\xFF\xFF", tempNode.startTime.year, tempNode.startTime.month, tempNode.startTime.day);
                sprintf(alarmBuff.time, "%02bd:%02bd:%02bd\xFF\xFF", tempNode.startTime.hour, tempNode.startTime.minute, tempNode.startTime.second);
                strncpy(alarmBuff.message, tempNode.message, 50);
                read_dgus_vp(tempNode.nextVP, (uint8_t *)&tempNode, sizeof(AlarmTypeDef));
            }
            write_dgus_vp(0xA100 + 0x50 * i, (uint8_t *)&alarmBuff, 45);
        }
    }
}

void historyAlarmDisplay()
{
    uint16_t curPage = 0;    // ���ǰҳ��
    uint16_t curPageDisplay; // ��ǰҳ��ʾ��
    uint16_t alarmNum;
    uint16_t maxPage;
    uint16_t i;
    alarmNum = pHistoryAlarm.head < pHistoryAlarm.tail ? pHistoryAlarm.tail - pHistoryAlarm.head : pHistoryAlarm.tail + HISTORY_ALARM_SIZE - pHistoryAlarm.head;
    maxPage = alarmNum ? (alarmNum - 1) / ROW_NUM : 0;
    // DEBUGINFO("maxPage = %d\n", maxPage);
    read_dgus_vp(0xAC00, (uint8_t *)&curPage, 1);
    if (curPage > maxPage)
    {
        curPage = maxPage;
        write_dgus_vp(0xAC00, (uint8_t *)&curPage, 1);
    }
    curPageDisplay = curPage + 1;
    // DEBUGINFO("%d\n", curPageDisplay);
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
                    strncpy(alarmTxt.message, alarmData.message, 40);
                    if (memcmp(&alarmData.endTime, "\x00\x00\x00\x00\x00\x00", 6) != 0) // �н���ʱ����ʾ����ʱ��
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
                    strncpy(alarmTxt.message, alarmData.message, 40);
                    if (memcmp(&alarmData.endTime, "\x00\x00\x00\x00\x00\x00", 6) != 0) // �н���ʱ����ʾ����ʱ��
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

            case 0xAC05: // �����ʷ����
                pHistoryAlarm.head = pHistoryAlarm.tail = 0;
                Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
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
    DEBUGINFO("%d %d\n", pHistoryAlarm.head, pHistoryAlarm.tail);
    if (pHistoryAlarm.head <= HISTORY_ALARM_SIZE && pHistoryAlarm.tail <= HISTORY_ALARM_SIZE)
    {
        uint16_t i;
        AlarmTypeDef buff;
        if (pHistoryAlarm.head <= pHistoryAlarm.tail)
        {
            for (i = pHistoryAlarm.head; i < pHistoryAlarm.tail; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * HISTORY_ALARM_NORFLASH_OFFSET,
                               (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * HISTORY_ALARM_NORFLASH_OFFSET,
                              (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
            }
        }
        else if (pHistoryAlarm.head > pHistoryAlarm.tail)
        {
            // norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
            //                HISTORY_ALARM_DATA_VP + pHistoryAlarm.head * HISTORY_ALARM_NORFLASH_OFFSET,
            //                (HISTORY_ALARM_SIZE - pHistoryAlarm.head) * HISTORY_ALARM_NORFLASH_OFFSET);
            // norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR,
            //                HISTORY_ALARM_DATA_VP,
            //                pHistoryAlarm.tail * HISTORY_ALARM_NORFLASH_OFFSET);

            for (i = pHistoryAlarm.head; i < HISTORY_ALARM_SIZE; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * HISTORY_ALARM_NORFLASH_OFFSET,
                               (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * HISTORY_ALARM_NORFLASH_OFFSET,
                              (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
            }
            for (i = 0; i < pHistoryAlarm.tail; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * HISTORY_ALARM_NORFLASH_OFFSET,
                               (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * HISTORY_ALARM_NORFLASH_OFFSET,
                              (uint8_t *)&buff, HISTORY_ALARM_NORFLASH_OFFSET);
            }
        }
    }
}