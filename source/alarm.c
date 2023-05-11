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
uint16_t listLen = 0; // 队列长度

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
code uint8_t outputCodeTable[31][40] = {
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
};

#define FAULT 1
#define NORMAL 0

uint16_t alarmTabFlag[31] = {0};

void synthesisCollect_JDQ_State() // 综合采集继电器输出报警状态
{
    uint16_t i, j;

    if (batterySet.batteryType == XJ24) // 充电模块通讯故障
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
    { // 充电模块故障
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
    { // 交流故障
        alarmTabFlag[AC_FAULT] = FAULT;
    }
    else
    {
        alarmTabFlag[AC_FAULT] = NORMAL;
    }

    for (i = 0; i < DC_ConverterSet.Num; i++)
    { // 直流故障
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
    { // 综合采样单元故障
        alarmTabFlag[SYNTHESIS_COLLECTION_MODULE_FAULT] = NORMAL;
    }
    else
    {
        alarmTabFlag[SYNTHESIS_COLLECTION_MODULE_FAULT] = FAULT;
    }

    for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
    { // 开关量故障通讯故障
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
    { // 单体电池通讯故障
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
    { // 单体电池故障
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
    { // 支路绝缘通讯故障
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
    { // 支路绝缘故障
        for (i = 0; i < insulationSet.DC.Num; i++)
        {
            for (j = 0; j < 30; i++)
            {
                int16_t insulationRes;
                uint8_t message[20];
                sprintf(message, "母线绝缘%d#%d路\xFF\xFF", i + 1, j + 1);
                insulationRes = DC_Insulation[i].res[j];
                insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // 如果为合母路数，数值加100.0
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
    { // 系统故障
        if (realTimeAlarmHeader.nextVP != 0)
        {
            alarmTabFlag[SYSTEM_FAULT] = NORMAL;
        }
        else
        {
            alarmTabFlag[SYSTEM_FAULT] = FAULT;
        }
    }
    { // INV逆变模块故障告警（含通信）
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
    { // DCDC模块故障告警（含通信）
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
    { // 任一单元通信故障告警
        if (alarmTabFlag[CHARGE_MODULE_COMMUNICATION_FAULT] || alarmTabFlag[SWITCH_MODULE_COMMUNICATION_FAULT])
        {
        }
    }
    { // 电池故障告警
        extern int16_t batteryVoltSum;
        if (batteryVoltSum > batteryInfoSet.batteryVoltMax || batteryVoltSum < batteryInfoSet.batteryVoltMin)
        {
            alarmTabFlag[BATTERY_FAULT] = FAULT;
        }
        {
            alarmTabFlag[BATTERY_FAULT] = NORMAL;
        }
    }
    { // UPS故障告警（含通信）
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
    { // 交流过欠压告警（含缺相告警）
        if (AC_InfoSet.AC_InputChannel == SINGLE_PHASE)
        {
            if (AC_InfoSet.AC_InputChannel == 1) // 1路
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
            if (AC_InfoSet.AC_InputChannel == 2) // 2路
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
        else if (AC_InfoSet.AC_InputChannel == THREE_PHASE) // 三相
        {

            if (AC_InfoSet.AC_InputChannel == 1) // 1路
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
    { // 一路交流停电告警
        if (synthesisCollection.AC_channel_1_Uab == 0)
        {
            alarmTabFlag[CHANNEL_1_POWER_FAILURE] = FAULT;
        }
        else
        {
            alarmTabFlag[CHANNEL_1_POWER_FAILURE] = NORMAL;
        }
    }
    { // 二路交流停电告警
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
    { // 直流母线不平衡告警（母线绝缘）
        alarmTabFlag[DC_BUS_INSULATION] = FAULT;
    }
    else
    {
        alarmTabFlag[DC_BUS_INSULATION] = NORMAL;
    }

    if (synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax ||
        synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin) // 控母过欠压告警
    {
        alarmTabFlag[CONTROL_BUS_UNDER_VOLT] = FAULT;
    }
    else
    {
        alarmTabFlag[CONTROL_BUS_UNDER_VOLT] = NORMAL;
    }
    // 合母过欠压告警
    if (synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax ||
        synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax)
    {
        alarmTabFlag[CLOSE_BUS_UNDER_VOLT] = FAULT;
    }
    else
    {
        alarmTabFlag[CLOSE_BUS_UNDER_VOLT] = NORMAL;
    }
    // 交流窜入直流告警
    if (AC_Insulation.positiveBusAlarm == 0 && AC_Insulation.negativeBusAlarm == 0)
    {
        for (i = 0; i < 30; i++)
        {
            int16_t insulationRes;
            insulationRes = DC_Insulation[0].res[i];
            insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // 如果为合母路数，数值加100.0
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
} pHistoryAlarm = {0}; // 历史报警队列头尾指针

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
 * @brief 头插法
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
 * @brief 封装报警功能，节省内存
 *
 * @param condition 报警触发条件
 * @param VP 报警数据缓冲地址
 * @param alarmMessage 报警信息
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

    { // 单相单相交流电压报警，page12
        if (AC_InfoSet.AC_InputChannel == SINGLE_PHASE)
        {
            if (AC_InfoSet.AC_InputChannel >= 1) // 1路
            {
                {
                    alarm(synthesisCollection.AC_channel_1_Uab > AC_InfoSet.singlePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 1, "单相1路交流过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 2, "单相1路交流欠压\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2路
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.singlePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 3, "单相2路交流过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.singlePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 4, "单相2路交流欠压\xFF\xFF");
                }
            }
        }

        else if (AC_InfoSet.AC_InputChannel == THREE_PHASE) // 三相
        {
            if (AC_InfoSet.AC_InputChannel >= 1) // 1路
            {
                {
                    alarm(synthesisCollection.AC_channel_1_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 5, "三相交流1路A相过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 6, "三相交流1路B相过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 7, "三相交流1路C相过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 8, "三相交流1路A相欠压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 9, "三相交流1路B相欠压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_1_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 10, "三相交流1路C相欠压\xFF\xFF");
                }
            }
            if (AC_InfoSet.AC_InputChannel >= 2) // 2路
            {
                {
                    alarm(synthesisCollection.AC_channel_2_Uab > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 11, "三相交流2路A相过压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 12, "三相交流2路B相过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca > AC_InfoSet.threePhaseAC_VolMax,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 13, "三相交流2路C相过\xFD压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uab < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 14, "三相交流2路A相欠压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Ubc < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 15, "三相交流2路B相欠压\xFF\xFF");
                }
                {
                    alarm(synthesisCollection.AC_channel_2_Uca < AC_InfoSet.threePhaseAC_VolMin,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 16, "三相交流2路C相欠压\xFF\xFF");
                }
            }
        }
    }

    {
        if (sysInfoSet.siliconChain == 0) // 无硅链
        {
            {
                alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 17, "母线过\xFD压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 18, "母线欠压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 19, "模块过\xFD压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 20, "模块欠压\xFF\xFF");
            }
        }
        else if (sysInfoSet.siliconChain == 1) // 有硅链
        {
            {
                alarm(synthesisCollection.closeBusVolt > DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 21, "合母过\xFD压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.closeBusVolt < DC_InfoSet.closeBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 22, "合母欠压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMax,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 23, "控母过\xFD压\xFF\xFF");
            }
            {
                alarm(synthesisCollection.controlBusVolt > DC_InfoSet.controlBusVoltMin,
                      REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 24, "控母欠压\xFF\xFF");
            }
        }
    }

    {
        extern int16_t batteryVoltSum;
        {
            alarm(batteryVoltSum > batteryInfoSet.batteryVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 25, "电池组过\xFD压\xFF\xFF");
        }
        {
            alarm(batteryVoltSum < batteryInfoSet.batteryVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 26, "电池组欠压\xFF\xFF");
        }
    }
    { // 综合测量继电器输出
        for (i = 0; i < switchModuleSet.synthesisCollection.alarmChannelNum; i++)
        {
            uint8_t message[30];
            sprintf(message, "综合采集开关%d故障\xFF\xFF", i + 1);
            alarm((synthesisCollection.sw_01to16 >> i) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (27 + i), message);
        }
        {
            alarm((synthesisCollection.sw_01to16 >> (13 - 1)) & 0x0001 != switchModuleSet.synthesisCollection.switchAccessMode,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 39, "综合采集开关13故障\xFF\xFF");
        }
    }
    { // 开关量
        for (i = 0; i < switchModuleSet.switchModule.switchNum; i++)
        {
            for (j = 0; j < switchModuleSet.switchModule.alarmChannelNum[i]; i++)
            {
                uint8_t message[20];
                sprintf(message, "%d#开关第%d路故障\xFF\xFF", i + 1, j + 1);
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
            sprintf(message, "单体电池%d#欠压\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.singleCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (160 + i * 2), message);
            sprintf(message, "单体电池%d#过\xFD压\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.singleCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (161 + i * 2), message);
        }
        for (; i < batterySet.xj24.cellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "末端电池%d#欠压\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] < batterySet.xj24.endCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (160 + i * 2), message);
            sprintf(message, "末端电池%d#过\xFD压\xFF\xFF", i + 1);
            alarm(battery_xj24[i / 24].volt[i % 24] > batterySet.xj24.endCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (161 + i * 2), message);
        }
    }
    else if (batterySet.batteryType == XJ55)
    {
        for (i = 0; i < batterySet.xj55.cellNum - batterySet.xj55.endCellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "单体电池%d#欠压\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.singleCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (300 + i * 2), message);
            sprintf(message, "单体电池%d#过\xFD压\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] > batterySet.xj55.singleCellVoltMax,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (301 + i * 2), message);
        }
        for (; i < batterySet.xj55.cellNum; i++)
        {
            uint8_t message[20];
            sprintf(message, "末端电池%d#欠压\xFF\xFF", i + 1);
            alarm(battery_xj55[i / 55].volt[i % 55] < batterySet.xj55.endCellVoltMin,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (300 + i * 2), message);
            sprintf(message, "末端电池%d#过\xFD压\xFF\xFF", i + 1);
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
                    sprintf(message, "母线绝缘%d#%d路\xFF\xFF", i + 1, j + 1);
                    insulationRes = DC_Insulation[i].res[j];
                    insulationRes += i < insulationSet.DC.closeBus_1_ChannelNum ? 1000 : 0; // 如果为合母路数，数值加100.0
                    alarm(insulationRes <= insulationSet.DC.resAlarm,
                          REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (540 + i * 30 + j), message);
                }
            }
        }
    }

    {
        alarm(AC_Insulation.positiveBusAlarm,
              REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 600, "母线绝缘-正对地\xFF\xFF");
        alarm(AC_Insulation.negativeBusAlarm,
              REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * 601, "母线绝缘-负对地\xFF\xFF");
    }

    {
        uint8_t message[20];
        for (i = 0; i < 8; i++)
        {
            sprintf(message, "%d#模块故障", i + 1);
            alarm(chargeModule[i].DI.faultState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (602 + i), message);
        }
        for (i = 0; i < 8; i++)
        {
            sprintf(message, "%d#模块保护", i + 1);
            alarm(chargeModule[i].DI.protectState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (610 + i), message);
        }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#输出过\xFD压", i + 1);
        //     alarm(chargeModule[i].outputVolt >= chargeModuleSet.,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (618 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#输出欠压", i + 1);
        //     alarm(chargeModule[i].outputVolt >= chargeModuleSet.,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (626 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#交流过\xFD压", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (634 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#交流欠压", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (642 + i), message);
        // }
        // for (i = 0; i < 8; i++)
        // {
        //     sprintf(message, "%d#交流缺相", i + 1);
        //     alarm(,
        //           REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (650 + i), message);
        // }
    }
    {
        uint8_t message[20];
        for (i = 0; i < UPS_INV_Set.UPS_Num; i++)
        {
            sprintf(message, "%d#遥信点旁路输出\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.bypassOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (658 + 9 * i), message);

            sprintf(message, "%d#遥信点整流逆变输出\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.rectifyInverterOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (659 + 9 * i), message);

            sprintf(message, "%d#遥信点直流逆变输出\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.DC_InverterOut,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (660 + 9 * i), message);

            sprintf(message, "%d#遥信点过温\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.overTemperature,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (661 + 9 * i), message);

            sprintf(message, "%d#遥信点过载\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.overload,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (662 + 9 * i), message);

            sprintf(message, "%d#遥信点旁路故障\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.bypassFault,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (663 + 9 * i), message);

            sprintf(message, "%d#遥信点主路故障\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.mainFault,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (664 + 9 * i), message);

            sprintf(message, "%d#遥信点电池高压\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.batteryHighVolt,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (665 + 9 * i), message);

            sprintf(message, "%d#遥信点电池低压\xFF\xFF", i + 1);
            alarm(ups[i].remoteSignal.battertLowVolt,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (666 + 9 * i), message);
        }
    }

    {
        uint8_t message[20];
        for (i = 0; i < DC_ConverterSet.Num; i++)
        {
            sprintf(message, "%d#DC模块故障\xFF\xFF", i + 1);
            alarm(dc_4850_Module[i].DI.faultState == 1,
                  REAL_TIME_ALARM_DATA_VP + ALARM_SIZE * (676 + i), message);
        }
        for (i = 0; i < DC_ConverterSet.Num; i++)
        {
            sprintf(message, "%d#DC模块保护状态\xFF\xFF", i + 1);
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

#define ROW_NUM 14 // 表格行数
void realTimeAlarmDisplay()
{
    uint16_t curPage = 0; // 表格当前页码
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
    uint16_t curPage = 0;    // 表格当前页码
    uint16_t curPageDisplay; // 当前页显示用
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
                dispalyNum = (pHistoryAlarm.tail - 1 - i - curPage * 14 + HISTORY_ALARM_NUM) % HISTORY_ALARM_NUM;
                if ((0 <= (int16_t)dispalyNum && (int16_t)dispalyNum <= pHistoryAlarm.tail) ||
                    (pHistoryAlarm.head <= dispalyNum && dispalyNum <= HISTORY_ALARM_NUM - 1))
                {
                    read_dgus_vp(HISTORY_ALARM_DATA_VP + dispalyNum * ALARM_SIZE, (uint8_t *)&alarmData, ALARM_SIZE);
                    sprintf(alarmTxt.date, "20%02bd-%02bd-%02bd\xFF\xFF", alarmData.startTime.year, alarmData.startTime.month, alarmData.startTime.day);
                    sprintf(alarmTxt.time, "%02bd:%02bd:%02bd\xFF\xFF", alarmData.startTime.hour, alarmData.startTime.minute, alarmData.startTime.second);
                    strncpy(alarmTxt.message, alarmData.message, 40);
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

            case 0xAC05: // 清楚历史报警
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