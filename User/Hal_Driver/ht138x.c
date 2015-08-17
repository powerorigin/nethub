/**
  ******************************************************************************
  * @file    HT138X.c 
  * @author  chyq
  * @version V1.1.0
  * @date    18-9-2013
  * @brief   HT138X函数.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "ht138x.h"
#include "myiic.h" 

#define  HT138X_CS   		PAout(7)

_timer_tab timer_tab;

extern void delay_us(uint32_t nus);

/*********************************************************************
 * @fn      ht138x_Init
 *
 * @brief
 *
 *   init.
 *
 * @param   void
 *
 * @return  void
 */
void Ht138x_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;			     	
	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      ht138x_writebyte
 *
 * @brief
 *
 *   write a byte to ht138x.
 *
 * @param   value -
 *
 * @return  void
 */
void ht138x_writebyte(uint8_t value)
{
    uint8_t  i;
    
    SDA_OUT();
    for(i = 0; i < 8; i ++) 
    { 
       	 IIC_SCL=0;
        
        if(value & 0x01) 
            IIC_SDA=1;
        else 
            IIC_SDA=0;
        
        delay_us(5);
        
        IIC_SCL=1;
        
        delay_us(5);
        
        value >>= 1;
    }
    
//    HT138X_SO_MODE();
    IIC_SCL=0;
}

/*********************************************************************
 * @fn      ht138x_readbyte
 *
 * @brief
 *
 *   read a byte from ht138x.
 *
 * @param   none
 *
 * @return  value
 */
uint8_t ht138x_readbyte(void)
{
    uint8_t i, value;

    SDA_IN();
    for(i = 0; i < 8;i ++) 
    { 
        value >>= 1;
        
        if(READ_SDA) 
            value |= 0x80;
        else 
            value &= 0x7F;
        
        IIC_SCL=1;    
        delay_us(5);     
        IIC_SCL=0;
        delay_us(5);
    }
       
    return value;
}

/*********************************************************************
 * @fn      ht138x_singleWrite
 *
 * @brief
 *
 *   write to ht138x.
 *
 * @param   cmd -
 *          value -
 *
 * @return  void
 */
void ht138x_singleWrite(uint8_t cmd, uint8_t value)
{
	HT138X_CS = 0;
	delay_us(5); 
	IIC_SCL=0;
	delay_us(5);
    HT138X_CS = 1;
    ht138x_writebyte(cmd);
    ht138x_writebyte(value);
	IIC_SCL=1;
	delay_us(5);
    HT138X_CS = 0;
}

/*********************************************************************
 * @fn      ht138x_singleRead
 *
 * @brief
 *
 *   read from ht138x.
 *
 * @param   cmd -
 *
 * @return  value
 */
uint8_t ht138x_singleRead(uint8_t cmd)
{
    uint8_t  value;
    
    HT138X_CS = 0;
	delay_us(5); 
	IIC_SCL=0;
	delay_us(5);
    HT138X_CS = 1;
    ht138x_writebyte(cmd | 0x01);
    value = ht138x_readbyte();
	IIC_SCL=0;
	delay_us(5);
    HT138X_CS = 0;
    
    return value;
}
void BCDToO()
{
	uint8_t year, month, date;
	uint8_t hour, minute, second;
	uint8_t week;
/*	unsigned char *timebuf;*/
	
	// BCD转10进制
	year = timer_tab.timeryear;
	timer_tab.timeryear = year/16*10 + year%16;
	
	month = timer_tab.timermonth;
	timer_tab.timermonth = month/16*10 + month%16;
	
	date = timer_tab.timerday;
	timer_tab.timerday = date/16*10 + date%16;
	
	hour = timer_tab.timerhour;
	timer_tab.timerhour = hour/16*10 + hour%16;
	
	minute = timer_tab.timermin;
	timer_tab.timermin = minute/16*10 + minute%16;
	
	second = timer_tab.timersec;
	timer_tab.timersec = second/16*10 + second%16;
	
	week = timer_tab.timerweek;
	timer_tab.timerweek = week/16*10 + week%16;
}	
void OToBCD()
{
	uint8_t year, month, date;
	uint8_t hour, minute, second;
	uint8_t week;
/*	unsigned char *timebuf;*/
	
	// 10进制转BCD
	year = timer_tab.timeryear;
	timer_tab.timeryear = year/10*16 + year%10;
	
	month = timer_tab.timermonth;
	timer_tab.timermonth = month/10*16 + month%10;
	
	date = timer_tab.timerday;
	timer_tab.timerday = date/10*16 + date%10;
	
	hour = timer_tab.timerhour;
	timer_tab.timerhour = hour/10*16 + hour%10;
	
	minute = timer_tab.timermin;
	timer_tab.timermin = minute/10*16 + minute%10;
	
	second = timer_tab.timersec;
	timer_tab.timersec = second/10*16 + second%10;
	
	week = timer_tab.timerweek;
	timer_tab.timerweek = week/10*16 + week%10;
}
/********************************************************************
* 名称: 读取HT1381当前时间 
* 调用: ht138x_singleRead
* 输入: 保存当前时间地址+当前时间格式为: 秒 分 时 日 月 星期 年 
* 7Byte (BCD码) 1B 1B 1B 1B 1B 1B 1B 
********************************************************************/ 
void Read_HT1381_NowTimer(void)
{ 	
	unsigned char i; 	
	unsigned char sAddr; 	
	unsigned char *p; 	
	sAddr = 0x81; 	
	p =(unsigned char*)&timer_tab.timersec; 	
	for(i=0;i<7;i++) 	
	{ 		
		*p = ht138x_singleRead(sAddr); 		
		p++; 		
		sAddr += 2; 	
	} 
	 BCDToO();
}

/******************************************************************** 
* 名称: 初始化HT1381 
* 调用: Write_HT1381Addr_1Byte 
* 输入: 初始时间地址+初始时间格式为: 秒 分 时 日 月 星期 年 
* 7Byte (BCD码) 1B 1B 1B 1B 1B 1B 1B ********************************************************************/ 
void Int_HT1381(void)
{ 	
	unsigned char i; 	
	unsigned char sAddr; 	
	unsigned char *p; 	
	sAddr = 0x80; 	
	p =(unsigned char*)&timer_tab.timersec; 	
	ht138x_singleWrite(0x8e,0x00);		//控制命令,WP=0,写操作? 	
	for(i=0;i<7;i++) 	
	{ 		
		ht138x_singleWrite(sAddr,*p);	//秒,分,时,日,月,星期,年 		
		p++; 		
		sAddr += 2; 	
	} 	
	ht138x_singleWrite(0x8e,0x80);		//控制命令,WP=1,写保护? 
}
/******************************************************************** 
			上电初始化日期时间 
********************************************************************/ 
void Set_Timer()
{		
	timer_tab.timersec = 0x00; 			
	timer_tab.timermin = 0x3a; 			
	timer_tab.timerhour = 0x17; 			
	timer_tab.timerday = 0x1e; 			
	timer_tab.timermonth = 0x06; 			
	timer_tab.timerweek = 0x01; 			
	timer_tab.timeryear = 0x0f;
	OToBCD();			
	Int_HT1381();		//初始化HT1381 			
//	BCDToO();
}