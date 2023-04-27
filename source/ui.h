#ifndef __PAGE_H__
#define __PAGE_H__
#include "sys.h"
#include "norflash.h"
#include "data.h"

typedef struct
{
    int8_t flag;
    int16_t addr;
    int8_t len;
} VariableChangedIndicationTypeDef;

#define SYSTEM_220V 0
#define SYSTEM_110V 1

#define XJ24 0
#define XJ55 1

#define THREE_PHASE 0  // ����
#define SINGLE_PHASE 1 // ����

#define OPENING 0 // ��բ
#define CLOSING 1 // ��բ
#define FAULT 2   // ����
#define NORMAL 3  // ����

#define ARD 0
#define TH 1

#define FLOAT_CHARGE 0    // ����״̬
#define EQUALIZE_CHARGE 1 // ����״̬
#define DISCHARGE 2//�ŵ�״̬

#define CHARGE_TIMER 1 // ��ع���ʱ��

typedef struct
{
    int8_t day;
    int8_t hour;
    int8_t minite;
    int8_t second;
} timeTypeDef;

void PageFunction(void);
void openScreenProtection(void);
void weekDisplay(void);
void chargeModeSwitch(uint16_t chargeMode);
void batteryManage(void);
void publicUI(void);
#endif
