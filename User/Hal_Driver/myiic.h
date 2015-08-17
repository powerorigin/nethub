#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//IO��������
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
//IO��������	 
#define IIC_SCL    		    PBout(1) //SCL
#define IIC_SDA    		    PBout(0) //SDA	 
#define READ_SDA  			PBin(0)  //����SDA 
#define AT24CXX_CS 			PBout(2) //SCL

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















