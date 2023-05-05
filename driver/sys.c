
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : sys.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#include "sys.h"
MNORFLASH Nor_Flash;
MSPIFLASH Spi_Flash;
MMUSIC MusicBuf;
s16 PosXChangeSpeed,PosYChangeSpeed,RealPosX,RealPosY;
s16 xdistance,ydistance,PressPosX,PressPosY,LastPosX,LastPosY;
u16 PressPageID;
u8 PressPosChange,Touchstate;

/*****************************************************************************
 函 数 名  : void read_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 读dgus地址的值
 输入参数  :	 addr：dgus地址值  len：读数据长度
 输出参数  : buf：数据保存缓存区
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
#define INTVPACTION
void read_dgus_vp(u32 addr, u8 *buf, u16 len)
{
	u16 OS_addr = 0;
	u16 OS_addr_offset = 0;
	u16 OS_len = 0, OS_len_offset = 0;
	u32 LenLimit;
	
	if(0==len)
		return;
	LenLimit = 0x1ffffU - addr + 1;
	if(LenLimit < len)
	{
		len = LenLimit;
	}
	OS_addr = addr >> 1;
	OS_addr_offset = addr & 0x01;
#ifdef INTVPACTION	
	EA = 0;
#endif
	ADR_H = 0;
	ADR_M = (u8)(OS_addr >> 8);
	ADR_L = (u8)OS_addr;
	ADR_INC = 1;	
	RAMMODE = 0xAF; 
	while (!APP_ACK);			
	if (OS_addr_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA1;
		*buf++ = DATA0;
		len--;
	}
	OS_len = len >> 1;
	OS_len_offset = len & 0x01;
	while (OS_len--)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
		*buf++ = DATA1;
		*buf++ = DATA0;
	}
	if (OS_len_offset)
	{
		APP_EN = 1;
		while (APP_EN);
		*buf++ = DATA3;
		*buf++ = DATA2;
	}
	RAMMODE = 0x00;
#ifdef INTVPACTION	
	EA = 1;
#endif
}

/*void read_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 xdata OS_addr;
	  u32 xdata OS_addr_offset;
    u16 xdata OS_len;
	  u16 xdata OS_len_offset;
	
		if(addr >= 0x10000)
			return;
		EA = 0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2;
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=1;                 //DGUS  OS存储器地址在读写后自动加1
    RAMMODE=0xAF;               //启动读模式
    if(OS_addr_offset==1)       //首地址有偏移，修正
    {
        while(APP_ACK==0);      //等待
        APP_EN=1;
        while(APP_EN==1); 
        *buf++=DATA1;
        *buf++=DATA0;              
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2;
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {          
            while(APP_ACK==0);
            APP_EN=1;
            while(APP_EN==1);       //读写执行结束
            *buf++=DATA3;
            *buf++=DATA2;           
            break;    
        } 
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束 
        *buf++=DATA3;
        *buf++=DATA2;
        *buf++=DATA1;
        *buf++=DATA0;                          
    }   
    RAMMODE=0x00;           //读写完成后RAMMODE必须置零
		EA = 1;
		delay_us(100);
}*/


