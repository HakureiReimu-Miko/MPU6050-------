#include "rtc.h"
#include "sys.h"
#include "umath.h"
#include "iic.h"

//兼容两套RTC程序,使用RTCUSE_RX8130(0: SD2058  1: RX8130),来切换RTC

#if	RTCUSE_RX8130

//**********************RX8130接口程序，SDA 10K上拉到3.3V**************
//上电时运行一次initrtc()，然后0.5秒间隔运行一次rdtime()读取时间到DGUS相应系统接口
unsigned char RTCdata[8];
IIC_CFG RX8130;
void init_rtc()
{	
	unsigned char i;

	RX8130.SCLPort = PORTP32;
    RX8130.SDAPort = PORTP33;
	Init_IIc_Interface(&RX8130);
	//检查有没有掉电
	i2cstart(&RX8130);
	i2cbw(0x64,&RX8130);
	i2cbw(0x1d,&RX8130);
	i2cstop(&RX8130);
	i2cstart(&RX8130);
	i2cbw(0x65,&RX8130);
	i=i2cbr(&RX8130);
	mack(&RX8130);
	i2cbr(&RX8130);
	mnak(&RX8130);
	i2cstop(&RX8130);
	if((i&0x02)==0x02)
	{	//重新配置时间
		i2cstart(&RX8130);		//30=00
		i2cbw(0x64,&RX8130);
		i2cbw(0x30,&RX8130);
		i2cbw(0x00,&RX8130);
		i2cstop(&RX8130);
		i2cstart(&RX8130);		//1C-1F=48 00 40 10
		i2cbw(0x64,&RX8130);
		i2cbw(0x1C,&RX8130);
		i2cbw(0x48,&RX8130);
		i2cbw(0x00,&RX8130);
		i2cbw(0x40,&RX8130);
		i2cbw(0x10,&RX8130);
		i2cstop(&RX8130);
		i2cstart(&RX8130);		//10-16=RTC设置值 BCD格式
		i2cbw(0x64,&RX8130);
		i2cbw(0x10,&RX8130);
		i2cbw(0x00,&RX8130);	//秒
		i2cbw(0x00,&RX8130);	//分
		i2cbw(0x00,&RX8130);	//时
		i2cbw(0x20,&RX8130);	//星期5
		i2cbw(0x01,&RX8130);	//1日
		i2cbw(0x01,&RX8130);	//1月
		i2cbw(0x21,&RX8130);	//21年
		i2cstop(&RX8130);
		i2cstart(&RX8130);		//1E-1F 00 10
		i2cbw(0x64,&RX8130);
		i2cbw(0x1E,&RX8130);
		i2cbw(0x00,&RX8130);	
		i2cbw(0x10,&RX8130);			
		i2cstop(&RX8130);
	}
}

//把RTC读取并处理，写到DGUS对应的变量空间，主程序中每0.5秒调用一次
void rdtime()
{	
	unsigned char i,n,m;
	i2cstart(&RX8130);
	i2cbw(0x64,&RX8130);
	i2cbw(0x10,&RX8130);
	i2cstop(&RX8130);
	i2cstart(&RX8130);
	i2cbw(0x65,&RX8130);
	for(i=6;i>0;i--)
	{	
		RTCdata[i]=i2cbr(&RX8130);
		mack(&RX8130);
	}
	RTCdata[0]=i2cbr(&RX8130);
	mnak(&RX8130);
	i2cstop(&RX8130);
	for(i=0;i<3;i++)	//年月日转换成HEX
	{	
		n=RTCdata[i]/16;
		m=RTCdata[i]%16;
		RTCdata[i]=n*10+m;
	}
	for(i=4;i<7;i++)	//时分秒转换成HEX
	{	
		n=RTCdata[i]/16;
		m=RTCdata[i]%16;
		RTCdata[i]=n*10+m;
	}
	//处理星期的数据格式
	n=0;
	m=RTCdata[3];			//bit     7654 3210
	for(i=0;i<7;i++)		//星期日  0000 0001
	{   					//星期一  0000 0010
		if(m&0x01)  break;	//星期二  0000 0100
							//星期三  0000 1000
		n++;				//星期四  0001 0000
		m=(m>>1);
	}
	RTCdata[3]=n;		//星期是  0-6   对应   星期日、星期一...星期六
	RTCdata[7]=0x00;
	write_dgus_vp(0x0010,RTCdata,0x04);	//写入DGUS变量空间
	
}

//计算星期,星期日-星期六  对应 0-6
unsigned char rtc_get_week(unsigned char year,unsigned char month,unsigned char day)
{	
	unsigned int tmp,mon,y;
	unsigned char week;
	if((month == 1) || (month == 2))
	{
		mon = month + 12;
		y = year - 1;
	}
	else 
	{
		mon = month;
		y = year;
	}
	tmp = y + (y / 4) +(((mon + 1) * 26) / 10) + day - 36;
	week = tmp % 7;
	return week;
}

