#include "key.h"

extern void keyaction(unsigned char keycode);

unsigned char xdata keysta[4][4] = {
 {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
unsigned char code keycodemap[4][4] = {
	{ 1, 2, 3, 'u' },
	{ 4, 5, 6, 'l' },
	{ 7, 8, 9, 'd' },
	{ 0, 'e', 'n', 'r' }
};

void keyscan()
{
	unsigned char i;
	static unsigned char keyout=0;
	static unsigned char xdata keybuf[4][4]={
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}
	};
	keyout=keyout&0x03;
	switch(keyout)
	{
		case 0: P2=0x7f;break;
		case 1: P2=0xbf;break;
		case 2: P2=0xdf;break;
		case 3: P2=0xef;break;
		default:break;
	}
	keybuf[keyout][0]=(keybuf[keyout][0]<<1)|P20;
	keybuf[keyout][1]=(keybuf[keyout][1]<<1)|P21;
	keybuf[keyout][2]=(keybuf[keyout][2]<<1)|P22;
	keybuf[keyout][3]=(keybuf[keyout][3]<<1)|P23;
	for(i=0;i<4;i++)
	{
		if((keybuf[keyout][i]&0x0f)==0x00)
		{
			keysta[keyout][i]=0;
		}
		else if((keybuf[keyout][i]&0x0f)==0x0f)
		{
			keysta[keyout][i]=1;
		}
	}
	keyout++;
}


void keydriver()
{
	unsigned char i, j;
	static unsigned char xdata backup[4][4] = {
	{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
	};
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (backup[i][j] !=keysta[i][j])
			{
				if (backup[i][j] == 0)
				{
					keyaction(keycodemap[i][j]);
				}
				backup[i][j] = keysta[i][j];
			}
		}
	}
}