/*****************************************************************************
 函 数 名  : void write_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 写dgus地址数据
输入参数  :	 addr：写地址值	buf：写入的数据保存缓存区 len：字长度
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void write_dgus_vp(u32 addr, u8 *buf, u16 len)
{
	u16 OS_addr = 0;
	u16 OS_addr_offset = 0;
	u16 OS_len = 0,OS_len_offset = 0;
	u32 LenLimit;
	
	if(0==len)
		return;
	LenLimit = 0x1ffffUL - addr + 1;
	if(LenLimit < len)
	{
		len = LenLimit;
	}
	OS_addr = addr >> 1;
	OS_addr_offset = addr & 0x01;
#ifdef INTVPACTION	
	EA = 0;
#endif
	ADR_H = 0;
	ADR_M = (u8)(OS_addr >> 8);
	ADR_L = (u8)OS_addr;
	ADR_INC = 0x01; 
	RAMMODE = 0x83;
	while (!APP_ACK);
	if (OS_addr_offset)
	{
		DATA1 = *buf++;
		DATA0 = *buf++;
		APP_EN = 1;
		while (APP_EN);
		len--;
	}
	OS_len = len >> 1;
	OS_len_offset = len & 0x01;
	RAMMODE = 0x8F;
	while (OS_len--)
	{
		DATA3 = *buf++;
		DATA2 = *buf++;
		DATA1 = *buf++;
		DATA0 = *buf++;
		APP_EN = 1;
		while (APP_EN);
	}
	if (OS_len_offset)
	{
		RAMMODE = 0x8C;
		DATA3 = *buf++;
		DATA2 = *buf++;
		APP_EN = 1;
		while (APP_EN);
	}
	RAMMODE = 0x00;
#ifdef INTVPACTION	
	EA = 1;
#endif
}

/*void write_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 xdata OS_addr;
	  u32 xdata OS_addr_offset;
    u16 xdata OS_len;
	  u16 xdata OS_len_offset;
	
		if(addr >= 0x10000)
			return;
    EA=0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2; 
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=0x01;                 //DGUS  OS存储器地址在读写后自动加1
    RAMMODE=0x8F;               //启动写模式 
    if(OS_addr_offset==1)
    {
        ADR_INC=0;
        RAMMODE=0xAF;
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
        ADR_INC=0x01; 
        RAMMODE=0x8F;
        while(APP_ACK==0);      
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2; 
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {
            ADR_INC=0;
            RAMMODE=0xAF;
            while(APP_ACK==0);
            APP_EN=1;                //增加一个读过程，以免写单字时会将另一个单字写0
            while(APP_EN==1);       //读写执行结束
            ADR_INC=0x01;
            RAMMODE=0x8F;
            while(APP_ACK==0);           
            DATA3=*buf++;
            DATA2=*buf++;
            APP_EN=1;
            while(APP_EN==1);       //读写执行结束
            break;
        }
        while(APP_ACK==0);        
        DATA3=*buf++;
        DATA2=*buf++;
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
    } 
    RAMMODE=0x00;       //读写完成后RAMMODE必须置零
    EA=1; 
		delay_us(100);		
}  */


/*****************************************************************************
 函 数 名  : void INIT_CPU(void)
 功能描述  : CPU初始化函数
			根据实际使用外设修改或单独配置
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月1日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
//void INIT_CPU(void)
//{
//    EA=0;
//    RS0=0;
//    RS1=0;

//    CKCON=0x00;
//    T2CON=0x70;
//    DPC=0x00;
//    PAGESEL=0x01;
//    D_PAGESEL=0x02;   //DATA RAM  0x8000-0xFFFF
//    MUX_SEL=0x60;   //UART2,UART2开启，WDT关闭
//    RAMMODE=0x00;
//    PORTDRV=0x01;   //驱动强度+/-8mA
//    IEN0=0x00;      //关闭所有中断
//    IEN1=0x00;
//    IEN2=0x00;
//    IP0=0x00;      //中断优先级默认
//    IP1=0x00;
///*****************
//    WDT_OFF();      //关闭开门狗
//************************************/
//	 ClearRAM();									//初始化RAM为0
//}

//void ClearRAM(void)
//{
//	u8 xdata*ptr;
//	u16 data i;
//	
//	ptr = 0x8000;
//	
//	for(i=0;i<0x8000;i++)
//		*ptr++ = 0;
//}


void delay_us(unsigned int t)
{ 
 u8 i;

 while(t)
 {
  for(i=0;i<50;i++)
   {i=i;}
  t--;
 }
}

