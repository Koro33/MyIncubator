#include "extra_gpio.h"
#include "main.h"

/**
	* @brief  ����GPIO����
	* @param  ��
	* @return ��
	* @attention ��
	*/
void extraGPIOInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//  //��λ����
//  GPIO_InitStructure.GPIO_Pin = GPIO_SWITCH1 | GPIO_SWITCH2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  GPIO_Init(PORT_SWITCH, &GPIO_InitStructure);

  //led
  GPIO_InitStructure.GPIO_Pin = PIN_BEEP | PIN_SWITCH2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_BEEP, &GPIO_InitStructure);
}
/**
	* @brief  beep����
	* @param  Num���������  IntervalNum��������(ms)
	* @return ��
	* @attention ��
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