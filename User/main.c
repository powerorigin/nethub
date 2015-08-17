/*******************************************************************************
1、本工程是GoKit 的 最 新 版 本 工 程；
2、mcu开发者可以关心protocol.h 和 protocol.c这两个文件；
3、用于传输的通用协议（非P0），mcu开发者可以最大程度的复用此工程源码；
4、mcu开发者重点关注protocol.c中的CmdSendMcuP0和CmdReportModuleStatus这两个函数（需要控制外设和处理外设状态）；
5、其余通用的通讯流程，可以不做改变或者替换改变；
6、有任何问题可以关注机智云开发者论坛；
*******************************************************************************/

#include <stm32f10x.h>
#include <includes.h>
#include "stm32f10x_iwdg.h"
#include "ucos_ii.h"
#include "hal_uart.h"
#include "hal_key.h"
//#include "hal_rgb_led.h"
#include "rf_protocol.h"
//#include "hal_motor.h"
#include "hal_temp_hum.h"
#include "protocol.h"
//#include "hal_infrared.h"
#include "433RF.h"
//#include "IR.h"
#include "LT8900_protocol.h"
#include "queue.h"
#include "24cxx.h" 
#include "ht138x.h"
#include "led.h"


static  OS_STK  message_task_stk[MESSAGE_TASK_STK_SIZE];   //开辟任务堆栈
//static  OS_STK  key_task_stk[KEY_TASK_STK_SIZE];    //开辟任务堆栈
static  OS_STK  checkstatus_task_stk[CHECKSTATUS_TASK_STK_SIZE];    //开辟任务堆栈
//static  OS_STK  rf_heart_task_stk[RFHEART_TASK_STK_SIZE];    //开辟任务堆栈
static  OS_STK  rf_receive_task_stk[RFRECEIVE_TASK_STK_SIZE];    //开辟任务堆栈
static  OS_STK  rf_send_task_stk[RFSEND_TASK_STK_SIZE];    //开辟任务堆栈

OS_EVENT  *message_event,*rf_receive_event,*rf_send_event;
//OS_STK_DATA  stk_data;
INT8U err;

extern queue_t  rf_send_queue;

static void systick_init(void); //函数声明

uint8_t 									get_one_package;							//判断是否接收到一个完整的串口数据包
uint8_t 									uart_buf[256]; 								//串口缓冲区
uint16_t 									uart_Count=0;									//串口缓冲区数据长度
uint8_t 									cmd_flag =0;									
uint16_t 									cmd_len =0 ;		
uint8_t 									wait_ack_time;								
//uint8_t 									check_status_time;
uint8_t 									report_status_idle_time;
uint32_t									SN;
uint8_t 									cmd_flag1, cmd_flag2;
uint32_t									wait_wifi_status;
uint8_t                   wifi_status;