void Page_Change(u16 PageID)
{
	u8 buf[4];
  
	buf[0] = 0x5a;
	buf[1] = 0x01;
	buf[2] = (u8)(PageID >> 8);
	buf[3] = (u8)PageID;
	write_dgus_vp(0x84,buf,2);
	do
	{
		delay_us(500);
		read_dgus_vp(0x14,buf,2);
	}while(*(u16*)buf!=PageID);
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	write_dgus_vp(0x16,buf,2);
}

void delay_ms(u16 t)
{
	u16 i,j;
	  for(i=0;i<t;i++)
	  {
			for(j=0;j<300;j++)
	     delay_us(1);
		}
}

void SetPinOut(u8 Port,u8 Pin)
{
switch(Port)
{
	case 0: P0MDOUT|=(1<<Pin);
			break;
	case 1: P1MDOUT|=(1<<Pin);
			break;
	case 2: P2MDOUT|=(1<<Pin);
			break;
	case 3: P3MDOUT|=(1<<Pin);
			break;	
	default:break;				
}

}

void SetPinIn(u8 Port,u8 Pin)
{
	switch(Port)
	{
		case 0: P0MDOUT&=~(1<<Pin);
				break;
		case 1: P1MDOUT&=~(1<<Pin);
				break;
		case 2: P2MDOUT&=~(1<<Pin);
				break;
		case 3: P3MDOUT&=~(1<<Pin);
				break;	
		default:break;				
	}
		
}

void PinOutput(u8 Port,u8 Pin,u8 value)
{
	switch(Port)
	{
		case 0: if(value) P0|=(1<<Pin);
				else      P0&=~(1<<Pin);
				break;
		case 1: if(value) P1|=(1<<Pin);
				else      P1&=~(1<<Pin);
				break;
		case 2: if(value) P2|=(1<<Pin);
				else      P2&=~(1<<Pin);
				break;
		case 3: if(value) P3|=(1<<Pin);
				else      P3&=~(1<<Pin);
				break;	
		default:break;				
	}
}

u8 GetPinIn(u8 Port,u8 Pin)
{	 
	u8 value;
	switch(Port)
	{
		case 0: value=P0&(1<<Pin);
				  break;
		case 1: value=P1&(1<<Pin);
				  break;
		case 2: value=P2&(1<<Pin);
				  break;
		case 3: value=P3&(1<<Pin);
				  break;	
		default:
				value=0;
				break;				
	}
	return value;
}


//不使用的函数
#if UnusedFunction

void TouchSwitch(u16 PageID, u8 TouchType, u8 TouchID, u8 Status)
{
	u8 k_data[8];
	
	*(u16*)k_data = 0x5aa5;
	*(u16*)&k_data[2] = PageID;
	k_data[4] = TouchID;
	k_data[5] = TouchType;
	if(Status)
		*(u16*)&k_data[6] = 1;
	else
		*(u16*)&k_data[6] = 0;
	write_dgus_vp(0xb0,k_data,4);
	do
	{
		delay_us(500);
		read_dgus_vp(0xb0,k_data,1);
	}while(k_data[0]!=0);
}

void NorFlash_Action(void)
{
	u8 buf[8];	
	
	buf[0] = Nor_Flash.Mode;
	buf[1] = (u8)(Nor_Flash.FLAddr>>16);
	buf[2] = (u8)(Nor_Flash.FLAddr>>8);
	buf[3] = (u8)Nor_Flash.FLAddr;
	buf[4] = (u8)(Nor_Flash.VPAddr>>8);
	buf[5] = (u8)Nor_Flash.VPAddr;
	buf[6] = (u8)(Nor_Flash.Len>>8);
	buf[7] = (u8)Nor_Flash.Len;
	if(Nor_Flash.Mode == 0xa5)//写数据
	{
		if(Nor_Flash.Buf != NULL)
			write_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
	}
	write_dgus_vp(0x08,buf,4);
	do
	{
		delay_us(500);
		read_dgus_vp(0x08,buf,1);
	}while(buf[0]!=0);
	if(Nor_Flash.Mode == 0x5a)//读数据
	{
		if(Nor_Flash.Buf != NULL)
			read_dgus_vp(Nor_Flash.VPAddr,Nor_Flash.Buf,Nor_Flash.Len);
	}
	delay_ms(FLASH_ACCESS_CYCLE);
}

