#include "usart.h"
#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//usart1�ж϶���
struct que rxQue;
// usart1�����жϻ���
uint8_t U1_IT_rxBuffer[U1_IT_MaxBufferSize];
// usart1 ���ͻ�����
extern char U1_rxBuffer[128];
// usart1 ���ջ�����
extern char U1_txBuffer[128];
// usart2 ���ͻ�����
extern char U2_rxBuffer[64];
// Ŀ���¶�
extern float TargetTemp;
// �ں����ݴ洢��
extern float SensorTempProcessed[3];
// �봮�����Ĺ����л���־ 0������������¶�  1��ͼ������������
extern int8_t Usart1TmtChoiceFlag;
// ���Ʒ�ʽ��ѡ��0���ֶ� 1��BB���� 2��PID����
extern int8_t CtrlMode;
// ����������
extern uint16_t HeatPower;
// ���ȹ���
extern uint16_t FanPower;
// PID����������
extern struct PID PID_1;



/**
	* @brief  USART����, 	usart1 ����ģʽ 115200 8-N-1 ˫�շ� + NVIC + QUE
	*											usart2 ����ģʽ 115200 8-N-1 �������� ��GPIO����Ȼ��ʼ������
	* @param  ��
	* @return ��
	* @attention ��
	*/
void USART_Config(void)
{
	//usart1��ʼ����GPIO+USART+NVIC+CircleReceive��
	/*************************************************************************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);		  //��ʼ������NVIC�Ĵ���USART1

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�����ж�

	//Usart1 ���ջ��ζ��г�ʼ��
	rxQue.head = U1_IT_rxBuffer;
	rxQue.tail = U1_IT_rxBuffer;
	/*************************************************************************************************/

	//usart2��ʼ����GPIO+USART��
	/*************************************************************************************************/

	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART1 GPIO config */
	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
	/*************************************************************************************************/
}

/**
	* @brief  �ض���c�⺯��printf��USART1
	* @param  ��printf
	* @return ��printf
	* @attention ��
	*/
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(USART1, (uint8_t)ch);

	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;

	return (ch);
}

///**
//	* @brief  �ض���c�⺯��scanf��USART1
//	* @param  ��
//	* @return ��
//	* @attention ��
//	*/
//int fgetc(FILE *f)
//{
//  /* �ȴ�����1�������� */
//  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
//    ;

//  return (int)USART_ReceiveData(USART1);
//}

/**
	* @brief  �Զ�����printf������UASRT2
	* @param  ��
	* @return ��
	* @attention DEBUG��
	*/
