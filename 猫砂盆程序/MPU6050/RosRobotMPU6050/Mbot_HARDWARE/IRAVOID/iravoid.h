#ifndef __IRAVOID_H
#define __IRAVOID_H



#define Iravoid_Port GPIOA
#define Iravoid_Pin  GPIO_Pin_5

#include "stm32f10x.h"

void IRAvoid_GPIO_Init();
void GetIRavoid(int *a);

#endif
