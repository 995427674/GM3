#include "UART5.h"
#include "sys.h"  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "delay.h"
#include "oled.h"
//#include "usart_debug.h"

//串口三gps数据接收配置文件
//串口发送缓存区 	

__align(8) u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

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
//	RCC->APB2ENR|=1<<11;	//TIM1时钟使能    
// 	TIM1->ARR=arr;  	//设定计数器自动重装值 
//	TIM1->PSC=psc;  	//预分频器	  
//	TIM1->DIER|=1<<0;   //允许更新中断	  
//	TIM1->CR1|=0x01;    //使能定时器1
//  	MY_NVIC_Init(1,3,TIM1_UP_IRQChannel ,2);	//抢占1，子优先级3，组2									 
//}
//void TIM1_UP_IRQHandler(void)
//{ 	  		    
//	if(TIM1->SR&0X01)//是更新中断
//	{	 			   
//		UART5_RX_STA|=1<<15;	//标记接收完成
//		TIM1->SR&=~(1<<0);		//清除中断标志位		   
//		TIM1_Set(0);		//关闭定时器7	  
//	}	      											 
//}



//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
//USART5_TX->PC12—>53//1011
//USART5_RX->PD2->54//0100
void UART5_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;//小数部分
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV,OVER8设置为0
	mantissa=temp;				 	//得到整数部分
	fraction=(temp-mantissa)*16; 	//得到小数部分,OVER8设置为0	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<4;   //使能PORTC口时钟
		
	RCC->APB1ENR|=1<<20;  //使能串口时钟 
	GPIOC->CRH&=0XFFF0FFFF; 
	GPIOC->CRH|=0X000B0000;//IO状态设置
	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000400;
	RCC->APB1RSTR|=1<<20;   //复位串口1
	RCC->APB1RSTR&=~(1<<20);//停止复位	   
	
 	UART5->BRR=mantissa;//(pclk1*1000000)/(bound);// 波特率设置	 
	UART5->CR1|=0X200C;  	//1位停止,无校验位.	
	
	UART5->CR3=1<<7;   	//使能串口2的DMA发送
	UART5->CR3=1<<6;
	UART5->CR1|=1<<8;    	//PE中断使能
	UART5->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	

  MY_NVIC_Init(1,1,UART5_IRQChannel,2);//组2，优先级0,0,最高优先级 
//	TIM1_Int_Init(10000-1,7199);	//1s中断一次
//	TIM1_Set(0);
//	UART5_RX_STA=0;				//清零 
	GPRS_ConSta_Ind_Init();
}
//有网络连接的时输出高电平，网络连接断开的时候输出低电平
void GPRS_ConSta_Ind_Init(void)//PC0
{
	RCC->APB2ENR|=1<<4;
	GPIOC->CRL&=0XFFFFFFF0;
	GPIOC->CRL|=0X00000008;//下拉输入
	GPIOC->ODR|=0<<0;
	Ex_NVIC_Config(GPIO_C,0,RTIR);	//上升沿触发
	Ex_NVIC_Config(GPIO_C,0,FTIR); 	//下降沿触发
	MY_NVIC_Init(2,3,EXTI0_IRQChannel,2);	//抢占2，子优先级3，组2
}

//外部中断0服务程序
u8 Flag_GPRS_Sta;
void EXTI0_IRQHandler(void)
{
	delay_ms(5);	//消抖
//	if(GPRS_Sta==1)	
//		flag_GPRS_Sta=1;
//	else
//		flag_GPRS_Sta=0;
//	GPRS_Sta==1?flag_GPRS_Sta=1:GPRS_Sta==0?
	Flag_GPRS_Sta=GPRS_Sta;
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
}

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度

//串口3,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u5_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)UART5_TX_BUF);//此次发送数据的长度
	
	for(j=0;j<i;j++)//循环发送数据
	{
		while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
		UART5->DR=UART5_TX_BUF[j];  
	}
}
//以0X0D,0X0A结尾
void UART5_IRQHandler(void)
{
	u8 res;
//	LCD_write_english_size8string(0,6,"OK!");
	if(UART5->SR&(1<<5))
	{
		res=UART5->DR;
		if((UART5_RX_STA&0X800)==0)//接收未完成
		{
			if(UART5_RX_STA&0X4000)//收到0x0d
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