void rtc_config(u8* prtc_set)
{
	i2cstart(&RX8130); //30=00 
	i2cbw(0x64,&RX8130); 
	i2cbw(0x30,&RX8130); 
	i2cbw(0x00,&RX8130); 
	i2cstop(&RX8130); 
	i2cstart(&RX8130); //1C-1F=48 00 40 10 
	i2cbw(0x64,&RX8130); 
	i2cbw(0x1C,&RX8130); 
	i2cbw(0x48,&RX8130); 
	i2cbw(0x00,&RX8130); 
	i2cbw(0x40,&RX8130); 
	i2cbw(0x10,&RX8130); 
	i2cstop(&RX8130); 
	i2cstart(&RX8130); //10-16=RTC设置值 BCD格式 
	i2cbw(0x64,&RX8130); 
	i2cbw(0x10,&RX8130); 
	
	i2cbw(prtc_set[0],&RX8130); //秒      
	i2cbw(prtc_set[1],&RX8130); //分 
	i2cbw(prtc_set[2],&RX8130); //时     
	i2cbw(prtc_set[3],&RX8130); //星期 
	i2cbw(prtc_set[4],&RX8130); //日 
	i2cbw(prtc_set[5],&RX8130); //月 
	i2cbw(prtc_set[6],&RX8130); //年 

	i2cstop(&RX8130); 
	i2cstart(&RX8130); //1E-1F 00 10 
	i2cbw(0x64,&RX8130); 
	i2cbw(0x1E,&RX8130); 
	i2cbw(0x00,&RX8130); 
	i2cbw(0x10,&RX8130); 
	i2cstop(&RX8130); 
}

void Write_RTC(u8* date) 
{ 
	unsigned char rtcdata[8];
	unsigned char week;

	week=rtc_get_week(date[0],date[1],date[2]);

	rtcdata[6]=BCD(date[0]);
	rtcdata[5]=BCD(date[1]);
	rtcdata[4]=BCD(date[2]);
	rtcdata[3]=(u8)(1<<week);//星期,bit0置1,代表星期日,bit1-bit6分别代表星期一到星期六
	rtcdata[2]=BCD(date[3]);
	rtcdata[1]=BCD(date[4]);
	rtcdata[0]=BCD(date[5]);

	rtc_config(rtcdata);
} 


#else
//******************************SD2058接口程序******************************
//上电时运行一次initrtc()，然后0.5秒间隔运行一次rdtime()读取时间到DGUS相应系统接口
IIC_CFG SD2058;

u8 RTCdata[8];

//检查RTC有没有掉电，掉电初始化2021.01.01 星期五 00:00:00
void init_rtc(void)
{	
	u8 dat1,dat2;

    SD2058.SCLPort = PORTP32;
    SD2058.SDAPort = PORTP33;
    Init_IIc_Interface(&SD2058);
	i2cstart(&SD2058);
	i2cbw(0x64,&SD2058);
	i2cbw(0x0f,&SD2058);		//0x0f最低位
	i2cstart(&SD2058);
	i2cbw(0x65,&SD2058);
	dat2 = i2cbr(&SD2058);
	mack(&SD2058);
	dat1 = i2cbr(&SD2058);
	mnak(&SD2058);
	i2cstop(&SD2058);
	if(dat2&0x01)
	{
		if(dat2&0x84)		//WRTC2 WRTC3是否为0，不为0，写0
		{
			dat2 &= ~0x84;
			i2cstart(&SD2058);		
			i2cbw(0x64,&SD2058);
			i2cbw(0x0,&SD2058);
			i2cbw(dat2,&SD2058);
			i2cstop(&SD2058);
		}
		dat2 &= ~0x01;
		//WRTC1是否为0
		if(dat1 & 0x80)		//WRTC1是否为1
		{
			dat1 &= ~0x80;
			i2cstart(&SD2058);		
			i2cbw(0x64,&SD2058);
			i2cbw(0x10,&SD2058);
			i2cbw(dat1,&SD2058);
			i2cstop(&SD2058);
		}
		
		//写使能
		dat1 |= 0x80;
		i2cstart(&SD2058);		
		i2cbw(0x64,&SD2058);
		i2cbw(0x10,&SD2058);
		i2cbw(dat1,&SD2058);
		i2cstop(&SD2058);
		dat2 |= 0x84;
		i2cstart(&SD2058);		
		i2cbw(0x64,&SD2058);
		i2cbw(0x0f,&SD2058);
		i2cbw(dat2,&SD2058);
		i2cstop(&SD2058);
		
		//重新配置时间
		i2cstart(&SD2058);		//10-16=RTC时间，BCD格式
		i2cbw(0x64,&SD2058);
		i2cbw(0x00,&SD2058);
		i2cbw(0x00,&SD2058);	//秒
		i2cbw(0x00,&SD2058);	//分
		i2cbw(0x80,&SD2058);	//时，最高位是配置12/24格式的
		i2cbw(0x05,&SD2058);	//星期
		i2cbw(0x01,&SD2058);	//日
		i2cbw(0x01,&SD2058);	//月
		i2cbw(0x21,&SD2058);	//年
		i2cstop(&SD2058);
		dat2 &= ~0x84;
		dat1 &= ~0x80;
		i2cstart(&SD2058);		
		i2cbw(0x64,&SD2058);
		i2cbw(0x10,&SD2058);
		i2cbw(dat1,&SD2058);
		i2cstop(&SD2058);
		i2cstart(&SD2058);		
		i2cbw(0x64,&SD2058);
		i2cbw(0x0f,&SD2058);
		i2cbw(dat2,&SD2058);
		i2cstop(&SD2058);
	}
}

