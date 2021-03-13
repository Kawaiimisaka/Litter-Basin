#include "usart3.h"

//////////////////////////////////////////////////////////////////
//??????,??printf??,??????use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//??????????                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//??_sys_exit()??????????    
_sys_exit(int x) 
{ 
	x = x; 
} 
//???fputc?? 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//Flag_Show=0  ????3   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/**************************************************************************
????:??3???
????:???
??  ?:?
**************************************************************************/
void usart3_init(uint32_t bound)
{  	 
	//GPIO????
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   //??UGPIOB??
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	   //??USART3??
	//USART3_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	           //??????
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART3_RX	  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;      //????
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart3 NVIC ??
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;   //?????
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		   //????
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQ????
	NVIC_Init(&NVIC_InitStructure);	                           //??????????VIC???
	//USART ?????
	USART_InitStructure.USART_BaudRate = bound;                //?????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//???8?????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //?????
	USART_InitStructure.USART_Parity = USART_Parity_No;        //??????
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//????????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//????
	USART_Init(USART1, &USART_InitStructure);                  //?????3
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);             //????????
	USART_Cmd(USART1, ENABLE);                                 //????3 
}