void usart2Printf(char *fmt, ...)
{
	char buffer[U2_CMD_BUFFER_LEN - 1];
	u8 i = 0;
	u8 len;

	va_list arg_ptr;						//Define convert parameters variable
	va_start(arg_ptr, fmt);					//Init variable
	len = vsnprintf(buffer, U2_CMD_BUFFER_LEN + 1, fmt, arg_ptr); //parameters list format to buffer

	while ((i < U2_CMD_BUFFER_LEN) && (i < len) && (len > 0))
	{
		USART_SendData(USART2, (u8)buffer[i++]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
	}
	va_end(arg_ptr);
}

/**
	* @brief  ��ȡ���λ�������е�Ԫ�أ����Ӷ�����ȥ��
	* @param  ��
	* @return ��ȡ�����ַ�
	* @attention Ԥ��Ҫ����uart1_avilable()���ж϶������Ƿ��д�����Ԫ��
	*/
uint8_t usart1_read(void)
{
	uint8_t temp = 0;
	if (usart1_avilable() == 0)
	{
		return 0;
	}
	if (rxQue.head != (U1_IT_rxBuffer + U1_IT_MaxBufferSize - 1))
	{
		temp = *rxQue.head;
		rxQue.head++;
	}
	else
	{
		temp = *rxQue.head;
		rxQue.head = U1_IT_rxBuffer;
	}

	return temp;
}

/**
	* @brief  ��ȡ���λ�������е�Ԫ�أ���ȥ��Ԫ��
	* @param  ��
	* @return ��ȡ�����ַ�
	* @attention Ԥ��Ҫ����uart1_avilable()���ж϶������Ƿ��д�����Ԫ��
	*/
uint8_t usart1_peek(void)
{
	uint8_t temp = 0;
	if (usart1_avilable() == 0)
	{
		return 0;
	}

	temp = *rxQue.head;

	return temp;
}

/**
	* @brief  �ж�usart1�Ļ��λ���������Ƿ���δ�����Ԫ��
	* @param  ��
	* @return 0: ��  1����
	* @attention ��
	*/
int usart1_avilable(void)
{
	if (rxQue.head != rxQue.tail)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/**
	* @brief  usart1���жϺ�������ļ��ϣ������USART1_IRQHandler()��
	* @param  ��
	* @return ��
	* @attention ��
	*/
void USART1_IRQHandler_ex(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			;
		if (rxQue.tail != (U1_IT_rxBuffer + U1_IT_MaxBufferSize - 1))
		{
			*rxQue.tail = USART_ReceiveData(USART1);
			rxQue.tail++;
		}
		else
		{
			*rxQue.tail = USART_ReceiveData(USART1);
			rxQue.tail = U1_IT_rxBuffer;
		}
	}
}

/**
	* @brief  usart1�Ľ������񣬶Զ����е�����֡���н�����������뵥Ƭ���Ľ���
	* @param  ��
	* @return 0��failed 1��successful
	* @attention ��⵽�����������������������
	*/
int usart1_RevTask(void)
{
	int RevCmdLength = 0;
	if (usart1_avilable() > 0)
	{
		while (usart1_peek() != 0xff)
		{
			strcatchar(U1_rxBuffer, (char)(usart1_read()));
			RevCmdLength++;
			Delay_us(200);
		}
		usart1_read();
		Delay_us(200);
		usart1_read();
		Delay_us(200);
		usart1_read();
		Delay_us(200);

		if (U1_rxBuffer[0] == 0x65) //ĳЩ���������£��л�ģʽ��ҳ��
		{
			if (U1_rxBuffer[1] == 0x01)
			{
				if ((U1_rxBuffer[2] == 0x05) || (U1_rxBuffer[2] == 0x06)) // ҳ���л���curve���л����ڷ��������־
				{
					Usart1TmtChoiceFlag = 1;
				}
				if (U1_rxBuffer[2] == 0x08) // �ֶ�����ģʽ
				{
					CtrlMode = 0;
				}
				if (U1_rxBuffer[2] == 0x0A) // BB����ģʽ
				{
					CtrlMode = 2;
				}
				if (U1_rxBuffer[2] == 0x0B) // PID����ģʽ
				{
					CtrlMode = 1;
				}
			}
			if (U1_rxBuffer[1] == 0x02)
			{
				if (U1_rxBuffer[2] == 0x02) // ��curve���淵�أ������л����ڷ��������־
				{
					Usart1TmtChoiceFlag = 0;
				}
			}
		}
		if (U1_rxBuffer[0] == 0x67) //��Ҫ�޸����ݵ�
		{
			if (U1_rxBuffer[1] == 0x01)
			{
				if (U1_rxBuffer[2] == 0x0C) // �ֶ�����Ƭ����
				{
					HeatPower = U1_rxBuffer[4] + U1_rxBuffer[5] * 256;
				}
				if (U1_rxBuffer[2] == 0x0D) // �ֶ����ȹ���
				{
					FanPower = U1_rxBuffer[4] + U1_rxBuffer[5] * 256;
				}
				if (U1_rxBuffer[2] == 0x07) // Ŀ���¶�
				{
					TargetTemp = Strnum2Num(U1_rxBuffer, 4, RevCmdLength);
				}
				if (U1_rxBuffer[2] == 0x0E) // Kp
				{
					PID_1.Kp = Strnum2Num(U1_rxBuffer, 4, RevCmdLength);
				}
				if (U1_rxBuffer[2] == 0x0F) // Ki
				{
					PID_1.Ki = Strnum2Num(U1_rxBuffer, 4, RevCmdLength);
				}
				if (U1_rxBuffer[2] == 0x10) // Kd
				{
					PID_1.Kd = Strnum2Num(U1_rxBuffer, 4, RevCmdLength);
				}
			}
		}
	}
	strclr(U1_rxBuffer);
	return 1;
}
/**
	* @brief  usart1�ķ�������
	* @param  ��
	* @return 0��failed 1��successful
	* @attention �¶�ÿ����һ�ε���һ��
	*/
int usart1_TmtTask(void)
{
	if (Usart1TmtChoiceFlag == 0)
	{
		printf("main.t0.txt=\"%.1f\"", SensorTempProcessed[0]);
		usart1_sendHMIEndCmd();
		if ((CtrlMode == 1) || (CtrlMode == 2))
		{
			printf("main.h0.val=%d", FanPower);
			usart1_sendHMIEndCmd();
			printf("main.n0.val=%d", FanPower);
			usart1_sendHMIEndCmd();
			printf("main.h1.val=%d", HeatPower);
			usart1_sendHMIEndCmd();
			printf("main.n1.val=%d", HeatPower);
			usart1_sendHMIEndCmd();
		}
	}
	else if (Usart1TmtChoiceFlag == 1)
	{
		printf("add 1,0,%d", (int)(SensorTempProcessed[0] * 4));
		usart1_sendHMIEndCmd();
		printf("add 1,1,%d", (int)(TargetTemp * 4));
		usart1_sendHMIEndCmd();
	}
	else
	{
		;
	}
	return 1;
}
/**
	* @brief  usart1�ĳ�ʼ���������񣬶�����Ŀ���¶ȣ����ȹ��ʣ����ȹ��ʣ�PID����������ʾ��ʼ��
	* @param  ��
	* @return 0��failed 1��successful
	* @attention ��
	*/
int usart1_InitTmtTask(void)
{
	printf("main.t1.txt=\"%.1f\"", TargetTemp);
	usart1_sendHMIEndCmd();
	printf("main.h0.val=%d", HeatPower);
	usart1_sendHMIEndCmd();
	printf("main.h1.val=%d", FanPower);
	usart1_sendHMIEndCmd();
	printf("main.n0.val=%d", HeatPower);
	usart1_sendHMIEndCmd();
	printf("main.n1.val=%d", FanPower);
	usart1_sendHMIEndCmd();
	printf("main.t2.txt=\"%.1f\"", PID_1.Kp);
	usart1_sendHMIEndCmd();
	printf("main.t3.txt=\"%.1f\"", PID_1.Ki);
	usart1_sendHMIEndCmd();
	printf("main.t4.txt=\"%.1f\"", PID_1.Kd);
	usart1_sendHMIEndCmd();

	return 1;
}
/**
	* @brief  ��char��׷�ӵ��ַ�������
	* @param  ��
	* @return 0��failed 1��successful
	* @attention ��
	*/
int strcatchar(char *front, char rear)
{
	uint8_t pos = 0;

	pos = strlen(front);
	front[pos] = rear;
	front[pos + 1] = '\0';

	return 1;
}
/**
	* @brief  ����ַ��������ַ����ĵ�һλд'\0'��
	* @param  ��
	* @return 0��failed 1��successful
	* @attention ��
	*/
int strclr(char *str)
{
	str[0] = '\0';
	return 1;
}
/**
	* @brief  ����HMIЭ��ֹͣ֡
	* @param  ��
	* @return 0��failed 1��successful
	* @attention ��
	*/
int usart1_sendHMIEndCmd(void)
{
	USART_SendData(USART1, 0xff);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART1, 0xff);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART1, 0xff);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;

	return 1;
}
/**
	* @brief  ��ĩβ��'\0'��char�����е�������ȡ����
	* @param  *Strnum��Ҫת����char����  StartNum����ʼλ�ã������䣩 EndNum������λ�ã������䣩
	* @return ת���ɵ�����(float)
	* @attention ��
	*/
float Strnum2Num(char *numStr, uint8_t StartNum, uint8_t EndNum)
{
	float num = 0.0;
	numStr[EndNum] = '\0';
	num = (float)atof(numStr + StartNum);
	return num;
}
/**
	* @brief  ��
	* @param  ��
	* @return ��
	* @attention ��
	*/

	/*********************************************END OF FILE**********************/
