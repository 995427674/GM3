#include "sys.h"
#include "oled.h"
/*
#define OLED_SCL PORTK_PK2  
#define OLED_SDA	PORTK_PK1
#define OLED_RST PORTK_PK0 
#define OLED_DC  PORTK_PK5 
*/
/*OLED常用的几个函数都在这里面
第一个参数0-20决定在屏幕上 横坐标
第二个参数0-7决定在屏幕上 纵坐标坐标
LCD_write_english_size8string(0,0,"iloveyouyouyouyou12344444");//size8大小的字符串
show_size8float4_6f(0,1,num);	 //size8大小的4位整数 6位小数的数据显示例如显示GPS的经纬度时候会用到
show_size8float4_2f(0,2,3.15555);//size8大小的4位整数 2位小数的数据显示
show_size8float3_1f(0,3,4.15555);	 //size8大小的3位整数 1位小数的数据显示
LCD_write_english_size16string(0,4,"iloveyou");//size16大小的字符串 显示
show_size16float4_2f(0,6,num);	//size16大小的4位整数 2位小数的数据显示
*/
	
#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		  64
#define	Brightness	0xCF 

/*
4线SPI使用说明：
VBT 供内部DC-DC电压，3.3~4.3V，如果使用5V电压，为保险起见串一个100~500欧的电阻  

CS  片选管脚
DC  命令数据选择管脚
RES 模块复位管脚 
D0（SCLK） ，时钟脚，由MCU控制
D1（MOSI） ，主输出从输入数据脚，由MCU控制
*/

