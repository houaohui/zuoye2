#ifndef _IRReceived_h_
#define _IRReceived_h_
#include <stc12c5a60s2.h>



extern unsigned char xdata ircode[4];
extern bit irflag;

void InitInfrared();




#endif