#include <stm32f10x.h>
#include "led.h"

void  Led_Init()
{
   	GPIO_InitTypeDef   GPIO_InitStructure; 


	GPIO_InitStructure.GPIO_Pin = LED_CONFIG_PIN;     //设置IO模式   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure); 						   	//使能TIMx	



	GPIO_InitStructure.GPIO_Pin = LED_SIGNAL_PIN;     //设置IO模式   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //必须要上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	LED_SIGNAL_OFF;
	LED_CONFIG_ON;
	
}