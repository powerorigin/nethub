/**
  ******************************************************************************
  * @file    LT8900_protocol.c 
  * @author  msl
  * @version V1.1.0
  * @date    17-9-2014
  * @brief   2.4G通信
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/


#include "LT8900_protocol.h"
#include "LT8900.h"
#include "protocol.h"
#include "stdio.h"
#include "string.h"
#include "rf_protocol.h"
#include "24cxx.h" 

u8 Lt8900_Rxdata[DATALEN];
u8 Lt8900_Txdata[DATALEN];

u8 LEDFlag[10];		//当前LED的控制状态
u8 RetryFlag; 
u8 flag1,flag2,flag3;

extern OS_EVENT  *LT8900_Event;

#define unLT8900lock() OSSemPost(LT8900_Event);
#define LT8900lock() OSSemPend(LT8900_Event,0,&err);


extern m2w_mcuStatus							m_m2w_mcuStatus;
extern w2m_controlMcu							m_w2m_controlMcu;

extern uint8_t                   LEDGroup_Status;
extern uint16_t				  LEDGroup_Wait_Status;
extern uint8_t				  LEDGroup_MatchCode_Status;
extern uint8_t 						wait_ack_time;


extern void delay_us(uint32_t nus);
extern uint8_t crc8_check(uint8_t *ptr,uint8_t len);
void LT_init()
{
	LT8900IO_Init();
	
	RESET_L(); 	
	delay_us(1000);
	RESET_H(); 	
	delay_us(5000); 	
	CLK_L();
	
	Lt8900_Txdata[0] = 0x10;
	Lt8900_Txdata[1] = 0x01;
	Lt8900_Txdata[2] = 0x01;
	Lt8900_Txdata[3] = 0x05;
	Lt8900_Txdata[4] = 0x06 ;
	Lt8900_Txdata[5] =100;
	Lt8900_Txdata[6] =100;
	Lt8900_Txdata[7] =5; 
	
	LT_writereg(0, 0x6F, 0xE0);
	LT_writereg(1, 0x56, 0x81);
	LT_writereg(2, 0x66, 0x17);	
	LT_writereg(4, 0x9C, 0xC9);
	LT_writereg(5, 0x66, 0x37);	
	LT_writereg(7, 0x00, 0x00);	
	LT_writereg(8, 0x6C, 0x90);	
	LT_writereg(9, 0x18, 0x40);		
	LT_writereg(10, 0x7F, 0xFD);//crystal osc.enabled 
	LT_writereg(11, 0x00, 0x08);//rssi enabled 
	LT_writereg(12, 0x00, 0x00);
	LT_writereg(13, 0x48, 0xBD);
	LT_writereg(22, 0x00, 0xff);
	LT_writereg(23, 0x80, 0x05);
	LT_writereg(24, 0x00, 0x67);
	LT_writereg(25, 0x16, 0x59);
	LT_writereg(26, 0x19, 0xE0);
	LT_writereg(27, 0x13, 0x00);
	LT_writereg(28, 0x18, 0x00);	
	LT_writereg(32, 0x48, 0x00);
	LT_writereg(33, 0x3f, 0xC7);
	LT_writereg(34, 0x20, 0x00);
	LT_writereg(35, 0x03, 0x00);	
	LT_writereg(36, 0x05, 0x0a);
	LT_writereg(39, 0x55, 0xaa);	 
	LT_writereg(40, 0x44, 0x01);		
	LT_writereg(41, 0x30, 0x00);	 
	LT_writereg(42, 0xFD, 0xB0);
	LT_writereg(43, 0x00, 0x0F);		//configure scan_rssi	
	LT_writereg(50, 0x00, 0x00);
	LT_writereg(52,0x80,0x80);
	
	LT_writereg(7,0x00,0x80);					    //接收模式
	delay_us(10000);				   
}

void LT_TxData()
{
	u8 i,err,buf[2]={0xEE,0xEE};
	u16 time = 20000;
#if 1	
	flag3++;

	Lt8900_Txdata[5] = crc8_check(Lt8900_Txdata,5);
	UART1_SendBuf_DATA(buf,2);
	UART1_SendBuf_DATA(Lt8900_Txdata,SEND_DATA_LEN);
	UART1_SendBuf_DATA(buf,2);
	LT_writereg(7,0x00,0x00);	
	LT_writereg(52,0x80,0x00);//清空收发FIFO中的数据及指针
	SS_L();	
	SPI_write_byte(0x7f&50);	
	SPI_write_byte(SEND_DATA_LEN+1);
				
	for(i=0;i<SEND_DATA_LEN;i++)
	{
		SPI_write_byte(Lt8900_Txdata[i]);
	}
	SS_H();
	LT_writereg(7,0x01,0x00);			 			//发送模式
	while(READ_PKT==0&&time--);
#if 0	
			TIM_SetCounter(TIM3,0);
			LEDGroup_Wait_Status=TIM_GetCounter(TIM3);
			UART1_SendBuf_DATA((u8*)&LEDGroup_Wait_Status,1);
			wait_ack_time=0;
#endif
	LEDGroup_Wait_Status=0;
	//delay_us(2000);	
	flag2++;
	OSTimeDlyHMSM(0,0,0,2); 	//2ms延时，释放CPU控制权	
	LT_writereg(7,0x00,0x00);
	LT_writereg(52,0x00,0x80);
	LT_writereg(7,0x00,0x80);					    //接收模式
#endif
}

void LT_RxData()
{
	u8 i,data_len,err,buf[2]={0xDD,0xDD},BUF[3]={0},buf2[2]={0xBB,0xBB};
	u16 data;
	//LT_readreg(48);
	if(READ_PKT)	
	{
		SS_L();
		SPI_write_byte(0x80+50);           
		data_len = SPI_write_byte(0xff)-1;
		for(i=0;i<data_len;i++)
		{
			Lt8900_Rxdata[i] = SPI_write_byte(0xff); 			    
		}	
	    SS_H();		

		LT_writereg(7,0x00,0x00);	
		LT_writereg(52,0x00,0x80);
		LT_writereg(7,0x00,0x80);
		#if 0
		UART1_SendBuf_DATA(buf2,2);
		LEDGroup_Wait_Status=TIM_GetCounter(TIM3)+wait_ack_time*100-LEDGroup_Wait_Status;		
		BUF[0]=LEDGroup_Wait_Status;
		BUF[1]=LEDGroup_Wait_Status>>8;
		UART1_SendBuf_DATA(BUF,2);
		UART1_SendBuf_DATA(buf2,2);
		#endif

		UART1_SendBuf_DATA(buf,2);
		UART1_SendBuf_DATA(Lt8900_Rxdata,data_len);
		UART1_SendBuf_DATA(buf,2);

	#if 1	
		if((Lt8900_Rxdata[0]==Lt8900_Txdata[0])&&(Lt8900_Rxdata[1]==Lt8900_Txdata[1]))
		{
			if((m_w2m_controlMcu.status_w.device_cmd&GROUP_CONTROL_CMD)==GROUP_CONTROL_CMD)
			{	
				if((m_w2m_controlMcu.status_w.device_cmd&0x0f)==MATCODE_CONTROL_CMD)
				{
					LEDGroup_Wait_Status=0;		
					LEDGroup_Status=0;
					Set_LEDStatus(Lt8900_Rxdata,MATCODE_CONTROL_CMD);
					Lt8900_Rxdata[ID]=m_w2m_controlMcu.status_w.device_id;
				}
				else if((m_w2m_controlMcu.status_w.device_cmd&0x0f)==READ_CONTROL_CMD)
				{
				}
				else if((m_w2m_controlMcu.status_w.device_cmd&0x0f)==WRITE_CONTROL_CMD)
				{
				#if 1
					LEDFlag[LEDGroup_MatchCode_Status]=1;
					LEDGroup_Wait_Status=0;

					if(LEDGroup_Status==WRITE_CONTROL_CMD)
					{
						return;
					}

				#endif
				}
			}			
		}
	#endif
		memcpy(&m_m2w_mcuStatus.status_w,&m_w2m_controlMcu.status_w,6);
		flag1--;
		flag2--;
		flag3--;
		ReportStatus(REPORT_STATUS);
	}

}
void LT_RetryData(void)
{
	static u8 Retry;
	if(LEDGroup_Status)
	{
		if(LEDGroup_Status==MATCODE_CONTROL_CMD)
		{
			if(LEDGroup_Wait_Status>1)
			{
				LT_LEDStatus();
			}
			RetryFlag++;
			if(RetryFlag==2)
			{
				LEDGroup_Status=0;
				RetryFlag=0;
			}
		}
		else if(LEDGroup_Status==READ_CONTROL_CMD)
		{
		}
		else if(LEDGroup_Status==WRITE_CONTROL_CMD)
		{
#if 1			
			if(LEDGroup_Wait_Status>1)
			{
				if(LEDFlag[LEDGroup_MatchCode_Status]==0)
				{
					Retry=1;
				}
				LEDGroup_MatchCode_Status++;
				for(;LEDFlag[LEDGroup_MatchCode_Status]&&LEDGroup_MatchCode_Status<10;LEDGroup_MatchCode_Status++);				
				LT_LEDStatus();
			}
			if(LEDGroup_Status==0)
			{	
				if(Retry==1&&RetryFlag<3)
				{
					LEDGroup_Status=WRITE_CONTROL_CMD;
					LEDGroup_MatchCode_Status=0;
					LEDGroup_Wait_Status=0;
					RetryFlag++;
					Retry=0;
					for(;LEDFlag[LEDGroup_MatchCode_Status]&&LEDGroup_MatchCode_Status<10;LEDGroup_MatchCode_Status++);				
					LT_LEDStatus();
				}
				else
				{
					Retry=0;
					RetryFlag=0;
					memset(LEDFlag,0,10);
					memcpy(&m_m2w_mcuStatus.status_w,&m_w2m_controlMcu.status_w.device_cmd,6);
					ReportStatus(REPORT_STATUS);
				}
			}
#endif
		}
	}
}

