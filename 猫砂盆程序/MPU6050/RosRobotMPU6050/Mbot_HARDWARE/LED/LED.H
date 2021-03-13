#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "sys.h"

//IO≥ı ºªØ
void LED_Init(void);  
//LED…¡À∏∫Ø ˝
void Led_Flash(uint16_t time);

#define LED0 PCout(13)

#endif