#define X_WIDTH 128
#define Y_WIDTH 64
//======================================
const unsigned char F6x8[][6] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
    { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %
    { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08,0x1C,0x2A,0x49,0x08,0x08 },   // <
    { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x08,0x08,0x49,0x2A,0x1C,0x08 },   // >
    { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0xFF, 0x02, 0x04, 0x00 },   // ^
    { 0x00, 0x20, 0x40, 0xFF, 0x40, 0x20 },   // _
    { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g
    { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j
    { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p
    { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y
    { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines
};


//======================================================
// 128X64I液晶底层驱动[8X16]字体库
// 设计者: powerint
// 描  述: [8X16]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
const unsigned char F8X16[]=
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
  0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//!1
  0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//"2
  0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//#3
  0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$4
  0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//%5
  0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//&6
  0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//'7
  0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//(8
  0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//)9
  0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//*10
  0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+11
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//,12
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//-13
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//.14
  0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,///15
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//016
  0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//117
  0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//218
  0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//319
  0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//420
  0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//521
  0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//622
  0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//723
  0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//824
  0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//925
  0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//:26
  0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//;27
  0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//<28
  0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//=29
  0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//>30
  0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//?31
  0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@32
  0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A33
  0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B34
  0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C35
  0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D36
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E37
  0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F38
  0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G39
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H40
  0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I41
  0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J42
  0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K43
  0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L44
  0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M45
  0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N46
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O47
  0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P48
  0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q49
  0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R50
  0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S51
  0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T52
  0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U53
  0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V54
  0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W55
  0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X56
  0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y57
  0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z58
  0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[59
  0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\60
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//]61
  0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^62
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_63
  0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//`64
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a65
  0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b66
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c67
  0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d68
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e69
  0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f70
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g71
  0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h72
  0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i73
  0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j74
  0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k75
  0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l76
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m77
  0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n78
  0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o79
  0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p80
  0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q81
  0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r82
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s83
  0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t84
  0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u85
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v86
  0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w87
  0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x88
  0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y89
  0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z90
  0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{91
  0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//|92
  0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//}93
  0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01//0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~94

};

  void OLED_WrDat(unsigned char data)
{
	unsigned char i=8;

    OLED_DC=1;
	  __NOP();
    OLED_D0=0;
    __NOP();
  while(i--)
  {
    if(data&0x80)
	  {OLED_D1=1;}
    else
	  {OLED_D1=0;}
	  OLED_D0=1;
    
    __NOP();
            
    OLED_D0=0;    
    data<<=1;    
  }
}
void OLED_WrCmd(unsigned char cmd)
{
	unsigned char i=8;
	
   OLED_DC=0;
   OLED_D0=0;

   __NOP();
    
   while(i--)
   {
    if(cmd&0x80)
	  {OLED_D1=1;}
    else
	  {OLED_D1=0;}

    OLED_D0=1;
    __NOP();
            
    OLED_D0=0;   
    cmd<<=1;;   
  } 	
	//LCD_CS=1;
}
void OLED_Set_Pos(unsigned char x, unsigned char y)
{ 
  OLED_WrCmd(0xb0+y);
  OLED_WrCmd(((x&0xf0)>>4)|0x10);
  OLED_WrCmd(x&0x0f);//|0x01);
} 
void OLED_Fill(unsigned char bmp_data) //写数据比如写0x00为清屏 0xff为满频
{
	unsigned char y,x;
	
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_data);
	}
}

void OLED_DLY_ms(unsigned int ms)
{                         
  unsigned int a;
  while(ms)
  {
    a=1335;
    while(a--);
    ms--;
  }
  return;
}
void adjust(unsigned char a)
{
  OLED_WrCmd(a);	//指令数据0x0000~0x003f  
}

void SetStartColumn(unsigned char d)
{
	OLED_WrCmd(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
						//   Default => 0x00
	OLED_WrCmd(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
						//   Default => 0x10
}

void SetAddressingMode(unsigned char d)	
{
	OLED_WrCmd(0x20);			// Set Memory Addressing Mode
	OLED_WrCmd(d);			//   Default => 0x02
						//     0x00 => Horizontal Addressing Mode
						//     0x01 => Vertical Addressing Mode
						//     0x02 => Page Addressing Mode
}

void SetColumnAddress(unsigned char a, unsigned char b)
{
	OLED_WrCmd(0x21);			// Set Column Address
	OLED_WrCmd(a);			//   Default => 0x00 (Column Start Address)
	OLED_WrCmd(b);			//   Default => 0x7F (Column End Address)
}

void SetPageAddress(unsigned char a, unsigned char b)
{
	OLED_WrCmd(0x22);			// Set Page Address
	OLED_WrCmd(a);			//   Default => 0x00 (Page Start Address)
	OLED_WrCmd(b);			//   Default => 0x07 (Page End Address)
}

  void SetStartLine(unsigned char d)
{
	OLED_WrCmd(0x40|d);			// Set Display Start Line
						//   Default => 0x40 (0x00)
}

  void SetContrastControl(unsigned char d)
{
	OLED_WrCmd(0x81);			// Set Contrast Control
	OLED_WrCmd(d);			//   Default => 0x7F
}

  void Set_Charge_Pump(unsigned char d)
{
	OLED_WrCmd(0x8D);			// Set Charge Pump
	OLED_WrCmd(0x10|d);			//   Default => 0x10
						//     0x10 (0x00) => Disable Charge Pump
						//     0x14 (0x04) => Enable Charge Pump
}

  void Set_Segment_Remap(unsigned char d)
{
	OLED_WrCmd(0xA0|d);			// Set Segment Re-Map
						//   Default => 0xA0
						//     0xA0 (0x00) => Column Address 0 Mapped to SEG0
						//     0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

  void Set_Entire_Display(unsigned char d)
{
	OLED_WrCmd(0xA4|d);			// Set Entire Display On / Off
						//   Default => 0xA4
						//     0xA4 (0x00) => Normal Display
						//     0xA5 (0x01) => Entire Display On
}

  void Set_Inverse_Display(unsigned char d)
{
	OLED_WrCmd(0xA6|d);			// Set Inverse Display On/Off
						//   Default => 0xA6
						//     0xA6 (0x00) => Normal Display
						//     0xA7 (0x01) => Inverse Display On
}

  void Set_Multiplex_Ratio(unsigned char d)
{
	OLED_WrCmd(0xA8);			// Set Multiplex Ratio
	OLED_WrCmd(d);			//   Default => 0x3F (1/64 Duty)
}

  void Set_Display_On_Off(unsigned char d)
{
	OLED_WrCmd(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off
						//     0xAF (0x01) => Display On
}

  void SetStartPage(unsigned char d)
{
	OLED_WrCmd(0xB0|d);			// Set Page Start Address for Page Addressing Mode
						//   Default => 0xB0 (0x00)
}

  void Set_Common_Remap(unsigned char d)
{
	OLED_WrCmd(0xC0|d);			// Set COM Output Scan Direction
						//   Default => 0xC0
						//     0xC0 (0x00) => Scan from COM0 to 63
						//     0xC8 (0x08) => Scan from COM63 to 0
}

  void Set_Display_Offset(unsigned char d)
{
	OLED_WrCmd(0xD3);			// Set Display Offset
	OLED_WrCmd(d);			//   Default => 0x00
}

  void Set_Display_Clock(unsigned char d)
{
	OLED_WrCmd(0xD5);			// Set Display Clock Divide Ratio / Oscillator Frequency
	OLED_WrCmd(d);			//   Default => 0x80
						//     D[3:0] => Display Clock Divider
						//     D[7:4] => Oscillator Frequency
}

  void Set_Precharge_Period(unsigned char d)
{
	OLED_WrCmd(0xD9);			// Set Pre-Charge Period
	OLED_WrCmd(d);			//   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 1~15 Display Clocks
						//     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

  void Set_Common_Config(unsigned char d)
{
	OLED_WrCmd(0xDA);			// Set COM Pins Hardware Configuration
	OLED_WrCmd(0x02|d);			//   Default => 0x12 (0x10)
						//     Alternative COM Pin Configuration
						//     Disable COM Left/Right Re-Map
}

  void Set_VCOMH(unsigned char d)
{
	OLED_WrCmd(0xDB);			// Set VCOMH Deselect Level
	OLED_WrCmd(d);			//   Default => 0x20 (0.77*VCC)
}

  void Set_NOP(void)
{
	OLED_WrCmd(0xE3);			// Command for No Operation
}

void LCD_init(void)        
{
	RCC->APB2ENR|=1<<4;    //使能PORTC时钟	   	    	 
 										  
	GPIOC->CRH&=0XFFFFFF00;//PC8 PC9推挽输出 最大50M
	GPIOC->CRH|=0X00000033;
	GPIOC->CRL&=0X00FFFFFF;//PC6 PC7推挽输出 最大50M
	GPIOC->CRL|=0X33000000;
	
  GPIOC->ODR|=0xf<<6;
//	RCC->APB2ENR|=1<<4;
//	GPIOC->CRL&=0XFFFF0000;
//	GPIOC->CRL|=0X00003333;
//	GPIOC->ODR|=0XF;
//	OLED_D0=1;
	//LCD_CS=1;	//预制SLK和SS为高电平   	
	
	OLED_RST=0;
	OLED_DLY_ms(50);
	OLED_RST=1;
	
	//从上电到下面开始初始化要有足够的时间，即等待RC复位完毕
    OLED_WrCmd(0xAE | 0x00);	 // Display Off (0x00)

    OLED_WrCmd(0xD5);
    OLED_WrCmd(0x80);		     // Set Clock as 100 Frames/Sec

    OLED_WrCmd(0xA8);
    OLED_WrCmd(0x3F);	         // 1/64 Duty (0x0F~0x3F)

    OLED_WrCmd(0xD3);
    OLED_WrCmd(0x00);		     // Shift Mapping RAM Counter (0x00~0x3F)

    OLED_WrCmd(0x40 | 0x00);     // Set Mapping RAM Display Start Line (0x00~0x3F)

    OLED_WrCmd(0x8D);
    OLED_WrCmd(0x10 | 0x04);	 // Enable Embedded DC/DC Converter (0x00/0x04)

    OLED_WrCmd(0x20);
    OLED_WrCmd(0x02);		     // Set Page Addressing Mode (0x00/0x01/0x02)

    OLED_WrCmd(0xA0 | 0x01);     // Set SEG/Column Mapping     0x00宸﹀彸鍙嶇疆 0x01姝ｅ父

    OLED_WrCmd(0xC0 | 0x08);     // Set COM/x Scan Direction   0x00涓婁笅鍙嶇疆 0x08姝ｅ父

    OLED_WrCmd(0xDA);
    OLED_WrCmd(0x02 | 0x10);     // Set Sequential Configuration (0x00/0x10)

    OLED_WrCmd(0x81);
    OLED_WrCmd(0xCF);	         // Set SEG Output Current

    OLED_WrCmd(0xD9);
    OLED_WrCmd(0xF1);            // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

    OLED_WrCmd(0xDB);
    OLED_WrCmd(0x40);	         // Set VCOM Deselect Level

    OLED_WrCmd(0xA4 | 0x00);	 // Disable Entire Display On (0x00/0x01)

    OLED_WrCmd(0xA6 | 0x00);	 // Disable Inverse Display On (0x00/0x01)

    OLED_WrCmd(0xAE | 0x01);     // Display On (0x01)

  OLED_Fill(0x00);  //初始清屏
  OLED_Set_Pos(0,0);  	
} 


 
//==============================================================
//函数名：LCD_P6x8Str(unsigned char x,unsigned char y,unsigned char *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================  
  void OLED_P6x8Str(u8 x,u8 y,u8 ch[])//小字符串
{
  u8 c=0,i=0,j=0;
  OLED_Set_Pos(x,y);      
  while (ch[j]!='\0')
  {    
    c =ch[j]-32;
    if(x>126){x=0;y++;}    
  	for(i=0;i<6;i++)     
  	  OLED_WrDat(F6x8[c][i]);  
  	x+=6;
  	j++;
  }
}




  void show_size8float4_2f(unsigned char X,unsigned char Y,float sum)
  {		int num=0;

      num=sum*100;
	  if(num>=0){OLED_write8char(X, Y,' ');}
	  else{OLED_write8char(X, Y,'-');num=0-num;} 
	
  	  OLED_write8char(X+1,Y,num%1000000/100000+48);
  	  OLED_write8char(X+2,Y,num%100000/10000+48);
    OLED_write8char(X+3,Y,num%10000/1000+48);
		  OLED_write8char(X+4,Y,num%1000/100+48);
	  OLED_write8char(X+5,Y,46);
		  OLED_write8char(X+6,Y,num%100/10+48);
		OLED_write8char(X+7,Y,num%10+48);
  
  }
	  void show_size8float3_1f(unsigned char X,unsigned char Y,float sum)
  {		  int num=0;
		 num=sum*10;
	  if(num>=0){OLED_write8char(X, Y,' ');}
	  else{OLED_write8char(X, Y,'-');num=0-num;} 
	
    OLED_write8char(X+1,Y,num%10000/1000+48);
		  OLED_write8char(X+2,Y,num%1000/100+48);
		OLED_write8char(X+3,Y,num%100/10+48);
	  OLED_write8char(X+4,Y,46);
		OLED_write8char(X+5,Y,num%10+48);
  
  }

void LCD_write_english_size8string(unsigned char X,unsigned char Y,u8 *s)
  {
    u8 i=0;	
    while (*s) 
      {
			if(X+i<=20)
	  OLED_write8char(X+i,Y,*s);
			
	      s++;
				i++;
      }
  }
  void show_size8float4_6f(unsigned char X,unsigned char Y,double sum)
  {		long long num=0;

      num=sum*1000000;
	  if(num>=0){OLED_write8char(X, Y,' ');}
	  else{OLED_write8char(X, Y,'-');num=0-num;} 
	
  	  OLED_write8char(X+1,Y,num/1000000000+48);
  	  OLED_write8char(X+2,Y,num%1000000000/100000000+48);
	  OLED_write8char(X+3,Y,num%100000000/10000000+48);
	 OLED_write8char(X+4,Y,num%10000000/1000000+48);
	  OLED_write8char(X+5,Y,46);
	  OLED_write8char(X+6,Y,num%1000000/100000+48);
  	  OLED_write8char(X+7,Y,num%100000/10000+48);
    OLED_write8char(X+8,Y,num%10000/1000+48);
		  OLED_write8char(X+9,Y,num%1000/100+48);
		  OLED_write8char(X+10,Y,num%100/10+48);
		OLED_write8char(X+11,Y,num%10+48);
  }
	void OLED_write8char(u8 x, u8 y,u8 chr)
	{
	u8 i=0; 
  if(x>126){x=0;y++;} 
	OLED_Set_Pos(x*6,y); 
  chr -=32;   
  for(i=0;i<6;i++)     
  	OLED_WrDat(F6x8[chr][i]);  	
	}
	void LCD_clear(void)
  {
 	unsigned char y,x;	
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10); 
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(0);
	}		
  }

	//==============================================================
//函数名： void LCD_PutPixel(unsigned char x,unsigned char y)
//功能描述：绘制一个点（x,y）
//参数：真实坐标值(x,y),x的范围0～127，y的范围0～64
//返回：无
//==============================================================
  void OLED_PutPixel(unsigned char x,unsigned char y)
{
	unsigned char data1;  //data1当前点的数据 
	 
    OLED_Set_Pos(x,y); 
	data1 = 0x01<<(y%8); 	
	OLED_WrCmd(0xb0+(y>>3));
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x00);
	OLED_WrDat(data1); 	 	
}
	
	/////////////////////////8*16函数
//==============================================================
//函数名：LCD_P8x16Str(unsigned char x,unsigned char y,unsigned char *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================  
  void OLED_P8x16Str(u8 x,u8 y,u8 ch[])//大字符串
{
  u8 c=0,i=0,j=0;
        
  while (ch[j]!='\0')
  {    
    c =ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);    
  	for(i=0;i<8;i++)     
  	  OLED_WrDat(F8X16[c*16+i]);
  	OLED_Set_Pos(x,y+1);    
  	for(i=0;i<8;i++)     
  	  OLED_WrDat(F8X16[c*16+i+8]);  
  	x+=8;
  	j++;
  }
}
 

void show_size16float4_2f(unsigned char X,unsigned char Y,float sum)
  {		int num=0;

      num=sum*100;
	  if(num>=0){OLED_write16char(X, Y,' ');}
	  else{OLED_write16char(X, Y,'-');num=0-num;} 
	
  	  OLED_write16char(X+1,Y,num%1000000/100000+48);
  	  OLED_write16char(X+2,Y,num%100000/10000+48);
    OLED_write16char(X+3,Y,num%10000/1000+48);
		  OLED_write16char(X+4,Y,num%1000/100+48);
	  OLED_write16char(X+5,Y,46);
		  OLED_write16char(X+6,Y,num%100/10+48);
		OLED_write16char(X+7,Y,num%10+48);
  
  }
	  void show_size16float3_1f(unsigned char X,unsigned char Y,float sum)
  {		  int num=0;
		 num=sum*10;
	  if(num>=0){OLED_write16char(X, Y,' ');}
	  else{OLED_write16char(X, Y,'-');num=0-num;} 
	
    OLED_write16char(X+1,Y,num%10000/1000+48);
		  OLED_write16char(X+2,Y,num%1000/100+48);
		OLED_write16char(X+3,Y,num%100/10+48);
	  OLED_write16char(X+4,Y,46);
		OLED_write16char(X+5,Y,num%10+48);
  
  }

void LCD_write_english_size16string(unsigned char X,unsigned char Y,u8 *s)
  {
    u8 i=0;	
    while (*s) 
      {
			if(X+i<=20)
	  OLED_write16char(X+i,Y,*s);
			
	      s++;
				i++;
      }
  }
  void show_size16float4_6f(unsigned char X,unsigned char Y,double sum)
  {		int num=0;

      num=sum*1000000;
	  if(num>=0){OLED_write16char(X, Y,' ');}
	  else{OLED_write16char(X, Y,'-');num=0-num;} 
	
  	  OLED_write16char(X+1,Y,num/1000000000+48);
  	  OLED_write16char(X+2,Y,num%1000000000/100000000+48);
	  OLED_write16char(X+3,Y,num%100000000/10000000+48);
	 OLED_write16char(X+4,Y,num%10000000/1000000+48);
	  OLED_write16char(X+5,Y,46);
	  OLED_write16char(X+6,Y,num%1000000/100000+48);
  	  OLED_write16char(X+7,Y,num%100000/10000+48);
    OLED_write16char(X+8,Y,num%10000/1000+48);
		  OLED_write16char(X+9,Y,num%1000/100+48);
		  OLED_write16char(X+10,Y,num%100/10+48);
		OLED_write16char(X+11,Y,num%10+48);
  }


	void OLED_write16char(u8 x, u8 y,u8 chr)
	{ u8 i=0,c=0;
    c =chr-32;
    OLED_Set_Pos(x*8,y);    
  	for(i=0;i<8;i++)     
  	  OLED_WrDat(F8X16[c*16+i]);
  	OLED_Set_Pos(x*8,y+1);    
  	for(i=0;i<8;i++)     
  	  OLED_WrDat(F8X16[c*16+i+8]);  
	}


	
	void printd( double A0,double A1,double A2,double A3,double A4, double A5,double A6,double A7)
{
		  show_size8float4_2f(9,0,A0);  // 在显示屏显示的一些变量的值
			show_size8float4_2f(9,1,A1);
			show_size8float4_2f(9,2,A2);
			show_size8float4_2f(9,3,A3);  // 在显示屏显示的一些变量的值
			show_size8float4_2f(9,4,A4);
			show_size8float4_2f(9,5,A5);
			show_size8float4_2f(9,6,A6);
			show_size8float4_2f(9,7,A7);



}
void prints( u8* A0,u8* A1,u8* A2,u8* A3,u8* A4, u8* A5,u8* A6,u8* A7)
{
		  LCD_write_english_size8string(0,0,A0);  // 在显示屏显示的一些变量的值
			LCD_write_english_size8string(0,1,A1);
			LCD_write_english_size8string(0,2,A2);
			LCD_write_english_size8string(0,3,A3);  // 在显示屏显示的一些变量的值
			LCD_write_english_size8string(0,4,A4);
			LCD_write_english_size8string(0,5,A5);
			LCD_write_english_size8string(0,6,A6);
			LCD_write_english_size8string(0,7,A7);
}
