/**
	******************************************************************************
	* @file    extra_gpio.h
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

#ifndef __EXTRAGPIO_H
#define __EXTRAGPIO_H

#include "stm32f10x.h"

#define PORT_BEEP GPIOB
#define PIN_BEEP GPIO_Pin_10
#define PIN_SWITCH2 GPIO_Pin_13

void extraGPIOInit(void);
void beep(uint8_t Num, uint8_t IntervalNum);

#endif /* __EXTRAGPIO_H */

/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
