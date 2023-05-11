#include "alarm.h"
#include "const.h"
#include "data.h"
#include <STDLIB.H>
#include <STRING.H>
#include "rtc.h"
#include "norflash.h"
#include "debug.h"
#include "ui.h"
#include "modbus.h"

AlarmTypeDef realTimeAlarmHeader = {0};
uint16_t listLen = 0; // ���г���

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
code uint8_t outputCodeTable[31][40] = {
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
};

#define FAULT 1
#define NORMAL 0

uint16_t alarmTabFlag[31] = {0};

void synthesisCollect_JDQ_State() // �ۺϲɼ��̵����������״̬
{
    uint16_t i, j;

    if (batterySet.batteryType == XJ24) // ���ģ��ͨѶ����
    {
        for (i = 0; i < batterySet.xj24.batteryModuleNum; i++)
        {
            if (chargeModule_Comm_Sta[i])
            {
                alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] = NORMAL;
            }
            else
            {
                alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] = FAULT;
                break;
            }
        }
    }
    else if (batterySet.batteryType == XJ55)
    {
        for (i = 0; i < batterySet.xj55.batteryModuleNum; i++)
        {
            if (chargeModule_Comm_Sta[i])
            {
                alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] = NORMAL;
            }
            else
            {
                alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] = FAULT;
                break;
            }
        }
    }

    for (i = 0; i < chargeModuleSet.moduleNum; i++)
    { // ���ģ�����
        if (chargeModule[i].DI.faultState == 0)
        {
            alarmTabFlag[CHARGE_MODULE_FAULT] = FAULT;
            break;
        }
        else
        {
            alarmTabFlag[CHARGE_MODULE_FAULT] = NORMAL;
        }
    }

    if (AC_Insulation.negativeBusAlarm || AC_Insulation.positiveBusAlarm)
    { // ��������
        alarmTabFlag[AC_FAULT] = FAULT;
    }
    else
    {
        alarmTabFlag[AC_FAULT] = NORMAL;
    }

    for (i = 0; i < DC_ConverterSet.Num; i++)
    { // ֱ������
        if (dc_4850_Module[i].DI.faultState == 0)
        {
            alarmTabFlag[DCDC_MODULE_FAULT] = FAULT;
            break;
        }
        else
        {
            alarmTabFlag[DCDC_MODULE_FAULT] = NORMAL;
        }
    }

    if (synthesisCollection_Comm_Sta == 0)
    { // �ۺϲ�����Ԫ����
        alarmTabFlag[SYNTHESIS_COLLECTION_MODULE_FAULT] = NORMAL;
    }
    else
    {
        alarmTabFlag[SYNTHESIS_COLLECTION_MODULE_FAULT] = FAULT;
    }

    for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
    { // ����������ͨѶ����
        if (switchModule_Comm_Sta[i])
        {
            alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT] = NORMAL;
        }
        else
        {
            alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT] = FAULT;
            break;
        }
    }
    for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
    { // ������ͨѶ����
        if (switchModule_Comm_Sta[i])
        {
            alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT] = NORMAL;
        }
        else
        {
            alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT] = FAULT;
            break;
        }
    }

    for (i = 0; i < batterySet.xj24.cellNum - batterySet.xj24.endCellNum; i++)
    { // �����ع���
        if (battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.singleCellVoltMin ||
            battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.singleCellVoltMax)
        {
            alarmTabFlag[SINGLE_CELL_FAULT] = FAULT;
            break;
        }
        else
        {
            alarmTabFlag[SINGLE_CELL_FAULT] = NORMAL;
        }
    }
    for (i = 0; i < insulationSet.DC.Num; i++)
    { // ֧·��ԵͨѶ����
        if (DC_Insulation_Comm_Sta[i])
        {
            alarmTabFlag[DC_INSULATION_COMMUNICATION_FAULT] = NORMAL;
        }
        else
        {
            alarmTabFlag[DC_INSULATION_COMMUNICATION_FAULT] = FAULT;
            break;
        }
    }
    if (AC_Insulation.negativeBusAlarm || AC_Insulation.positiveBusAlarm)
    { // ֧·��Ե����
        for (i = 0; i < insulationSet.DC.Num; i++)
        {
            for (j = 0; j < 30; i++)
            {
                int16_t insulationRes;
                uint8_t message[20];
                sprintf(message, "ĸ�߾�Ե%d#%d·\xFF\xFF", i + 1, j + 1);
                insulationRes = DC_Insulation[i].res[j];
                insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // ���Ϊ��ĸ·������ֵ��100.0
                if (insulationRes <= insulationSet.DC.resAlarm)
                {
                    alarmTabFlag[DC_INSULATION_FAULT] = FAULT;
                    break;
                }
                else
                {
                    alarmTabFlag[DC_INSULATION_FAULT] = NORMAL;
                }
            }
        }
    }
    { // ϵͳ����
        if (realTimeAlarmHeader.nextVP != 0)
        {
            alarmTabFlag[SYSTEM_FAULT] = NORMAL;
        }
        else
        {
            alarmTabFlag[SYSTEM_FAULT] = FAULT;
        }
    }
    { // INV���ģ����ϸ澯����ͨ�ţ�
        for (i = 0; i < UPS_INV_Set.INV_Num; i++)
        {
            if (inv_Comm_Sta[i] && inv[i].DI.faultState == 0)
            {
                alarmTabFlag[INV_FAULT] = NORMAL;
            }
            else
            {
                alarmTabFlag[INV_FAULT] = FAULT;
                break;
            }
        }
    }
    { // DCDCģ����ϸ澯����ͨ�ţ�
        for (i = 0; i < insulationSet.DC.Num; i++)
        {
            if (DC_Insulation_Comm_Sta[i] && dc_4850_Module[i].DI.faultState == 0)
            {
                alarmTabFlag[DCDC_MODULE_FAULT] = NORMAL;
            }
            else
            {
                alarmTabFlag[DCDC_MODULE_FAULT] = FAULT;
                break;
            }
        }
    }
    { // ��һ��Ԫͨ�Ź��ϸ澯
        if (alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] || alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT])
        {
        }
    }
    { // ��ع��ϸ澯
        extern int16_t batteryVoltSum;
        if (batteryVoltSum > batteryInfoSet.batteryVoltMax || batteryVoltSum < batteryInfoSet.batteryVoltMin)
        {
            alarmTabFlag[BATTERY_FAULT] = FAULT;
        }
        {
            alarmTabFlag[BATTERY_FAULT] = NORMAL;
        }
    }
    { // UPS���ϸ澯����ͨ�ţ�
        for (i = 0; i < UPS_INV_Set.UPS_Num; i++)
        {
            if (ups_Comm_Sta[i] || *((uint16_t *)&ups[i].remoteSignal) == 0)
            {
                alarmTabFlag[UPS_FAULT] = NORMAL;
            }
            else
            {
                alarmTabFlag[UPS_FAULT] = FAULT;
                break;
            }
        }
    }
    { // ������Ƿѹ�澯����ȱ��澯��
        if (AC_InfoSet.AC_InputChannel == SINGLE_PHASE)
        {
            if (AC_InfoSet.AC_InputChannel == 1) // 1·
            {
                if (synthesisCollection.AC_channel_1_Uab > AC_InfoSet.singlePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uab < AC_InfoSet.singlePhaseAC_VolMin)
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = FAULT;
                }
                else
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
                }
            }
            if (AC_InfoSet.AC_InputChannel == 2) // 2·
            {
                if (synthesisCollection.AC_channel_1_Uab > AC_InfoSet.singlePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uab < AC_InfoSet.singlePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_2_Uab > AC_InfoSet.singlePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_2_Uab < AC_InfoSet.singlePhaseAC_VolMin)
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = FAULT;
                }
                else
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
                }
            }
            else if (AC_InfoSet.AC_InputChannel == 0)
            {
                alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
            }
        }
        else if (AC_InfoSet.AC_InputChannel == THREE_PHASE) // ����
        {

            if (AC_InfoSet.AC_InputChannel == 1) // 1·
            {
                if (synthesisCollection.AC_channel_1_Uab > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Ubc > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uca > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uab < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_1_Ubc < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_1_Uca < AC_InfoSet.threePhaseAC_VolMin)
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = FAULT;
                }
                else
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
                }
            }
            else if (AC_InfoSet.AC_InputChannel == 2)
            {
                if (synthesisCollection.AC_channel_1_Uab > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Ubc > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uca > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_1_Uab < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_1_Ubc < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_1_Uca < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_2_Uab > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_2_Ubc > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_2_Uca > AC_InfoSet.threePhaseAC_VolMax ||
                    synthesisCollection.AC_channel_2_Uab < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_2_Ubc < AC_InfoSet.threePhaseAC_VolMin ||
                    synthesisCollection.AC_channel_2_Uca < AC_InfoSet.threePhaseAC_VolMin)
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = FAULT;
                }
                else
                {
                    alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
                }
            }
            else if (AC_InfoSet.AC_InputChannel == 0)
            {
                alarmTabFlag[AC_OVER_VOLT_OR_UNDER_VOLT] = NORMAL;
            }
        }
    }
    { // һ·����ͣ��澯
        if (synthesisCollection.AC_channel_1_Uab == 0)
        {
            alarmTabFlag[CHANNEL_1_POWER_FAILURE] = FAULT;
        }
        else
        {
            alarmTabFlag[CHANNEL_1_POWER_FAILURE] = NORMAL;
        }
    }
    { // ��·����ͣ��澯
        if (synthesisCollection.AC_channel_2_Uab == 0)
        {
            alarmTabFlag[CHANNEL_2_POWER_FAILURE] = FAULT;
        }
        else
        {
            alarmTabFlag[CHANNEL_2_POWER_FAILURE] = NORMAL;
        }
    }

    if (AC_Insulation.positiveBusAlarm || AC_Insulation.negativeBusAlarm)
    { // ֱ��ĸ�߲�ƽ��澯��ĸ�߾�Ե��
        alarmTabFlag[DC_BUS_INSULATION] = FAULT;
    }
    else
    {
        alarmTabFlag[DC_BUS_INSULATION] = NORMAL;
    }

    if (synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax ||
        synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin) // ��ĸ��Ƿѹ�澯
    {
        alarmTabFlag[CONTROL_BUS_UNDER_VOLT] = FAULT;
    }
    else
    {
        alarmTabFlag[CONTROL_BUS_UNDER_VOLT] = NORMAL;
    }
    // ��ĸ��Ƿѹ�澯
    if (synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax ||
        synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax)
    {
        alarmTabFlag[CLOSE_BUS_UNDER_VOLT] = FAULT;
    }
    else
    {
        alarmTabFlag[CLOSE_BUS_UNDER_VOLT] = NORMAL;
    }
    // ��������ֱ���澯
    if (AC_Insulation.positiveBusAlarm == 0 && AC_Insulation.negativeBusAlarm == 0)
    {
        for (i = 0; i < 30; i++)
        {
            int16_t insulationRes;
            insulationRes = DC_Insulation[0].res[i];
            insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // ���Ϊ��ĸ·������ֵ��100.0
            if (insulationRes <= insulationSet.DC.resAlarm)
            {
                alarmTabFlag[AC_INTO_DC] = FAULT;
                break;
            }
            else
            {
                alarmTabFlag[AC_INTO_DC] = NORMAL;
            }
        }
    }
}

