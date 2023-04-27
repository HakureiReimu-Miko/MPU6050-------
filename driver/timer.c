/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : timer.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 实现了定时器    硬件定时器0    实现了8个软定时器
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#include "timer.h"

//注意：禁止私自访问这些变量，只能用接口函数访问
//u8  EnableTimer;   //8个软定时器是否使能
//u8  OutTimeFlag;   //8个软定时器是否超时
//u32 TimerTime[8];  //8个软定时器定时时间

//注意：禁止私自访问这些变量，只能用接口函数访问
u8 data EnableTimer;   //8个软定时器是否使能
u8 data OutTimeFlag;   //8个软定时器是否超时
u16 data TimerTime[8];  //8个软定时器定时时间
u8 data SysTick;

/*****************************************************************************
 函 数 名  : void T0_Init(void)
 功能描述  : 定时器0初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   :  陈美书
    修改内容   : 创建
*****************************************************************************/
//定时器时钟为系统时钟的12分频，最大能设定到3.8ms
//计算公式为(65536-n*FOSC/12),其中n的单位为秒
void T0_Init(void)
{
	  TMOD=0x11;          //16位定时器
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;
	
	  OutTimeFlag=0;
	  EnableTimer=0;
	
    TMOD|=0x01;
    TH0=T1MS>>8;        //1ms定时器
    TL0=T1MS;
    ET0=1;              //开启定时器0中断
//    EA=1;               //开总中断
    TR0=1;              //开启定时器
}
/*****************************************************************************
 函 数 名  : void T0_ISR_PC(void)    interrupt 1
 功能描述  : 定时器0处理函数，毫秒增加
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
void T0_ISR_PC(void)    interrupt 1
{
	 u8 data i;
	
    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
		SysTick++;
		for(i=0;i<8;i++)
		{
			if(EnableTimer&(0x01<<i))
			{
				TimerTime[i]--;
				if(TimerTime[i]==0)
				{
					OutTimeFlag |= 0x01<<i;
					EnableTimer &= ~(0x01<<i); 					 
				}
			}
		}
    EA=1;
}
//定时器时钟为系统时钟的12分频，最大能设定到3.8ms
//计算公式为(65536-n*FOSC/12),其中n的单位为秒
void T1_Init(void)
{
	  TMOD=0x11;          //16位定时器
    //T1
    TH1=0x00;
    TL1=0x00;
    TR1=0x00;
	
    TMOD |= 0x10;
    TH1=T1MS>>8;        //1ms定时器
    TL1=T1MS;
    ET1=1;              //开启定时器0中断
    TR1=1;              //开启定时器
}

void timer1_Isr() interrupt 3
{
	EA = 0;
	TH1=T1MS>>8;
  TL1=T1MS;
	P10 = !P10;
	EA = 1;
}

//定时器时钟为系统时钟的24分频，最大能设定到7.6ms
//计算公式为(65536-n*FOSC/24),其中n的单位为秒
void T2_Init(void)
{
	T2CON = 0xF0;		//选择24分频
	
	TH2 = 0x58;
	TL2 = 0x00;
	TRL2H = 0x58;
	TRL2L = 0x00;		//定时5ms
	T2CON |= 0x01;	//启动定时器
	ET2 = 1;			  //打开定时器2中断		
}

void timer2_Isr() interrupt 5
{
	EA = 0;
	TF2 = 0;
	P11 = !P11;
	EA = 1;
}


/*****************************************************************************
 函 数 名  : void StartTimer(u8 ID, u16 nTime)
 功能描述  : 启动一个软定时器，
 输入参数  :	 u8 ID         定时器ID
               u16 nTime     定时ms数

 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
void StartTimer(u8 ID, u16 nTime)
{
	  EA=0;
	  EnableTimer=EnableTimer|(1<<ID);
	  TimerTime[ID]=nTime;
	  OutTimeFlag&=~(1<<ID);
	  EA=1; 
}


/*****************************************************************************
 函 数 名  : void KillTimer(u8 ID)
 功能描述  : 停止一个软定时器，
 输入参数  :	u8 ID  定时器ID
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
void KillTimer(u8 ID)
{
	  EA=0;
	  EnableTimer&=~(1<<(ID));
	  OutTimeFlag&=~(1<<ID);
	  EA=1;
}

/*****************************************************************************
 函 数 名  : u8 GetTimeOutFlag(u8 ID)
 功能描述  : 获得定时器是否超时
 输入参数  :	u8 ID  定时器ID
 输出参数  :  0  未超时    非0  超时
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
u8 GetTimeOutFlag(u8 ID)
{
  u8 flag;
	EA=0;
	flag=OutTimeFlag&(1<<ID);
	EA=1;
	return flag;
	
}

#ifdef PWMENABLE
u8  EnablePWM;
u16 PWMTotal[8];
u16 PWMLow[8];
u16 PWMTicks[8];
u8  PWMPort[8];
u8  PWMPin[8];

void StartPWM(u8 PWMID, u16 VPWMTotal,u16 VPWMLow,u8 Port,u8 Pin )
{
	  EA=0;
	  SetPinOut(Port,Pin);
	  PWMTicks[PWMID]=0;
	  PWMTotal[PWMID]=VPWMTotal;
    PWMLow[PWMID]=VPWMLow;
    PWMPort[PWMID]=Port;
    PWMPin[PWMID]=Pin;	
	  EnablePWM|=(1<<PWMID);
	  EA=1;
}

void StopPWM(u8 PWMID)
{
	  EA=0;
	  EnablePWM&=~(1<<PWMID);	
	  PinOutput(PWMPort[PWMID],PWMPin[PWMID],0);
	  EA=1;
}

void StopAllPWM()
{
	 u8 i;
	 EnablePWM=0;
	 for(i=0;i<8;i++)
	{
		PinOutput(PWMPort[i],PWMPin[i],0);
	}
}
#endif