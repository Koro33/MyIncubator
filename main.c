#include "main.h"
#define DEBUGIT

//// test
//uint8_t bbb[100];
//static uint8_t temp[3] = {0x32, 0x00, 0x65};
//static uint8_t temp1[3] = {65, 10, 23};
// 采样缓冲区，以及滤波后数据存储区，每次新采样均存储在数组的第一个位置，最后一个数据被清除，剩余数据依次后移
float SensorTemp_1[3] = {0, 0, 0};
float SensorTemp_2[3] = {0, 0, 0};
// 融合数据存储区，融合后的数据存储在数组的第一个位置，最后一个数据被清除，剩余数据依次后移
float SensorTempProcessed[3] = {0, 0, 0};
// 数据更新的标志，当定时器4中断中更新了温度采样数据此值为1，未更新此值为0
int8_t TempUpdateFlag = 0;
// 目标温度
float TargetTemp = 30.0;
// usart1 发送缓冲区
char U1_rxBuffer[128];
// usart1 接收缓冲区
char U1_txBuffer[128];
// usart2 发送缓冲区
char U2_rxBuffer[64];
// 与串口屏的功能切换标志 0：主界面更新温度  1：图表界面更新曲线
int8_t Usart1TmtChoiceFlag = 0;
// 控制方式的选择，0：手动 1：BB控制 2：PID控制
int8_t CtrlMode = 0;
// PID控制器参数 	(1) 150.0, 0.13, 0.0, 999.0 稳定在40度 存在上下两次过冲（+1.5 -1.5）
// 								(2) 150.0, 0.13, 0.0, 999.0 稳定在40度 存在上下两次过冲（+1.5 -1.5）
struct PID PID_1 = {150.0, 0.13, 0.0, 999.0};
// 加热器功率
uint16_t HeatPower = 0;
// 风扇功率
uint16_t FanPower = 0;

int main(void)
{
  incubatorInit();
  while (1)
  {
    //		int i;

    if (usart1_avilable() == 1)
    {
      usart1_RevTask();
    }

    if (TempUpdateFlag == 1)
    {
#ifdef DEBUGIT
      usart2Printf("RawTemp [1] %.2f  [2] %.2f\n", SensorTemp_1[0], SensorTemp_2[0]);
#endif
      sensorFilter(SensorTemp_1, SensorTemp_2);
      sensorFusion(SensorTemp_1, SensorTemp_2, SensorTempProcessed);
#ifdef DEBUGIT
      usart2Printf("ProcessedTemp [*] %.3f\n", SensorTempProcessed[0]);
			usart2Printf("%.3f ", SensorTempProcessed[0]);
			usart2Printf("TargetTemp %.2f ", TargetTemp);
			usart2Printf("PID_1.Kp %.2f ", PID_1.Kp);
			usart2Printf("PID_1.Ki %.2f ", PID_1.Ki);
			usart2Printf("PID_1.Kd %.2f\n", PID_1.Kd);
			usart2Printf("HEATPOWER %d\n", HeatPower);
#endif
      switch (CtrlMode)
      {
				case 0:
				{
					break;
				}
				case 1:
				{
					FanPower = 999;
					HeatPower = BB_Control_1(SensorTempProcessed[0], TargetTemp);
					usart2Printf("BB mode\n");
					break;
				}
				case 2:
				{
					FanPower = 999;
					HeatPower = (uint16_t)PID_Control_1(SensorTempProcessed[0], TargetTemp);
					usart2Printf("PID mode\n");
					break;
				}
				default:
				{
					break;
				}
      }
      setPower_Heat1_Heat2(HeatPower, HeatPower);
      setPower_Fan(FanPower);
      usart1_TmtTask();
      TempUpdateFlag = 0;
    }
    //		Delay_ms(100);
  }
}

int incubatorInit(void)
{
  int i; // 重启次数标志

  // NVIC初始化，确定组号
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  // USART初始化
  USART_Config();
#ifdef DEBUGIT
  Delay_ms(10);
  usart2Printf("MyIncubator START \n");
  usart2Printf("[ok] [1] NVIC ok! \n[ok] [2] USART ok! \n");
  Delay_ms(20);
#endif
  // TIM3初始化
  TIM3_PWM_Init();
#ifdef DEBUGIT
  usart2Printf("[ok] [3] TIM3 ok! \n");
  Delay_ms(20);
#endif

  // DS18B20_1初始化
  for (i = 0; i < 3; i++)
  {
    if (DS18B20_1_Init() == 0)
    {
      float temp;
      temp = DS18B20_1_Get_Temp() / 10.0;
      if ((temp > 5.0) && (temp < 90.0))
      {
				SensorTemp_1[0] = temp;
				SensorTemp_1[1] = temp;
				SensorTemp_1[2] = temp;
      }
      else
      {
				temp = 25;
				SensorTemp_1[0] = temp;
				SensorTemp_1[1] = temp;
				SensorTemp_1[2] = temp;
      }
      break;
    }
    else
    {
#ifdef DEBUGIT
      usart2Printf("[fail] [4] DS18B20_1 Check Failed!\n");
      while (1)
				;
#endif
    }
  }
  // DS18B20_2初始化
  for (i = 0; i < 3; i++)
  {
    if (DS18B20_2_Init() == 0)
    {
      float temp;
      temp = DS18B20_2_Get_Temp() / 10.0;
      if ((temp > 5.0) && (temp < 90.0))
      {
				SensorTemp_2[0] = temp;
				SensorTemp_2[1] = temp;
				SensorTemp_2[2] = temp;
      }
      else
      {
				temp = 25;
				SensorTemp_2[0] = temp;
				SensorTemp_2[1] = temp;
				SensorTemp_2[2] = temp;
      }
      break;
    }
    else
    {
#ifdef DEBUGIT
      usart2Printf("[fail] [4] DS18B20_2 Check Failed!\n");
      while (1)
				;
#endif
    }
  }
#ifdef DEBUGIT
  usart2Printf("[ok] [4] DS18B20 ok! \n");
  Delay_ms(20);
#endif
  // 其他GPIO初始化(led & switch)
  extraGPIOInit();
#ifdef DEBUGIT
  usart2Printf("[ok] [5] extraGPIO ok! \n");
  Delay_ms(20);
#endif
	// HMILCD初始化
	usart1_InitTmtTask();
#ifdef DEBUGIT
  usart2Printf("[ok] [6] LCD ok! \n");
  Delay_ms(20);
#endif
  // TIM4初始化
  TIM4_Init();
#ifdef DEBUGIT
  usart2Printf("[ok] [7] TIM4 ok! \n");
  Delay_ms(20);
#endif

  Delay_ms(750); // ds18b20两次采样时间间隔不小于750ms

  return 1;
}