void SPIFlash_Action(void)//对于写操作需要提前准备好32K的VP数据
{
	u8 buf[12];
	
	buf[0] = 0x5a;
	buf[1] = Spi_Flash.Mode;
	if(Spi_Flash.Mode == 1)
	{
		buf[2] = Spi_Flash.ID;
		buf[3] = (u8)(Spi_Flash.FLAddr>>16);
		buf[4] = (u8)(Spi_Flash.FLAddr>>8);
		buf[5] = (u8)Spi_Flash.FLAddr;
		buf[6] = (u8)(Spi_Flash.VPAddr>>8);
		buf[7] = (u8)Spi_Flash.VPAddr;
		buf[8] = (u8)(Spi_Flash.Len>>8);
		buf[9] = (u8)Spi_Flash.Len;
		buf[10] = 0;
		buf[11] = 0;
	}
	else if(Spi_Flash.Mode == 2)
	{
		buf[2] = (u8)(Spi_Flash.ID>>8);
		buf[3] = (u8)Spi_Flash.ID;
		buf[4] = (u8)(Spi_Flash.VPAddr>>8);
		buf[5] = (u8)Spi_Flash.VPAddr;
		buf[6] = (u8)(Spi_Flash.Delay>>8);
		buf[7] = (u8)Spi_Flash.Delay;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = 0;
		buf[11] = 0;
	}	
	write_dgus_vp(0xaa,buf,6);
	do
	{
		delay_us(500);
		read_dgus_vp(0xaa,buf,1);
	}while(buf[0]!=0);
	if(Spi_Flash.Mode == 1)//读数据
	{
		if(Spi_Flash.Buf != NULL)
			read_dgus_vp(Spi_Flash.VPAddr,Spi_Flash.Buf,Spi_Flash.Len);
	}
	delay_ms(FLASH_ACCESS_CYCLE);
}

//获得当前页面
u16 GetPageID()
{
	u16  nPage;

	read_dgus_vp(PIC_NOW,(u8*)(&nPage),1);	
	return nPage;
}

void ResetT5L(void)
{
	u32 tmp;
	
	tmp = 0x55aa5aa5;
	write_dgus_vp(0x04,(u8*)&tmp,2);
}

void BackLight_Control(u8 light)
{
	u16 DATA_RAM;
	u16 DATA_RAM1;
	read_dgus_vp(0x0082,(u8*)&DATA_RAM,1);
	DATA_RAM&=0x00FF;
	DATA_RAM1=light;
	DATA_RAM1<<=8;
	DATA_RAM|=DATA_RAM1;
	write_dgus_vp(0x0082,(u8*)&DATA_RAM,1);
}

//音乐播放
void MusicPlay(u8 MusicId)
{
	u8 k_data[4];
	
	read_dgus_vp(0xa0,k_data,2);
	k_data[0] = MusicId;
	k_data[1] = 0x01;
	k_data[3] = 0;
	write_dgus_vp(0xa0,k_data,2);
}

//控制按键声音和休眠  0关闭  1开启  2忽略
void system_config(u8 is_beep,u8 is_sleep)
{
	u8 config_cmd[4];
	read_dgus_vp(0x0080,config_cmd,2);
	if(is_beep == 1)
	{
		config_cmd[3] |= (1<<3);
	}
	else if(is_beep == 0)
	{
		config_cmd[3] &= ~(1<<3);
	}
	else 
	{
		;
	}
	
	if(is_sleep == 1)
	{
		config_cmd[3] |= (1<<2);
	}
	else if(is_sleep == 0)
	{
		config_cmd[3] &= ~(1<<2);
	}
	else 
	{
		;
	}
	config_cmd[0] = 0x5A;
	write_dgus_vp(0x0080,config_cmd,2);
}

