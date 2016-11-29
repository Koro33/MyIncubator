#include "systick_delay.h"
#include "main.h"

/**
	* @brief  Delay (n)us
	* @param  n (n)us
	* @return 无
	* @attention 调用systick
	*/
void Delay_us(uint32_t n)
{
  SysTick->LOAD = 72 * n;     //装载计数值，因为时钟72M，72次在1μs
//  SysTick->CTRL = 0x00000005; //时钟来源设为为HCLK(72M)，打开定时器
	SysTick->CTRL = 0x00000005; //时钟来源设为为HCLK(72M)，打开定时器
  while (!(SysTick->CTRL & 0x00010000))
    ;			      //等待计数到0
  SysTick->CTRL = 0x00000004; //关闭定时器
}

/**
	* @brief  Delay (n)ms
	* @param  n (n)ms
	* @return 无
	* @attention 无
	*/
void Delay_ms(uint32_t n)
{
  int i;
  for (i = 0; i < n; i++)
  {
    Delay_us(1000);
  }
}
