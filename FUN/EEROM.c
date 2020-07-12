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
	
	do{	//用寻址操作查询当前是否可进行读写操作
		 I2CStart();
		 if(I2CWrite(0xa2)) //应答则跳出循环，非应答则进行下一次查询
		 {
			 break;
		 }
		 I2CStop();
		} while(1);
	I2CWrite(addr); //写入起始地址
	I2CStart(); //发送重复启动信号
	I2CWrite(0xa3); //寻址器件，后续为读操作
	while (len > 1) //连续读取 len-1 个字节
	{
		*buf++ = I2CReadACK(); //最后字节之前为读取操作+应答
		len--;
	}
	*buf = I2CReadNAK(); //最后一个字节为读取操作+非应答
	I2CStop();
}
	 
	 
	 
/* E2 写入函数，buf-源数据指针，addr-E2 中的起始地址，len-写入长度 */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
	while (len--)
	{
		do { //用寻址操作查询当前是否可进行读写操作
		I2CStart();
		if(I2CWrite(0xa2)) //应答则跳出循环，非应答则进行下一次查询
		{
			break;
		}
		I2CStop();
		}while(1);
		
		I2CWrite(addr++); //写入起始地址
		I2CWrite(*buf++); //写入一个字节数据
		I2CStop(); //结束写操作，以等待写入完成
	}
}



//	E2Write(str,0x00,3);
//	Delay5ms();
//	E2Read(strbuf,0x00,3);






