#ifndef __DWIN8283_H__
#define __DWIN8283_H__
#include "sys.h"
#define NOT_SEND_DATA 0
#define DO_SEND_DATA 1

typedef struct _quene8283
{
	u8 *pQuene;
	u16 *QueneHead,*QueneTail;	  
    u16 QueneSize;
}QUENE;

void Pro8283Init(void);
void Pro8283Deal(void);

#endif