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
//	Stm32_Clock_Init(9);           //ϵͳʱ������
//	LCD_init();                    //OLED��ʼ��
//	delay_init(72);	  	           //��ʱ��ʼ��
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
	Stm32_Clock_Init(9);           //ϵͳʱ������
	LCD_init();                    //OLED��ʼ��
	delay_init(72);	  	           //��ʱ��ʼ��
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
		//�������еİ�׿�������������
	}
}

//��ǰʵ�����������Ҫ��ζϿ�����USR-CLOUD-TEST���ܷ����з�Ӧ����һ�η���start����GET��������ֵ���ڶ��η���start�з���ֵ����OLED����ʾ��bufֵ��startGM3...
