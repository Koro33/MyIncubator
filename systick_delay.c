#include "systick_delay.h"
#include "main.h"

/**
	* @brief  Delay (n)us
	* @param  n (n)us
	* @return ��
	* @attention ����systick
	*/
void Delay_us(uint32_t n)
{
  SysTick->LOAD = 72 * n;     //װ�ؼ���ֵ����Ϊʱ��72M��72����1��s
//  SysTick->CTRL = 0x00000005; //ʱ����Դ��ΪΪHCLK(72M)���򿪶�ʱ��
	SysTick->CTRL = 0x00000005; //ʱ����Դ��ΪΪHCLK(72M)���򿪶�ʱ��
  while (!(SysTick->CTRL & 0x00010000))
    ;			      //�ȴ�������0
  SysTick->CTRL = 0x00000004; //�رն�ʱ��
}

/**
	* @brief  Delay (n)ms
	* @param  n (n)ms
	* @return ��
	* @attention ��
	*/
void Delay_ms(uint32_t n)
{
  int i;
  for (i = 0; i < n; i++)
  {
    Delay_us(1000);
  }
}
