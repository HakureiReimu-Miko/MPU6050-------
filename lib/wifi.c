#include "string.h"
#include "excel.h"
#include "wifi.h"
#include "sys.h"

u16 ssid_number=0;

/* 
void ScanWifi(void)
{
	u8 xdata temp[96] = {'1',0x0a, 'b','c', '\n', 'd', '\n', 'e', '\n'};

	char * start_post = temp;
	char * end_post;
	u8 ssid_number=0;
	
	read_dgus_vp(START_WIFI_SCAN_ADDRESS,temp,1);
	temp[1] = 0xA5;//0xa5启动wifi扫描
	write_dgus_vp(START_WIFI_SCAN_ADDRESS,temp,1);//启动扫描 		 速度不宜过快
	
	read_dgus_vp(WIFI_SSID_ADDRESS,temp,sizeof(temp));

	while((end_post = strchr(start_post,'\n'))) 
	{
		u8 xdata ssid[SSID_OFFSET * 2] = {0};	
		if (end_post > start_post) 
		{
			size_t len = end_post - start_post;
			len = len > SSID_OFFSET ? SSID_OFFSET : len;
			memcpy((void *)ssid, (const void *)start_post, len );
			ssid[len] = 0xff;
			ssid[len+1] = 0xff;
			write_dgus_vp(SSID_LIST_PTR + ssid_number * SSID_OFFSET, ssid, sizeof(ssid)/2);
		}
		if ((unsigned char *)end_post+1 < temp+strlen((const char *)temp)) 
		{
			start_post = end_post+1;
		} 
		else 
		{
			break;
		}
		ssid_number++;
	}
} */

// void utf_to_unicode(unsigned char *utf, unsigned char *unicode)
void utf_to_unicode(unsigned char *utf,unsigned char *unicode,unsigned int len_code)
{
	unsigned int i=0,j=0;
	unsigned char b1, b2, b3, b4, b5, b6;
	while(len_code-i)
	{
		if((utf[i]&0xFE)==0xFC)
		{
			b1 = utf[i];
			b2 = utf[i+1];
			b3 = utf[i+2];
			b4 = utf[i+3];
			b5 = utf[i+4];
			b6 = utf[i+5];

			unicode[j+3] = ((b5&0x3F)<<6)|((b6&0x3F)<<0);
			unicode[j+2] = ((b4&0x3F)<<4)|((b5&0x3F)>>2);
			// unicode[j+2] = (b5 << 4) + ((b6 >> 2) & 0x0F);
			unicode[j+1] = ((b3&0x3F)<<2)|((b4&0x3F)>>4);
			unicode[j] = ((b1&0x01)<<6)|((b2&0x3F)<<0);
			j = j+4;
			i = i+6;
		}
		else if((utf[i]&0xFC)==0xF8)
		{
			b1 = utf[i];
			b2 = utf[i+1];
			b3 = utf[i+2];
			b4 = utf[i+3];
			b5 = utf[i+4];

			unicode[j+3] = ((b4&0x3F)<<6)|((b5&0x3F)<<0);
			unicode[j+2] = ((b3&0x3F)<<4)|((b4&0x3F)>>2);
			unicode[j+1] = ((b2&0x3F)<<2)|((b3&0x3F)>>4);
			unicode[j] = ((b1&0x03)<<6);
			j = j+4;
			i = i+5;
		}
		else if((utf[i]&0xF8)==0xF0)
		{
			b1 = utf[i];
			b2 = utf[i+1];
			b3 = utf[i+2];
			b4 = utf[i+3];

			unicode[j+2] = ((b3&0x3F)<<6)|((b4&0x3F)<<0);
			unicode[j+1] = ((b2&0x3F)<<4)|((b3&0x3F)>>2);
			unicode[j] = ((b1&0x07)<<2)|((b2&0x3F)>>4);
			j = j+3;
			i = i+4;
		}
		else if((utf[i]&0xF0)==0XE0)
		{
			b1 = utf[i];
			b2 = utf[i+1];
			b3 = utf[i+2];

			unicode[j+1] = ((b2&0x3F)<<6)|((b3&0x3F)<<0);
			unicode[j] = ((b1&0x0F)<<4)|((b2&0x3F)>>2);
			j = j+2;
			i = i+3;
		}
		else if((utf[i]&0xE0)==0xC0)
		{
			b1 = utf[i];
			b2 = utf[i+1];
			unicode[j+1] = ((b2&0x3F)<<6)|((b3&0x3F)<<0);
			unicode[j] = ((b1&0x1F)>>2);
			j = j+2;
			i = i+2;
		}
		else if((utf[i]&0x80)==0x00)
		{
			unicode[j] = 0;
			unicode[j+1] = utf[i];
			j = j+2;
			i++;
			len_code++;
		}
		else
			break;
	}
}

