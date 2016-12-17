/**
	******************************************************************************
	* @file    ds18b20.h
	* @author  ycg1024
	* @version V1.0.0
	* @date    26-Oct-2016
	* @brief
	*
	*
	******************************************************************************
	* @attention
	*
	******************************************************************************
	*/

#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f10x.h"

#define DS18B20_1_GPIOCLOCK RCC_APB2Periph_GPIOB
#define DS18B20_2_GPIOCLOCK RCC_APB2Periph_GPIOB

#define DS18B20_1_BIT GPIO_Pin_8
#define DS18B20_1_PORT GPIOB

#define DS18B20_2_BIT GPIO_Pin_9
#define DS18B20_2_PORT GPIOB

#define CLR_DS18B20_1() GPIO_WriteBit(DS18B20_1_PORT, DS18B20_1_BIT, Bit_RESET)
#define SET_DS18B20_1() GPIO_WriteBit(DS18B20_1_PORT, DS18B20_1_BIT, Bit_SET)

#define CLR_DS18B20_2() GPIO_WriteBit(DS18B20_2_PORT, DS18B20_2_BIT, Bit_RESET)
#define SET_DS18B20_2() GPIO_WriteBit(DS18B20_2_PORT, DS18B20_2_BIT, Bit_SET)

#define DS18B20_1_DQ_IN GPIO_ReadInputDataBit(DS18B20_1_PORT, DS18B20_1_BIT)

#define DS18B20_2_DQ_IN GPIO_ReadInputDataBit(DS18B20_2_PORT, DS18B20_2_BIT)

/**************************************************************/
/************DS18B20_1*****************************************/
/**************************************************************/
void DS18B20_1_GPIO_Config(void);       //����GPIO
uint8_t DS18B20_1_Init(void);           //��ʼ��DS18B20
short DS18B20_1_Get_Temp(void);         //��ȡ�¶�
void DS18B20_1_Start(void);             //��ʼ�¶�ת��
void DS18B20_1_Write_Byte(uint8_t dat); //д��һ���ֽ�
uint8_t DS18B20_1_Read_Byte(void);      //����һ���ֽ�
uint8_t DS18B20_1_Read_Bit(void);       //����һ��λ
uint8_t DS18B20_1_Check(void);          //����Ƿ����DS18b20
void DS18B20_1_Rst(void);               //��λDS18B20
/**************************************************************/
/************DS18B20_2*****************************************/
/**************************************************************/
void DS18B20_2_GPIO_Config(void);       //����GPIO
uint8_t DS18B20_2_Init(void);           //��ʼ��DS18B20
short DS18B20_2_Get_Temp(void);         //��ȡ�¶�
void DS18B20_2_Start(void);             //��ʼ�¶�ת��
void DS18B20_2_Write_Byte(uint8_t dat); //д��һ���ֽ�
uint8_t DS18B20_2_Read_Byte(void);      //����һ���ֽ�
uint8_t DS18B20_2_Read_Bit(void);       //����һ��λ
uint8_t DS18B20_2_Check(void);          //����Ƿ����DS18b20
void DS18B20_2_Rst(void);               //��λDS18B20
/**************************************************************/
void delay_us_ds18b20(uint32_t value);



#endif


/******************* (C) COPYRIGHT 2016 ycg1024 ***********END OF FILE*********/