/*
	操作系统变量0x0082，控制背光的亮度和待机
	work_light  	工作时亮度
	sleep_light		待机时亮度
	await_time		多久后待机
*/
void system_led_config(u8 sleep_light, u16 await_time)
{
	u8 led_config_cmd[4];
	read_dgus_vp(0x0082,(u8*)&led_config_cmd,2);
	//led_config_cmd[0] = work_light;
	led_config_cmd[1] = sleep_light;
	led_config_cmd[2] = (u8)(await_time>>8);
	led_config_cmd[3] = (u8)(await_time); 
	
	write_dgus_vp(0x0082,led_config_cmd,2);
	
}

//获取屏幕是否被触摸 0未触摸屏幕   1有触摸屏幕
u8 ScreenTouchOrNot(void)
{
	u8 k_data[2];
	
	read_dgus_vp(0x16,k_data,1);
	if(k_data[0]==0x5a)
	{
		k_data[0]=0;
		write_dgus_vp(0x16,k_data,1);
		return 1;
	}
	return 0;
}

u8 GetTouchStatus(void)
{
	u8 k_data[8];
	
	read_dgus_vp(0x16,k_data,4);
	if(k_data[0]==0x5a)
	{
		if(k_data[1]==0x01)//第一次按下
		{
			LastPosX = PressPosX = *(s16*)&k_data[2];
			LastPosY = PressPosY = *(s16*)&k_data[4];
			Touchstate = FIRST_PRESS;
		}
		else if(k_data[1]==0x03)//长按
		{
			LastPosY = RealPosY;
			LastPosX = RealPosX;
			Touchstate = UNDER_PRESS;
		}
		else//抬起
		{
			Touchstate = RELEASE_PRESS;
		}
		RealPosX = *(s16*)&k_data[2];
		RealPosY = *(s16*)&k_data[4];
		k_data[0] = 0;
		write_dgus_vp(0x16,k_data,2);
		return Touchstate;
	}
	else
	{
		if((k_data[1]==0x02)||(k_data[1]==0x00))
		{
			Touchstate = IDLE_PRESS;
		}
		return Touchstate;
	}
}

u8 GetTouchAnction(u16 PageID)
{
	u8 status;
	u32 x2distance,y2distance;
	
	status = GetTouchStatus();
	if(IDLE_PRESS==status)
	{
		PressPosChange = UNTOUCH;
		return PressPosChange;
	}
	else if(FIRST_PRESS==status)//首次按下，记录按下页面，防止切换到其他页面产生影响
	{
		read_dgus_vp(0x14,(u8*)&PressPageID,1);
		PressPosChange = UNSLIDE;
		return UNSLIDE;
	}
	else
	{
		if(PageID != PressPageID)
		{
			return UNSLIDE;
		}
		xdistance = (PressPosX - RealPosX);
		x2distance = xdistance * xdistance;
		ydistance = (PressPosY - RealPosY);
		y2distance = ydistance * ydistance;
		if(UNDER_PRESS==status)//长按中
		{
			if(UNSLIDE == PressPosChange)//未动作
			{
				if((y2distance >= SLIDE_THRESHOLD)||(x2distance >= SLIDE_THRESHOLD))
				{
					if(y2distance*ANGLE_THRESHOLD >= x2distance)//   Y/X > 0.5，即30度角作为横向滑动还是竖向滑动的分界线
					{
						PressPosChange = VERTICAL_SLIDE;
						return VERTICAL_SLIDE;//处理纵向滑动
					}
					else//左右滑动
					{
						PressPosChange = HORIZONTAL_SLIDE;
						return HORIZONTAL_SLIDE;
					}
				}
			}
			return PressPosChange;
		}
		else//抬起状态
		{
			if(UNSLIDE == PressPosChange)//未动作
			{
				if((y2distance >= SLIDE_THRESHOLD)||(x2distance >= SLIDE_THRESHOLD))//当快速滑动时，坐标来不及进入长按状态，抬起时同样判断一下坐标坐标改变距离
				{
					if(y2distance*ANGLE_THRESHOLD >= x2distance)//   Y/X > 0.5，即30度角作为横向滑动还是竖向滑动的分界线
					{
						PressPosChange = VERTICAL_SLIDE;//处理纵向滑动
					}
					else//左右滑动
					{
						PressPosChange = HORIZONTAL_SLIDE;
					}
				}
				else
				{
					return CLICKTOUCH;//处理点击事件
				}
			}
			if(VERTICAL_SLIDE==PressPosChange)
			{
				PosYChangeSpeed = LastPosY-RealPosY;//移动速度，纵向PosX_ChangeSpeed,PosY_ChangeSpeed，计算上一个坐标点和抬起坐标点的线段长度
				return AUTOVERTICAL_SLIDE;//根据移动速度处理惯性
			}
			else
			{
				PosXChangeSpeed = LastPosX-RealPosX;//移动速度,横向
				return AUTOHORIZONTAL_SLIDE;//根据移动速度处理惯性
			}
		}
	}
}

