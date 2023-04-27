#include "sys.h"
#include "iic.h"
#include "rtc.h"
#include "const.h"
 

void  Init_IIc_Interface(IIC_CFG *cfg)
{
	SetPinOut((cfg->SCLPort&0xf0)>>4,cfg->SCLPort&0x0f);
	// SetPinOut((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f);
}

void SetSdaIn(IIC_CFG *cfg) 
{ 
    SetPinIn((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f);
} 

void SetSdaOut(IIC_CFG *cfg) 
{ 
    SetPinOut((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f);
} 

u8  GetSdaIn(IIC_CFG *cfg)
{
	return GetPinIn((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f);
}

void SdaOutH(IIC_CFG *cfg)     
{
	PinOutput((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f,1);
}

void SdaOutL(IIC_CFG *cfg)
{
	PinOutput((cfg->SDAPort&0xf0)>>4,cfg->SDAPort&0x0f,0);
}

void SckOutH(IIC_CFG *cfg)
{
	PinOutput((cfg->SCLPort&0xf0)>>4,cfg->SCLPort&0x0f,1);
}

void SckOutL(IIC_CFG *cfg)
{
	PinOutput((cfg->SCLPort&0xf0)>>4,cfg->SCLPort&0x0f,0);
}    


void i2cstart(IIC_CFG *cfg) 
{ 
		SetSdaOut(cfg); 
		SdaOutH(cfg); 
		SckOutH(cfg); 
		delay_us(15); 
		SdaOutL(cfg); 
		delay_us(15); 
		SckOutL(cfg); 
		delay_us(15);
} 

void i2cstop(IIC_CFG *cfg) 
{ 
		SetSdaOut(cfg); 
		SdaOutL(cfg); 
		SckOutH(cfg); 
		delay_us(15); 
		SdaOutH(cfg); 
		delay_us(15); 
		SetSdaIn(cfg);
} 

void mack(IIC_CFG *cfg) 
{ 
		SetSdaOut(cfg); 
		SdaOutL(cfg); 
		delay_us(5); 
		SckOutH(cfg); 
		delay_us(5); 
		SckOutL(cfg); 
		delay_us(5); 
} 

void mnak(IIC_CFG *cfg) 
{ 
		SetSdaOut(cfg); 
		SdaOutH(cfg); 
		delay_us(5); 
		SckOutH(cfg); 
		delay_us(5); 
		SckOutL(cfg); 
		delay_us(5);
} 

void cack(IIC_CFG *cfg)
{ 
		unsigned char i; 
		SetSdaIn(cfg); 
		SdaOutH(cfg); 
		delay_us(5); 
		SckOutH(cfg); 
		delay_us(5); 
		for(i=0;i<50;i++) 
		{ 
			if(!GetSdaIn(cfg)) break; 
		  delay_us(5);
		} 
		SckOutL(cfg); 
		delay_us(5); 
		SetSdaOut(cfg);
} 

//I2C 写入1个字节 
void i2cbw(u8 WData, IIC_CFG *cfg) 
{ 
		char i; 
		SetSdaOut(cfg); 
		for(i=0;i<8;i++) 
		{ 
				if(WData&0x80) SdaOutH(cfg); 
				else SdaOutL(cfg); 
				WData=(WData<<1); 
				delay_us(5); 
				SckOutH(cfg); 
				delay_us(5); 
				SckOutL(cfg); 
				// delay_us(5);
		} 
		cack(cfg);
} 

//i2c 读取1个字节数据 
u8 i2cbr(IIC_CFG *cfg) 
{ 
		u8 i; 
		u8 RData; 		
		SetSdaIn(cfg); 
		for(i=0;i<8;i++) 
		{ 
				delay_us(5); 
				SckOutH(cfg); 
				delay_us(5); 
				RData=(RData<<1); 
				if(GetSdaIn(cfg)) 
				RData=RData|0x01; 
				else 
				RData=RData&0xFE; 
				SckOutL(cfg); 
				// delay_us(5); 
		} 
		return(RData); 
} 
	