
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : V1.0
  作    者   : chen meishu
  生成日期   : 2019年8月22日
  功能描述   : 主函数，外设和参数初始化，主循环中主要功能函数入口。
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__


#include "sys.h"

extern u8 data SysTick;

void T0_Init(void);
void T1_Init(void);
void T2_Init(void);
void StartTimer(u8 ID,u16 nTime);
void KillTimer(u8 ID);
u8 GetTimeOutFlag(u8 ID);
void StopTimer(u8 ID);
void ResumeTimer(u8 ID);

void StartPWM(u8 PWMID, u16 VPWMTotal,u16 VPWMLow,u8 Port,u8 Pin );

void StopAllPWM();
void StopPWM(u8 PWMID);
#endif