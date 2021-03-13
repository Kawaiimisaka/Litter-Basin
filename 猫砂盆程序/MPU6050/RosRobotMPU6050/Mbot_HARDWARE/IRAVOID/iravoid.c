#include "iravoid.h"



void IRAvoid_GPIO_Init ()
{
	/*¶¨ÒåÒ»¸öGPIO_InitTypeDefÀàĞÍµÄ½á¹¹Ìå*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/*¿ªÆôÍâÉèÊ±ÖÓ*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	/*Ñ¡ÔñÒª¿ØÖÆµÄÒı½Å*/															   
  	GPIO_InitStructure.GPIO_Pin = Iravoid_Pin;	
	/*ÉèÖÃÒı½ÅÄ£Ê½ÎªÉÏÀ­ÊäÈëö*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*ÉèÖÃÒı½ÅËÙÂÊÎª50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*µ÷ÓÃ¿âº¯Êı£¬³õÊ¼»¯Servo_J4_PORT*/
  	GPIO_Init(Iravoid_Port, &GPIO_InitStructure);		
}

void GetIRavoid(int *a)
{
	*a = GPIO_ReadInputDataBit(Iravoid_Port,Iravoid_Pin);
}
