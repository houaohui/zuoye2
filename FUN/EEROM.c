#include "EEROM.h"
#include <intrins.h>
#define I2CDelay() {_nop_();_nop_();_nop_();_nop_();}
sbit I2C_SCL = P3^6;
sbit I2C_SDA = P3^7;
	
//void Delay5ms()		//@33.1776MHz
//{
//	unsigned char i, j, k;

//	i = 1;
//	j = 162;
//	k = 89;
//	do
//	{
//		do
//		{
//			while (--k);
//		} while (--j);
//	} while (--i);
//}



void I2CStart()
{
	I2C_SDA = 1;
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 0;
}

void I2CStop()
{
	I2C_SCL = 0;
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 1;
	I2CDelay();
}

bit I2CWrite(unsigned char dat)
{
	bit ack;
	unsigned char mask;
	for (mask=0x80; mask!=0; mask>>=1)
	{
		if ((mask&dat) == 0)
		I2C_SDA = 0;
		else
		I2C_SDA = 1;
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0; 
	}
	I2C_SDA = 1;
	I2CDelay();
	I2C_SCL = 1;
	ack = I2C_SDA; 
	I2CDelay();
	I2C_SCL = 0; 
	return (~ack);
}

unsigned char I2CReadNAK()
{
	unsigned char mask;
	unsigned char dat;
	I2C_SDA = 1; 
	for (mask=0x80; mask!=0; mask>>=1)
	{
		I2CDelay();
		I2C_SCL = 1;
		if(I2C_SDA == 0) 
		dat &= ~mask; 
		else
		dat |= mask;
		I2CDelay();
		I2C_SCL = 0;
	}
	I2C_SDA = 1;
	I2CDelay();
	I2C_SCL = 1; 
	I2CDelay();
	I2C_SCL = 0;
	return dat;
}

unsigned char I2CReadACK()
{
	unsigned char mask;
	unsigned char dat;
	I2C_SDA = 1;
	for (mask=0x80; mask!=0; mask>>=1)
	{
		I2CDelay();
		I2C_SCL = 1;
		if(I2C_SDA == 0) 
		dat &= ~mask; 
		else
		dat |= mask;
		I2CDelay();
		I2C_SCL = 0;
	}
		I2C_SDA = 0;
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0; 
		return dat;
}
unsigned char E2ReadByte(unsigned char addr)
{
	unsigned char dat;
	EA=0;
	I2CStart();
	I2CWrite(0xa2); 
	I2CWrite(addr); 
	I2CStart();
	I2CWrite(0xa3); 
	dat = I2CReadNAK(); 
	I2CStop();
	EA=1;
	return dat;
}

void E2WriteByte(unsigned char addr, unsigned char dat)
{
	EA=0;
	I2CStart();
	I2CWrite(0xa2); 
	I2CWrite(addr); 
	I2CWrite(dat);
	I2CStop();
	EA=1;
}

void E2Read(unsigned char *buf, unsigned char addr, unsigned char len)
{
	
	do{	//��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
		 I2CStart();
		 if(I2CWrite(0xa2)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
		 {
			 break;
		 }
		 I2CStop();
		} while(1);
	I2CWrite(addr); //д����ʼ��ַ
	I2CStart(); //�����ظ������ź�
	I2CWrite(0xa3); //Ѱַ����������Ϊ������
	while (len > 1) //������ȡ len-1 ���ֽ�
	{
		*buf++ = I2CReadACK(); //����ֽ�֮ǰΪ��ȡ����+Ӧ��
		len--;
	}
	*buf = I2CReadNAK(); //���һ���ֽ�Ϊ��ȡ����+��Ӧ��
	I2CStop();
}
	 
	 
	 
/* E2 д�뺯����buf-Դ����ָ�룬addr-E2 �е���ʼ��ַ��len-д�볤�� */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
	while (len--)
	{
		do { //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
		I2CStart();
		if(I2CWrite(0xa2)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
		{
			break;
		}
		I2CStop();
		}while(1);
		
		I2CWrite(addr++); //д����ʼ��ַ
		I2CWrite(*buf++); //д��һ���ֽ�����
		I2CStop(); //����д�������Եȴ�д�����
	}
}



//	E2Write(str,0x00,3);
//	Delay5ms();
//	E2Read(strbuf,0x00,3);






