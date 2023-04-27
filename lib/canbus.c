#include "canbus.h"
#include "timer.h"
//如果使用CAN，那么sys.h  必须增加如下宏定义  #define INTVPACTION

/*CAN总线位时间参数的设定与调整
1、	确定时间份额
		时间份额数量为8~25
		位时间=1/波特率           ( 250K           4us)
		时间份额=位时间/时间份额数量        
		BRP=时间份额/(2*tclk)=  时间份额/2*FOSC=206*200/2000=40      
2、	设置时间段和采样点
		时间份额数量=1+T1+T2
		采样点80%最佳
		(1+T1)/(1+T1+T2)=0.8
		就可以确定T1  T2
		传播时间段和相位缓冲器段1  =T1
		相位缓冲器段2              =T2
3、	确定同步跳转宽度和采样次数
		同步跳转宽度1~4  尽量大

*/
CANBUSUNIT   CanData;
//https://blog.csdn.net/weixin_44536482/article/details/89030152
//125K{0x3F,0x40,0x72,0x00},250K{0x1F,0x40,0x72,0x00},500K{0x0F,0x40,0x72,0x00},1M{0x07,0x40,0x72,0x00}
//根据T5L应用开发指南3.8对CAN接口进行初始化
void CanBusInit(u8* RegCfg)
{
	SetPinOut(0,2);
	SetPinIn(0,3);
	PinOutput(0,2,1);
	MUX_SEL |= 0x80;		//将CAN接口引出到P0.2,P0.3	
	ADR_H = 0xFF;
	ADR_M = 0x00;
	ADR_L = 0x60;
	ADR_INC = 1;
	RAMMODE = 0x8F;		//写操作
	while(!APP_ACK);
	#if 0
	DATA3 = 0x3f;
	DATA2 = 0x40;
	DATA1 = 0x72;
	DATA0 = 0x00;	
	#else
	DATA3 = RegCfg[0];
	DATA2 = RegCfg[1];
	DATA1 = RegCfg[2];
	DATA0 = RegCfg[3];	 		
	#endif		
	APP_EN = 1;
	while(APP_EN);
	DATA3 = 0;
	DATA2 = 0;
	DATA1 = 0;
	DATA0 = 0;	 		//配置验收寄存器ACR
	APP_EN = 1;	  
	while(APP_EN);
	DATA3 = 0xFF;
	DATA2 = 0xFF;
	DATA1 = 0xFF;
	DATA0 = 0xFF;	 	//配置AMR
	APP_EN = 1;	
	while(APP_EN);
	RAMMODE = 0;
	CAN_CR = 0xA0;
	while(CAN_CR&0x20);	//执行配置FF0060-FF0062动作
	ECAN = 1;			//打开CAN中断	
}


/**************************************************************
D3  1  CAN_RX_BUFFER  [7] IDE ，[6]RTR， [3:0]—DLC，帧数据长度。
0xFF:0068
D2:D0  3  未定义 
ID  ID，扩展帧时 29bit 有效，标准帧时 11bit 有效。
D3  1  ID 第一个字节，标准帧与扩展帧。
D2  1  ID 第二个字节，[7:5]为标准帧的高 3bit，扩展帧第 2 字节。
D1  1  ID 第三个字节，标准帧无效，扩展帧第 3 字节。
0xFF:0069
D0  1  ID 第四个字节，标准帧无效，[7:3]-扩展帧的高 5bit。
0xFF:006A  D3:D0  4  数据  接收数据，DATA1-DATA4。
0xFF:006B  D3:D0  4  数据  接收数据，DATA5-DATA8。
******************************************************************/
//对于T5L1和T5L2必须在main函数，while(1)中调用
void CanErrorReset(void)
{
	// EA=0;
	if(CAN_ET&0X20)
	{
		CAN_ET &= 0XDF;
		CAN_CR |= 0X40;
		delay_us(1000);
		CAN_CR &= 0XBF;  
		CanData.CanTxFlag = 0;
	}
	// EA=1;
}


