#include <stm32f10x.h>
#include "LT8900.h"
#include "protocol.h"
#include "ucos_ii.h"

u8 RegH,RegL;
OS_EVENT  *LT8900_Event;



void LT8900IO_Init()
{
    GPIO_InitTypeDef   GPIO_InitStructure; 


	  GPIO_InitStructure.GPIO_Pin = MOSI;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//    GPIO_Mode_Out_PP;   
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	  GPIO_InitStructure.GPIO_Pin = MISO;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //必须要上拉
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	  GPIO_InitStructure.GPIO_Pin = CLK;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //必须要上拉
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure); 

		GPIO_InitStructure.GPIO_Pin = SS;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //必须要上拉
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
		GPIO_InitStructure.GPIO_Pin = RESET_N;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //必须要上拉
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	 
		GPIO_InitStructure.GPIO_Pin = PKT_FLAG;     //设置IO模式   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //必须要上拉
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	  LT8900_Event = OSSemCreate(1);
}	

u8 SPI_write_byte(u8 byte)
{
	u8 i;
   	for(i=0;i<8;i++) 
   	{
		if(byte & 0x80)
			 MOSI_H();
		else 
     		 MOSI_L();			
		byte = (byte << 1); 
		//delay_us(1);
		CLK_H(); 
		//delay_us(1);	
		byte |= READ_MISO;       		  
		CLK_L(); 
		//delay_us(1);
   	}
	delay_us(1);
    return(byte);           		  
}
/*c SPI_read_byte(c dat)
{
	c k;
	
	SS = 0;                
	SPI_write_byte(dat);          
	k = SPI_write_byte(0);    
	SS = 1;               
	return(k);       
}*/
void LT_readreg(unsigned char reg)
{
	SS_L();
	reg=SPI_write_byte(0x80+reg);
	RegH=SPI_write_byte(0xff);
	RegL=SPI_write_byte(0xff);
	SS_H();	
}
void LT_writereg(unsigned char reg,unsigned char H,unsigned char L)
{
	SS_L();
	reg=SPI_write_byte(0x7f&reg);
	RegH = SPI_write_byte(H);
	RegL = SPI_write_byte(L);
	SS_H();
}

void LT_cleanFIFO()
{
	LT_readreg(52);
	RegH &= 0x7f;
	LT_writereg(52,RegH,RegL);
}
/*void LT_readFIFOPoint()
{
	LT_readreg(52);
	W_FIFO = RegH & 0x3F;
	R_FIFO = RegL & 0x3F;
}*/



