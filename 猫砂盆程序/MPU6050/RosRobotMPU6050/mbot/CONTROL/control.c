#include "control.h"	
#include "led.h"

float yaw              =0;           //转向陀螺仪
float yaw_acc_error    =0;           //yaw累积误差
#define FIVE_MS_ERROR   0.00002115   //yaw每5ms的向上漂移的度数，这里近似线性，可以做到半小时偏1度，每个人的这个值可能有所不同，可以自行计算

/**************************************************************************
函数功能：所有的控制代码都在这里面
          5ms定时中断由MPU6050的INT引脚触发		
**************************************************************************/
void EXTI15_10_IRQHandler(void) 
{                                                         
	EXTI_ClearITPendingBit(EXTI_Line12);                            //===清除LINE12线路挂起位
	
	yaw_acc_error += FIVE_MS_ERROR;								    //===yaw漂移误差累加
	
	Led_Flash(200);                                                 //===LED闪烁，证明程序正常运行	
		
} 












