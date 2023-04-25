#ifndef __IR_H__
#define __IR_H__
#include "sys.h"
#include "timer2.h"
#include <STRING.H>
#include <STDIO.H>

#define IR_pin P16

typedef enum{
    START_CODE = 1,
    REPEAT_CODE,
}LeaderCode_TypeDef;

typedef struct{
    LeaderCode_TypeDef leaderCode;
    uint8_t address;
    uint8_t command;
}IRBuffer_TypeDef;

int8_t isIRFalling();
void IR_RecvScan();
void IR_RecvHandler();
// void IR_DataHandler();
void IR_Display();

#endif // !__IR_H