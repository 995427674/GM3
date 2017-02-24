#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "UART5.h"
#include "G3MM.h"
//extern gprs_msg GPRSS;
//int main(void)
//{
//	u8 dis=1;
////	u8 disflag=0;
//	Stm32_Clock_Init(9);           //系统时钟设置
//	LCD_init();                    //OLED初始化
//	delay_init(72);	  	           //延时初始化
//	UART5_init(36,115200); 
//	LCD_write_english_size8string(0,0,"login ...");
//	
//	//dis=GM3_SW_Cmd_Mode();
//	while(dis) 
////	{
//		dis=GM3_SW_Cmd_Mode();
////		disflag++;
////		show_size8float4_2f(0,0,dis);	
////		show_size8float4_2f(1,1,disflag);	
////	}
////	show_size8float4_2f(0,0,dis);	
//	GM3_Wait_CREG();
//	LCD_write_english_size8string(0,0,"GM3 IS OK!");
////	show_size8float4_2f(6,6,6);
//	
//	while(1)
//	{
//	}
//}

int main(void)
{
//	u8 dis=1;
	//u16 rxlen;
	Stm32_Clock_Init(9);           //系统时钟设置
	LCD_init();                    //OLED初始化
	delay_init(72);	  	           //延时初始化
	UART5_init(36,115200); 
	
//	LCD_write_english_size8string(0,0,"login ...");
//	while(dis)	dis=GM3_SW_Cmd_Mode();
//	GM3_Wait_CREG();
//	LCD_write_english_size8string(0,0,"GM3 IS OK!");
  GM3_Init();
	while(1)
	{	
		LCD_write_english_size8string(0,1,"GPRS IS OK!");	
		while(GPRS_Sta)
		{
		//	u5_printf("right1234568\r\n");
//			
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
//			delay_ms(500);
			
			LCD_write_english_size8string(0,5,"aaa");
			LCD_write_english_size8string(0,6,UART5_RX_BUF);	
			if(UART5_RX_STA&0X8000)
			{
				//LCD_write_english_size8string(0,6,UART5_RX_BUF);	
			//	rxlen=UART5_RX_STA&0X7FFF;	
				UART5_RX_STA=0;		
				GPRS_data_analyze(&GPRSS,UART5_RX_BUF);
			}
		}

		//show_size8float4_6f(0,5,GPRS_Sta);
		//由于现有的安卓测试软软件发送
	}
}

//当前实验程序现象是要多次断开连接USR-CLOUD-TEST才能发送有反应，第一次发送start出现GET但不反回值，第二次发送start有返回值但是OLED上显示的buf值是startGM3...
