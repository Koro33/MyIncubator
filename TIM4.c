#include "TIM4.h"
#include "main.h"

extern float SensorTemp_1[3];
extern float SensorTemp_2[3];
extern int8_t TempUpdateFlag;

/**
	* @brief  ����TIM4
	* @param  ��
	* @return ��
	* @attention ��
	*/
static void TIM4_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM4 clock enable (72MHz)*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	/* TIM_Period * TIM_Prescaler * Hz = TIM_Clock */
  /* Time base configuration ��10Hz��*/
  TIM_TimeBaseStructure.TIM_Period = 2999;		      // (999+1)�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;		      // ����Ԥ��Ƶ��7200��Ƶ����Ϊ10KHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ����ʱ�ӷ�Ƶϵ��������Ƶ 72MHz
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //��������ж�
  //�ж����ȼ� NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	   //TIM4 �ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 0 ��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	//�����ȼ� 3 ��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	   //IRQ ͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);			    // ��ʼ�� NVIC �Ĵ���

  TIM_ARRPreloadConfig(TIM4, ENABLE); // ʹ��TIM4���ؼĴ���ARR
  TIM_Cmd(TIM4, ENABLE);	      // ʹ�� TIM4
}

/**
	* @brief  ����TIM4���жϺ���
	* @param  ��
	* @return ��
	* @attention ��
	*/
void TIM4_IRQHandler_ex(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) // ��� TIM4 �����жϷ������
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // ��� TIM4 �����жϱ�־
		// add code
		SensorTemp_1[0]=DS18B20_1_Get_Temp()/10.0;
		SensorTemp_2[0]=DS18B20_2_Get_Temp()/10.0;
		TempUpdateFlag = 1;
		
		// end
  }
}
/**
	* @brief TIM4��ʼ��
	* @param  ��
	* @return ��
	* @attention ��
	*/
void TIM4_Init(void)
{
  TIM4_Config();
}

/******************* (C) COPYRIGHT 2016  *****END OF FILE************/
