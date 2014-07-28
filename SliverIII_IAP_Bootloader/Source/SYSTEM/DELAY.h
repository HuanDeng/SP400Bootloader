
#ifndef __DELAY_H
#define __DELAY_H 
#include "stm32f10x.h"

void Delay_uS(u16 nCount);
void Delay_mS(u16 nCount);
void delayByTIM_Init(u8 SYSCLK);

#endif 