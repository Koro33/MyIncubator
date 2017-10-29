#include "usart.h"
#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//usart1中断队列
struct que rxQue;
// usart1队列中断缓冲
uint8_t U1_IT_rxBuffer[U1_IT_MaxBufferSize];
// usart1 发送缓冲区
extern char U1_rxBuffer[128];
// usart1 接收缓冲区
extern char U1_txBuffer[128];
// usart2 发送缓冲区
extern char U2_rxBuffer[64];
// 目标温度
extern float TargetTemp;
// 融合数据存储区
extern float SensorTempProcessed[3];
// 与串口屏的功能切换标志 0：主界面更新温度  1：图表界面更新曲线
extern int8_t Usart1TmtChoiceFlag;
// 控制方式的选择，0：手动 1：BB控制 2：PID控制
extern int8_t CtrlMode;
// 加热器功率
extern uint16_t HeatPower;
// 风扇功率
extern uint16_t FanPower;
// PID控制器参数
extern struct PID PID_1;



/**
	* @brief  USART配置, 	usart1 工作模式 115200 8-N-1 双收发 + NVIC + QUE
	*											usart2 工作模式 115200 8-N-1 单发不收 但GPIO口仍然初始化保留
	* @param  无
	* @return 无
	* @attention 无
	*/
void USART_Config(void)
{
	//usart1初始化（GPIO+USART+NVIC+CircleReceive）
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

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);		  //初始化外设NVIC寄存器USART1

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启中断

	//Usart1 接收环形队列初始化
	rxQue.head = U1_IT_rxBuffer;
	rxQue.tail = U1_IT_rxBuffer;
	/*************************************************************************************************/

	//usart2初始化（GPIO+USART）
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
	* @brief  重定向c库函数printf到USART1
	* @param  见printf
	* @return 见printf
	* @attention 无
	*/
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(USART1, (uint8_t)ch);

	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;

	return (ch);
}

///**
//	* @brief  重定向c库函数scanf到USART1
//	* @param  无
//	* @return 无
//	* @attention 无
//	*/
//int fgetc(FILE *f)
//{
//  /* 等待串口1输入数据 */
//  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
//    ;

//  return (int)USART_ReceiveData(USART1);
//}

/**
	* @brief  自定义类printf函数到UASRT2
	* @param  无
	* @return 无
	* @attention DEBUG用
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
	* @brief  读取环形缓冲队列中的元素，并从队列中去除
	* @param  无
	* @return 读取到的字符
	* @attention 预先要经过uart1_avilable()的判断队列中是否有待处理元素
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
	* @brief  读取环形缓冲队列中的元素，不去除元素
	* @param  无
	* @return 读取到的字符
	* @attention 预先要经过uart1_avilable()的判断队列中是否有待处理元素
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
	* @brief  判断usart1的环形缓冲队列中是否有未处理的元素
	* @param  无
	* @return 0: 无  1：有
	* @attention 无
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
	* @brief  usart1的中断函数处理的集合，会放入USART1_IRQHandler()中
	* @param  无
	* @return 无
	* @attention 无
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
	* @brief  usart1的接收任务，对队列中的命令帧进行解析，完成屏与单片机的交互
	* @param  无
	* @return 0：failed 1：successful
	* @attention 检测到串口有命令发过来后立即调用
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

		if (U1_rxBuffer[0] == 0x65) //某些按键被按下，切换模式或页面
		{
			if (U1_rxBuffer[1] == 0x01)
			{
				if ((U1_rxBuffer[2] == 0x05) || (U1_rxBuffer[2] == 0x06)) // 页面切换到curve，切换串口发送任务标志
				{
					Usart1TmtChoiceFlag = 1;
				}
				if (U1_rxBuffer[2] == 0x08) // 手动控制模式
				{
					CtrlMode = 0;
				}
				if (U1_rxBuffer[2] == 0x0A) // BB控制模式
				{
					CtrlMode = 2;
				}
				if (U1_rxBuffer[2] == 0x0B) // PID控制模式
				{
					CtrlMode = 1;
				}
			}
			if (U1_rxBuffer[1] == 0x02)
			{
				if (U1_rxBuffer[2] == 0x02) // 从curve界面返回，继续切换串口发送任务标志
				{
					Usart1TmtChoiceFlag = 0;
				}
			}
		}
		if (U1_rxBuffer[0] == 0x67) //需要修改数据的
		{
			if (U1_rxBuffer[1] == 0x01)
			{
				if (U1_rxBuffer[2] == 0x0C) // 手动加热片功率
				{
					HeatPower = U1_rxBuffer[4] + U1_rxBuffer[5] * 256;
				}
				if (U1_rxBuffer[2] == 0x0D) // 手动风扇功率
				{
					FanPower = U1_rxBuffer[4] + U1_rxBuffer[5] * 256;
				}
				if (U1_rxBuffer[2] == 0x07) // 目标温度
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
	* @brief  usart1的发送任务
	* @param  无
	* @return 0：failed 1：successful
	* @attention 温度每更新一次调用一次
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
	* @brief  usart1的初始化发送任务，对屏的目标温度，风扇功率，加热功率，PID参数进行显示初始化
	* @param  无
	* @return 0：failed 1：successful
	* @attention 无
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
	* @brief  将char型追加到字符串后面
	* @param  无
	* @return 0：failed 1：successful
	* @attention 无
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
	* @brief  清除字符串（将字符串的第一位写'\0'）
	* @param  无
	* @return 0：failed 1：successful
	* @attention 无
	*/
int strclr(char *str)
{
	str[0] = '\0';
	return 1;
}
/**
	* @brief  发送HMI协议停止帧
	* @param  无
	* @return 0：failed 1：successful
	* @attention 无
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
	* @brief  将末尾无'\0'的char数组中的数字提取出来
	* @param  *Strnum：要转换的char数组  StartNum：开始位置（闭区间） EndNum：结束位置（开区间）
	* @return 转换成的数字(float)
	* @attention 无
	*/
float Strnum2Num(char *numStr, uint8_t StartNum, uint8_t EndNum)
{
	float num = 0.0;
	numStr[EndNum] = '\0';
	num = (float)atof(numStr + StartNum);
	return num;
}
/**
	* @brief  无
	* @param  无
	* @return 无
	* @attention 无
	*/

	/*********************************************END OF FILE**********************/
