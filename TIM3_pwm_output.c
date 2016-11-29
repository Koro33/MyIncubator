#include "TIM3_pwm_output.h"
#include "main.h"
/**
	* @brief  ����TIM3�������PWMʱ�õ���I/O
	* @param  ��
	* @return ��
	* @attention ��
	*/
void TIM3_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM3 clock enable */
  //PCLK1����2��Ƶ����ΪTIM3��ʱ��Դ����72MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*GPIOB Configuration: TIM3 channel 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
	* @brief  ����TIM3�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
	* @param  ��
	* @return ��
	* @attention Ƶ�ʵļ��㷽�� (TIM_Period + 1) * (TIM_Prescaler + 1) * n Hz = RCC_APB1Periph_TIM3(72MHz)
	* 					 TIM_PeriodӰ���ͨ������ֵ�����ֵ��Ϊ���㷽�����ȡ��
	* 					 (999 + 1) * (71 + 1) * 1000 Hz = 72000000
	*/
void TIM3_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;		      // ���ڴ��� TIM_Period * TIM_Prescaler * Hz = TIM_Clock
  TIM_TimeBaseStructure.TIM_Prescaler = 71;		      //����Ԥ��Ƶ����Ƶ72����Ϊ1M
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷ�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
							      //���Ϊ1KHz
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
}

/**
	* @brief  ����TIM3�����Channel1��ģʽ
	* @param  CCR1_Val ����ֵ
	* @return ��
	* @attention ��Ҫ����TIM_Period��ֵ
	*/
void TIM3_CH1_Mode_Config(uint16_t CCR1_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;		    //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC1Init(TIM3, &TIM_OCInitStructure); //ʹ��ͨ��1
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  ����TIM3�����Channel2��ģʽ
	* @param  CCR2_Val ����ֵ
	* @return ��
	* @attention ��Ҫ����TIM_Period��ֵ
	*/
void TIM3_CH2_Mode_Config(uint16_t CCR2_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;		    //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ʱ������ֵС��CCR2_ValʱΪ�ߵ�ƽ

  TIM_OC2Init(TIM3, &TIM_OCInitStructure); //ʹ��ͨ��2
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  ����TIM3�����Channel3��ģʽ
	* @param  CCR3_Val ����ֵ
	* @return ��
	* @attention ��Ҫ����TIM_Period��ֵ
	*/
void TIM3_CH3_Mode_Config(uint16_t CCR3_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;		    //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ʱ������ֵС��CCR3_ValʱΪ�ߵ�ƽ

  TIM_OC3Init(TIM3, &TIM_OCInitStructure); //ʹ��ͨ��3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  ����TIM3�����Channel4��ģʽ
	* @param  CCR4_Val ����ֵ
	* @return ��
	* @attention ��Ҫ����TIM_Period��ֵ
	*/
void TIM3_CH4_Mode_Config(uint16_t CCR4_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;		    //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ʱ������ֵС��CCR4_ValʱΪ�ߵ�ƽ

  TIM_OC4Init(TIM3, &TIM_OCInitStructure); //ʹ��ͨ��4
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  ����TIM3����ĳ�ʼ��
	* @param  ��
	* @return ��
	* @attention 
	*/
void TIM3_PWM_Init(void)
{
  TIM3_GPIO_Config();
  TIM3_Mode_Config();

  TIM3_CH1_Mode_Config(10);
  TIM3_CH2_Mode_Config(10);
  TIM3_CH3_Mode_Config(10);
  TIM3_CH4_Mode_Config(10);

  TIM_ARRPreloadConfig(TIM3, ENABLE); // ʹ��TIM3���ؼĴ���ARR
  TIM_Cmd(TIM3, ENABLE);	      //ʹ�ܶ�ʱ��3
}
//����Ϊ������ʼ������
/********************************************************************************************/

/********************************************************************************************/
//����Ϊ�߼��Զ��庯��

int setPower_Heat1(uint16_t power)
{
	
	return 1;
}

int setPower_Heat2(uint16_t power)
{
	
	return 1;
}
int setPower_Heat1_Heat2(uint16_t power1, uint16_t power2)
{
	setPower_Heat1(power1);
	setPower_Heat2(power2);
	
	return 1;
}
int setPower_Fan(uint16_t power)
{
	
	return 1;
}

/******************* (C) COPYRIGHT 2016 ycg1024  *****END OF FILE************/