void LoadOneFrame(void)
{
	ADR_H = 0xFF;
	ADR_M = 0x00;
	ADR_L = 0x64;
	ADR_INC = 1;
	RAMMODE = 0x8F;		//写操作
	while(!APP_ACK);
	DATA3 = CanData.BusTXbuf[CanData.CanTxTail].status;			//帧类长度型以及数据
	DATA2 = 0;
	DATA1 = 0;
	DATA0 = 0;	 		
	APP_EN = 1;
	while(APP_EN);		//写入RTR,IDE,DLC等数据
	DATA3 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>24);
	DATA2 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>16);
	DATA1 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID>>8);
	DATA0 = (u8)(CanData.BusTXbuf[CanData.CanTxTail].ID);	 		
	APP_EN = 1;
	while(APP_EN);		//写入ID数据
	DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[0];
	DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[1];
	DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[2];
	DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[3];	 		
	APP_EN = 1;
	while(APP_EN);		//写入发送数据前4字节
	DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[4];
	DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[5];
	DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[6];
	DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[7];	 		
	APP_EN = 1;
	while(APP_EN);		//写入发送数据后4字节
	CanData.CanTxTail++;
	RAMMODE = 0;
}

//status主要用于提供IDE 和 RTR状态，实际发送长度有len自动处理，大于8字节会自动拆分成多包
/*主循环调用，将需要发送的数据放在缓存区即可，同时CAN发送会占用定时器7，其余位置则不能在使用*/
void CanTx(u32 ID, u8 status, u16 len, const u8 *pData)
{
	u8 i,j,k,framnum,framoffset;
	u32 idtmp,statustmp;

	if(len>2048)//发送长度大于队列长度
		return;
	if(status&0x80)//扩展帧
	{
		idtmp = ID << 3;
	}
	else
	{
		idtmp = ID << 21;
	}
	if(CanData.BusTXbuf[CanData.CanTxHead].status&0x40)//远程帧不需要发送数据
	{
		CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
		CanData.BusTXbuf[CanData.CanTxHead].status = status&0xC0;//远程帧发送长度强制清零
		CanData.CanTxHead++;
	}
	else
	{
		framnum = len >> 3;
		framoffset = len % 8;
		k=0;
		statustmp = status&0xC0;
		for(i=0;i<framnum;i++)
		{
			CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
			CanData.BusTXbuf[CanData.CanTxHead].status = statustmp | 0x08;
			for(j=0;j<8;j++)
			{
				CanData.BusTXbuf[CanData.CanTxHead].candata[j] = pData[k];
				k++;
			}
			CanData.CanTxHead++;
		}
		if(framoffset)
		{
			CanData.BusTXbuf[CanData.CanTxHead].ID = idtmp;
			CanData.BusTXbuf[CanData.CanTxHead].status = statustmp | framoffset;
			for(j=0;j<framoffset;j++)
			{
				CanData.BusTXbuf[CanData.CanTxHead].candata[j] = pData[k];
				k++;
			}
			for(;j<8;j++)
				CanData.BusTXbuf[CanData.CanTxHead].candata[j] = 0;
			CanData.CanTxHead++;
		}
	}
	if(0==CanData.CanTxFlag)
	{
		EA = 0;
		LoadOneFrame();
		EA = 1;
		CanData.CanTxFlag = 1;
		StartTimer(7,3000);//3S还未发送出去，则清空发送标记
		CAN_CR |= 0x04;		//启动发送
	}
 	if(CanData.CanTxFlag!=0)
 	{
 		if(GetTimeOutFlag(7))
 		{
 			CanData.CanTxFlag = 0;
 		}
 	}
}