struct
{
    uint16_t head;
    uint16_t tail;
} pHistoryAlarm = {0}; // ��ʷ��������ͷβָ��

void pHistoryAlarmMove()
{
    pHistoryAlarm.tail = (pHistoryAlarm.tail + 1) % HISTORY_ALARM_NUM;
    if (pHistoryAlarm.tail == pHistoryAlarm.head)
    {
        pHistoryAlarm.head = (pHistoryAlarm.head + 1) % HISTORY_ALARM_NUM;
    }
    Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
}

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

    write_dgus_vp(node->curVP, (uint8_t *)node, sizeof(AlarmTypeDef) / 2);
    listLen++;
}

void listDelete(/* AlarmTypeDef *list */ uint32_t VP /* , AlarmTypeDef *node */)
{
    AlarmTypeDef prevNode;
    AlarmTypeDef curNode;
    DEBUGINFO("listDelete\n");
    prevNode = realTimeAlarmHeader;
    while (prevNode.nextVP != VP && prevNode.nextVP != 0)
    {
        read_dgus_vp(prevNode.nextVP, (uint8_t *)&prevNode, sizeof(AlarmTypeDef) / 2);
    }
    if (prevNode.nextVP == 0)
    {
        return;
    }
    read_dgus_vp(VP, (uint8_t *)&curNode, sizeof(AlarmTypeDef) / 2);
    prevNode.nextVP = curNode.nextVP;
    write_dgus_vp(prevNode.curVP, (uint8_t *)&prevNode, sizeof(AlarmTypeDef) / 2);
    curNode.nextVP = 0;
    write_dgus_vp(curNode.curVP, (uint8_t *)&curNode, sizeof(AlarmTypeDef) / 2);
    listLen--;
}

