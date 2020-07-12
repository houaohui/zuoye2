/*
��ϸ������˵��
*/
#include <stc12c5a60s2.h>
#include "lcd1602.h"
#include "key.h"
#include "IRReceived.h"
#include "d18b20.h"
#include "led.h"
#include "EEROM.h"

sbit ENLED=P1^1;
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2);

void report();
void configuart();
void UARTdriver();
void uartaction();
void UARTsend(unsigned char *buf);
void disply();
void ledscan();
void Timer0Init();
void IRAction();

unsigned char mima[7]="123456";
unsigned char code start0[] = "Loading...";
unsigned char code start1[] = "Please input password!";
unsigned char code start2[] = "Welcome to you!";
unsigned char code start3[] = "The password is error! pleease input again!";
unsigned char TIM[3]={8,25,0},sw=0,ab=0,Timer=60;
unsigned char bufRxd[16],cntRxd=0;
unsigned int index=0,index2=0;
bit ui,uart,flagTxd,en,disp,yi,flag18,gaimi,gaishi,run,storTIM,repo;



void main()
{
	ENLED=1;
	EA=1;
	Timer0Init();
	configuart();
	InitLcd1602();
	InitInfrared();
	UARTsend(start1);
	LcdShowStr(0,0,start0);//��ʾ�ȴ�
	LcdShowStr(0,0,start0);//��ʾ�ȴ�
	Timer=E2ReadByte(0x03);//��ȡ�ϴε����
	E2Read(TIM,0x00,3);//��ȡ�����ʱ��
	while(1)
	{
		
		UARTdriver();
		
		if(ui)
			{
				report();
				switch(ab)
				{
					case 0:if(disp){disply();disp=0;ds18b20();}break;//A����0.1��ˢ��һ��
					case 1:if(yi){LcdShowStr(0,0,"B Timer");LcdShowStr(0,1,"Press n to save");yi=0;ENLED=0;}break;//ֻˢ��һ��B����
					case 2:if(yi){LcdShowStr(0,0,"C Changepassword");yi=0;gaimi=1;}break;//���������
				  case 3:if(yi){LcdShowStr(0,0,"D Change Time");LcdShowStr(0,1,"Press n to save");yi=0;gaishi=1;}break;//��ʱ�����
				}
				
				
			}
			keydriver();
			IRAction();
	}
}
//ʵʱ�ϴ�ʱ����¶�
void report()
{
	unsigned char report[12];
	if(repo)
	{
		repo=0;
		report[0]=TIM[0]/10+'0';
		report[1]=TIM[0]%10+'0';
		report[2]=TIM[1]/10+'0';
		report[3]=TIM[1]%10+'0';
		report[4]=(TIM[2]/10%10)+'0';
		report[5]=TIM[2]%10+'0';
		report[6]='/';
		report[7]=strT[0];
		report[8]=strT[1];
		report[9]=strT[2];
		report[10]=strT[3];
		report[11]='\0';
		UARTsend(report);
		UARTsend("\r\n");
	}
}
//ʱ����ʾ���ַ���
void disply()
{
	unsigned char abs[14];
	if(ab==0)//Ҫ��ʾ����A
	{
		abs[0]='A';
		abs[1]=' ';
		abs[2]=TIM[0]/10+'0';
		abs[3]=TIM[0]%10+'0';
		abs[4]='-';
		abs[5]=TIM[1]/10+'0';
		abs[6]=TIM[1]%10+'0';
		abs[7]='-';
		abs[8]=(TIM[2]/10%10)+'0';
		abs[9]=TIM[2]%10+'0';
		abs[10]='\0';
		LcdShowStr(0,0,abs);
	}
}

bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2)//ֻ��������Ƚ�
{
	while (*ptr2!='\0')
	{
			if (*ptr1++ != *ptr2++) //�������������ʱ���̷��� 0
		{
			return 0;
		}
	}
	return 1; //�Ƚ���ȫ���������ݶ�����򷵻� 1 
}
void UARTsend(unsigned char *buf)
{
	while(*buf!='\0')
	{
		flagTxd=0;
		SBUF=*buf++;
		while(!flagTxd);
	}
}
void uartaction()
{
	if(ui==0)//������ִ�������ж�
	{
		if(CmpMemory(bufRxd,mima))//����Ƚϳɹ��󿪻�
		{
			ui=1;
			UARTsend(start2);
		}
		else//û�гɹ���
		{
			UARTsend(start3);
		}
	}
	
	if(bufRxd[0]=='T')//ͨ����λ������ʱ��
	{
		TIM[0]=(bufRxd[1]-'0')*10+(bufRxd[2]-'0');
		TIM[1]=(bufRxd[3]-'0')*10+(bufRxd[4]-'0');
		TIM[2]=(bufRxd[5]-'0')*10+(bufRxd[6]-'0');
	}
}
void UARTdriver()
{
	if(uart)//һ���Ժ����û�н���������ʼ
	{
		if(cntRxd!=0)//cntRxd��Ϊ�㣬˵��һ֡���ݽ�����ɣ���ʼ��������
		{
			uartaction();
		}
		uart=0;
		cntRxd=0;//�´δ�ͷ��ʼ��������
	}
}

void configuart()
{
	SCON=0x50; //����ģʽ1
	AUXR=0x11;
	BRT=0xf7;
	ES=1;//ʹ�ܴ����ж�
}

void interruptuart()  interrupt 4
{
	if(RI)
	{
		RI=0;
		bufRxd[cntRxd++]=SBUF;
		index=0;//���յ�һ���ֽ��������¼�ʱ,û���յ�ʱ��
		uart=0;//0.2���Ժ�uart������һ����ʼ��UARTdriver()�д�������
	}
	if(TI)
	{
		TI=0;
		flagTxd=1;
	}
}


void Timer0Init()		//1����@33.1776MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x33;		//���ö�ʱ��ֵ
	TH0 = 0xF5;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
}
//�����ѡ����˸��Ч
void ledswitch()
{
	static unsigned int inde=0;
	inde++;
	if(sw==0)
	{
		ledbuff[0]=ledchar[Timer/10%10];
		ledbuff[1]=ledchar[Timer%10];
	}
	
	if(sw==1)
	{
		if(inde>500)
		{
			ledbuff[0]=0xff;
		}
		if(inde<=500)
		{
			ledbuff[0]=ledchar[Timer/10%10];
		}
		ledbuff[1]=ledchar[Timer%10];
	}
		
	if(sw==2)
	{
		if(inde>500)
		{
			ledbuff[1]=0xff;
		}
		if(sw==2&&inde<=500)
		{
			ledbuff[1]=ledchar[Timer%10];
		}
		ledbuff[0]=ledchar[Timer/10%10];
	}
	if(inde>=1000)inde=0;
						/*switch(sw)
						{
							case 0:ledbuff[0]=ledchar[Timer/10%10];
							ledbuff[1]=ledchar[Timer%10];
							case 1:if(inde>500)
							{
								ledbuff[0]=0xff;
							}
							if(inde<=500)
							{
								ledbuff[0]=ledchar[Timer/10%10];
							}
							break;
							case 2:if(inde>500)
							{
								ledbuff[1]=0xff;
							}
							if(sw==2&&inde<=500)
							{
								ledbuff[1]=ledchar[Timer%10];
							}
							break;
						}
						if(inde>=1000)inde=0;*/
}

