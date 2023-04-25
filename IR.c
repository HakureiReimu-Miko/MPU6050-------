#include "ir.h"
int8_t isIRFalling()
{
    static bit IR_pin_old = 1;
    int8_t ret;
    if (IR_pin_old && !IR_pin)
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    IR_pin_old = IR_pin;
    return ret;
}

void IR_RecvScan()
{
    if (isIRFalling())
    {
        IR_RecvHandler();
    }
}

// #define IDLE_STATE 0
#define LEADER_CODE_STATE 0
#define DATA_RECV_STATE 1

#define IR_START_CODE_TIME 135
#define IR_REPEAT_CODE_TIME 113
#define IR_1_CODE_TIME 23
#define IR_0_CODE_TIME 11
#define IR_ERROR_RANGE 4

static IRBuffer_TypeDef IRBuffer;
static uint8_t IR_DataFlag;
void IR_RecvHandler()
{
    uint16_t IR_Time;
    static int8_t IR_State = 0;
    uint8_t data_recv[4];
    static uint8_t pDataBit;
    IR_Time = getTimer2Time();
    clearTimer2();
    if (IR_Time > 30)
    {
        IR_State = LEADER_CODE_STATE;
    }


    if (IR_State == LEADER_CODE_STATE)
    {
        if ((IR_START_CODE_TIME - IR_ERROR_RANGE) < IR_Time && IR_Time < (IR_START_CODE_TIME + IR_ERROR_RANGE))
        {
            IRBuffer.leaderCode = START_CODE;
            pDataBit = 0;
            IR_State = DATA_RECV_STATE;
        }
        else if ((IR_REPEAT_CODE_TIME - IR_ERROR_RANGE) < IR_Time && IR_Time < (IR_REPEAT_CODE_TIME + IR_ERROR_RANGE))
        {
            IRBuffer.leaderCode = REPEAT_CODE;
            IR_DataFlag = 2; //重发
            IR_State = LEADER_CODE_STATE;
        }
        else
        {
            //接受出错
            IR_State = LEADER_CODE_STATE;
        }
    }
    else if (IR_State == DATA_RECV_STATE)
    {
        if ((IR_1_CODE_TIME - IR_ERROR_RANGE) < IR_Time && IR_Time < (IR_1_CODE_TIME + IR_ERROR_RANGE))
        {
            data_recv[pDataBit / 8] |= 0x01 << pDataBit % 8;
        }
        else if ((IR_0_CODE_TIME - IR_ERROR_RANGE) < IR_Time && IR_Time < (IR_0_CODE_TIME + IR_ERROR_RANGE))
        {
            data_recv[pDataBit / 8] &= ~(0x01 << pDataBit % 8);
        }
        else
        {
            //接受错误
            IR_State = LEADER_CODE_STATE;
        }
        pDataBit++;
        if (pDataBit >= 32)
        {
            if (data_recv[0] == ~data_recv[1] && data_recv[2] == ~data_recv[3])
            {
                IRBuffer.address = data_recv[0];
                IRBuffer.command = data_recv[2];
                IR_State = LEADER_CODE_STATE;
                IR_DataFlag = 1;
            }
            pDataBit = 0;
        }
    }
}

// void IR_DataHandler()
// {
//     if (IR_DataFlag)
//     {
//         IR_DataFlag = 0;
//         IR_Display();
//     }
// }

#define LOG_ROW 9
#define LOG_COL 30
void IR_Display()
{
    static char IR_Log[LOG_ROW][LOG_COL] = {0};
    static uint8_t logLEN[LOG_ROW] = {0};
    static uint16_t logNum = 0;

    char displayBuff[LOG_ROW * LOG_COL] = {0};

    uint16_t i;
    u8 buzzer[4] = {0x00,0x08,0x40,0x00};
    
    if (IR_DataFlag == 1)
    {
        
        write_dgus_vp(0xA0,buzzer,2);
        IR_DataFlag = 0;
        if (logNum < LOG_ROW)
        {
            logLEN[logNum] = sprintf(IR_Log[logNum], "%d==> addr:%bd, cmd:%bd\r\n", logNum, IRBuffer.address, IRBuffer.command);
            logNum++;
        }
        else if (logNum >= LOG_ROW)
        {
            memmove(IR_Log[0], IR_Log[1], (LOG_ROW - 1) * LOG_COL);
            logLEN[LOG_ROW - 1] = sprintf(IR_Log[LOG_ROW - 1], "%d==> addr:%bd, cmd:%bd\r\n", logNum, IRBuffer.address, IRBuffer.command);
            logNum++;
        }
    }
    else if (IR_DataFlag == 2)
    {
        IR_DataFlag = 0;
        if (logNum < LOG_ROW)
        {
            logLEN[logNum] = sprintf(IR_Log[logNum], "%d==> 重复\r\n", logNum, IRBuffer.address, IRBuffer.command);
            logNum++;
        }
        else if (logNum >= LOG_ROW)
        {
            memmove(IR_Log[0], IR_Log[1], (LOG_ROW - 1) * LOG_COL);
            logLEN[LOG_ROW - 1] = sprintf(IR_Log[LOG_ROW - 1], "%d==> 重复\r\n", logNum, IRBuffer.address, IRBuffer.command);
            logNum++;
        }
    }
    for (i = 0; i < LOG_ROW; i++)
    {
        strcat(displayBuff, IR_Log[i]);
    }
    strcat(displayBuff, "\xFF\xFF");
    write_dgus_vp(0xE000, displayBuff, strlen(displayBuff) / 2 + strlen(displayBuff) % 2);
}

void timer2Handler() interrupt 5
{
    extern uint16_t timer2_time;
    timer2_time++;
    IR_RecvScan();
    TF2 = 0;
}