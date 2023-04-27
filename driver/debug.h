#ifndef  __DEBUG__H___
#define  __DEBUG__H___
#include "sys.h"

#define DEBUG
#ifdef DEBUG 
    #define DEBUGINIT initDebugUart
#else
    #define DEBUGINIT /\
/DEBUGINIT
#endif

#ifdef DEBUG 
    #define DEBUGINFO printf
#else
    #define DEBUGINFO /\
/DEBUGINFO
#endif

void initDebugUart(void);
#endif