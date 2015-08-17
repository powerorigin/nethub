#ifndef _HAL_433RF_H
#define _HAL_433RF_H

#include <stm32f10x.h>
#define RF_DATA_PIN	 		   GPIO_Pin_9
#define RF_RECEIVE_PIN   	 GPIO_Pin_8
#define RF_DATA_PORT 		   GPIOB
#define RF_DATALEN    3


#define RF_SET_DATA   GPIO_SetBits(RF_DATA_PORT,RF_DATA_PIN)
#define RF_CLR_DATA   GPIO_ResetBits(RF_DATA_PORT,RF_DATA_PIN)

#define A4  200	  //窄脉冲
#define A12  400         //宽
#define A124 6000	  //同步

#define RF_DATA      0x10		   //无线遥控器

#define CONTROL_ALL_ON      0x09
#define CONTROL_ALL_OFF     0x0a
#define CONTROL_1_ON        0x01
#define CONTROL_1_OFF       0x02
#define CONTROL_2_ON        0x03
#define CONTROL_2_OFF       0x04
#define CONTROL_3_ON        0x05
#define CONTROL_3_OFF       0x06
#define CONTROL_4_ON        0x07
#define CONTROL_4_OFF       0x08

//extern u8 rf_flag;
#define RFSTART0            0x01
#define RFSTART1            0x02
#define RFDATA0             0x04
#define RFDATA1             0x08


#define RFDATALEN           20
#define RFSENDRFQUENCY      8                         //发送次数
#define REMOTECONTROL       1                         //遥控器
#define REMOTEDATALEN       4                         //遥控器数据长度

#define RFSTART_L_TIME              5500
#define RFSTART_H_TIME              6500
#define RFDATA0_L_TIME              350
#define RFDATA0_H_TIME              450
#define RFDATA1_L_TIME              150
#define RFDATA1_H_TIME              250 

#define MAX_SEND_NUM                3				 //RF重发次数
typedef struct	_rf_send										rf_send;
typedef struct	_rf_receive 									rf_receive;
struct	_rf_send
{
	uint8_t							flag;		     //RF数据头和数据标志
	uint8_t							data_len;		 //RF数据长度
	uint8_t					  		data[RFDATALEN]; //RF数据缓存	
	uint8_t					    	remote_flag;	 //RF遥控器数据和普通数据标志
	uint8_t                         frequency;		 //RF发送次数
};

struct	_rf_receive
{
//	uint8_t							flag;		     //RF数据头和数据标志
	uint8_t							data_len;		 //RF数据长度
	uint8_t					  		data[RFDATALEN]; //RF数据缓存	
};

extern rf_send	   m_rf_send;
extern rf_receive m_rf_receive;
extern uint8_t	  rf_state;
			   
#define RF_RECEIVE_STATE           0x01
#define RF_SEND_STATE              0x02

void RF_Init(void);
//void Wireless_control(uint8_t *data);
void Rf_Receive(void);
uint8_t crc8_check(uint8_t *ptr,uint8_t len);
u8 Send_Byte(u8 send_data_len, u8 remote_control_flag, u8 send_frequency);

#endif /*_HAL_433RF_H*/


	
			