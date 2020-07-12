#ifndef _lcd1602_h_
#define _lcd1602_h_

#include <stc12c5a60s2.h>

#define LCD1602_DB P0

sbit LCD1602_RS = P2^0;
sbit LCD1602_RW = P2^1;
sbit LCD1602_E = P1^2;

void InitLcd1602();
void LcdFullClean();
void LcdAreaClean(unsigned char x, unsigned char y, unsigned char len);
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

#endif