#include "stm32f10x.h"
#include "delay.h"   

#define DelayTIM TIM1 //TIM 1 2 3 4 ���á�
#define RCC_DelayTIM RCC_APB2Periph_TIM1
void delayByTIM_Init(u8 SYSCLK)//SYSCLK=system clock/1M
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 1;                 //�Զ�װ��
	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK;       //72M��Ƶ�ʵ�1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //���¼���
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);

	RCC_APB2PeriphClockCmd(RCC_DelayTIM, ENABLE);//����޸��� ʹ�õ�TIM �˴�ҲӦ������Ӧ�޸�
}
void Delay_uS(u16 nCount)//��������65535uS һֱ�ȴ� nCount ��1Mhz�ݼ���0 ���ж�ģʽ
{
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while (DelayTIM->CNT>1);
  TIM_Cmd(DelayTIM, DISABLE);
}
void Delay_mS(u16 nCount)//��������65mS
{
  nCount*=1000;	
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while (DelayTIM->CNT>1);
  TIM_Cmd(DelayTIM, DISABLE);
}  

