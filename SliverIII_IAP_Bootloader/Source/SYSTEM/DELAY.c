#include "stm32f10x.h"
#include "delay.h"   

#define DelayTIM TIM1 //TIM 1 2 3 4 可用。
#define RCC_DelayTIM RCC_APB2Periph_TIM1
void delayByTIM_Init(u8 SYSCLK)//SYSCLK=system clock/1M
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 1;                 //自动装载
	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK;       //72M分频率到1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //向下计数
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);

	RCC_APB2PeriphClockCmd(RCC_DelayTIM, ENABLE);//如果修改了 使用的TIM 此处也应该做相应修改
}
void Delay_uS(u16 nCount)//最大可阻塞65535uS 一直等待 nCount 以1Mhz递减到0 非中断模式
{
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while (DelayTIM->CNT>1);
  TIM_Cmd(DelayTIM, DISABLE);
}
void Delay_mS(u16 nCount)//最大可阻塞65mS
{
  nCount*=1000;	
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while (DelayTIM->CNT>1);
  TIM_Cmd(DelayTIM, DISABLE);
}  

