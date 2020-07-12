#ifndef _EEROM_h_
#define _EEROM_h_
#include <stc12c5a60s2.h>

unsigned char E2ReadByte(unsigned char addr);

void E2WriteByte(unsigned char addr, unsigned char dat);

void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);

void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);


#endif