void rtc_config(u8* prtc_set)
{
	u8 dat,dat1;

	i2cstart(&SD2058);
	i2cbw(0x64,&SD2058);
	i2cbw(0x0f,&SD2058);		//0x10
	i2cstart(&SD2058);
	i2cbw(0x65,&SD2058);
	dat = i2cbr(&SD2058);
	mack(&SD2058);
	dat1 = i2cbr(&SD2058);
	mnak(&SD2058);
	i2cstop(&SD2058);
	dat1 |= 0x80;
	i2cstart(&SD2058);		
	i2cbw(0x64,&SD2058);
	i2cbw(0x10,&SD2058);
	i2cbw(dat1,&SD2058);
	i2cstop(&SD2058);
	dat |= 0x84;
	i2cstart(&SD2058);		
	i2cbw(0x64,&SD2058);
	i2cbw(0x0f,&SD2058);
	i2cbw(dat,&SD2058);
	i2cstop(&SD2058);
	

	i2cstart(&SD2058);		//10-16=RTC时间，BCD格式
	i2cbw(0x64,&SD2058);
	i2cbw(0x00,&SD2058);
	i2cbw(prtc_set[6],&SD2058);	//秒
	i2cbw(prtc_set[5],&SD2058);	//分
	i2cbw(prtc_set[4],&SD2058);	//时
	i2cbw(prtc_set[3],&SD2058);	//星期
	i2cbw(prtc_set[2],&SD2058);	//日
	i2cbw(prtc_set[1],&SD2058);	//月
	i2cbw(prtc_set[0],&SD2058);	//年
	i2cstop(&SD2058);
	dat &= ~0x84;
	dat1 &= ~0x80;
	i2cstart(&SD2058);		
	i2cbw(0x64,&SD2058);
	i2cbw(0x10,&SD2058);
	i2cbw(dat1,&SD2058);
	i2cstop(&SD2058);
	i2cstart(&SD2058);		
	i2cbw(0x64,&SD2058);
	i2cbw(0x0f,&SD2058);
	i2cbw(dat,&SD2058);
	i2cstop(&SD2058);
}

//读取rtc时间
void rdtime(void)
{	
	unsigned char rtcdata[8];
	unsigned char i,n,m;
	i2cstart(&SD2058);
	i2cbw(0x64,&SD2058);
	i2cbw(0x00,&SD2058);
	i2cstart(&SD2058);
	i2cbw(0x65,&SD2058);
	for(i=6;i>0;i--)
	{	
		rtcdata[i]=i2cbr(&SD2058);
		mack(&SD2058);
	}
	rtcdata[0]=i2cbr(&SD2058);
	mnak(&SD2058);
	i2cstop(&SD2058);
	rtcdata[4] &= 0x7F;

	

	for(i=0;i<3;i++)	//年月日转换成hex
	{	
		n=rtcdata[i]/16;
		m=rtcdata[i]%16;
		rtcdata[i]=n*10+m;
	}
	for(i=4;i<7;i++)	//时分秒转换成hex
	{	
		n=rtcdata[i]/16;
		m=rtcdata[i]%16;
		rtcdata[i]=n*10+m;
	}
	//星期不用处理
	rtcdata[7]=0;
	StrCopy(RTCdata,rtcdata,8);//将时间存储到全局变量中
	write_dgus_vp(0x0010,(u8*)rtcdata,4);	//写入系统接口
	
	
}

u8 rtc_get_week(u8 year,u8 month,u8 day)
{	
	u16 tmp,mon,y;
	u8 week;
	if((month == 1) || (month == 2))
	{
		mon = month + 12;
		y = year - 1;
	}
	else 
	{
		mon = month;
		y = year;
	}
	tmp = y + (y / 4) +(((mon + 1) * 26) / 10) + day - 36;
	week = tmp % 7;
	return week;
}

void Write_RTC(u8* date)
{
	u8 rtc_set[8];
	rtc_set[0] = BCD(date[0]);
	rtc_set[1] = BCD(date[1]);
	rtc_set[2] = BCD(date[2]);
	rtc_set[3] = rtc_get_week(date[0],date[1],date[2]);
	rtc_set[4] = BCD(date[3]);
	rtc_set[4] |= 0x80;
	rtc_set[5] = BCD(date[4]);
	rtc_set[6] = BCD(date[5]);
	rtc_config(rtc_set);


}


#endif


