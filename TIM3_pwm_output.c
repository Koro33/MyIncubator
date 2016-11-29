#include "TIM3_pwm_output.h"
#include "main.h"
/**
	* @brief  配置TIM3复用输出PWM时用到的I/O
	* @param  无
	* @return 无
	* @attention 无
	*/
void TIM3_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM3 clock enable */
  //PCLK1经过2倍频后作为TIM3的时钟源等于72MHz
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
	* @brief  配置TIM3输出的PWM信号的模式，如周期、极性、占空比
	* @param  无
	* @return 无
	* @attention 频率的计算方法 (TIM_Period + 1) * (TIM_Prescaler + 1) * n Hz = RCC_APB1Periph_TIM3(72MHz)
	* 					 TIM_Period影响各通道跳变值的最大值，为计算方便最好取整
	* 					 (999 + 1) * (71 + 1) * 1000 Hz = 72000000
	*/
void TIM3_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;		      // 周期次数 TIM_Period * TIM_Prescaler * Hz = TIM_Clock
  TIM_TimeBaseStructure.TIM_Prescaler = 71;		      //设置预分频：分频72，即为1M
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
							      //结果为1KHz
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
}

/**
	* @brief  配置TIM3输出的Channel1的模式
	* @param  CCR1_Val 跳变值
	* @return 无
	* @attention 不要超过TIM_Period的值
	*/
void TIM3_CH1_Mode_Config(uint16_t CCR1_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;		    //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC1Init(TIM3, &TIM_OCInitStructure); //使能通道1
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  配置TIM3输出的Channel2的模式
	* @param  CCR2_Val 跳变值
	* @return 无
	* @attention 不要超过TIM_Period的值
	*/
void TIM3_CH2_Mode_Config(uint16_t CCR2_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;		    //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //当定时器计数值小于CCR2_Val时为高电平

  TIM_OC2Init(TIM3, &TIM_OCInitStructure); //使能通道2
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  配置TIM3输出的Channel3的模式
	* @param  CCR3_Val 跳变值
	* @return 无
	* @attention 不要超过TIM_Period的值
	*/
void TIM3_CH3_Mode_Config(uint16_t CCR3_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;		    //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //当定时器计数值小于CCR3_Val时为高电平

  TIM_OC3Init(TIM3, &TIM_OCInitStructure); //使能通道3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  配置TIM3输出的Channel4的模式
	* @param  CCR4_Val 跳变值
	* @return 无
	* @attention 不要超过TIM_Period的值
	*/
void TIM3_CH4_Mode_Config(uint16_t CCR4_Val)
{
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;		    //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //当定时器计数值小于CCR4_Val时为高电平

  TIM_OC4Init(TIM3, &TIM_OCInitStructure); //使能通道4
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
	* @brief  配置TIM3输出的初始化
	* @param  无
	* @return 无
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

  TIM_ARRPreloadConfig(TIM3, ENABLE); // 使能TIM3重载寄存器ARR
  TIM_Cmd(TIM3, ENABLE);	      //使能定时器3
}
//以上为基础初始化函数
/********************************************************************************************/

/********************************************************************************************/
//以下为高级自定义函数

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
