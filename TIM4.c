#include "TIM4.h"
#include "main.h"

extern float SensorTemp_1[3];
extern float SensorTemp_2[3];
extern int8_t TempUpdateFlag;

/**
	* @brief  配置TIM4
	* @param  无
	* @return 无
	* @attention 无
	*/
static void TIM4_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM4 clock enable (72MHz)*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	/* TIM_Period * TIM_Prescaler * Hz = TIM_Clock */
  /* Time base configuration （10Hz）*/
  TIM_TimeBaseStructure.TIM_Period = 2999;		      // (999+1)次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;		      // 设置预分频：7200分频，即为10KHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置时钟分频系数：不分频 72MHz
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //允许更新中断
  //中断优先级 NVIC 设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	   //TIM4 中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 0 级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	//从优先级 3 级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	   //IRQ 通道被使能
  NVIC_Init(&NVIC_InitStructure);			    // 初始化 NVIC 寄存器

  TIM_ARRPreloadConfig(TIM4, ENABLE); // 使能TIM4重载寄存器ARR
  TIM_Cmd(TIM4, ENABLE);	      // 使能 TIM4
}

/**
	* @brief  配置TIM4的中断函数
	* @param  无
	* @return 无
	* @attention 无
	*/
void TIM4_IRQHandler_ex(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) // 检查 TIM4 更新中断发生与否
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除 TIM4 更新中断标志
		// add code
		SensorTemp_1[0]=DS18B20_1_Get_Temp()/10.0;
		SensorTemp_2[0]=DS18B20_2_Get_Temp()/10.0;
		TempUpdateFlag = 1;
		
		// end
  }
}
/**
	* @brief TIM4初始化
	* @param  无
	* @return 无
	* @attention 无
	*/
void TIM4_Init(void)
{
  TIM4_Config();
}

/******************* (C) COPYRIGHT 2016  *****END OF FILE************/
