#include "433RF.h"
#include "stdio.h"
#include "ucos_ii.h"
#include "rf_protocol.h"
#include "protocol.h"
#include <includes.h>

/********************************************************
		   PT2262���� ϵͳʱ��12MHz	 ͬ����124A=16.5ms
		   4A = 500us ��ʱ�����500�� ��ֵ=65536-500=65036=fe0c
		   12A =1500us ��ʱ�����1500��	��ֵ=65536-1500=64036=fa24
		   32A =4000us ��ʱ�����4000��	��ֵ=65536-4000=61536=f060
		   124A=15.5mus ��ʱ�����15500�� ��ֵ=65536-15500=50036=c374
		   �񵴵���PT2262/1.2M��PT2272/200K���
********************************************************/



rf_send	   m_rf_send;
rf_receive m_rf_receive;

extern m2w_mcuStatus							m_m2w_mcuStatus;
uint8_t	  rf_state;
uint8_t  send_num = 0;
u8 const Rf_code[10][3] = {0xfb,0xff,0x6f,
                           0xfb,0xff,0x5f,
						   0xfb,0xff,0xef,
						   0xfb,0xff,0xaf,
						   0xfb,0xff,0xdf,
						   0xfb,0xff,0x9f,
						   0xfb,0xff,0xbf,
						   0xfb,0xff,0x7f,
						   0xfb,0xff,0xcf,
						   0xfb,0xff,0x8f};

static void TIM3_Init(void);
static void TIM4_Init(void);
static void  Rf_IO_Init();

void RF_Init(void)
{
	TIM3_Init();
	TIM4_Init();
	Rf_IO_Init();
//	m_rf_receive.flag = 0;	
	rf_state = 0;		 																	
}

static void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 						//ʱ��ʹ��
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 65535; 												//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 											//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 								//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 											//ʹ��ָ����TIM3�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  								//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  			//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  						//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 								//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  																//��ʼ��NVIC�Ĵ��� 
}

static void  TIM4_Init()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 						//ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��					  
	TIM_TimeBaseStructure.TIM_Period = 20000; 												//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 											//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 								//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); 											//ʹ��ָ����TIM3�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  								//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  			//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  						//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 								//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  																//��ʼ��NVIC�Ĵ���
}

static void  Rf_IO_Init()
{
   	GPIO_InitTypeDef   GPIO_InitStructure; 
	EXTI_InitTypeDef   EXTI_InitStructure;  
 	NVIC_InitTypeDef NVIC_InitStructure;

//	TIM_Cmd(TIM2, ENABLE);  	
	


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //����IO ʱ��   	
		/* Configure PA.00 pin as input floating */   
	GPIO_InitStructure.GPIO_Pin = RF_DATA_PIN;     //����IOģʽ   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(RF_DATA_PORT, &GPIO_InitStructure); 						   	//ʹ��TIMx	



	GPIO_InitStructure.GPIO_Pin = RF_RECEIVE_PIN;     //����IOģʽ   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //����Ҫ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(RF_DATA_PORT, &GPIO_InitStructure);  

	   /* Connect EXTI0 Line to PA.00 pin */    
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8); //�趨�����ж��ߡ�
		
	
    EXTI_ClearITPendingBit(EXTI_Line8);       //�����·����λ  
	/* Configure EXTI0 line */    
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;     //�����ж�����   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;    //�����ش���   
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;   
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;      //�����ж����ȼ�    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;   
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;   
	NVIC_Init(&NVIC_InitStructure);
}
/****************************************************************************
*   Function name ��crc8_check
*   Description   ������CRC8У�飬������1�ֽ�У���루����ʽ��X8 + X5 + X4 + 1��
*   Call By       ��rc_data_dispose
*   Input         ��(uint8 *ptr,uint8 len)У�����ݡ�У���ֽ���
*   Return        : (uint8 crc8)1�ֽ�CRCУ����
*****************************************************************************/
uint8_t crc8_check(uint8_t *ptr,uint8_t len)
{
	uint8_t i,j=0;
	uint8_t crc8=0;								//crc8У���뻺��

	while(len--)
	{
		crc8 ^= ptr[j++];
		for(i = 0;i < 8;i++)
		{
			if(crc8 & 0x80)
				crc8 = (crc8 << 1) ^ 0x0131;
			 else 
				crc8 <<= 1;
		}
	}
	return crc8;								//����CRC8У����
}