pro_commonCmd							m_pro_commonCmd;							//通用命令，心跳、ack等可以复用此帧
m2w_returnMcuInfo					m_m2w_returnMcuInfo;					//返回mcu信息帧
m2w_setModule							m_m2w_setModule;							//配置模块帧
w2m_controlMcu						m_w2m_controlMcu;							//控制命令帧
m2w_mcuStatus							m_m2w_mcuStatus;							//当前最新的mcu状态帧
m2w_mcuStatus							m_m2w_mcuStatus_reported;			//上次发送的mcu状态，当与最新的mcu状态不同时，需要上报；
w2m_reportModuleStatus		m_w2m_reportModuleStatus;			//wifi模块上报状态帧
pro_errorCmd							m_pro_errorCmd;								//错误命令帧


 
int	McuStatusInit()
{
	SN = 0;
	cmd_flag1 = 0;
	cmd_flag2 = 0;
	get_one_package = 0;
	uart_Count = 0;
	cmd_flag = 0;
	cmd_len = 0 ;	
	wait_wifi_status = 0;
	wifi_status= 0;
	
	memset(uart_buf, 0, 256);
	
	//初始化通用命令帧，命令字和sn需要传入，校验和发送前计算，其他信息相同；
	memset(&m_pro_commonCmd, 0, sizeof(pro_commonCmd));
	m_pro_commonCmd.head_part.head[0] = 0xFF;
	m_pro_commonCmd.head_part.head[1] = 0xFF;
	m_pro_commonCmd.head_part.len = exchangeBytes(sizeof(pro_commonCmd) - 4);

	//初始化返回mcu信息帧，sn和校验和需要根据实际填写；
	memset(&m_m2w_returnMcuInfo, 0, sizeof(m2w_returnMcuInfo));
	m_m2w_returnMcuInfo.head_part.head[0] = 0xFF;
	m_m2w_returnMcuInfo.head_part.head[1] = 0xFF;
	//长度值不包含包头和长度字段，所以要减去4个字节；
	m_m2w_returnMcuInfo.head_part.len = exchangeBytes(sizeof(m2w_returnMcuInfo) - 4);			
	m_m2w_returnMcuInfo.head_part.cmd = CMD_GET_MCU_INFO_ACK;
	memcpy(m_m2w_returnMcuInfo.pro_ver, PRO_VER, 8);
	memcpy(m_m2w_returnMcuInfo.p0_ver, P0_VER, 8);
	memcpy(m_m2w_returnMcuInfo.hard_ver, HARD_VER, 8);
	memcpy(m_m2w_returnMcuInfo.soft_ver, SOFT_VER, 8);
	memcpy(m_m2w_returnMcuInfo.product_key, PRODUCT_KEY, 32);
	//binable_time默认0，可以随时被绑定；
	m_m2w_returnMcuInfo.binable_time = 0;																		
		
	//初始化mcu状态帧，sn和校验和需要根据实际填写；
	memset(&m_m2w_mcuStatus, 0, sizeof(m2w_mcuStatus));
	m_m2w_mcuStatus.head_part.head[0] = 0xFF;
	m_m2w_mcuStatus.head_part.head[1] = 0xFF;
	m_m2w_mcuStatus.head_part.len = exchangeBytes(sizeof(m2w_mcuStatus) - 4);
//	DHT11_Read_Data((uint8_t *)&(m_m2w_mcuStatus.status_r.temputure), (uint8_t *)&(m_m2w_mcuStatus.status_r.humidity));
//	m_m2w_mcuStatus.status_w.motor_speed = 5;
	
	//初始化配置wifi模块帧，sn和校验和需要根据实际填写；
	memset(&m_m2w_setModule, 0, sizeof(m2w_setModule));
	m_m2w_setModule.head_part.head[0] = 0xFF;
	m_m2w_setModule.head_part.head[1] = 0xFF;
	m_m2w_setModule.head_part.cmd = CMD_SET_MODULE_WORKMODE;
	m_m2w_setModule.head_part.len = exchangeBytes(sizeof(m2w_setModule) - 4);

	//初始化错误命令帧，sn和校验和需要根据实际填写；
	memset(&m_pro_errorCmd, 0, sizeof(pro_errorCmd));
	m_pro_errorCmd.head_part.head[0] = 0xFF;
	m_pro_errorCmd.head_part.head[1] = 0xFF;
	m_pro_errorCmd.head_part.cmd = CMD_MODULE_CMD_ERROR_ACK;
	m_pro_errorCmd.head_part.len = exchangeBytes(sizeof(pro_errorCmd) - 4);
	
	
	return 0;
}

void  Mcu_Data_Init(void)
{
	AT24CXX_Check();
	AT24CXX_Read(AT24CXX_DEVICE_LIST_ADDR,(u8 *)rf_device_list,sizeof(rf_device_list));	
	AT24CXX_Read(AT24CXX_TIME_ADDR,&timer_tab.timersec,sizeof(timer_tab));	
	Set_Timer();	
}
void Wdt_Init(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);    /*WWDG????*/
 // Enable write access to IWDG_PR and IWDG_RLR registers 
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //IWDG->KR = 0x5555
 // IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz 
  IWDG_SetPrescaler(IWDG_Prescaler_256);   //IWDG->PR = 0x06;

 // Set counter reload value to 1250 
  IWDG_SetReload(781);           //IWDG->RLR =0xFFF           //5S不喂狗复位

  IWDG_ReloadCounter();   //IWDG->KR = 0xAAAA

  IWDG_Enable();   //IWDG->KR = 0xCCCC

}
void  BSP_Init (void)
{
	//系统初始化
	SystemInit();
	systick_init();     /* Initialize the SysTick. */  
	Wdt_Init();
 	UART_Configuration();
	KEY_GPIO_Init();
	TIM3_Int_Init(100,7199);
	Rf_Data_Init();
	RF_Init();
	
	AT24CXX_Init();
	Ht138x_Init();
	LT_init();
	Led_Init();
//	DHT11_Init(); 
			
	//初始化各类型数据帧
	McuStatusInit();
	//Mcu_Data_Init();
	//
}

void App_TaskIdleHook(void)
{
	 Rf_Receive();																											//RF接收
	 MessageHandle();																										//串口数据
	 LT_RxData();																												//2.4G接收
	 KeyHandle();																												//按键
	 IWDG_ReloadCounter(); 																							//喂狗
}		   

static void systick_init(void) 
{ 
    RCC_ClocksTypeDef  rcc_clocks; 
    RCC_GetClocksFreq(&rcc_clocks);   //调用标准库函数，获取系统时钟。
    SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC); //初始化并使能SysTick
		fac_us = rcc_clocks.HCLK_Frequency/1000000;									         
}


