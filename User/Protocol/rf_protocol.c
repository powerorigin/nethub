/**
  ******************************************************************************
  * @file    rf_protocol.c 
  * @author  msl
  * @version V1.1.0
  * @date    17-9-2014
  * @brief   无线通信协议
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "433RF.h"
#include "rf_protocol.h"
#include "protocol.h"
#include "stdio.h"
#include "string.h"
#include "ucos_ii.h"
#include "queue.h"
#include <includes.h>
#include "ht138x.h"
#include "24cxx.h" 

uint8_t  rf_device_list[MAX_DEVICE_NUMBER][DEVICE_DATA_LEN];  //在线设备列表
volatile uint8_t  rf_inquire_flag;                                     //反馈标志
uint8_t  temperature_humidity_new[2];						  //温湿度缓存
uint8_t  temperature_humidity_old[2];

queue_t  rf_send_queue;

extern m2w_mcuStatus							m_m2w_mcuStatus;
extern 	OS_EVENT *rf_send_event;

void Rf_Data_Init(void)
{
	memset(rf_device_list,0,MAX_DEVICE_NUMBER);
	rf_inquire_flag = 0;
}
/*******************************************************************************
* Function Name  :  Rf_Send_Process
* Description    : 串口数据转RF数据转发出去
* Input          : data：串口接收缓冲区地址
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Rf_Send_Process(uint8_t *data)			 
{
//	uint8_t str[20] = {'\0'};
  uint8_t i;

//	memcpy(str,data,5);
	Push(&rf_send_queue, data);
	if(rf_send_event->OSEventCnt == 0)
    {
		OSSemPost(rf_send_event);
	}
	else
	{
		rf_send_event->OSEventCnt = 0;
		OSSemPost(rf_send_event);
	}
	
//	Send_Byte(5,REMOTECONTROL,RFSENDRFQUENCY);

}
/*******************************************************************************
* Function Name  :  Host_Search_Device_Cmd
* Description    : 主机搜索设备命令
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Host_Search_Device_Cmd(uint8_t *data)
{
     uint8_t	i,j;
   memcpy(&timer_tab.timermin,&data[4],6);	
	 Set_Timer();
	 memcpy(&m_m2w_mcuStatus.status_w,data,3);
	 j = 0;
	 for(i=0;i<MAX_DEVICE_NUMBER;i++)
	 {
		 if(rf_device_list[i][CLASS] == 0)
			break; 
		 if((i+1)%4 == 0)
		{
			j++;
			memcpy(&m_m2w_mcuStatus.status_w.device_data[1],&rf_device_list[4*j-4],8);
			ReportStatus(REPORT_STATUS);								
		}
	 }	
	 memcpy(&m_m2w_mcuStatus.status_w.device_data[1],&rf_device_list[4*j],(MAX_DEVICE_NUMBER-4*j)*2);
	 ReportStatus(REPORT_STATUS);
}
/*******************************************************************************
* Function Name  :  Host_Search_Device_Cmd
* Description    : 主机删除设备命令
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Host_Delete_Device_Cmd(uint8_t *data)
{
	uint8_t	i,j;

	 for(i=0;i<MAX_DEVICE_NUMBER;i++)
	{
		if(rf_device_list[i][CLASS] == data[4]&&rf_device_list[i][ID] == data[5])
		{
			break;
		}
	}
	for(j=0;j<MAX_DEVICE_NUMBER;j++)
	{
		if(rf_device_list[j][CLASS] == 0)
			break;
	}
	for(;i<j;i++)
	{
		memcpy(&rf_device_list[i],&rf_device_list[i+1],2);	
	}
	memset(&rf_device_list[i],0,2);
	memcpy(&m_m2w_mcuStatus.status_w,data,6);
	ReportStatus(REPORT_STATUS);
	AT24CXX_Write(AT24CXX_DEVICE_LIST_ADDR,(u8 *)rf_device_list,sizeof(rf_device_list));
}

/*******************************************************************************
* Function Name  :  Host_Back_State_Cmd
* Description    : 主机查设备状态命令
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Host_Back_State_Cmd(uint8_t *data)
{
	 memcpy(&m_m2w_mcuStatus.status_w,&data,3);
	 m_m2w_mcuStatus.status_w.device_data[1] = temperature_humidity_new[0];
	 m_m2w_mcuStatus.status_w.device_data[2] = temperature_humidity_new[1];
	 ReportStatus(REPORT_STATUS);
}
/*******************************************************************************
* Function Name  :  Host_Configuration_Cmd
* Description    : 主机的配置命令解析
* Input          : data：串口接收缓冲区地址
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/

void Host_Configuration(uint8_t *data)
{ 	

	    switch(data[CMD])
		{
			case   HOST_SEARCH_CMD:	  
				   Host_Search_Device_Cmd(data);
			       break;
		    case   HOST_DELETE_CMD:	  
				   Host_Delete_Device_Cmd(data);
			       break;
			case   HOST_BACK_CMD:	 
			       Host_Back_State_Cmd(data);
			       break;
			default : break;	  
		}
}
/*******************************************************************************
* Function Name  :  Rf_Heart_package
* Description    : 主机定时发查询数据包
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Rf_Heart_package(void)
{
	uint8_t	i;
	uint8_t str[20] = {'\0'};
	
	for(i=0;i<MAX_DEVICE_NUMBER;i++)
	{
		if(rf_device_list[i][CLASS] == RF_DATA )
		{
		   str[0] = rf_device_list[i][CLASS];
			 str[1] = rf_device_list[i][ID];
			 str[2] = RF_HEART_CMD;
			 str[3] = timer_tab.timermonth;
			 str[4] = timer_tab.timerday;
			 Push(&rf_send_queue, str);
		 	 OSSemPost(rf_send_event);
			 OSTimeDlyHMSM(0,0,1,0); 	
		}
		else 	break;
	}
	  
}
/*******************************************************************************
* Function Name  :  Rf_Receive_Process
* Description    : 主机接收到RF数据处理
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Rf_Receive_Process(void)
{
	uint8_t	i;
	 if(memcmp(m_rf_send.data,m_rf_receive.data,3) == 0)
	 {
	   rf_inquire_flag = RF_INQUIRE_SUCCESS;
		 switch(m_rf_receive.data[0])
		 {
		     case   RF_DATA: if(m_rf_receive.data[ID] != 0x00&&m_rf_receive.data[CMD] == RF_SET_CMD)
			 				{
								 for(i=0;i<MAX_DEVICE_NUMBER;i++)
								 {
								 	if(rf_device_list[i][CLASS] == 0 || rf_device_list[i][ID] == m_rf_receive.data[ID])
									   break;
								 }
								 if(rf_device_list[i][CLASS] == 0)
								 {
									 rf_device_list[i][CLASS] = m_rf_receive.data[CLASS];
									 rf_device_list[i][ID] = m_rf_receive.data[ID];			
									 AT24CXX_Write(AT24CXX_DEVICE_LIST_ADDR,(u8 *)rf_device_list,sizeof(rf_device_list));									 
								//	 rf_device_list[i][LIST_STATE] = 1;
							 	 }
								 else 	m_rf_receive.data[ID] = 0;
							}
							break;
		   default  : break;
		 }
		memcpy(&m_m2w_mcuStatus.status_w,&m_rf_receive.data[0],12);
		ReportStatus(REPORT_STATUS);
		OSTimeDlyHMSM(0,0,0,250); 	//等待200ms反馈
		rf_state = 0;	
	}
	memset(m_rf_receive.data,0,sizeof(m_rf_receive.data));	
}

