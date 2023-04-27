
#ifndef __IIC__H__
#define __IIC__H__
#include "sys.h"
#include "rtc.h"
#include "const.h"
//#define  SDAPORT       3
//#define  SDAPIN        3
//#define  SCKPORT       3
//#define  SCKPIN        2
//#define  SdaOutH()     PinOutput(SDAPORT,SDAPIN,1)  
//#define  SdaOutL()     PinOutput(SDAPORT,SDAPIN,0)
//#define  SckOutH()     PinOutput(SCKPORT,SCKPIN,1)
//#define  SckOutL()     PinOutput(SCKPORT,SCKPIN,0)



#define PORTP00 0x00
#define PORTP01 0x01
#define PORTP02 0x02
#define PORTP03 0x03
#define PORTP04 0x04
#define PORTP05 0x05
#define PORTP06 0x06
#define PORTP07 0x07

#define PORTP10 0x10
#define PORTP11 0x11
#define PORTP12 0x12
#define PORTP13 0x13
#define PORTP14 0x14
#define PORTP15 0x15
#define PORTP16 0x16
#define PORTP17 0x17

#define PORTP20 0x20
#define PORTP21 0x21
#define PORTP22 0x22
#define PORTP23 0x23
#define PORTP24 0x24
#define PORTP25 0x25
#define PORTP26 0x26
#define PORTP27 0x27

#define PORTP30 0x30
#define PORTP31 0x31
#define PORTP32 0x32
#define PORTP33 0x33


typedef struct __iic_cfg
{
	u8 SDAPort;
	u8 SCLPort;
	u16 Delay_Time;
}IIC_CFG;

void SetSdaIn(IIC_CFG *cfg); 
void SetSdaOut(IIC_CFG *cfg); 
void i2cstart(IIC_CFG *cfg);
void i2cstop(IIC_CFG *cfg);
void mack(IIC_CFG *cfg); 
void i2cbw(u8 WData, IIC_CFG *cfg);
u8   i2cbr(IIC_CFG *cfg); 
void mnak(IIC_CFG *cfg); 
void Init_IIc_Interface(IIC_CFG *cfg);

#endif


