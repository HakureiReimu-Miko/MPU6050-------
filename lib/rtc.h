#ifndef __RTC_H__
#define __RTC_H__
#include "t5los8051.h"
#include "sys.h"

#define RTCUSE_RX8130	0	//0 rtc使用SD2058     1 rtc使用RX8130

extern unsigned char RTCdata[8];//时间设为全局变量  0年1月2日 3星期  4时5分6秒

void init_rtc();//检查有没有掉电，掉电则初始化RTC
void rdtime();//把RTC读取并处理，写到DGUS对应的变量空间，主程序中每0.5秒调用一次 
void Write_RTC(u8* date);//
unsigned char rtc_get_week(unsigned char year,unsigned char month,unsigned char day);

#endif