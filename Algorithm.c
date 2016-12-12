#include "Algorithm.h"
#include "main.h"

// �������������Լ��˲������ݴ洢��
extern float SensorTemp_1[3];
extern float SensorTemp_2[3];
// �ں����ݴ洢��
extern float SensorTempProcessed[3];
// PID����������
extern struct PID PID_1;
// ����������
extern uint16_t HeatPower;
/**
	* @brief  ������PID�㷨
	*         PID = PID->UK_REAL + PID->KP*[E(k)-E(k-1)]+PID->KI*E(k)+PID->KD*[E(k)-2E(k-1)+E(k-2)]
	* @param  current_position
	* @param  target_position
	* @return output
	* @attention ���˸Ľ������¶�С����ֵ��ʱ��ʹ��PID����
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
	* @brief  BB�����㷨
	* @param  current_position
	* @param  target_position
	* @return output
	* @attention ��
	*/
float BB_Control_1(float current_position, float target_position)
{
	static int8_t DirFlag = 0; // ��������Ϊ0 ��������Ϊ1 Ĭ��0
	float HeatTempThreshold = target_position + BB_Heat_Threshold; // 25
	float CoolTempThreshold = target_position + BB_Cool_Threshold; // 35
	if(current_position <= HeatTempThreshold)
	{
		DirFlag = 0; //��������
		return 999;
	}
	if(current_position >= CoolTempThreshold)
	{
		DirFlag = 1; // ��������
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
	* @brief  �������ں��㷨����Ȩƽ����
	* 				�������˲��㷨����е��ã��ں���ɺ�洢���ں����ݴ洢�������еĵ�һ��λ�ã����һ�����ݱ������ʣ���������κ���
	* @param[1]  SensorTemp_1 
	* @param[2]  SensorTemp_2
 	* @param[3]  SensorTempProcessed
	* @return �Ƿ��ںϳɹ� 1	�ɹ�
												0	 ���ɹ�
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
	* @brief  �������˲��㷨���޷��˲������Բ��������������ݽ����˲�
	*         ����������һ�ε���һ�Σ����²ɼ��������ݽ����˲����˲�����������ɴ洢������ĵ�һ��λ��
	* @param[1]  SensorTemp_1 
	* @param[2]  SensorTemp_2
	* @return �Ƿ��˲��ɹ� 1	�ɹ�
												0	������������������
												-1	1��������������
												-2	2��������������
	* @attention �����д��������ݺ��öԻ����������ݽ��к��ƴ�����һ�����ڲ���ʱ��ɡ�
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
