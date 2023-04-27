#ifndef __UMATH__H___
#define __UMATH__H___
#include "sys.h"

#define MAX(A, B) (A >= B ? A : B)

void StrClear(u8 *str1, u16 len);
void StrCopy(u8 *str1, u8 *str2, u16 len);
u8 StrCopare(u8 *str1, u8 *str2, u16 len);
u8 *Hex_input_to_ASCII(u32 value, u8 *buf_write, u8 maxLen);
u16 hexToAscii(u8 str);      // Hex×ªBCDÂë
u16 hexToAsciiSmall(u8 str); // Hex×ªBCDÂë,Ð¡Ð´
void getCharValue(u8 *sourceTxt, u16 *value);
void getCharValueASCII(u8 *sourceTxt, u8 *ascii);
void getCharAscii(u8 *sourceTxt, u8 *ascii);
unsigned char sizeArray(unsigned char *str);
unsigned char mStrCmp(unsigned char *str1, unsigned char *str2);

u16 Calculate_CRC16(unsigned char *updata, unsigned char len, unsigned char mode);

u8 BCD(u8 dat);
u8 IBCD(u8 dat);
#endif