u8 Send_Byte(u8 send_data_len, u8 remote_control_flag, u8 send_frequency)
{ 	
/********ͬ��ͷ*************/
//	static  uint8_t  send_num = 0;
 /*  while(rf_state != 0)
   {
   		OSTimeDlyHMSM(0,0,0,100); 	//�ȴ�100ms���ͽ���
   }	*/
	   rf_inquire_flag = RF_SEND_ING;
	   send_num = 0;
	   while(send_num < MAX_SEND_NUM && rf_inquire_flag != 0)
	   {
	        
			m_rf_send.data_len = send_data_len;
			m_rf_send.remote_flag =  remote_control_flag;
			m_rf_send.frequency =  0;
			m_rf_send.flag = RFSTART0;
		
			rf_state = RF_SEND_STATE;
			RF_SET_DATA;
			TIM_SetCounter(TIM2,65536 - A4);
			TIM_Cmd(TIM2, ENABLE); 
	
			OSTimeDlyHMSM(0,0,0,500); 	//�ȴ�500ms����
																																		     
			send_num++; 
	    if(rf_inquire_flag == RF_INQUIRE_SUCCESS)
			{
				rf_inquire_flag = 0;

			    break;
			}				
				 
		}
		if(rf_inquire_flag != 0)
		{
			memcpy(&m_m2w_mcuStatus.status_w,&m_rf_send.data[0],5);
			m_m2w_mcuStatus.status_w.device_cmd = 0xff;
			ReportStatus(REPORT_STATUS);
		}							  
		 rf_inquire_flag = 0;
	/*	if(send_num == MAX_SEND_NUM)
		{
			
		}  */
}	   

void Rf_Receive(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure;

	 static u8 last_state = 0;
	 u16 time;
	 OS_CPU_SR  cpu_sr;
	 
	 if(rf_state == 0)
	 { 	    
   		 if((GPIO_ReadInputDataBit(RF_DATA_PORT,RF_RECEIVE_PIN) == 0)&&(last_state == 1))
		 {
		   		TIM_SetCounter(TIM4,0);
				TIM_Cmd(TIM4, ENABLE); 	
					
				last_state = 0;

		 }
		 else  if((GPIO_ReadInputDataBit(RF_DATA_PORT,RF_RECEIVE_PIN) == 1)&&(last_state == 0))
		 {
		       last_state = 1;
		       time = TIM_GetCounter(TIM4);
               
			   	if(time > RFSTART_L_TIME && RFSTART_H_TIME > time)
			   {					      
			   	   /* Enable and set EXTI0 Interrupt to the lowest priority */   
				//	EXTI->SWIER = EXTI_Line4;
					    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
					EXTI_ClearITPendingBit(EXTI_Line8);       //�����·����λ  
					NVIC->ISER[23 >> 0x05] = (uint32_t)0x01 << (23 & (uint8_t)0x1F);
					TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  		//���TIMx�����жϱ�־
										
			//		TIM_Cmd(TIM4, ENABLE);
			        OS_EXIT_CRITICAL();
					TIM_SetCounter(TIM4,0); 	
					rf_state = RF_RECEIVE_STATE;
					m_rf_receive.data_len = 0;
					memset(m_rf_receive.data, 0, RFDATALEN);
				       
			   }
			   else TIM_Cmd(TIM4, DISABLE);	
		 }
	 }
}
/*	
void Wireless_control(uint8_t *data)
{
    u8 len;
	switch(data[1])
	{
	 	  case CONTROL_ALL_ON: memcpy(m_rf_send.data,Rf_code[0],3);
				                     break;	
		  case CONTROL_ALL_OFF:memcpy(m_rf_send.data,Rf_code[1],3);
				                     break;	
		  case CONTROL_1_ON:   m_rf_send.data[0] = 0x10;
		                       m_rf_send.data[1] = 0x01;
							   m_rf_send.data[2] = 0x01;
							   m_rf_send.data[3] = 0x01;
							   m_rf_send.data[4] = Rf_CheckSum( m_rf_send.data,5);
							   len = 5;
				                     break;		
		  case CONTROL_1_OFF:  m_rf_send.data[0] = 0x10;
		                       m_rf_send.data[1] = 0x01;
							   m_rf_send.data[2] = 0x01;
							   m_rf_send.data[3] = 0x00;
							   m_rf_send.data[4] = Rf_CheckSum( m_rf_send.data,5);
							   len = 5;
				                     break;		
		  case CONTROL_2_ON:   m_rf_send.data[0] = 0x10;
		                       m_rf_send.data[1] = 0x01;
							   m_rf_send.data[2] = 0x02;
							   m_rf_send.data[3] = Rf_CheckSum( m_rf_send.data,4);
							   len = 4;
				                     break;		
		  case CONTROL_2_OFF:  m_rf_send.data[0] = 0x10;
		                       m_rf_send.data[1] = 0x01;
							   m_rf_send.data[2] = 0x04;
							   m_rf_send.data[3] = Rf_CheckSum( m_rf_send.data,4);
							   len = 4;
				                     break;		
		  case CONTROL_3_ON:   memcpy(m_rf_send.data,Rf_code[6],3);
				                     break;		
		  case CONTROL_3_OFF:  memcpy(m_rf_send.data,Rf_code[7],3);
				                     break;		
		  case CONTROL_4_ON:   memcpy(m_rf_send.data,Rf_code[8],3);
				                     break;		
		  case CONTROL_4_OFF:  memcpy(m_rf_send.data,Rf_code[9],3);
				                     break;							 					 					 					 					 					 					 					 						 	
			default :break;					 					    		
	}	   
 
    Send_Byte(len,REMOTECONTROL,RFSENDRFQUENCY); 
}			 	 */