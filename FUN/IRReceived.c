
#include "IRReceived.h"

#include <STC12C5A60S2.H>
sbit IR_INPUT = P3^2; 

unsigned char xdata ircode[4];
bit irflag;

void InitInfrared()
{	
	IR_INPUT = 1;
	TMOD &= 0X0F;
	TMOD |= 0x10;
	TR1 = 0;
	ET1 = 0;
	IT0 = 1;
	EX0 = 1;
}
unsigned int GetHighTime()
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	while(IR_INPUT)
	{
		if(TH1 > 0xC2)
		{
			break;
		}
	}
	TR1 = 0;

	return(TH1 * 256 + TL1);
}

unsigned int GetLowTime()
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	while(!IR_INPUT)
	{
		if(TH1 > 0xC2)
		{
			break;
		}
	}
	TR1 = 0;

	return(TH1 * 256 + TL1);
}


void EXINT0_IR() interrupt 0
{
	unsigned char i, j;
	unsigned int time;
	unsigned char byt;

	time = GetLowTime();
	if((time <23500) || (time > 26265))	   
	{
		IE0 = 0;
		return;	
	}

	time = GetHighTime();
	if((time<11059) || (time > 13824))
	{
		IE0 = 0;
		return;
	}
	for(i=0; i<4; i++)
	{
		for(j=0; j<8; j++)
		{
			time = GetLowTime();
			if((time<940) ||(time >2157))
			{
				IE0 = 0;
				return;
			}
			time = GetHighTime();
			if((time>940) && (time <2157))
			{
				byt >>= 1;	
			}
			else if((time>4037) && (time<5253))
			{
				byt >>= 1;
				byt |= 0x80;
			}
			else
			{
				IE0 = 0;
				return;
			}
		}
		ircode[i] = byt;
	}
	IE0 = 0;
	irflag=1;
}

