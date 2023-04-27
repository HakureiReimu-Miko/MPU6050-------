#include "umath.h"

void StrCopy(u8 *str1,u8 *str2, u16 len)
{
	u16 i;
	
	if((str1==NULL)||(str2==NULL))
		return;
	for(i=0;i<len;i++)
	{
		str1[i] = str2[i];
	}
}

u8 StrCopare(u8 *str1,u8 *str2, u16 len)
{
	u16 i;
	
	if((str1==NULL)||(NULL == str2))
		return 0;
	for(i=0;i<len;i++)
	{
		if(*str1++ != *str2++)
			return 1;
	}
	return 0;
}

void StrClear(u8 *str1,u16 len)
{
	u16 i;
	
	if(str1==NULL)
		return;
	for(i=0;i<len;i++)
	{
		*str1++ = 0;
	}
}

//HEX码转为ASCII，将数字转成文本显示,例如：0x0A转为字符"10"
u8 *Hex_input_to_ASCII(u32 value,u8 *buf_write,u8 maxLen)
{
	u8 i,j;
	u8 buf_write_tmp[16] = {0};
	i = 0;

    if(maxLen>=16)
        maxLen=16;
	
    do{
        buf_write_tmp[i++] = value % 10 + '0';  //取下一个数字
     }while((value /= 10) > 0);

    //write_dgus_vp(0x1331,(u8*)&buf_write_tmp[0],3);//测试使用
    for(j=maxLen;j>0;j--)
    {
        buf_write[j-1]=0;
        if((buf_write_tmp[j-1]!='\0') && (buf_write_tmp[j-1]!=0xff))
        {
            *buf_write++ = buf_write_tmp[j-1];
        }
    }
    
	return buf_write;
}

code u8 asciiTable[6]={'A','B','C','D','E','F'};
u16 hexToAscii(u8 str)//Hex转BCD码
{
    u8 high4bit,low4bit;
    u16 result;
    
    high4bit = (str & 0xF0)>>4;
    low4bit = (str & 0x0F);
    if(high4bit >= 10)
    {
        high4bit = asciiTable[high4bit-10];
    }
    else
    {
        high4bit |= 0x30;
    }
    if(low4bit >= 10)
    {
        low4bit = asciiTable[low4bit-10];
    }
    else
    {
        low4bit |= 0x30;
    }
    result = high4bit;
    result <<= 8;
    result |= low4bit;
    return result;
}

code u8 asciiTableSmall[6]={'a','b','c','d','e','f'};//小写
u16 hexToAsciiSmall(u8 str)//Hex转BCD码,小写
{
    u8 high4bit,low4bit;
    u16 result;
    
    high4bit = (str & 0xF0)>>4;
    low4bit = (str & 0x0F);
    if(high4bit >= 10)
    {
        high4bit = asciiTableSmall[high4bit-10];
    }
    else
    {
        high4bit |= 0x30;
    }
    if(low4bit >= 10)
    {
        low4bit = asciiTableSmall[low4bit-10];
    }
    else
    {
        low4bit |= 0x30;
    }
    result = high4bit;
    result <<= 8;
    result |= low4bit;
    return result;
}

#define MAX_SEARCH_BYTE 20//最大读取的字符串长度,20字节
void getCharValue(u8 *sourceTxt, u16 *value)//寻址数字字符串转成数字
{
    u8 i;
    u16 tmp;

    for(i=0;i<MAX_SEARCH_BYTE;i++)
    {
        if(sourceTxt[i]>=0x30 && sourceTxt[i]<=0x39)//找到第一个数字
            break;
    }
    tmp = 0;
    for(;i<MAX_SEARCH_BYTE;i++)
    {
        if(sourceTxt[i]>=0x30 && sourceTxt[i]<=0x39)
        {
            tmp *= 10;
            tmp += (sourceTxt[i] - 0x30);
        }
        else
            break;
    }
    *value = tmp;
}

#define MAX_SEARCH_BYTE 20//最大读取的字符串长度,20字节
void getCharValueASCII(u8 *sourceTxt,u8 *ascii)//寻找数字字符串,以字符串形式暂时存储
{
    u8 i;
    for(i=0;i<MAX_SEARCH_BYTE;i++)
    {
        if(sourceTxt[i]>=0x30 && sourceTxt[i]<=0x39)
            break;
    }
    for(;i<MAX_SEARCH_BYTE;i++)
    {
        if(sourceTxt[i]>=0x30 && sourceTxt[i]<=0x39)
        {
            *ascii++ = sourceTxt[i];
        }
        else
            break;
    }
}

#define MAX_SEARCH_TXT_BYTE 50//最大读取的字符串长度,50字节
void getCharAscii(u8 *sourceTxt, u8 *ascii)//寻找""中的字符串,以字符串形式暂时存储
{
    u8 i;

    for(i=0;i<MAX_SEARCH_TXT_BYTE;i++)
    {
        if(sourceTxt[i]=='\"')
            break;
    }
    i++;
    for(;i<MAX_SEARCH_TXT_BYTE;i++)
    {
        if(sourceTxt[i]!='\"')
        {
            *ascii++ = sourceTxt[i];
        }
        else
            break;
    }
}


unsigned char sizeArray(unsigned char *str)
{
    unsigned char numArray;
    numArray=0;
    if(str==NULL)
        return 0;
    while(*str!='\0' && *str!=0xFF)
    {
        str++;
        numArray++;
        if(numArray>1024)
        {
            numArray=0;
            break;
        }
    }

    return numArray;
}

//0相同  1不同
unsigned char mStrCmp(unsigned char *str1,unsigned char *str2)
{
    unsigned char i;
    u16 len1,len2;
    len1=sizeArray(str1);
    len2=sizeArray(str2);
    // write_dgus_vp(0x1F5E,(u8*)&len1,1);
    // write_dgus_vp(0x1F5F,(u8*)&len2,1);
    if(len1!=len2)
        return 1;

	for(i=0;i<len1;i++)
	{
		if(*str1++ != *str2++)
			return 1;
	}
    return 0;
}

/*****************************************************************************
 函 数 名  :u16 Calculate_CRC16(unsigned char *updata, unsigned char len)
 功能描述  : CRC-16校验
 输入参数  : updata		要处理的数据
			len	长度
			mode  0用于验算CRC      1用于生成CRC值，并复制到数组中
 输出参数  : 
*****************************************************************************/
u16 Calculate_CRC16(unsigned char *updata, unsigned char len ,unsigned char mode)
{
    unsigned int Reg_CRC=0xffff;
    unsigned char i,j;
    for (i=0;i<len;i++)
    {
        Reg_CRC^=*updata++;
        for (j=0;j<8;j++)
        {
            if (Reg_CRC & 0x0001)
            {
               Reg_CRC=Reg_CRC>>1^0XA001;
            }
            else
            {
               Reg_CRC>>=1;
            }
        }
    }
	if(mode == 1)
	{
		*updata++ = (u8)Reg_CRC;
		*updata = (u8)(Reg_CRC>>8);
	}
	
	return Reg_CRC;
}

u8 BCD(u8 dat)
{
	  return ((dat/10)<<4)|(dat%10);
}

u8 IBCD(u8 dat)
{
	 return (dat>>4)*10+(dat&0x0f);
}