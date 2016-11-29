/**
  ******************************************************************************
  * @file    Algorithm.h
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

#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include "stm32f10x.h"

// PID�����������ṹ��
struct PID
{
	float Kp;
	float Ki;
	float Kd;
	float MAXOUT;
};

//����¶ȱ仯��ֵ
#define MaxTempChange 10
//������1����Ȩ��
#define Sensor1_Weight 0.5
//������2����Ȩ��
#define Sensor2_Weight 0.5

float PID_Control_1(float current_position, float target_position);
int8_t sensorFusion(float *SensorTemp_1, float *SensorTemp_2, float *SensorTempProcessed);
int8_t sensorFilter(float *SensorTemp_1, float *SensorTemp_2);

#endif //__ALGORITHM_H

/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
