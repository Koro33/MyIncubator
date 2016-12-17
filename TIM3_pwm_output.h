/**
	******************************************************************************
	* @file    TIM3_pwm_output.h
	* @author  ycg1024
	* @version V1.0.0
	* @date    26-Oct-2016
	* @brief
	*						PA.06: (TIM3_CH1) 加热片1
	*						PA.07: (TIM3_CH2) 加热片2
	*						PB.00: (TIM3_CH3) 风扇1、2
	*						PB.01: (TIM3_CH4) NC
	*
	*
	*
	******************************************************************************
	* @attention
	*
	******************************************************************************
	*/
#ifndef __PWM3_OUTPUT_H
#define __PWM3_OUTPUT_H

#include "stm32f10x.h"

#define SERVO_ON 1
#define SERVO_OFF 0
#define SERVO_ON_DEG 440
#define SERVO_OFF_DEG 590

void TIM3_PWM_Init(void);
void TIM3_GPIO_Config(void);
void TIM3_Mode_Config(void);
void TIM3_CH1_Mode_Config(uint16_t CCR1_Val);
void TIM3_CH2_Mode_Config(uint16_t CCR2_Val);
void TIM3_CH3_Mode_Config(uint16_t CCR3_Val);
void TIM3_CH4_Mode_Config(uint16_t CCR4_Val);
void TIM3_direction_gpio_config(void);

int setPower_Heat1(uint16_t power);
int setPower_Heat2(uint16_t power);
int setPower_Heat1_Heat2(uint16_t power1, uint16_t power2);
int setPower_Fan(uint16_t power);

#endif /* __PWM3_OUTPUT_H */

/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
