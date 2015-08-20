
#ifndef RF_PROTOCL
#define RF_PROTOCL

#include <stm32f10x.h>

#define RF_CONTROL_CMD         0x01            //控制命令
#define RF_INQUIRE_CMD         0x02            //查询命令  
#define RF_SET_CMD             0x04            //对码命令
#define RF_LINK_CMD            0x05            //连接命令
#define RF_HEART_CMD           0x06            //心跳包



#define HOST_DELETE_CMD        0x03            //删除命令 
#define HOST_SEARCH_CMD        0x07            //查询设备
#define HOST_BACK_CMD          0x08            //网关状态反馈命令

#define CLASS                  0x00             //RF数据位
#define ID                     0x01
#define CMD                    0x02
#define STATE                  0x03
#define ELECTRIC_U             0x04
#define ELECTRIC_I             0x06
#define ELECTRIC_P             0x08

#define MAX_DEVICE_NUMBER      10	         //最大设备数
#define DEVICE_DATA_LEN        2

#define POWER_SOCKET_CLASS     0x10
#define LIST_STATE                  2

extern uint8_t rf_device_list[MAX_DEVICE_NUMBER][DEVICE_DATA_LEN];  //在线设备列表
extern volatile uint8_t  rf_inquire_flag;																	      
extern uint8_t  temperature_humidity_new[2];						  //温湿度缓存
extern uint8_t  temperature_humidity_old[2];


#define RF_INQUIRE_SUCCESS          1
#define RF_SEND_ING                 2
#define RF_SEND_SUCCESS 			3

void Rf_Heart_package(void);
void Rf_Receive_Process(void);
void Rf_Data_Init(void);
void Host_Configuration(uint8_t *data);
void Rf_Send_Process(uint8_t *data);
#endif
