/**
	******************************************************************************
	* @file    main.h
	* @author  ycg1024
	* @version V1.0.0
	* @date    26-Oct-2016
	* @brief 
	* 			PA.02: (USART2_TX) DEBUG
	* 			PA.03: (USART2_RX) DEBUG
	* 			PA.09: (USART1_TX) HMI LCD
	* 			PA.10: (USART1_RX) HMI LCD
	* 			PA.06: (TIM3_CH1) 加热片1
	*				PA.07: (TIM3_CH2) 加热片1
	*				PB.00: (TIM3_CH3) 风扇1、2
	*				PB.01: (TIM3_CH4) NC
	*
	******************************************************************************
	* @attention
	*
	******************************************************************************
	*/

#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "systick_delay.h"
#include "usart.h"
#include "TIM3_pwm_output.h"
#include "TIM4.h"
#include "extra_gpio.h"
#include "math.h"
#include "string.h"
#include "ds18b20.h"
#include "Algorithm.h"
// #include "arm_math.h"

int incubatorInit(void);

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