void Can_Isr() interrupt 9
{
	u8 status;
	
	EA = 0;
	if((CAN_IR&0x80) == 0x80)
	{
		CAN_IR &= 0x3F;	//清空远程帧标记位			
	}
	if((CAN_IR&0x40) == 0x40)
	{
		CAN_IR &= 0xBF;	//清空数据帧标记位
		ADR_H = 0xFF;
		ADR_M = 0x00;
		ADR_L = 0x68;
		ADR_INC = 1;
		RAMMODE = 0xAF;		//读操作
		while(!APP_ACK);
		APP_EN = 1;
		while(APP_EN);
		status = DATA3;
		CanData.BusRXbuf[CanData.CanRxHead].status = status;
		APP_EN = 1;
		while(APP_EN);			
		CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
		CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA3;
		CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
		CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA2;
		CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
		CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA1;
		CanData.BusRXbuf[CanData.CanRxHead].ID <<= 8;
		CanData.BusRXbuf[CanData.CanRxHead].ID |= DATA0;
		CanData.BusRXbuf[CanData.CanRxHead].ID=CanData.BusRXbuf[CanData.CanRxHead].ID>>3;
		if(0==(status&0x80))//标准帧ID还需要右移18位
		{
			CanData.BusRXbuf[CanData.CanRxHead].ID >>= 18;
		}
		if(0==(status&0x40))//数据帧才需要读取数据
		{
			APP_EN = 1;
			while(APP_EN);
			CanData.BusRXbuf[CanData.CanRxHead].candata[0] = DATA3;
			CanData.BusRXbuf[CanData.CanRxHead].candata[1] = DATA2;
			CanData.BusRXbuf[CanData.CanRxHead].candata[2] = DATA1;
			CanData.BusRXbuf[CanData.CanRxHead].candata[3] = DATA0;
			APP_EN = 1;
			while(APP_EN);
			CanData.BusRXbuf[CanData.CanRxHead].candata[4] = DATA3;
			CanData.BusRXbuf[CanData.CanRxHead].candata[5] = DATA2;
			CanData.BusRXbuf[CanData.CanRxHead].candata[6] = DATA1;
			CanData.BusRXbuf[CanData.CanRxHead].candata[7] = DATA0;	
		}
		RAMMODE = 0;
		CanData.CanRxHead++;
	}
	if((CAN_IR&0x20) == 0x20)
	{
		CAN_IR &= ~(0x20);	//清空发送帧标记位
		if(CanData.CanTxTail != CanData.CanTxHead)
		{
			LoadOneFrame();
			CAN_CR |= 0x04;		//启动发送		
			StartTimer(7,3000);//3S还未发送出去，则清空发送标记
		}
		else
		{
			CanData.CanTxFlag = 0;//清空发送标记位
		}
	}
	if((CAN_IR&0x10) == 0x10)
	{
		CAN_IR &= 0xEF;	//清空接收溢出标记位
	}
	if((CAN_IR&0x08) == 0x08)
	{
		CAN_IR &= 0xF7;	//清空错误标记位
	}
	if((CAN_IR&0x04) == 0x04)
	{
		CAN_IR &= 0xFB;	//清空仲裁失败标记位
		CAN_CR |= 0x04;	//重新启动发送	
	}
	CAN_ET=0;
	EA = 1;  
}


/*主循环调用，将需要发送的数据放在缓存区即可，同时CAN发送会占用定时器7，其余位置则不能在使用*/
// void CANTx(void)
// {
// 	u32 tmp32;
// 	if((0==CanData.CanTxFlag)&&(CanData.CanTxTail != CanData.CanTxFlag))
// 	{
// 		EA = 0;
// 		tmp32=CanData.BusTXbuf[CanData.CanTxTail].ID;
// 		ADR_H = 0xFF;
// 		ADR_M = 0x00;
// 		ADR_L = 0x64;
// 		ADR_INC = 1;
// 		RAMMODE = 0x8F;		//写操作
// 		while(!APP_ACK);
// 		DATA3 = CanData.BusTXbuf[CanData.CanTxTail].status;			//帧类长度型以及数据
// 		DATA2 = 0;
// 		DATA1 = 0;
// 		DATA0 = 0;	 		
// 		APP_EN = 1;
// 		while(APP_EN);		//写入RTR,IDE,DLC等数据
// 		DATA3 = (u8)(tmp32>>24);
// 		DATA2 = (u8)(tmp32>>16);
// 		DATA1 = (u8)(tmp32>>8);
// 		DATA0 = (u8)(tmp32>>0);	 		
// 		APP_EN = 1;
// 		while(APP_EN);		//写入ID数据
// 		DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[0];
// 		DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[1];
// 		DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[2];
// 		DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[3];	 		
// 		APP_EN = 1;
// 		while(APP_EN);		//写入发送数据前4字节
// 		DATA3 = CanData.BusTXbuf[CanData.CanTxTail].candata[4];
// 		DATA2 = CanData.BusTXbuf[CanData.CanTxTail].candata[5];
// 		DATA1 = CanData.BusTXbuf[CanData.CanTxTail].candata[6];
// 		DATA0 = CanData.BusTXbuf[CanData.CanTxTail].candata[7];	 		
// 		APP_EN = 1;
// 		while(APP_EN);		//写入发送数据后4字节
// 		CanData.CanTxTail++;
// 		CanData.CanTxFlag = 1;
// 		StartTimer(7,3000);//3S还未发送出去，则清空发送标记
// 		EA = 1;
// 		RAMMODE = 0;
// 		CAN_CR |= 0x04;		//启动发送		
// 	}
// 	if(CanData.CanTxFlag!=0)
// 	{
// 		if(GetTimeOutFlag(7))
// 		{
// 			CanData.CanTxFlag = 0;
// 		}
// 	}
// }