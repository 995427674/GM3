#include "UART5.h"
#include "sys.h"  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "delay.h"
#include "oled.h"
//#include "usart_debug.h"

//������gps���ݽ��������ļ�
//���ڷ��ͻ����� 	

__align(8) u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.

vu16 UART5_RX_STA=0;   

//void TIM1_Set(u8 stau)
//{
//	if(stau)
//	{
//		TIM1->CNT=0;
//		TIM1->CR1|=1<<0;
//	}else TIM1->CR1&=~(1<<0);
//}
//void TIM1_Int_Init(u16 arr,u16 psc)
//{
//	RCC->APB2ENR|=1<<11;	//TIM1ʱ��ʹ��    
// 	TIM1->ARR=arr;  	//�趨�������Զ���װֵ 
//	TIM1->PSC=psc;  	//Ԥ��Ƶ��	  
//	TIM1->DIER|=1<<0;   //��������ж�	  
//	TIM1->CR1|=0x01;    //ʹ�ܶ�ʱ��1
//  	MY_NVIC_Init(1,3,TIM1_UP_IRQChannel ,2);	//��ռ1�������ȼ�3����2									 
//}
//void TIM1_UP_IRQHandler(void)
//{ 	  		    
//	if(TIM1->SR&0X01)//�Ǹ����ж�
//	{	 			   
//		UART5_RX_STA|=1<<15;	//��ǽ������
//		TIM1->SR&=~(1<<0);		//����жϱ�־λ		   
//		TIM1_Set(0);		//�رն�ʱ��7	  
//	}	      											 
//}



//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
//USART5_TX->PC12��>53//1011
//USART5_RX->PD2->54//0100
void UART5_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;//С������
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV,OVER8����Ϊ0
	mantissa=temp;				 	//�õ���������
	fraction=(temp-mantissa)*16; 	//�õ�С������,OVER8����Ϊ0	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<4;   //ʹ��PORTC��ʱ��
		
	RCC->APB1ENR|=1<<20;  //ʹ�ܴ���ʱ�� 
	GPIOC->CRH&=0XFFF0FFFF; 
	GPIOC->CRH|=0X000B0000;//IO״̬����
	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000400;
	RCC->APB1RSTR|=1<<20;   //��λ����1
	RCC->APB1RSTR&=~(1<<20);//ֹͣ��λ	   
	
 	UART5->BRR=mantissa;//(pclk1*1000000)/(bound);// ����������	 
	UART5->CR1|=0X200C;  	//1λֹͣ,��У��λ.	
	
	UART5->CR3=1<<7;   	//ʹ�ܴ���2��DMA����
	UART5->CR3=1<<6;
	UART5->CR1|=1<<8;    	//PE�ж�ʹ��
	UART5->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	

  MY_NVIC_Init(1,1,UART5_IRQChannel,2);//��2�����ȼ�0,0,������ȼ� 
//	TIM1_Int_Init(10000-1,7199);	//1s�ж�һ��
//	TIM1_Set(0);
//	UART5_RX_STA=0;				//���� 
	GPRS_ConSta_Ind_Init();
}
//���������ӵ�ʱ����ߵ�ƽ���������ӶϿ���ʱ������͵�ƽ
void GPRS_ConSta_Ind_Init(void)//PC0
{
	RCC->APB2ENR|=1<<4;
	GPIOC->CRL&=0XFFFFFFF0;
	GPIOC->CRL|=0X00000008;//��������
	GPIOC->ODR|=0<<0;
	Ex_NVIC_Config(GPIO_C,0,RTIR);	//�����ش���
	Ex_NVIC_Config(GPIO_C,0,FTIR); 	//�½��ش���
	MY_NVIC_Init(2,3,EXTI0_IRQChannel,2);	//��ռ2�������ȼ�3����2
}

//�ⲿ�ж�0�������
u8 Flag_GPRS_Sta;
void EXTI0_IRQHandler(void)
{
	delay_ms(5);	//����
//	if(GPRS_Sta==1)	
//		flag_GPRS_Sta=1;
//	else
//		flag_GPRS_Sta=0;
//	GPRS_Sta==1?flag_GPRS_Sta=1:GPRS_Sta==0?
	Flag_GPRS_Sta=GPRS_Sta;
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ  
}

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���

//����3,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u5_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)UART5_TX_BUF);//�˴η������ݵĳ���
	
	for(j=0;j<i;j++)//ѭ����������
	{
		while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
		UART5->DR=UART5_TX_BUF[j];  
	}
}
//��0X0D,0X0A��β
void UART5_IRQHandler(void)
{
	u8 res;
//	LCD_write_english_size8string(0,6,"OK!");
	if(UART5->SR&(1<<5))
	{
		res=UART5->DR;
		if((UART5_RX_STA&0X800)==0)//����δ���
		{
			if(UART5_RX_STA&0X4000)//�յ�0x0d
			{
				if(res!=0X0A)	UART5_RX_STA=0;
				else UART5_RX_STA|=0X8000;
			}
			else
			{
				if(res==0X0D) UART5_RX_STA|=0X4000;
				else
				{
					UART5_RX_BUF[UART5_RX_STA&0X3FFF]=res;
					UART5_RX_STA++;
					if(UART5_RX_STA>(UART5_MAX_RECV_LEN-1))UART5_RX_STA=0;
				}
			}
		}
//		UART5_RX_BUF[UART5_RX_STA++]=res;	
//		if(UART5_RX_STA>UART5_MAX_RECV_LEN)
//			UART5_RX_STA=0;
	}
}