void InterruptTimer0() interrupt 1
{
	TL0 = 0x33;
	TH0 = 0xF5;
	keyscan();
	if(ENLED==0)
	{
		ledswitch();
		ledscan();
	}
	index++;
	index2++;
	if(index>=1000)
	{
		index=0;
		TIM[2]++;
		repo=1;
		flag18=1;//flag1s�����¶�ˢ��
		if(run&&sw==0)Timer--;
		if(Timer==0)
		{
			en=1;
			Timer=60;
		}
	}
	if(index2>=300)
	{
		disp=1;//disp����ģ��ʱ��ˢ��
		index2=0;
		uart=1;
	}
	
	if(TIM[2]>=60)
	{
		TIM[2]=0;
		TIM[1]++;
	}
	if(TIM[1]>=60)
	{
		TIM[1]=0;
		TIM[0]++;
	}
	if(TIM[0]>=24)
	{
		TIM[0]=0;
	}

	if(en)//���Ӻ͵�ѹ�����޶����Դ򿪷�����
	{
		P13=~P13;
	}
}
//�����뺯��
void gaimiaction(unsigned char i)
{
	static unsigned char steap=0;
	unsigned char s[2];
	s[1]='\0';
	s[0]=mima[steap]=(i+'0');
  LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		steap=0;
		LcdAreaClean(0,1,16);
		LcdShowStr(0,1,"OK");
	}
}
//�������뺯��
void inputmima(unsigned char i)
{
	static unsigned char steap=0;
	static unsigned char str[7],s[2];
	str[7]='\0';
	s[1]='\0';
	s[0]=str[steap]=(i+'0');
	LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		steap=0;
		LcdAreaClean(0,1,16);
		if(CmpMemory(str, mima))
		{
			ui=1;
		}
		else
		{
			LcdFullClean();
			LcdShowStr(0,0,"Try again!");
		}
	}
}
//��ʱ�亯��
void gaishijian(unsigned char i)
{
	static unsigned char steap=0;
	static unsigned char s[2],str[6];
	if(steap==0)
	{
		LcdAreaClean(0,1,16);
	}
	s[1]='\0';
	s[0]=str[steap]=(i+'0');
  LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		storTIM=1;//ֻ���ڵ�6���������������ܽ��д�������
		TIM[0]=(str[0]-'0')*10+(str[1]-'0');
		TIM[1]=(str[2]-'0')*10+(str[3]-'0');
		TIM[2]=(str[4]-'0')*10+(str[5]-'0');
		steap=0;
		LcdAreaClean(0,1,16);
		LcdShowStr(0,1,"OK");
	}
	else storTIM=0;
}
//�������
void changeTimer(unsigned char unmber)
{
	if(sw>0)
	{
		switch(sw)
		{
			case 0:break;
			case 1:Timer=unmber*10+Timer%10;break;
			case 2:Timer=(Timer/10)*10+unmber;break;
		}
		
	}
}

void keyaction(unsigned char keycode)
{
	if(ui)//������
	{
		switch(keycode)
		{
			case 'u':ab++;yi=1;gaimi=0;gaishi=0;ENLED=1;if(ab==4)ab=0;LcdFullClean();break;
			case 'd':en=0;P13=1;break;
			case 'e':ui=0;ab=0;gaimi=0;gaishi=0;ENLED=1;LcdFullClean();LcdShowStr(0,0,start0);break;//�ػ�
		}
	}
	else//�ػ����ר�ü���
	{
		switch(keycode)
		{
			case 1:inputmima(1);break;
			case 2:inputmima(2);break;
			case 3:inputmima(3);break;
			case 4:inputmima(4);break;
			case 5:inputmima(5);break;
			case 6:inputmima(6);break;
			case 7:inputmima(7);break;
			case 8:inputmima(8);break;
			case 9:inputmima(9);break;
			case 0:inputmima(0);break;
		}
	}
	
	if(gaimi)//������ר�ü���
	{
		switch(keycode)
		{
			case 1:gaimiaction(1);break;
			case 2:gaimiaction(2);break;
			case 3:gaimiaction(3);break;
			case 4:gaimiaction(4);break;
			case 5:gaimiaction(5);break;
			case 6:gaimiaction(6);break;
			case 7:gaimiaction(7);break;
			case 8:gaimiaction(8);break;
			case 9:gaimiaction(9);break;
			case 0:gaimiaction(0);break;
		}
	}
	if(gaishi)//��ʱ�����
	{
		switch(keycode)
		{
			case 'n':if(storTIM)//����ʱ���6��storTIM=1���Դ��浱ǰ��ʱ��
			{
				E2Write(TIM,0x00,3);
				LcdShowStr(0,1,"ok and saved");
			}break;
			
			
			case 1:gaishijian(1);break;
			case 2:gaishijian(2);break;
			case 3:gaishijian(3);break;
			case 4:gaishijian(4);break;
			case 5:gaishijian(5);break;
			case 6:gaishijian(6);break;
			case 7:gaishijian(7);break;
			case 8:gaishijian(8);break;
			case 9:gaishijian(9);break;
			case 0:gaishijian(0);break;
		}
	}
	if(ENLED==0)//��������
	{
		
		switch(keycode)
		{
			case 'l':sw++;if(sw==3)sw=0;break;
			case 'r':run=~run;break;
			case 'n':if(sw==0&&run==0)//ֻ���������ͣ�Ͳ�����ʱ���Դ���
			{
				E2WriteByte(0x03,Timer);//�ѵ�ǰ��ʱ�䴢������
				LcdAreaClean(0,1,16);
				LcdShowStr(0,1,"Saved");
			}break;
			
			case 1:changeTimer(1);break;
			case 2:changeTimer(2);break;
			case 3:changeTimer(3);break;
			case 4:changeTimer(4);break;
			case 5:changeTimer(5);break;
			case 6:changeTimer(6);break;
			case 7:changeTimer(7);break;
			case 8:changeTimer(8);break;
			case 9:changeTimer(9);break;
			case 0:changeTimer(0);break;
		
		}
	}
}


