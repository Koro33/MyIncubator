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

// PID控制器参数结构体
struct PID
{
	float Kp;
	float Ki;
	float Kd;
	float MAXOUT;
};

/* sensorFilter */
// 最大温度变化阈值
#define MaxTempChange 45

/* sensorFusion */
// 传感器1测量权重
#define Sensor1_Weight 0.5
// 传感器2测量权重
#define Sensor2_Weight 0.5

/* BB_Control_1 */
// BB控制上升下阈值
#define BB_Heat_Threshold -1.5
// BB控制下降上阈值
#define BB_Cool_Threshold 0

float BB_Control_1(float current_position, float target_position);
float PID_Control_1(float current_position, float target_position);
int8_t sensorFusion(float *SensorTemp_1, float *SensorTemp_2, float *SensorTempProcessed);
int8_t sensorFilter(float *SensorTemp_1, float *SensorTemp_2);

#endif //__ALGORITHM_H

/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