void UnicodeToGBK(unsigned char *unicode,unsigned char *gbk,unsigned int len_code)
{
	unsigned int unicode16=0,k=0;
	unsigned int a=0,b=0,m=0,w=0;
	unsigned int count=0;
	a=0;
	//len_code+=len_code;
	while(len_code-count)//while(len_code-a) //字符的个数
	{
		unicode16 = 0;
		// unicode16 |= unicode[a]<<8;
		// unicode16 |= unicode[a+1];
		unicode16=unicode[a];
		unicode16<<=8;
		unicode16 |= unicode[a+1];
		if (0x0080 < unicode16)//非普通字符
		{
			if(0x5740 > unicode16)
			{
				k = 0;
			}
			else if(0x635e > unicode16)
			{
				k = 500;
			}
			else if(0x72fc > unicode16)
			{
				k = 1000;
			}
			else if(0x86c7 > unicode16)
			{
				k = 1500;
			}
			else 			
			{
				k = 2000;
			}
			for(b=0;b<500;b++)
			{
				if(unicode16==utf_data[w*20+b+k])
				{
					gbk[m] = gbk_data[w*20+b+k]>>8;
					gbk[m+1] = gbk_data[w*20+b+k]&0xff;
					m+=2;
					a+=2;
					count+=2;
					break;
				}
				else if(w*20+b==499)
				{
					gbk[m] = 0x2a;
					m++;
					a+=2;
					count+=2;
				}
			}
		}
		else if(unicode16)//0x80之前的是各种字符，它们的值与ASCII的值一样
		{
			gbk[m] = unicode[a+1];
			m++;
			a=a+2;
			count+=1;
		}
		else break;
	}
	gbk[m] = 0xff;
	gbk[m+1] = 0xff;
}

void scanWifiGBK(void)
{
	u8 xdata temp[512] = {0};
	char * start_post;
	char * end_post;
	u16 tmp16;
	u16 xdata len_code = 0;
	u8 xdata unicode[128];
	u8 xdata gbk[64];

	ssid_number=0;
	
	read_dgus_vp(WIFI_SSID_ADDRESS,(u8*)&temp,sizeof(temp)/2);
	start_post = temp;
	while((end_post = strchr(start_post,0x0A)))//找到换行符的位置，传给end_post，这里传的是一个地址
	{
		u8 xdata ssid[SSID_OFFSET * 2] = {0};
		memset(unicode,0,sizeof(unicode));
		memset(gbk,0,sizeof(gbk));
			
		if (end_post > start_post) 
		{
			len_code = end_post - start_post;
			len_code = len_code > SSID_OFFSET ? SSID_OFFSET : len_code;
			memcpy((void *)ssid, (const void *)start_post, len_code );
			write_dgus_vp(SSID_LIST_PTR_SRC + ssid_number * SSID_OFFSET, (u8*)&ssid, sizeof(ssid)/2);//UTF8的源码wifi名称

			utf_to_unicode(ssid,unicode,len_code);//转Unicode码
			UnicodeToGBK(unicode,gbk,len_code);//转GBK码
			write_dgus_vp(SSID_LIST_PTR + ssid_number * SSID_OFFSET, (u8*)&gbk, sizeof(gbk)/2);//GBK码的WiFi名称
			ssid_number++;
		}
		if ((unsigned char *)end_post+1 < temp+strlen((const char *)temp)) 
		{
			start_post = end_post+1;
		} 
		else 
		{
			break;
		}
		// ssid_number++;
	}

	write_dgus_vp(0x1E00,(u8*)&ssid_number,1);//测试
} 
