void IRAction()
{
	if(irflag)
	{
		irflag=0;
		
		
		if(ui)//������
		{
			switch(ircode[2])
			{
				case 0x40:ab++;yi=1;gaimi=0;gaishi=0;ENLED=1;if(ab==4)ab=0;LcdFullClean();break;
				case 0x19:en=0;P13=1;break;
				case 0x45:ui=0;ab=0;gaimi=0;gaishi=0;ENLED=1;LcdFullClean();LcdShowStr(0,0,start0);break;//�ػ�
			}
		}
		else//�ػ����ר�ü���
		{
			switch(ircode[2])
			{
				case 0x0c:inputmima(1);break;
				case 0x18:inputmima(2);break;
				case 0x5e:inputmima(3);break;
				case 0x08:inputmima(4);break;
				case 0x1c:inputmima(5);break;
				case 0x5a:inputmima(6);break;
				case 0x42:inputmima(7);break;
				case 0x52:inputmima(8);break;
				case 0x4a:inputmima(9);break;
				case 0x16:inputmima(0);break;
			}
		}
		
		if(gaimi)//������ר�ü���
		{
			switch(ircode[2])
			{
				case 0x0c:gaimiaction(1);break;
				case 0x18:gaimiaction(2);break;
				case 0x5e:gaimiaction(3);break;
				case 0x08:gaimiaction(4);break;
				case 0x1c:gaimiaction(5);break;
				case 0x5a:gaimiaction(6);break;
				case 0x42:gaimiaction(7);break;
				case 0x52:gaimiaction(8);break;
				case 0x4a:gaimiaction(9);break;
				case 0x16:gaimiaction(0);break;
			}
		}
		
		if(gaishi)//��ʱ��ר�ü���
		{
			switch(ircode[2])
			{
				case 0x44:if(storTIM)
				{
					E2Write(TIM,0x00,3);
					LcdShowStr(0,1,"ok and saved");
				}break;
			
				case 0x0c:gaishijian(1);break;
				case 0x18:gaishijian(2);break;
				case 0x5e:gaishijian(3);break;
				case 0x08:gaishijian(4);break;
				case 0x1c:gaishijian(5);break;
				case 0x5a:gaishijian(6);break;
				case 0x42:gaishijian(7);break;
				case 0x52:gaishijian(8);break;
				case 0x4a:gaishijian(9);break;
				case 0x16:gaishijian(0);break;
			}
		}
		
		if(ENLED==0)//��������
		{
			switch(ircode[2])
			{
				case 0x07:sw++;if(sw==3)sw=0;break;
				case 0x09:run=~run;break;
				case 0x44:if(sw==0&&run==0)
				{
					E2WriteByte(0x03,Timer);
					LcdAreaClean(0,1,16);
					LcdShowStr(0,1,"Saved");
				}break;
				
				case 0x0c:changeTimer(1);break;
				case 0x18:changeTimer(2);break;
				case 0x5e:changeTimer(3);break;
				case 0x08:changeTimer(4);break;
				case 0x1c:changeTimer(5);break;
				case 0x5a:changeTimer(6);break;
				case 0x42:changeTimer(7);break;
				case 0x52:changeTimer(8);break;
				case 0x4a:changeTimer(9);break;
				case 0x16:changeTimer(0);break;
			}
		}
		
	}
}