#include "sys.h"

//====================自己加入的头文件===============================
#include "delay.h"
#include "led.h"
#include "usart3.h"

#include "ioi2c.h"
#include "mpu6050.h"

#include "iravoid.h"

#include "motor.h"
#include "pwm.h"

#include "control.h"
#include "myexti.h"
#include <stdio.h>
//===================================================================

/*===================================================================
程序功能：MPU6050 DMP数据读取
程序编写：公众号：小白学移动机器人
其他    ：如果对代码有任何疑问，可以私信小编，一定会回复的。
=====================================================================
------------------关注公众号，获得更多有趣的分享---------------------
===================================================================*/

int main(void)
{ 

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁用JTAG 启用 SWD
	
	MY_NVIC_PriorityGroupConfig(2);	//=====设置中断分组
	
	delay_init();	    	        //=====延时函数初始化
	LED_Init();                     //=====LED初始化    程序灯
	usart3_init(9600);              //=====串口3初始化  蓝牙

  IIC_Init();                     //=====IIC初始化    读取MPU6050数据
	MPU6050_initialize();           //=====MPU6050初始化	
	DMP_Init();                     //=====初始化DMP 
	
	MBOT_EXTI_Init();               //=====MPU6050 5ms定时中断初始化
	IRAvoid_GPIO_Init();
	Motor_Init(7199,0);             //=====初始化PWM 10KHZ，用于驱动电机 如需初始化驱动器接口


	while(1)
	{
		int a = 0;
		GetIRavoid(&a);
		if(a == 0)                  //红外检测到猫
		{
			printf("cat is in");
			while(1)                  //一直等待，直到猫离开
			{
				GetIRavoid(&a);
				if(a == 1)
				{
					break;
				}
			}
			
			while(1)
		{
			getAngle(&yaw,&yaw_acc_error);      //获取角度
			Set_Pwm(1000,1000);
			printf("%d\r\n",(int)yaw);
			if((int)yaw==3)                     //转到预设角度后停止
			{
				Set_Pwm(0,0);
				break;
			}
		}
	  }
		__set_FAULTMASK(1);
		NVIC_SystemReset();
		
	} 
}


