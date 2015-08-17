#include <includes.h>

uint8_t 									get_one_package;							//判断是否接收到一个完整的串口数据包
uint8_t 									uart_buf[256]; 								//串口缓冲区
uint16_t 									uart_Count=0;									//串口缓冲区数据长度
uint8_t 									cmd_flag =0;									
uint16_t 									cmd_len =0 ;		
uint8_t 									wait_ack_time;								
uint8_t 									check_status_time;
uint8_t 									report_status_idle_time;
uint32_t									SN;
uint8_t 									cmd_flag1, cmd_flag2;
uint32_t									wait_wifi_status;

pro_commonCmd							m_pro_commonCmd;							//通用命令，心跳、ack等可以复用此帧
m2w_returnMcuInfo					m_m2w_returnMcuInfo;					//返回mcu信息帧
m2w_setModule							m_m2w_setModule;							//配置模块帧
w2m_controlMcu						m_w2m_controlMcu;							//控制命令帧
m2w_mcuStatus							m_m2w_mcuStatus;							//当前最新的mcu状态帧
m2w_mcuStatus							m_m2w_mcuStatus_reported;			//上次发送的mcu状态，当与最新的mcu状态不同时，需要上报；
w2m_reportModuleStatus		m_w2m_reportModuleStatus;			//wifi模块上报状态帧
pro_errorCmd							m_pro_errorCmd;								//错误命令帧

static  void  BSP_LED_Init(void);
//	static  void  BSP_KEY_Init (void); 

void  BSP_Init (void)
{
	SystemInit();
	UART_Configuration();	
//	KEY_GPIO_Init();
//	TIM3_Int_Init(100,7199);
	

	//应用初始化，包括电机、LED、温湿度、红外；
//	Motor_Init();	
//	RGB_LED_Init();
//	LT8900IO_Init();
//	LT_init();

//	RF_Init();
//	IrInit();
//	DHT11_Init();
//BSP_LED_Init();                 /* Initialize the LED  */
//	Init_Uart_on_Chip(9600);
//	BSP_KEY_Init();  
//初始化各类型数据帧
	McuStatusInit();
}

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

 /*
static  void BSP_LED_Init(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_GPIO_PORT_LED, ENABLE);  //使能时钟 //根据自己板子情况修改

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_led1|GPIO_Pin_led2; //根据自己板子情况修改
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIO_PORT_LED, &GPIO_InitStructure); //根据自己板子情况修改
} 

void led_on(uint32_t n) 
{ 
	switch (n) 
	{ 
		case LED1:  GPIO_SetBits(GPIO_PORT_LED, GPIO_Pin_led1); 	break; 
		case LED2:  GPIO_SetBits(GPIO_PORT_LED, GPIO_Pin_led2); 	break; 
		default: 	break; 
	} 
} 
 
void led_off(uint32_t n)
{ 
switch (n) 
{ 
    	case LED1:  GPIO_ResetBits(GPIO_PORT_LED, GPIO_Pin_led1);   break; 
    	case LED2:  GPIO_ResetBits(GPIO_PORT_LED, GPIO_Pin_led2);   break;  
    	default:  break; 
} 
}	 */
