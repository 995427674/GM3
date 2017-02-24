#ifndef __UART5_H
#define __UART5_H 
#include "sys.h"
#include "stdio.h"	  
	

#define UART5_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define UART5_MAX_SEND_LEN		800					//����ͻ����ֽ���
#define UART5_RX_EN 			1					//0,������;1,����.

#define GPRS_Sta PCin(0)//pc0

extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern vu16 UART5_RX_STA;   						//��������״̬
extern u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 
void UART5_init(u32 pclk1,u32 bound);
void u5_printf(char* fmt,...);
void GPRS_ConSta_Ind_Init(void);


#endif	   
