/**
	******************************************************************************
	* @file    TIM4.h
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

#ifndef __PWM4_H
#define __PWM4_H

#include "stm32f10x.h"

static void TIM4_Config(void);
void TIM4_IRQHandler_ex(void);

void TIM4_Init(void);

#endif /* __PWM4_OUTPUT_H */

/******************* (C) COPYRIGHT 2016  *****END OF FILE************/