//设置弹窗提示语句
#define TIMERNUM	5	//定时器
#define AddrpopUpCode   0x6FE0
#define PopUpLen	64//字节
void setPopupMessage(u8 *sourceStr,u16 len)
{
	
	u8 tmp[PopUpLen];
	StrClear(tmp,sizeof(tmp));
	StrCopy(tmp,sourceStr,len);
	write_dgus_vp(AddrpopUpCode,(u8*)&tmp,sizeof(tmp)/2);
	StartTimer(TIMERNUM,2500);
}

//弹窗显示超时关闭
void PopupMessageTimeOutClose(void)
{
	//此函数要直接放在while(1){};中
	u8 tmp[PopUpLen];
	if(GetTimeOutFlag(TIMERNUM))//判断弹窗的超时,隐藏
	{
		StrClear(tmp,sizeof(tmp));
		write_dgus_vp(AddrpopUpCode,(u8*)&tmp,sizeof(tmp)/2);
		KillTimer(TIMERNUM);
	}
}


//启动PWM0的配置
//pwm_psc			分频系数
//pwm0_precision	PWM0精度 （上限）
//计算公式：  PWM0载波频率 = 825.7536MHz / (分频系数 * PWM0精度)
void PWM0_Set(u8 pwm_psc,u16 pwm0_precision)
{
	#define PWM0_SET_ADDR 0x86
	u8 buf[4];
	buf[0] = 0x5A;
	buf[1] = pwm_psc;
	buf[2] = (u8)(pwm0_precision>>8);
	buf[3] = (u8)pwm0_precision;
	
	write_dgus_vp(PWM0_SET_ADDR,buf,2);
}

//调节占空比
//duty_cycle		占空比		取值1-100
//pwm0_precision	精度（上限）
void PWM0_Out(u16 duty_cycle,u16 pwm0_precision)
{
	#define PWM0_OUT_ADDR 0x92
	u8 buf[2];
	u16 tmp;
	tmp = (pwm0_precision/100)*duty_cycle;		//这里是1%  2%  3% ... 100%
	buf[0] = (u8)(tmp>>8);
	buf[1] = (u8)tmp;
	
	write_dgus_vp(PWM0_OUT_ADDR,buf,1);
}

//绘制动态曲线
//chart_id:曲线通道id,范围[0,7],系统总共支持8个曲线通道,每个曲线通道占用2K的字空间,
//				 总缓冲区范围:0x1000-0x4FFF,没有使用的曲线缓冲区可以做其他用途
//axit_y:写入某一个数据点的数据值
void system_draw_dynamic_graph(u8 chart_id, u16 axit_y)
{
	#define CHART_ADDR	0x0310	//动态曲线功能所对应的系统变量接口地址
	#define	CHART_NUM	1		//一次性写入几个曲线通道的数据,我们只写入chart_id指定的曲线通道,即1个
	#define	POINT_NUM	1		//一次性写入多少个数据点
	
	u8 chart_cmd[6+POINT_NUM*2] = {0x5A,0xA5,CHART_NUM,0x00};
	
	chart_cmd[4] = chart_id;
	chart_cmd[5] = POINT_NUM;
	chart_cmd[6] = (u8)(axit_y>>8);
	chart_cmd[7] = (u8)axit_y;
	
	write_dgus_vp(CHART_ADDR,chart_cmd,3+POINT_NUM);
}