static void checkstatus_task(void *p_arg)
{   
    p_arg=p_arg;      //防止编译器产生警告
	while(1)
	{	 
		 //检查系统最新状态
		 CheckStatus();
		 OSTimeDlyHMSM(0,1,0,0); 	//1分钟延时，释放CPU控制权
	}
}
/*					 
static void key_task(void *p_arg)
{   
    p_arg=p_arg;      //防止编译器产生警告
	while(1)
	{	 
	     OSSemPend(key_event,0,&err);
		 //处理来自于按键的事件
		 KeyHandle(); 
//		 OSTimeDlyHMSM(0,0,1,0); 	//1s延时，释放CPU控制权
	}
}
				   

static void rf_heart_task(void *p_arg)	  //RF发送心跳包
{
 	p_arg=p_arg;      //防止编译器产生警告
	while(1)
	{	 
	   Rf_Heart_package();
		 OSTimeDlyHMSM(0,3,0,0); 	//1s延时，释放CPU控制权
		 
	}
}
*/
static void rf_send_task(void *p_arg)
{   
//	OS_CPU_SR  cpu_sr;
	uint8_t i;
	p_arg=p_arg;      //防止编译器产生警告
	

	while(1)
	{	 
		  if(Pop(&rf_send_queue,m_rf_send.data))
		  {				
				 if(m_rf_send.data[0]!= 0)
				 {
					    if(m_rf_send.data[2] == 0x02)
							{
									for(i=1;i<13;i++)
									{
										  m_rf_send.data[3] = i;
											m_rf_send.data[5] =crc8_check(&m_rf_send.data[0],5);
										  Send_Byte(6,REMOTECONTROL,RFSENDRFQUENCY);
									}
							}
							else if(m_rf_send.data[2] == 0x03)
							{
									for(i=1;i<8;i++)
									{
										  m_rf_send.data[3] = i;
											m_rf_send.data[5] =crc8_check(&m_rf_send.data[0],5);
										  Send_Byte(6,REMOTECONTROL,RFSENDRFQUENCY);
									}
							}
							else
							{
									m_rf_send.data[5] =crc8_check(&m_rf_send.data[0],5);
									Send_Byte(6,REMOTECONTROL,RFSENDRFQUENCY);
									m_rf_send.data[0] = 0;
							}
				}	
		  }
		  else OSSemPend(rf_send_event,0,&err);
	}
}

static void rf_receive_task(void *p_arg)   //RF接收数据处理
{
	p_arg=p_arg;      //防止编译器产生警告
	while(1)
	{	 
		OSSemPend(rf_receive_event,0,&err);
		
		Rf_Receive_Process();
	}
}

static void message_task(void *p_arg) 
{ 
	 //   systick_init();     /* Initialize the SysTick. */ 
	//	message_event =  OSSemCreate(0);
		DHT11_Init();
		Mcu_Data_Init();
		rf_receive_event =  OSSemCreate(0);
		rf_send_event = OSSemCreate(0);
	//	OSSemPend(message_event,0,&err);
	//	OSSemPend(key_event,0,&err);
		/*创建新任务*/					  					
	//	OSTaskCreateExt(key_task, 0, &key_task_stk[KEY_TASK_STK_SIZE-1], KEY_TASK_PRIO,KEY_TASK_PRIO,&key_task_stk[0],
	//					KEY_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
	  	
	//	OSTaskCreateExt(rf_heart_task, 0, &rf_heart_task_stk[RFHEART_TASK_STK_SIZE-1], RFHEART_TASK_PRIO,RFHEART_TASK_PRIO,&rf_heart_task_stk[0],
	//					RFHEART_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

		OSTaskCreateExt(rf_receive_task, 0, &rf_receive_task_stk[RFRECEIVE_TASK_STK_SIZE-1], RFRECEIVE_TASK_PRIO,RFRECEIVE_TASK_PRIO,&rf_receive_task_stk[0],
						RFRECEIVE_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	    OSTaskCreateExt(rf_send_task, 0, &rf_send_task_stk[RFSEND_TASK_STK_SIZE-1], RFSEND_TASK_PRIO,RFSEND_TASK_PRIO,&rf_send_task_stk[0],
						RFSEND_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	    OSTaskCreateExt(checkstatus_task, 0, &checkstatus_task_stk[CHECKSTATUS_TASK_STK_SIZE-1],
						 CHECKSTATUS_TASK_PRIO,CHECKSTATUS_TASK_PRIO,&checkstatus_task_stk[0],
						CHECKSTATUS_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
		
		while(1) 
		{
			 // OSSemPend(message_event,0,&err);			
			//	KeyHandle();	
				Rf_Heart_package();
				OSTimeDlyHMSM(0,3,0,0); 	//1s延时，释放CPU控制权
		}        
}
//u8 rxdata[4];			  
//extern u8  RegH,RegL;
int main(void)
   {
      BSP_Init ();
			OSInit(); 
// g_TxMbox=OSMboxCreate((void*)0); //创建全局信号-消息邮箱
        OSTaskCreateExt(message_task, (void *)0, 
              &message_task_stk[MESSAGE_TASK_STK_SIZE - 1], 
              MESSAGE_TASK_PRIO,MESSAGE_TASK_PRIO,&message_task_stk[0],MESSAGE_TASK_STK_SIZE,(void *)0,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
         OSStart(); 
         return 0; 		
/*		 	
	while(1)
	{

		//处理来自于串口的数据帧
		MessageHandle();
		
		//处理来自于按键的事件
		KeyHandle(); 		

		//检查系统最新状态
		CheckStatus();	

//	    ReadIrData();
 	}		*/	
 
}





