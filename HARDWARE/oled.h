#ifndef _OLED_H
#define _OLED_H

#include "sys.h" 

//#define OLED_D0   PCout(0)// PBout(12)
//#define OLED_D1    PCout(1)//PBout(13)
//#define OLED_RST   PCout(2)//PBout(14)
//#define OLED_DC    PCout(3)//PBout(15)

#define OLED_D0    PCout(9)
#define OLED_D1    PCout(8)
#define OLED_RST   PCout(7)
#define OLED_DC    PCout(6)

#define __NOP __nop
  extern unsigned char longqiu96x64[768];
   void setgpio(void);
   void OLED_CLS(void);
  void OLED_Print(unsigned char x, unsigned char y, unsigned char ch[]);
  void OLED_PutPixel(unsigned char x,unsigned char y);
 void OLED_Fill(unsigned char dat);
	void LCD_init(void);
		 void show_size16float4_2f(unsigned char X,unsigned char Y,float sum) ;
	 void show_size16float3_1f(unsigned char X,unsigned char Y,float sum);
	void LCD_write_english_size16string(unsigned char X,unsigned char Y,u8 *s);
	  void show_size16float4_6f(unsigned char X,unsigned char Y,double sum);
	void OLED_write16char(u8 x, u8 y,u8 chr);
	
	
	 void show_size8float4_2f(unsigned char X,unsigned char Y,float sum) ;
	 void show_size8float3_1f(unsigned char X,unsigned char Y,float sum);
	void LCD_write_english_size8string(unsigned char X,unsigned char Y,u8 *s);
	  void show_size8float4_6f(unsigned char X,unsigned char Y,double sum);
	void OLED_write8char(u8 x, u8 y,u8 chr);
	void LCD_clear(void);
	
void printd( double A0,double A1,double A2,double A3,double A4, double A5,double A6,double A7);
void prints( u8* A0,u8* A1,u8* A2,u8* A3,u8* A4, u8* A5,u8* A6,u8* A7);














#endif