// uint16_t getListLenth(/* AlarmTypeDef *list */)
// {
//     AlarmTypeDef temp;
//     uint16_t i = 0;
//     temp = realTimeAlarmHeader;
//     // DEBUGINFO("i = %d\n",i);
//     while (temp.nextVP != 0)
//     {
//         // DEBUGINFO("temp.nextVP = %ld\n",temp.nextVP);
//         read_dgus_vp(temp.nextVP, (uint8_t *)&temp, sizeof(AlarmTypeDef) / 2);
//         // DEBUGINFO("i = %d\n",i);
//         i++;
//     }
//     return i;
// }

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
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * ALARM_SIZE, (uint8_t *)&alarm, ALARM_SIZE);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * ALARM_SIZE, (uint8_t *)&alarm, ALARM_SIZE);
    pHistoryAlarmMove();
    Nor_Flash_write(P_HISTORY_ALARM_NORFLASH_ADDR, (uint8_t *)&pHistoryAlarm, 2);
}

void saveEndAlarm(AlarmTypeDef alarm)
{
    Nor_Flash_write(HISTORY_ALARM_NORFLASH_ADDR + alarm.saveSerialNum * ALARM_SIZE, (uint8_t *)&alarm, ALARM_SIZE);
    write_dgus_vp(HISTORY_ALARM_DATA_VP + alarm.saveSerialNum * ALARM_SIZE, (uint8_t *)&alarm, ALARM_SIZE);
}