//修改增量调节的参数
void modifyInceaseControlPara(u8 page_id,u8 id,u16 minLimit,u16 step,u16 maxLimit)
{
	u8 tmp[20];
	
	tmp[0] = 0x5a;
	tmp[1] = 0xa5;
	tmp[2] = 0;
	tmp[3] = page_id;
	tmp[4] = id;
	tmp[5] = 2;
	tmp[6] = 0;
	tmp[7] = 2;//读取
	write_dgus_vp(0x00B0,tmp,4);	
	do
	{
		delay_ms(2);
		read_dgus_vp(0x00B0,tmp,1);
	}while(tmp[0]!=0);
	
	write_dgus_vp(0xb4+11,(u8*)&step,1);//步长
	write_dgus_vp(0xb4+12,(u8*)&minLimit,1);//下限
	write_dgus_vp(0xb4+13,(u8*)&maxLimit,1);//上限
	
	tmp[0] = 0x5a;
	tmp[1] = 0xa5;
	tmp[2] = 0;
	tmp[3] = page_id;
	tmp[4] = id;
	tmp[5] = 2;
	tmp[6] = 0;
	tmp[7] = 3;//写入
	write_dgus_vp(0x00B0,tmp,4);	
	do
	{
		delay_ms(2);
		read_dgus_vp(0x00B0,tmp,1);
	}while(tmp[0]!=0);
}


u16 readADC(u16 channel)
{
	read_dgus_vp(0x0032+channel,(u8*)&channel,1);
	return channel;
}

/*
函数:读取SPIFlash中的数据
参数:
	spiAddr		当前spiID中的读取起始地址,范围0x000000-0x01FFFF
	spiID		字库 ID， 0x10-0x1F，每个字库 256Kbytes，最大 4Mbytes。
	vpAddr		读取到的dgus地址,必须是偶数
	len			读取的字长度,按照字定义，必须是偶数
	buff		缓存的buff[]的地址,若不用缓存,则填NULL
*/
void readSPIFlashToDgus(u32 spiAddr,u16 spiID,u16 vpAddr,u16 len,u8 *buff)
{
	Spi_Flash.Mode=1;
	Spi_Flash.ID=spiID;
	Spi_Flash.FLAddr=spiAddr;
	Spi_Flash.VPAddr=vpAddr;
	Spi_Flash.Len=len;
	Spi_Flash.Buf=buff;
	SPIFlash_Action();
}

/*
函数:写入数据到SPIFlash中,一次按照32KB去写
参数:
	spiID		32Kbytes 存储器块编号，0x0000-0x01FF，对应整个 16Mbytes 存储器。
	vpAddr		需要更新的数据保存在数据变量空间的首地址，必须是偶数。
*/
void writeCodeToSPIFlash(u16 spiID,u16 vpAddr)
{//每个256K的bin文件,分成8块,spiID编号从0x0000-0x1fff
	Spi_Flash.Mode=2;
	Spi_Flash.ID=spiID;//例如17.bin其实编号是:17*8=136
	Spi_Flash.VPAddr=vpAddr;
	Spi_Flash.Delay=20;
	SPIFlash_Action();
}


//模拟触控:touchid:模拟触控的id
void analogTouch(u8 touchid)
{
    u16 tmp[4];
    tmp[0]=0x5aa5;
    tmp[1]=4;
    tmp[2]=(0xFF00|touchid);
    tmp[3]=1;
    write_dgus_vp(0x00d4,(u8*)&tmp,4);
}


#endif