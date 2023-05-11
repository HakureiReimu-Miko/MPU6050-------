#ifndef __PAGE_H__
#define __PAGE_H__
#include "sys.h"
#include "norflash.h"
#include "data.h"

typedef struct
{
    uint8_t flag;
    uint16_t addr;
    uint8_t len;
} VariableChangedIndicationTypeDef;

#define SYSTEM_220V 0
#define SYSTEM_110V 1

#define XJ24 0
#define XJ55 1

#define THREE_PHASE 0  // 三相
#define SINGLE_PHASE 1 // 单相

#define OPENING 0 // 分闸
#define CLOSING 1 // 合闸
#define SW_FAULT 2   // 故障
#define SW_NORMAL 3  // 正常

#define ARD 0
#define TH 1

#define FLOAT_CHARGE 0    // 浮充状态
#define EQUALIZE_CHARGE 1 // 均充状态
#define DISCHARGE 2//放电状态

#define CHARGE_TIMER 1 // 电池管理定时器

typedef struct
{
    int8_t day;
    int8_t hour;
    int8_t minite;
    int8_t second;
} timeTypeDef;

extern int16_t batteryVoltSum;

void PageFunction(void);
void weekDisplay(void);
void chargeModeSwitch(uint16_t chargeMode);
void batteryManage(void);
void alarmSoundPlay();
void screenProtection(void);
void publicUI(void);
#endif
