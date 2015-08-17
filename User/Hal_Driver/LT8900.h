#ifndef LT8900_H
#define LT8900_H


#include "sys.h"


#define MOSI        GPIO_Pin_5
#define MISO        GPIO_Pin_4
#define CLK         GPIO_Pin_2
#define SS          GPIO_Pin_1

#define RESET_N     GPIO_Pin_3
#define PKT_FLAG    GPIO_Pin_0


#define	MOSI_H() 	    GPIO_SetBits(GPIOA,MOSI)                     
#define MOSI_L()	    GPIO_ResetBits(GPIOA,MOSI)

#define	CLK_H() 	    GPIO_SetBits(GPIOA,CLK)                  
#define CLK_L()	      GPIO_ResetBits(GPIOA,CLK)

#define	SS_H() 	      GPIO_SetBits(GPIOA,SS)                  
#define SS_L()	      GPIO_ResetBits(GPIOA,SS)

#define	RESET_H() 	  GPIO_SetBits(GPIOA,RESET_N)                  
#define RESET_L()	    GPIO_ResetBits(GPIOA,RESET_N)

#define READ_MISO 	PAin(4)
#define READ_PKT 	  PAin(0)


#define CHANNEL     0

#define KEYDATA     2
#define SEND_DATA_LEN			6

void LT8900IO_Init();
void LT_readreg(unsigned char reg);
void LT_writereg(unsigned char reg,unsigned char H,unsigned char L);

u8 SPI_write_byte(u8 byte);

#endif /* LT8900_H */