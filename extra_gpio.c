#include "extra_gpio.h"
#include "main.h"

/**
	* @brief  其他GPIO配置
	* @param  无
	* @return 无
	* @attention 无
	*/
void extraGPIOInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = PIN_BEEP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_BEEP, &GPIO_InitStructure);
}
/**
	* @brief  beep方法
	* @param  Num：鸣响次数  IntervalNum：鸣响间隔(ms)
	* @return 无
	* @attention 无
	*/
void beep(uint8_t Num, uint8_t IntervalNum)
{
	int i;
	for(i=0; i<Num; i++)
	{
		GPIO_SetBits(PORT_BEEP, PIN_BEEP);
		Delay_ms(IntervalNum);
		GPIO_ResetBits(PORT_BEEP, PIN_BEEP);
		Delay_ms(IntervalNum);
	}

}
