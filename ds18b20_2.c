#include "ds18b20.h"
#include "main.h"

/**
	* @brief  DS18B20_GPIO_Config
	* @param  ��
	* @return ��
	* @attention ��
	*/
void DS18B20_2_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(DS18B20_2_GPIOCLOCK, ENABLE);
  GPIO_WriteBit(DS18B20_2_PORT, DS18B20_2_BIT, Bit_SET);
  GPIO_InitStructure.GPIO_Pin = DS18B20_2_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(DS18B20_2_PORT, &GPIO_InitStructure);
}

/**
	* @brief  ��λDS18B20
	* @param  ��
	* @return ��
	* @attention ��
	*/
void DS18B20_2_Rst(void)
{
  CLR_DS18B20_2();	 //����DQ
  delay_us_ds18b20(950); //����750us
  SET_DS18B20_2();	 //DQ=1
  delay_us_ds18b20(60);  //15us
}

/**
	* @brief  ����Ƿ��л��DS18B20�Ĵ���
	* @param  ��
	* @return 0 ����
	*					1 ������
	* @attention ��
	*/
uint8_t DS18B20_2_Check(void)
{
  uint8_t retry = 0;

  while (DS18B20_2_DQ_IN && retry < 200)
  {
    retry++;
    delay_us_ds18b20(2);
  }
  if (retry >= 200)
    return 1;
  else
    retry = 0;
  while (!DS18B20_2_DQ_IN && retry < 240)
  {
    retry++;
    delay_us_ds18b20(2);
  }
  if (retry >= 240)
    return 1;
  return 0;
}

/**
	* @brief  ��DS18B20��ȡһ��λ
	* @param  ��
	* @return 0 & 1
	* @attention ��
	*/
uint8_t DS18B20_2_Read_Bit(void) // read one bit
{
  uint8_t data;

  CLR_DS18B20_2(); //����DQ
  delay_us_ds18b20(3);
  SET_DS18B20_2(); //DQ=1
  delay_us_ds18b20(4);
  if (DS18B20_2_DQ_IN)
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
	* @brief  ��ds18b20��ȡһ���ֽ�
	* @param  ��
	* @return ��ȡ���ֽ�
	* @attention ��
	*/
uint8_t DS18B20_2_Read_Byte(void) // read one byte
{
  uint8_t i, j, dat;
  dat = 0;

  for (i = 1; i <= 8; i++)
  {
    j = DS18B20_2_Read_Bit();
    dat = (j << 7) | (dat >> 1);
  }
  return dat;
}

/**
	* @brief	дһ���ֽڵ�DS18B20
	* @param  dat д����ֽ�
	* @return ��
	* @attention ��
	*/
void DS18B20_2_Write_Byte(uint8_t dat)
{
  uint8_t j;
  uint8_t testb;

  for (j = 1; j <= 8; j++)
  {
    testb = dat & 0x01;
    dat = dat >> 1;
    if (testb)
    {
      CLR_DS18B20_2(); //DS18B20_DQ_OUT=0;// Write 1
      delay_us_ds18b20(3);
      SET_DS18B20_2(); //DS18B20_DQ_OUT=1;
      delay_us_ds18b20(65);
    }
    else
    {
      CLR_DS18B20_2(); //DS18B20_DQ_OUT=0;// Write 0
      delay_us_ds18b20(65);
      SET_DS18B20_2(); //DS18B20_DQ_OUT=1;
      delay_us_ds18b20(3);
    }
  }
}
/**
	* @brief  ��ʼ�¶�ת��
	* @param  ��
	* @return ��
	* @attention ��
	*/
void DS18B20_2_Start(void) // ds1820 start convert
{
  DS18B20_2_Rst();
  DS18B20_2_Check();
  DS18B20_2_Write_Byte(0xcc); // skip rom
  DS18B20_2_Write_Byte(0x44); // convert
}
/**
	* @brief  ��ʼ��DS18B20��IO��DQ��ͬʱ���DS?�Ĵ���
	* @param  ��
	* @return 0	����
	*					1	������
	* @attention �������ʼ������
	* 						while(DS18B20_Init())
	* 						{
	* 							printf ("DS18B20 Check Failed!");
	* 							Delay_ms(500);
	* 							printf ("Please Check!");
	* 							Delay_ms(500);
	* 						}
	*/
uint8_t DS18B20_2_Init(void)
{
  DS18B20_2_GPIO_Config();
  SET_DS18B20_2(); //ʹPA2����ߵ�ƽ

  DS18B20_2_Rst();
  return DS18B20_2_Check();
}

/**
	* @brief  ��DS18B20����¶�ֵ
	* @param  ��
	* @return �¶� ��-550~1250��
	* @attention ����:0.1�� ����ֵ/10
	*/
short DS18B20_2_Get_Temp(void)
{
  uint8_t temp;
  uint8_t TL, TH;
  short tem;

  DS18B20_2_Start(); // ds1820 start convert
  DS18B20_2_Rst();
  DS18B20_2_Check();
  DS18B20_2_Write_Byte(0xcc); // skip rom
  DS18B20_2_Write_Byte(0xbe); // convert
  TL = DS18B20_2_Read_Byte(); // LSB
  TH = DS18B20_2_Read_Byte(); // MSB

  if (TH > 7)
  {
    TH = ~TH;
    TL = ~TL;
    temp = 0; //�¶�Ϊ��ֵ
  }
  else
  {
    temp = 1; //�¶�Ϊ��
  }
  tem = TH; //��ø�8λ
  tem <<= 8;
  tem += TL;		    //��õ�8λ
  tem = (float)tem * 0.625; //ת��
  if (temp)
  {
    return tem; //����¶�ֵ
  }
  else
  {
    return -tem;
  }
}

