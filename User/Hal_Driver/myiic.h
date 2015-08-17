#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//IO方向设置
#define SDA_IN() 		 GPIO_InitTypeDef GPIO_InitStruct;\
	                     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;\
	                     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;\
	                     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;\
	                     GPIO_Init(GPIOB, &GPIO_InitStruct);

#define SDA_OUT() 		 GPIO_InitTypeDef GPIO_InitStruct;\
	                     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;\
	                     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;\
	                     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;\
	                     GPIO_Init(GPIOB, &GPIO_InitStruct);

/*#define	SCL_H() 	    GPIO_SetBits(GPIOB,GPIO_Pin_1)                  
#define SCL_L()	      GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define	SDA_H() 	    GPIO_SetBits(GPIOB,GPIO_Pin_0)                  
#define SDA_L()	      GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define	CS_H() 	    GPIO_SetBits(GPIOB,GPIO_Pin_2)                  
#define CS_L()	      GPIO_ResetBits(GPIOB,GPIO_Pin_2)*/
//IO操作函数	 
#define IIC_SCL    		    PBout(1) //SCL
#define IIC_SDA    		    PBout(0) //SDA	 
#define READ_SDA  			PBin(0)  //输入SDA 
#define AT24CXX_CS 			PBout(2) //SCL

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















