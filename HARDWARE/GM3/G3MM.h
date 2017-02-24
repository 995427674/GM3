#ifndef __G3MM_H
#define __G3MM_H 
#include "sys.h"

typedef struct{
	u8 status;
	u8 a;
}gprs_msg;
extern gprs_msg GPRSS;
void Clear_buffer2(void);
u8 GM3_Find(char *a);
u8 GM3_SW_Cmd_Mode(void);
u8 GM3_Send_Cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* GM3_Check_Cmd(u8 *str);
void GM3_Wait_CREG(void);
void GM3_Init(void);
void GM3_Set_MODE(u8 mode);
void GPRS_data_analyze(gprs_msg*gprs,u8 *buf);
#endif	   
