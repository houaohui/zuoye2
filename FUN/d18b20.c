#include "d18b20.h"
#include "lcd1602.h"
#include <stc12c5a60s2.h>
sbit DS = P3^5; //1_wire
unsigned char strT[6];
extern bit flag18;
void delay(unsigned int us)
{
	while(us--);
}

//��ʼ������
bit ds_init()
{
	bit i;
	DS=1;
	delay(1);
	DS=0;
	delay(998);//��ʱ480us���ϣ�ds18b20������λ
	DS=1;
	delay(59);//15~60us,�ȴ�
	i=DS;
	delay(300);//����֮��ȴ�60~240us
	DS=1;
	delay(1);
	return (i);
}

//дһ���ֽ�
void write_byte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DS=0;
		delay(1);
		DS=dat&0x01;
		delay(150);//��ʱ60us����
		DS=1; //�ͷ�����׼���´�����д��
		delay(1);//1us
		dat >>= 1;
	}
}
//��һ���ֽ�
unsigned char read_byte()
{
	unsigned char i,j,dat;
	for(i=0;i<8;i++)
	{
		DS=0;//������ʱ��
		delay(1);//1us
		DS=1;//�ͷ����߲���
		delay(1);//1us
		j=DS;
		delay(150);//60us����
		DS=1;
		delay(1);//1us
		dat=(j<<7)|(dat>>1);
	}
	return(dat);
}
bit Start18B20()
{
	bit ack;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0x44);
	}
	EA=1;
	return(~ack);
}


bit Get18B20Temp(int *temp)
{
	bit ack;
	unsigned char LSB,MSB;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0xbe);
		LSB=read_byte();
		MSB=read_byte();
		*temp=((int)MSB<<8)+LSB;
	}
	EA=1;
	return ~ack;
}

unsigned char IntToString(unsigned char *str,int dat)
{
	signed char i=0;
	unsigned char len=0;
	unsigned char buf[6];

	if(dat<0)
	{
		dat=-dat;
		*str++='-';
		len++;
	}
	do{
		buf[i++]=dat%10;
		dat/=10;
	}while(dat>0);
	len+=i;
	while(i-->0)
	{
		*str++=buf[i]+'0';
	}
	*str='\0';
	return len;
}

void ds18b20()
{
	int xdata temp;
	int xdata intT,decT;
	unsigned char len;
	if(flag18)
		{
			if(Get18B20Temp(&temp))
			{
				intT=temp>>4;
				decT=temp&0xF;
				len=IntToString(strT,intT);
				strT[len++]='.';
				decT=(decT*10)/16;
				strT[len++]=decT+'0';
				strT[len]='c';
				strT[++len]='\0';
				LcdShowStr(0,1,strT);
			}
			Start18B20();
		}
}