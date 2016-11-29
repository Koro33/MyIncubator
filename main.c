#include "main.h"
#define DEBUGIT

//// test
//uint8_t bbb[100];
//static uint8_t temp[3] = {0x32, 0x00, 0x65};
//static uint8_t temp1[3] = {65, 10, 23};
// �������������Լ��˲������ݴ洢����ÿ���²������洢������ĵ�һ��λ�ã����һ�����ݱ������ʣ���������κ���
float SensorTemp_1[3] = {0, 0, 0};
float SensorTemp_2[3] = {0, 0, 0};
// �ں����ݴ洢�����ںϺ�����ݴ洢������ĵ�һ��λ�ã����һ�����ݱ������ʣ���������κ���
float SensorTempProcessed[3] = {0, 0, 0};
// ���ݸ��µı�־������ʱ��4�ж��и������¶Ȳ������ݴ�ֵΪ1��δ���´�ֵΪ0
int8_t TempUpdateFlag = 0;
// Ŀ���¶�
float TargetTemp = 30.0;
// usart1 ���ͻ�����
char U1_rxBuffer[128];
// usart1 ���ջ�����
char U1_txBuffer[128];
// usart2 ���ͻ�����
char U2_rxBuffer[64];
// �봮�����Ĺ����л���־ 0������������¶�  1��ͼ�������������
int8_t Usart1TmtChoiceFlag = 0;
// ���Ʒ�ʽ��ѡ��0���ֶ� 1��BB���� 2��PID���� 
int8_t ctrlMode = 0;
// PID����������
struct PID PID_1={20.0,2.0,1.0,999.0};
// ����������
uint16_t HeatPower = 0;
// ���ȹ���
uint16_t FanPower = 0;

int main(void)
{
	incubatorInit();
	//	int i=0;
	while (1)
	{
//		int i;

//		while (usart1_avilable())
//		{
//			bbb[0] = usart1_read();
//			//			Delay_ms(1);
//			Delay_us(200);
//			USART_SendData(USART1, bbb[0]);
//		}
		
		if(TempUpdateFlag == 1)
		{
			SensorTemp_1[0]=DS18B20_1_Get_Temp() / 10.0;
			SensorTemp_2[0]=DS18B20_2_Get_Temp() / 10.0;
			#ifdef DEBUGIT
			usart2Printf("RawTemp [1] %.2f  [2] %.2f\n", SensorTemp_1[0], SensorTemp_2[0]);
			#endif
			sensorFilter(SensorTemp_1, SensorTemp_2);
			sensorFusion(SensorTemp_1, SensorTemp_2, SensorTempProcessed);
			#ifdef DEBUGIT
			usart2Printf("ProcessedTemp [*] %.3f\n", SensorTempProcessed[0]);
			#endif
			switch (ctrlMode)
			{
				case 0: 
				{
					break;
				}
				case 1: 
				{
					break;
				}
				case 2: 
				{
					HeatPower = PID_Control_1(SensorTempProcessed[0], TargetTemp);
					break;
				}
				default:
				{
					break;
				}
			}
			setPower_Heat1_Heat2(HeatPower, HeatPower);
			usart1_TmtTask();
			TempUpdateFlag = 0;
		}
		Delay_ms(500);
	}
}

int incubatorInit(void)
{ 
	int i; // ����������־
	
	// NVIC��ʼ����ȷ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	// USART��ʼ��
	USART_Config();
	#ifdef DEBUGIT
	Delay_ms(10);
	usart2Printf("MyIncubator START \n");
	usart2Printf("[ok] [1] NVIC ok! \n[ok] [2] USART ok! \n");
	Delay_ms(20);
	#endif
	// TIM3��ʼ��
	TIM3_PWM_Init();
	#ifdef DEBUGIT
	usart2Printf("[ok] [3] TIM3 ok! \n");
	Delay_ms(20);
	#endif
	// TIM4��ʼ��
	TIM4_Init();
	TIM_Cmd(TIM4, DISABLE); // �رն�ʱ�����ж�
	#ifdef DEBUGIT
	usart2Printf("[ok] [4] TIM4 ok! \n");
	Delay_ms(20);
	#endif
	// DS18B20_1��ʼ��
	for(i=0;i<3;i++)
	{
		if(DS18B20_1_Init() == 0)
		{
			float temp;
			temp = DS18B20_1_Get_Temp()/10.0;
			SensorTemp_1[0] = temp; 
			SensorTemp_1[1] = temp; 
			SensorTemp_1[2] = temp;
			break;
		}else
		{
			#ifdef DEBUGIT
			usart2Printf("[fail] [5] DS18B20_1 Check Failed!\n");
			while(1);
			#endif
		}
	}
	// DS18B20_2��ʼ��
	for(i=0;i<3;i++)
	{
		if(DS18B20_2_Init() == 0)
		{
			float temp;
			temp = DS18B20_2_Get_Temp()/10.0;
			SensorTemp_2[0] = temp; 
			SensorTemp_2[1] = temp; 
			SensorTemp_2[2] = temp;
			break;
		}else
		{
			#ifdef DEBUGIT
			usart2Printf("[fail] [5] DS18B20_2 Check Failed!\n");
			while(1);
			#endif
		}
	}
	#ifdef DEBUGIT
	usart2Printf("[ok] [5] DS18B20 ok! \n");
	Delay_ms(20);
	#endif
	// ����GPIO��ʼ��(led & switch)
  extraGPIOInit();
	#ifdef DEBUGIT
	usart2Printf("[ok] [6] extraGPIO ok! \n");
	Delay_ms(20);
	#endif
	Delay_ms(100);
	TIM_Cmd(TIM4, ENABLE); //������ʱ�����ж�

	return 1;
}