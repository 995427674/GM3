#include "sys.h"
#include "G3MM.h"
#include "UART5.h"
#include "delay.h"
#include "string.h"
#include "oled.h"
#include "malloc.h"
#include "stdio.h"
/*
	

*/
const char *GM3_MESSAGE[]={"00005678000000000001",//�豸���
										 "12345678"//ͨѶ����
										};

const char *GM3_Mode[]={"CMD",//ATָ��ģʽ
												"SMS",//����͸��ģʽ
												"NET",//����͸��ģʽ
												"HTTPD"//HTTPDģʽ
											 };
typedef enum{
	CMD=0,
	SMS=1,
	NET=2,
	HTTPD=3
}Mode;
const char *CM3_Protocol[2]={"TCP","UDP"};
const char *GM3_Ser_Adr[2]={"test.usr.cn","cloud.usr.cn"};//��������ַ
const char *GM3_Ser_Port[2]={"2317","15000"};//�������˿ں�

//��ʱ���ã����Ի�δ���Ƴ���

void GM3_Set_MODE(u8 mode)
{
	u8 *pat;
	pat=mymalloc(50);
	sprintf((char*)pat,"AT+WKMOD=\"%s\"",GM3_Mode[mode]);
	GM3_Send_Cmd(pat,"OK",300);
	GM3_Send_Cmd("AT+SOCKAEN=\"on\"","OK",300);
	GM3_Send_Cmd("AT+SOCKA=\"TCP\",\"test.usr.cn\",2317","OK",300);//��һ��AT+S
	myfree(pat);
}
//GM3��ʼ����ʱ�ϳ�	
void GM3_Init(void)
{	
	u8 dis=1;
	u8 *pat;
	u8 count_t=0;
	//u8 *p_temp;
	u8 aa=1,bb=1,cc=1;//,dd;
	//����Ƿ����GPRS
	LCD_write_english_size8string(0,0,"login ...");
	while(dis)	dis=GM3_SW_Cmd_Mode();
	GM3_Wait_CREG();
	
	//����͸����//˵�����е�ATָ�����ȫ��ȷ���Ƿ���Ҫ��""����ֵ��ע��
	pat=mymalloc(50);
	while((aa+bb+cc)!=0)
	{
		aa=GM3_Send_Cmd("AT+CLOUDEN=\"on\"","OK",300);//ʹ��͸���ƹ���
		
		sprintf((char*)pat,"AT+CLOUDID=\"%s\"",GM3_MESSAGE[0]);
		bb=GM3_Send_Cmd(pat,"OK",300);//����20λ�豸ID
		//memset(pat,'\0',50*sizeof(u8));
		
		sprintf((char*)pat,"AT+CLOUDPA=\"%s\"",GM3_MESSAGE[1]);
		//LCD_write_english_size8string(0,3,pat);
		cc=GM3_Send_Cmd(pat,"OK",300);//����8λͨѶ����
		
	//	dd=GM3_Send_Cmd("AT+CLOUDPA","PA:",300);
	//	p_temp=GM3_Check_Cmd("PA:");
	//	LCD_write_english_size8string(0,2,p_temp);
	}
	
	sprintf((char*)pat,"AT+WKMOD=\"%s\"",GM3_Mode[2]);
	GM3_Send_Cmd(pat,"OK",300);
	
	GM3_Send_Cmd("AT+SOCKAEN=\"on\"","OK",300);
//	const char *CM3_Protocol[2]={"TCP","UDP"};
//const char *GM3_Ser_Adr[2]={"test.usr.cn","cloud.usr.cn"};//��������ַ
//const char *GM3_Ser_Port[2]={"2317","15000"};//�������˿ں�
	sprintf((char*)pat,"AT+SOCKA=\"%s\",\"%s\",%s",CM3_Protocol[0],GM3_Ser_Adr[1],GM3_Ser_Port[1]);
	GM3_Send_Cmd(pat,"OK",300);//��һ��AT+S
	GM3_Send_Cmd("AT+SOCKASL=\"long\"","OK",300);
	//AT+UART=115200,"NONE",8,1,"RS485"
	GM3_Send_Cmd("AT+UART=115200,\"NONE\",8,1,\"RS485\"","OK",300);
	//AT+APN="CMNET","",""�ƶ���Ӫ��
	GM3_Send_Cmd("AT+APN=\"CMNET\",\"\",\"\"","OK",300);
	//AT+E="on"����
	GM3_Send_Cmd("AT+E=\"ON\"","OK",300);
	//AT+CMDPW="usr.cn"��������
	GM3_Send_Cmd("AT+CMDPW=\"\"","OK",300);
	//AT+STMSG="USR-GM3 V2.1"
	GM3_Send_Cmd("AT+STMSG=\"USR-GM3\"","OK",300);
	myfree(pat);
//	show_size8float4_2f(0,4,4);
//	show_size8float4_2f(0,5,bb);
//	show_size8float4_2f(0,6,cc);
	
	//LCD_write_english_size8string(0,0,"GM3 IS OK!");	
	//dd=
	GM3_Send_Cmd("AT+S","OK",300);
	
//	show_size8float4_2f(0,7,dd);
	delay_ms(5000);
	delay_ms(5000);//���룺?8///ʵ�����Ƿ���ע����Ľ��հ���ֵ��E3 8E 38 00 01 06 F9
	Clear_buffer2();
	LCD_write_english_size8string(0,0,"GM3 IS OK!");	
	//���������
	while(!GPRS_Sta);
	for(count_t=0;count_t<20;count_t++)
		delay_ms(500);
	while(!GPRS_Sta);
//	{
//		delay_ms(500);
//		count_t++;
//		if(count_t++>200)//100s
//		{
//			count_t=0;
//			goto save;
//		}
//	}
	
//	save:	LCD_write_english_size8string(0,1,"GPRS IS NOT OK!");
}
void GM3_Wait_CREG(void)
{
	u8 *p;
	while(1)
	{
		GM3_Send_Cmd("AT+CREG?","+CREG:",300);
		p=GM3_Check_Cmd("+CREG:");
		if((*(p+9)=='1')||(*(p+9)=='5'))
			break;
	}
}
u8* GM3_Check_Cmd(u8 *str)
{
	char *strx=0;
	strx=strstr((const char*)UART5_RX_BUF,(const char*)str);
	return (u8*)strx;
}
//����ATָ��ɹ����� 0
u8 GM3_Send_Cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;
	Clear_buffer2();
	u5_printf("%s\r",cmd);
	if(ack&&waittime)
	{
		while(--waittime)
		{
			delay_ms(10);
			if(GM3_Check_Cmd(ack))break;
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//		res=UART5->DR;
//		UART5_RX_BUF[UART5_RX_STA++]=res;	
//		if(UART5_RX_STA>UART5_MAX_RECV_LEN)
//			UART5_RX_STA=0;

//����������ݻ���
void Clear_buffer2(void)
{
	memset(UART5_RX_BUF,0,UART5_MAX_RECV_LEN*sizeof(char));
	UART5_RX_STA=0;
}

//�һ����е��ַ������ⳡ����
u8 GM3_Find(char *a)
{ 
  if(strstr((const char *)UART5_RX_BUF,a)!=NULL)
	    return 1;
	else
			return 0;
}
//+++ 	a	����ATָ��ģʽע��ʱ��Ҫ��
u8 GM3_SW_Cmd_Mode(void)
{
	u16 waittimesw=60;//�ȴ�ʱ��3s����
	Clear_buffer2();
	while((UART5->SR&0X40)==0);
	delay_ms(250);//t1>200ms
	UART5->DR='+';      
	delay_ms(15);		//������֡ʱ���Ƕ��٣�
	//t2<50ms
	while((UART5->SR&0X40)==0);
	UART5->DR='+';      
	delay_ms(15);		//������֡ʱ���Ƕ��٣�
	//t3<50ms
	while((UART5->SR&0X40)==0);
	UART5->DR='+';      
	delay_ms(15);		//������֡ʱ���Ƕ��٣�
	while(--waittimesw)
	{
		delay_ms(100);//
		if(GM3_Find("a"))
			break;
		else if((!GM3_Find("a"))&&(waittimesw<5))
			return 2;
	}
	Clear_buffer2();
	while((UART5->SR&0X40)==0);
	UART5->DR='a';      
	delay_ms(150);		//������֡ʱ���Ƕ��٣�
	Clear_buffer2();
	return GM3_Send_Cmd("AT","OK",300);
}
gprs_msg GPRSS;
//��������
void GPRS_data_analyze(gprs_msg*gprs,u8 *buf)
{
	LCD_write_english_size8string(0,3,"get");
	if(strstr((const char *)buf,"start"))
	{
		gprs->status=1;
		u5_printf("right !!!\r\n");
	}
}

//u8 GM3_SW_Cmd_Mode(void)
//{
//	u16 waittimesw=30;
//	while((UART5->SR&0X40)==0);
//	delay_ms(200);//t1>200ms
//	UART5->DR='+';      
//	delay_ms(15);		//������֡ʱ���Ƕ��٣�
//	//t2<50ms
//	while((UART5->SR&0X40)==0);
//	UART5->DR='+';      
//	delay_ms(15);		//������֡ʱ���Ƕ��٣�
//	//t3<50ms
//	while((UART5->SR&0X40)==0);
//	UART5->DR='+';      
//	delay_ms(15);		//������֡ʱ���Ƕ��٣�
//	while(--waittimesw)
//	{
//		delay_ms(100);
//		if(GM3_Find("a"))
//			break;
//		else if((!GM3_Find("a"))&&(waittimesw<5))
//			return 1;
//	}
//	Clear_buffer2();
//	while((UART5->SR&0X40)==0);
//	UART5->DR='a';      
//	delay_ms(15);		//������֡ʱ���Ƕ��٣�
//	waittimesw=30;
//	while(--waittimesw)
//	{
//		delay_ms(100);
//		if(GM3_Find("+ok"))
//			break;
//		else if((!GM3_Find("+ok"))&&(waittimesw<5))
//			return 1;
//	}
//	Clear_buffer2();
//	return 2;
//}

////���ö����ı�ģʽ;��������δ�����ź�����͸�����ж�
////������������ʹ�õ�����
////δ����ʱ�޷�ֱ�Ӳ�ѯID�����Ź�����ʱ����
//void GM3_Set_Text_Mode(void)
//{
//	
//	
//}




