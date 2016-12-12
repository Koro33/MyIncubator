#include "Algorithm.h"
#include "main.h"

// 采样缓冲区，以及滤波后数据存储区
extern float SensorTemp_1[3];
extern float SensorTemp_2[3];
// 融合数据存储区
extern float SensorTempProcessed[3];
// PID控制器参数
extern struct PID PID_1;
// 加热器功率
extern uint16_t HeatPower;
/**
	* @brief  增量型PID算法
	*         PID = PID->UK_REAL + PID->KP*[E(k)-E(k-1)]+PID->KI*E(k)+PID->KD*[E(k)-2E(k-1)+E(k-2)]
	* @param  current_position
	* @param  target_position
	* @return output
	* @attention 做了改进，当温度小于阈值的时候不使用PID控制
	*/
float PID_Control_1(float current_position, float target_position)
{
  static float error_l[2] = {0.0, 0.0};
  static float output = 0;
  static float inte = 0;
	
	if((current_position - target_position) > 5)
	{
		return 0;
	}
	if((current_position - target_position) < -5)
	{
		return PID_1.MAXOUT;
	}

  error_l[0] = error_l[1];
  error_l[1] = target_position - current_position;
  inte += error_l[1];

  output = error_l[1] * PID_1.Kp + inte * PID_1.Ki + (error_l[1] - error_l[0]) * PID_1.Kd;

  if (output > PID_1.MAXOUT)
  {
    output = PID_1.MAXOUT;
  }

  if (output < -PID_1.MAXOUT)
  {
    output = -PID_1.MAXOUT;
  }

  return (output + PID_1.MAXOUT) / 2;
}

/**
	* @brief  BB控制算法
	* @param  current_position
	* @param  target_position
	* @return output
	* @attention 无
	*/
float BB_Control_1(float current_position, float target_position)
{
	static int8_t DirFlag = 0; // 经过低温为0 经过高温为1 默认0
	float HeatTempThreshold = target_position + BB_Heat_Threshold; // 25
	float CoolTempThreshold = target_position + BB_Cool_Threshold; // 35
	if(current_position <= HeatTempThreshold)
	{
		DirFlag = 0; //经过低温
		return 999;
	}
	if(current_position >= CoolTempThreshold)
	{
		DirFlag = 1; // 经过高温
		return 0;
	}
	if((current_position < CoolTempThreshold)&&(current_position > HeatTempThreshold))
	{
		if(DirFlag == 0)
		{
			return 0;
		}
		if(DirFlag == 1)
		{
			return 999;
		}
	}
	
	return 0;
}

/**
	* @brief  传感器融合算法（加权平均）
	* 				调用完滤波算法后进行调用，融合完成后存储在融合数据存储区数组中的第一个位置，最后一个数据被清除，剩余数据依次后移
	* @param[1]  SensorTemp_1 
	* @param[2]  SensorTemp_2
 	* @param[3]  SensorTempProcessed
	* @return 是否融合成功 1	成功
												0	 不成功
	* @attention 
	*/
int8_t sensorFusion(float *SensorTemp_1, float *SensorTemp_2, float *SensorTempProcessed)
{
  SensorTempProcessed[2] = SensorTempProcessed[1];
  SensorTempProcessed[1] = SensorTempProcessed[0];

  SensorTempProcessed[0] = SensorTemp_1[0] * Sensor1_Weight + SensorTemp_2[0] * Sensor2_Weight;

  SensorTemp_1[2] = SensorTemp_1[1];
  SensorTemp_1[1] = SensorTemp_1[0];
  SensorTemp_2[2] = SensorTemp_2[1];
  SensorTemp_2[1] = SensorTemp_2[0];

  return 1;
}

/**
	* @brief  传感器滤波算法（限幅滤波），对采样缓冲区的数据进行滤波
	*         传感器采样一次调用一次，对新采集到的数据进行滤波，滤波后的数据依旧存储在数组的第一个位置
	* @param[1]  SensorTemp_1 
	* @param[2]  SensorTemp_2
	* @return 是否滤波成功 1	成功
												0	两传感器均出现问题
												-1	1传感器出现问题
												-2	2传感器出现问题
	* @attention 函数中处理完数据后不用对缓冲区的数据进行后移处理，这一步会在采样时完成。
	*/
int8_t sensorFilter(float *SensorTemp_1, float *SensorTemp_2)
{
  int8_t Sensor1_Status, Sensor2_Status;
  Sensor1_Status = fabs(SensorTemp_1[0] - SensorTemp_1[1]) <= MaxTempChange ? 1 : 0;
  Sensor2_Status = fabs(SensorTemp_2[0] - SensorTemp_2[1]) <= MaxTempChange ? 1 : 0;
  if ((Sensor1_Status == 1) && (Sensor2_Status == 1))
  {

    return 1;
  }
  else if ((Sensor1_Status == 0) && (Sensor2_Status == 1))
  {
    SensorTemp_1[0] = SensorTemp_1[1];
    return -1;
  }
  else if ((Sensor1_Status == 1) && (Sensor2_Status == 0))
  {
    SensorTemp_2[0] = SensorTemp_2[1];
    return -2;
  }
  else
  {
    SensorTemp_1[0] = SensorTemp_1[1];
    SensorTemp_2[0] = SensorTemp_2[1];
    return 0;
  }
}
