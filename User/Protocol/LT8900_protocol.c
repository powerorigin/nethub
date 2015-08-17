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

extern m2w_mcuStatus							m_m2w_mcuStatus;

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
	u8 i;
	u16 time = 20000;
	
	Lt8900_Txdata[5] = crc8_check(Lt8900_Txdata,5);
	LT_writereg(7,0x00,0x00);	
	LT_writereg(52,0x80,0x80);
	SS_L();	
	SPI_write_byte(50);	
	SPI_write_byte(SEND_DATA_LEN+1);
			
	for(i=0;i<SEND_DATA_LEN;i++)
	{
		SPI_write_byte(Lt8900_Txdata[i]);
	}
	SS_H();
	LT_writereg(7,0x01,0x00);			 			//发送模式
	
  while(READ_PKT==0&&time--);
	
	delay_us(2000);	
	LT_writereg(52,0x80,0x80);
	LT_writereg(7,0x00,0x80);					    //接收模式
}

void LT_RxData()
{
	u8 i,data_len;
	//LT_readreg(48);
	if(READ_PKT)	
	{
		SS_L();
		SPI_write_byte(0x80+50);           
		data_len = SPI_write_byte(0xff);
		for(i=0;i<data_len;i++)
		{
				Lt8900_Rxdata[i] = SPI_write_byte(0xff); 			    
		}	
    SS_H();
		if(Lt8900_Rxdata[0] == LT8900_DATA && Lt8900_Rxdata[data_len-2] == crc8_check(Lt8900_Rxdata,data_len-2))
		{
			  if(Lt8900_Rxdata[CMD] == 0x01)
			 	{
						for(i=0;i<MAX_DEVICE_NUMBER;i++)
						{
								 	if(rf_device_list[i][CLASS] == 0 || rf_device_list[i][ID] == Lt8900_Rxdata[ID]|| (rf_device_list[i][ID] == Lt8900_Rxdata[ID]&&rf_device_list[i][CLASS] == Lt8900_Rxdata[CLASS]))
									   break;
					  }
					 if(rf_device_list[i][CLASS] == 0 || rf_device_list[i][ID] == 0)
					 {
									 rf_device_list[i][CLASS] = Lt8900_Rxdata[CLASS];
									 rf_device_list[i][ID] = Lt8900_Rxdata[ID];			
									 AT24CXX_Write(AT24CXX_DEVICE_LIST_ADDR,(u8 *)rf_device_list,sizeof(rf_device_list));									 
								//	 rf_device_list[i][LIST_STATE] = 1;
				   }
					 else Lt8900_Rxdata[ID] = 0;
				}
		}
		memcpy(&m_m2w_mcuStatus.status_w,&Lt8900_Rxdata[0],6);
		ReportStatus(REPORT_STATUS);
		LT_writereg(7,0x00,0x00);	
		LT_writereg(52,0x80,0x80);
		LT_writereg(7,0x00,0x80);	
	
	}
	
}
