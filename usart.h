/**
	******************************************************************************
	* @file    usart.h
	* @author  ycg1024
	* @version V1.0.0
	* @date    26-Oct-2016
	* @brief
	*
	*
	******************************************************************************
	* @attention
	*
	******************************************************************************
	*/

#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

// usart1队列中断缓冲大小
#define U1_IT_MaxBufferSize 128
// usart2发送缓冲
#define U2_CMD_BUFFER_LEN 128
// usart1中断队列结构体
struct que
{
	uint8_t *head;
	uint8_t *tail;
};

void USART_Config(void);
void usart2Printf(char *fmt, ...);
int usart1_avilable(void);
uint8_t usart1_read(void);
uint8_t usart1_peek(void);
void USART1_IRQHandler_ex(void);
int usart1_RevTask(void);
int usart1_TmtTask(void);
int usart1_InitTmtTask(void);
int strcatchar(char *front, char rear);
int strclr(char *str);
int usart1_sendHMIEndCmd(void);
float Strnum2Num(char *Strnum, uint8_t StartNum, uint8_t EndNum);

#endif /* __USART_H */


/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
