#ifndef __NORFLASH_H__
#define __NORFLASH_H__
#include "sys.h"
#include <string.h>
#define NORFLASH_READ 0x5A
#define NORFLASH_WRITE 0xA5



#define CACHE_ADDR 0xFA00	 // dgus�����ַ ��Χ 0xfA00---0xffff
#define NORFLASH_ADDR 0x0008 // �Ĵ���
void Nor_Flash_write(u32 addr, u8 *buff, u16 len);
void Nor_Flash_read(u32 addr, u8 *buff, u16 len);
void dgusToNorFlash(u32 addrForNorFlash, u32 addrInDgus, u16 len);
void norFlashToDgus(u32 addrForNorFlash, u32 addrInDgus, u16 len);
void sysParameterRead(void);

#endif