void alarmStart(AlarmTypeDef *alarmData, uint32_t VP, uint8_t *alarmMessage)
{
    alarmData->flag = ALARM;
    // DEBUGINFO("alarmData->flag = %d",alarmData->flag);
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
    read_dgus_vp(VP, (uint8_t *)&alarmData, sizeof(AlarmTypeDef) / 2);
    // DEBUGINFO("alarmData.flag = %d\n",alarmData.flag);
    // DEBUGINFO("VP = %ld\n",VP);
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
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 1, "����1·������\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 2, "����1·����Ƿѹ\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2·
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.singlePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 3, "����2·������\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 4, "����2·����Ƿѹ\xFF\xFF");
                }
            }
        }

        else if (AC_InfoSet.AC_InputChannel == THREE_PHASE) // ����
        {
            if (AC_InfoSet.AC_InputChannel >= 1) // 1·
            {
                {
                    alarm(synthesisCollection.AC_channel_1_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 5, "���ཻ��1·A���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 6, "���ཻ��1·B���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 7, "���ཻ��1·C���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 8, "���ཻ��1·A��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 9, "���ཻ��1·B��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 10, "���ཻ��1·C��Ƿѹ\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2·
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 11, "���ཻ��2·A���ѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 12, "���ཻ��2·B���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 13, "���ཻ��2·C���\xFDѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 14, "���ཻ��2·A��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 15, "���ཻ��2·B��Ƿѹ\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 16, "���ཻ��2·C��Ƿѹ\xFF\xFF");
                }
            }
        }
    }

    {
        if (sysInfoSet.siliconChain == 0) // �޹���
        {
            {
                alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 17, "ĸ�߹�\xFDѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 18, "ĸ��Ƿѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 19, "ģ���\xFDѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 20, "ģ��Ƿѹ\xFF\xFF");
            }
        }
        else if (sysInfoSet.siliconChain == 1) // �й���
        {
            {
                alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 21, "��ĸ��\xFDѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 22, "��ĸǷѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 23, "��ĸ��\xFDѹ\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 24, "��ĸǷѹ\xFF\xFF");
            }
        }
    }

    {
        extern int16_t batteryVoltSum;
        {
            alarm(batteryVoltSum > batteryInfoSet.batteryVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 25, "������\xFDѹ\xFF\xFF");
        }
        {
            alarm(batteryVoltSum < batteryInfoSet.batteryVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 26, "�����Ƿѹ\xFF\xFF");
        }
    }
    { // �ۺϲ����̵������
        for (i = 0; i < switchModuleSet.synthesisCollection.alarmChannelNum; i++)
        {
            uint8_t message[30];
            sprintf(message, "�ۺϲɼ�����%d����\xFF\xFF", i + 1);
            alarm((synthesisCollection.sw_01to16 >> i) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (27 + i), message);
        }
        {
            alarm((synthesisCollection.sw_01to16 >> (13 - 1)) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 39, "�ۺϲɼ�����13����\xFF\xFF");
        }
    }
    { // ������
        for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
        {
            for (j = 0; j < switchModuleSet.switchModule.alarmChannelNum[i]; i++)
            {
                uint8_t message[20];
                sprintf(message, "%d#���ص�%d·����\xFF\xFF", i + 1, j + 1);
                alarm(switchModule[i].sw_u16[j / 16] >> j % 16 != switchModuleSet.switchModule.accessMode[i],
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (40 + i * 30 + j), message);
            }
        }
    }

    if (batterySet.batteryType == XJ24)
    {
        for (i = 0; i < batterySet.xj24.cellNum - batterySet.xj24.endCellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "������%d#Ƿѹ\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.singleCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (160 + i * 2), message);
            sprintf(message, "������%d#��\xFDѹ\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.singleCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (161 + i * 2), message);
        }
        for (; i < batterySet.xj24.cellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "ĩ�˵��%d#Ƿѹ\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.endCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (160 + i * 2), message);
            sprintf(message, "ĩ�˵��%d#��\xFDѹ\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.endCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (161 + i * 2), message);
        }
    }
    else if (batterySet.batteryType == XJ55)
    {
        for (i = 0; i < batterySet.xj55.cellNum - batterySet.xj55.endCellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "������%d#Ƿѹ\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.singleCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (300 + i * 2), message);
            sprintf(message, "������%d#��\xFDѹ\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] > batterySet.xj55.singleCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (301 + i * 2), message);
        }
        for (; i < batterySet.xj55.cellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "ĩ�˵��%d#Ƿѹ\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.endCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (300 + i * 2), message);
            sprintf(message, "ĩ�˵��%d#��\xFDѹ\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] > batterySet.xj55.endCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (301 + i * 2), message);
        }
    }

    {
        if (AC_Insulation.negativeBusAlarm || AC_Insulation.positiveBusAlarm)
        {
            for (i = 0; i < insulationSet.DC.Num; i++)
            {
                for (j = 0; j < 30; i++)
                {
                    int16_t insulationRes;
                    uint8_t message[20];
                    sprintf(message, "ĸ�߾�Ե%d#%d·\xFF\xFF", i + 1, j + 1);
                    insulationRes = DC_Insulation[i].res[j];
                    insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // ���Ϊ��ĸ·������ֵ��100.0
                    alarm(insulationRes <= insulationSet.DC.resAlarm,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (540 + i * 30 + j), message);
                }
            }
        }
    }

    {
        alarm(AC_Insulation.positiveBusAlarm,
              REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 600, "ĸ�߾�Ե-���Ե�\xFF\xFF");
        alarm(AC_Insulation.negativeBusAlarm,
              REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 601, "ĸ�߾�Ե-���Ե�\xFF\xFF");
    }

    {
        uint8_t message[20];
        for (i = 0; i < 8; i++)
        {
            sprintf(message, "%d#ģ�����", i + 1);
            alarm(chargeModule[i].DI.faultState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (602 + i), message);
        }
        for (i = 0; i < 8; i++)
        {
            sprintf(message, "%d#ģ�鱣��", i + 1);
            alarm(chargeModule[i].DI.protectState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (610 + i), message);
        }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#�����\xFDѹ", i + 1);
        //     alarm(chargeModule[i].outputVolt >= chargeModuleSet.,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (618 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#���Ƿѹ", i + 1);
        //     alarm(chargeModule[i].outputVolt >= chargeModuleSet.,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (626 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#������\xFDѹ", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (634 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#����Ƿѹ", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (642 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#����ȱ��", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (650 + i), message);
        // }
    }
    {
        uint8_t message[20];
        for (i = 0; i < UPS_INV_Set.UPS_Num; i++)
        {
            sprintf(message, "%d#ң�ŵ���·���\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.bypassOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (658 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ�����������\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.rectifyInverterOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (659 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ�ֱ��������\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.DC_InverterOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (660 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ����\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.overTemperature,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (661 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ����\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.overload,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (662 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ���·����\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.bypassFault,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (663 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ���·����\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.mainFault,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (664 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ��ظ�ѹ\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.batteryHighVolt,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (665 + 9 * i), message);

            sprintf(message, "%d#ң�ŵ��ص�ѹ\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.battertLowVolt,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (666 + 9 * i), message);
        }
    }

    {
        uint8_t message[20];
        for (i = 0; i < DC_ConverterSet.Num; i++)
        {
            sprintf(message, "%d#DCģ�����\xFF\xFF", i + 1);
            alarm(dc_4850_Module[i].DI.faultState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (676 + i), message);
        }
        for (i = 0; i < DC_ConverterSet.Num; i++)
        {
            sprintf(message, "%d#DCģ�鱣��״̬\xFF\xFF", i + 1);
            alarm(dc_4850_Module[i].DI.protectState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (680 + i), message);
        }
    }

    {
        static MMODBUS outputCmd[8];
        synthesisCollect_JDQ_State();
        for (i = 0; i < 8; i++)
        {
            if (sysInfoSet.synthesisMeasurementRelayOutput[i] != 0)
            {
                alarm(alarmTabFlag[sysInfoSet.synthesisMeasurementRelayOutput[i]] == 0,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (684 + i), outputCodeTable[sysInfoSet.synthesisMeasurementRelayOutput[i]]);
                if (alarmTabFlag[sysInfoSet.synthesisMeasurementRelayOutput[i]] == 0)
                {
                    if (synthesisCollection.sw_01to16 & (0x0001 << i))
                    {
                        synthesisCollection.sw_01to16 |= 0x0001 << i;
                        outputCmd[i].SlaveAddr = 0x61;
                        outputCmd[i].mode = 0x00;
                        outputCmd[i].flag = 0x00;
                        outputCmd[i].Order = 0x06;
                        outputCmd[i].Length = 1;
                        outputCmd[i].reserved = 0x00;
                        outputCmd[i].waitTime = 1000;
                        outputCmd[i].VPaddr = 0;
                        outputCmd[i].ModbusReg = 0x0000;
                        outputCmd[i].databuff = (uint16_t *)(&synthesisCollection.sw_01to16);
                        pushToEmergency(outputCmd + i);
                    }
                }
                else
                {
                    if (synthesisCollection.sw_01to16 & (0x0001 << i))
                    {
                        synthesisCollection.sw_01to16 &= ~(0x0001 << i);
                        outputCmd[i].SlaveAddr = 0x61;
                        outputCmd[i].mode = 0x00;
                        outputCmd[i].flag = 0x00;
                        outputCmd[i].Order = 0x06;
                        outputCmd[i].Length = 1;
                        outputCmd[i].reserved = 0x00;
                        outputCmd[i].waitTime = 1000;
                        outputCmd[i].VPaddr = 0;
                        outputCmd[i].ModbusReg = 0x0000;
                        outputCmd[i].databuff = (uint16_t *)(&synthesisCollection.sw_01to16);
                        pushToEmergency(outputCmd + i);
                    }
                }
            }
        }
    }

    {
        if (sysInfoSet.synthesisMeasurementRelayInput22 != 0)
        {
            alarm(synthesisCollection.sw_17to24 >> ((22 - 1) % 16) & 0x0001, 
            REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 692,inputCodeTable[sysInfoSet.synthesisMeasurementRelayInput22]);
        }
    }
}

#define ROW_NUM 14 // �������
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // ���ǰҳ��
    uint16_t curPageDisplay;
    uint16_t alarmNum;
    uint16_t maxPage;
    // DEBUGINFO("realTimeAlarmDisplay");
    alarmNum = listLen;
    // DEBUGINFO("getListLenth");
    DEBUGINFO("alarmNum = %d\n", alarmNum);
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
        tempNode.curVP = realTimeAlarmHeader.nextVP;
        read_dgus_vp(tempNode.curVP, (uint8_t *)&tempNode, sizeof(AlarmTypeDef) / 2);
        while (i < curPage * 14)
        {
            read_dgus_vp(tempNode.nextVP, (uint8_t *)&tempNode, sizeof(AlarmTypeDef) / 2);
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
                read_dgus_vp(tempNode.nextVP, (uint8_t *)&tempNode, sizeof(AlarmTypeDef) / 2);
            }
            write_dgus_vp(0xA100 + 0x50 * i, (uint8_t *)&alarmBuff, 45);
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
            case 0xB040:
            {
                read_dgus_vp(0xb040, (uint8_t *)alarmSoundOnOff, 1);
                Nor_Flash_write(ALARM_SOUND_ONOFF_NORFLASH_ADDR, (uint8_t *)alarmSoundOnOff, 2);
            }
            break;

            default:
                break;
            }

            variableChangedIndication.flag = 0;
            write_dgus_vp(0x0F00, (uint8_t *)&variableChangedIndication, 1);
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
    alarmNum = pHistoryAlarm.head <= pHistoryAlarm.tail ? pHistoryAlarm.tail - pHistoryAlarm.head : pHistoryAlarm.tail + HISTORY_ALARM_NUM - pHistoryAlarm.head;
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
                    read_dgus_vp(HISTORY_ALARM_DATA_VP + dispalyNum * ALARM_SIZE, (uint8_t *)&alarmData, ALARM_SIZE);
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
                dispalyNum = (pHistoryAlarm.tail - 1 - i - curPage * 14 + HISTORY_ALARM_NUM) % HISTORY_ALARM_NUM;
                if ((0 <= (int16_t)dispalyNum && (int16_t)dispalyNum <= pHistoryAlarm.tail) ||
                    (pHistoryAlarm.head <= dispalyNum && dispalyNum <= HISTORY_ALARM_NUM - 1))
                {
                    read_dgus_vp(HISTORY_ALARM_DATA_VP + dispalyNum * ALARM_SIZE, (uint8_t *)&alarmData, ALARM_SIZE);
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
    // DEBUGINFO("%d %d\n", pHistoryAlarm.head, pHistoryAlarm.tail);
    if (pHistoryAlarm.head <= HISTORY_ALARM_NUM && pHistoryAlarm.tail <= HISTORY_ALARM_NUM)
    {
        uint16_t i;
        AlarmTypeDef buff;
        if (pHistoryAlarm.head <= pHistoryAlarm.tail)
        {
            for (i = pHistoryAlarm.head; i < pHistoryAlarm.tail; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * ALARM_SIZE,
                               (uint8_t *)&buff, ALARM_SIZE);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * ALARM_SIZE,
                              (uint8_t *)&buff, ALARM_SIZE);
            }
        }
        else if (pHistoryAlarm.head > pHistoryAlarm.tail)
        {
            // norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR + pHistoryAlarm.head * ALARM_SIZE,
            //                HISTORY_ALARM_DATA_VP + pHistoryAlarm.head * ALARM_SIZE,
            //                (HISTORY_ALARM_NUM - pHistoryAlarm.head) * ALARM_SIZE);
            // norFlashToDgus(HISTORY_ALARM_NORFLASH_ADDR,
            //                HISTORY_ALARM_DATA_VP,
            //                pHistoryAlarm.tail * ALARM_SIZE);

            for (i = pHistoryAlarm.head; i < HISTORY_ALARM_NUM; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * ALARM_SIZE,
                               (uint8_t *)&buff, ALARM_SIZE);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * ALARM_SIZE,
                              (uint8_t *)&buff, ALARM_SIZE);
            }
            for (i = 0; i < pHistoryAlarm.tail; i++)
            {
                Nor_Flash_read(HISTORY_ALARM_NORFLASH_ADDR + i * ALARM_SIZE,
                               (uint8_t *)&buff, ALARM_SIZE);
                write_dgus_vp(HISTORY_ALARM_DATA_VP + i * ALARM_SIZE,
                              (uint8_t *)&buff, ALARM_SIZE);
            }
        }
    }
}