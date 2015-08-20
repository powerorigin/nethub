#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stdio.h>
#include <stm32f10x.h>
#include "hal_key.h"
#include "hal_rgb_led.h"
#include "hal_motor.h"
#include "hal_temp_hum.h"
#include "433RF.h"

#define MATCODE_CONTROL_CMD     0x01		   //对码命令字
#define ERASURE_CONTROL_CMD     0x02		   //擦除命令字
#define READ_CONTROL_CMD      	0x03		   //读功能命令字-
#define WRITE_CONTROL_CMD      	0x04		   //写功能命令字-

#define GROUP_CONTROL_CMD      	0x10		   //组控
#define SINGLE_CONTROL_CMD      0x00		   //单控



#define LT8900_DATA      0x20		   //无线遥控器

#define AT24CXX_DEVICE_LIST_ADDR		0x20		

#define		REPORT_STATUS													0x00
#define 	REQUEST_STATUS												0x01

#define		ERROR_CHECKSUM												0x01
#define		ERROR_CMD															0x02
#define		ERROR_OTHER														0x03

#define		CMD_GET_MCU_INFO                      0x01
#define		CMD_GET_MCU_INFO_ACK                  0x02
#define		CMD_SEND_MCU_P0												0x03
#define		CMD_SEND_MCU_P0_ACK										0x04
#define		CMD_SEND_MODULE_P0										0x05
#define		CMD_SEND_MODULE_P0_ACK								0x06
#define		CMD_SEND_HEARTBEAT										0x07
#define		CMD_SEND_HEARTBEAT_ACK								0x08
#define		CMD_SET_MODULE_WORKMODE								0x09
#define		CMD_SET_MODULE_WORKMODE_ACK						0x0A
#define		CMD_RESET_MODULE											0x0B
#define		CMD_RESET_MODULE_ACK									0x0C
#define		CMD_REPORT_MODULE_STATUS							0x0D
#define		CMD_REPORT_MODULE_STATUS_ACK					0x0E
#define		CMD_REBOOT_MCU												0x0F
#define		CMD_REBOOT_MCU_ACK										0x10
#define		CMD_MODULE_CMD_ERROR_ACK							0x11
#define		CMD_MCU_CMD_ERROR_ACK									0x12

#define		SUB_CMD_CONTROL_MCU										0x01
#define		SUB_CMD_REQUIRE_STATUS								0x02
#define		SUB_CMD_REQUIRE_STATUS_ACK						0x03
#define		SUB_CMD_REPORT_MCU_STATUS							0x04
  
#define  	MAX_SEND_TIME                         40

#define		PRO_VER																"00000004"
#define		P0_VER																"00000004"
#define		HARD_VER															"00000001"
#define		SOFT_VER															"00000001"
#define		PRODUCT_KEY												"24e527ddf5364dbfad066d32cdc988f3"

typedef	struct	_status_writable								status_writable;
typedef	struct	_status_readonly								status_readonly;
typedef struct	_pro_headPart										pro_headPart;
typedef	struct	_pro_commonCmd									pro_commonCmd;
typedef struct	_m2w_returnMcuInfo							m2w_returnMcuInfo;
typedef	struct	_m2w_setModule									m2w_setModule;
typedef	struct	_w2m_controlMcu									w2m_controlMcu;
typedef	struct	_m2w_mcuStatus									m2w_mcuStatus;
typedef	struct	_w2m_reportModuleStatus					w2m_reportModuleStatus;
typedef struct	_pro_errorCmd										pro_errorCmd;

__packed	struct	_status_writable
{
	uint8_t							device_sort;//类别
	uint8_t					        device_id;	//设备ID
	uint8_t							device_cmd;	//命令字
	uint8_t					    	device_data[10];//数据
};

__packed	struct	_status_readonly
{
	uint8_t							ir_status;
	uint8_t							temputure;
	uint8_t							humidity;
	uint8_t							alert_byte;
	uint8_t							fault_byte;
};

__packed	struct	_pro_headPart
{
	uint8_t							head[2];
	short								len;
	uint8_t							cmd;
	uint8_t							sn;
	uint8_t							flags[2];
};

__packed	struct	_pro_commonCmd
{
	pro_headPart 				head_part;
	uint8_t							sum;
};

__packed	struct	_pro_errorCmd
{
	pro_headPart 				head_part;
	uint8_t							error;
	uint8_t							sum;	
};

__packed	struct	_m2w_returnMcuInfo
{
	pro_headPart				head_part;
	uint8_t							pro_ver[8];
	uint8_t							p0_ver[8];
	uint8_t							hard_ver[8];
	uint8_t							soft_ver[8];
	uint8_t							product_key[32];
	short								binable_time;
	uint8_t							sum;
};

__packed	struct	_m2w_setModule
{
	pro_headPart				head_part;
	uint8_t							config_info;
	uint8_t							sum;
};

__packed	struct	_w2m_controlMcu
{
	pro_headPart				head_part;
	uint8_t							sub_cmd;
	uint8_t							cmd_tag;
	status_writable		   	status_w;
	uint8_t							sum;
};

__packed	struct	_m2w_mcuStatus
{
	pro_headPart				head_part;
	uint8_t							sub_cmd;
	status_writable			status_w;
//	status_readonly			status_r;
	uint8_t							sum;
};

__packed	struct	_w2m_reportModuleStatus
{
	pro_headPart				head_part;
	uint8_t							status[2];
	uint8_t							sum;
};

short	exchangeBytes(short value);
uint8_t	MessageHandle(void);
void	SendToUart(uint8_t *buf, uint16_t packLen, uint8_t tag);
void	ReportStatus(uint8_t tag);
void	CheckStatus(void);
uint8_t CheckSum(  uint8_t *buf,int packLen );

void	LEDStatus(void);
void	LT_LEDStatus(void);
void	Set_LEDStatus(u8*Char,u8 cmd);
void	Get_LEDStatus(u8*Char);	
void RetryTaskInit(void);
void RetryTask(void);




#endif /*_PROTOCOL_H*/


