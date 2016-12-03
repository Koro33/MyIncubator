#include "ds18b20.h"
#include "main.h"

/**
	* @brief  DS18B20_GPIO_Config
	* @param  无
	* @return 无
	* @attention 无
	*/
void DS18B20_1_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(DS18B20_1_GPIOCLOCK, ENABLE);
  GPIO_WriteBit(DS18B20_1_PORT, DS18B20_1_BIT, Bit_SET);
  GPIO_InitStructure.GPIO_Pin = DS18B20_1_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(DS18B20_1_PORT, &GPIO_InitStructure);
}

/**
	* @brief  复位DS18B20
	* @param  无
	* @return 无
	* @attention 无
	*/
void DS18B20_1_Rst(void)
{
  CLR_DS18B20_1();	 //拉低DQ
  delay_us_ds18b20(750); //拉低750us
  SET_DS18B20_1();	 //DQ=1
  delay_us_ds18b20(60);  //15us
}

/**
	* @brief  检查是否有活的DS18B20的存在
	* @param  无
	* @return 0 存在
	*					1 不存在
	* @attention 无
	*/
uint8_t DS18B20_1_Check(void)
{
  uint8_t retry = 0;

  while (DS18B20_1_DQ_IN && retry < 200)
  {
    retry++;
    delay_us_ds18b20(2);
  }
  if (retry >= 200)
    return 1;
  else
    retry = 0;
  while (!DS18B20_1_DQ_IN && retry < 240)
  {
    retry++;
    delay_us_ds18b20(2);
  }
  if (retry >= 240)
    return 1;
  return 0;
}

/**
	* @brief  从DS18B20读取一个位
	* @param  无
	* @return 0 & 1
	* @attention 无
	*/
uint8_t DS18B20_1_Read_Bit(void) // read one bit
{
  uint8_t data;

  CLR_DS18B20_1(); //拉低DQ
  delay_us_ds18b20(3);
  SET_DS18B20_1(); //DQ=1
  delay_us_ds18b20(4);
  if (DS18B20_1_DQ_IN)
  {
    data = 1;
  }
  else
  {
    data = 0;
  }
  delay_us_ds18b20(55);

  return data;
}

/**
	* @brief  从ds18b20读取一个字节
	* @param  无
	* @return 读取的字节
	* @attention 无
	*/
uint8_t DS18B20_1_Read_Byte(void) // read one byte
{
  uint8_t i, j, dat;
  dat = 0;

  for (i = 1; i <= 8; i++)
  {
    j = DS18B20_1_Read_Bit();
    dat = (j << 7) | (dat >> 1);
  }
  return dat;
}

/**
	* @brief	写一个字节到DS18B20
	* @param  dat 写入的字节
	* @return 无
	* @attention 无
	*/
void DS18B20_1_Write_Byte(uint8_t dat)
{
  uint8_t j;
  uint8_t testb;

  for (j = 1; j <= 8; j++)
  {
    testb = dat & 0x01;
    dat = dat >> 1;
    if (testb)
    {
      CLR_DS18B20_1(); //DS18B20_DQ_OUT=0;// Write 1
      delay_us_ds18b20(3);
      SET_DS18B20_1(); //DS18B20_DQ_OUT=1;
      delay_us_ds18b20(65);
    }
    else
    {
      CLR_DS18B20_1(); //DS18B20_DQ_OUT=0;// Write 0
      delay_us_ds18b20(65);
      SET_DS18B20_1(); //DS18B20_DQ_OUT=1;
      delay_us_ds18b20(3);
    }
  }
}
/**
	* @brief  开始温度转换
	* @param  无
	* @return 无
	* @attention 无
	*/
void DS18B20_1_Start(void) // ds1820 start convert
{
  DS18B20_1_Rst();
  DS18B20_1_Check();
  DS18B20_1_Write_Byte(0xcc); // skip rom
  DS18B20_1_Write_Byte(0x44); // convert
}
/**
	* @brief  初始化DS18B20的IO口DQ，同时检查DS?的存在
	* @param  无
	* @return 0	存在
	*					1	不存在
	* @attention 主程序初始化调用
	* 						while(DS18B20_Init())
	* 						{
	* 							printf ("DS18B20 Check Failed!");
	* 							Delay_ms(500);
	* 							printf ("Please Check!");
	* 							Delay_ms(500);
	* 						}
	*/
uint8_t DS18B20_1_Init(void)
{
  DS18B20_1_GPIO_Config();
  SET_DS18B20_1(); //使PA2输出高电平

  DS18B20_1_Rst();
  return DS18B20_1_Check();
}

/**
	* @brief  从DS18B20获得温度值
	* @param  无
	* @return 温度 （-550~1250）
	* @attention 精度:0.1℃ 返回值/10
	*/
short DS18B20_1_Get_Temp(void)
{
  uint8_t temp;
  uint8_t TL, TH;
  short tem;

  DS18B20_1_Start(); // ds1820 start convert
  DS18B20_1_Rst();
  DS18B20_1_Check();
  DS18B20_1_Write_Byte(0xcc); // skip rom
  DS18B20_1_Write_Byte(0xbe); // convert
  TL = DS18B20_1_Read_Byte(); // LSB
  TH = DS18B20_1_Read_Byte(); // MSB

  if (TH > 7)
  {
    TH = ~TH;
    TL = ~TL;
    temp = 0; //温度为负值
  }
  else
  {
    temp = 1; //温度为正
  }
  tem = TH; //获得高8位
  tem <<= 8;
  tem += TL;		    //获得低8位
  tem = (float)tem * 0.625; //转换
  if (temp)
  {
    return tem; //获得温度值
  }
  else
  {
    return -tem;
  }
}
/**
	* @brief  单独为DS18B20配置延时函数，单总线协议时序要求比较严格
	* @param  无
	* @return 无
	* @attention 	注意编译器优化级别 一般-O2就好，-O3有些地方会出问题
	*	@bug	(uint32_t i;)和(volatile uint32_t i;)两个值不一样？？？？？？？？？？？？？？？？
	*/
void delay_us_ds18b20(uint32_t value)
{
//	volatile uint32_t i=0;
////	volatile uint32_t i=0;
//	i = value * 11;
//	while(i--)
//		;
	value = value * 6;
	for( ; value > 0; value--)
	{
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		
	}
//	Delay_us(value);
}
