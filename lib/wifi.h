#ifndef __WIFI_H__
#define __WIFI_H__
#include "sys.h"

extern u16 ssid_number;


#define START_WIFI_SCAN_ADDRESS 0x0498		//wifi信息读取地址
#define WIFI_SSID_ADDRESS 		0x04D0		//wifi信息读取地址
#define SSID_LIST_PTR_SRC     	0x7000        //wifi信息存放地址,存放UTF8源码
#define SSID_LIST_PTR     		0x7D00        //wifi信息存放地址,存放转换后的GBK码
#define SSID_OFFSET       		0x0020        //wifi信息存放长度
// void ScanWifi(void);
void utf_to_unicode(unsigned char *utf,unsigned char *unicode,unsigned int len_code);
void UnicodeToGBK(unsigned char *unicode,unsigned char *gbk,unsigned int len_code);
void scanWifiGBK(void);



#endif