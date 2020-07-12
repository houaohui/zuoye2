#include "led.h"

#include <STC12C5A60S2.H>
unsigned char code ledchar[]={
	0xC0,0xF9,0xA4,0xB0,
	0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,
	0xC6,0xA1,0x86,0x8E
};
unsigned char xdata ledbuff[2] = {
	0xFF,0xFF};

//extern unsigned char xdata Timer;
void ledscan()
{
	static unsigned char i = 0;
	P0 = 0xFF;
	P2 = (P2 & 0xF8) | i;
	P0 = ledbuff[i];
	if (i < 1)
	i++;
	else
	i = 0;
}