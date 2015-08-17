
#ifndef IR
#define IR

#include <stm32f10x.h>
#define IRPIN  PIND
#define RXPIN  0x08
#define IRDATALEN   30

#define IR_PORT           GPIOA
#define IRSEND_PIN        GPIO_Pin_5
#define IRRED_PIN         GPIO_Pin_4

#define TEMPTIMEBUF   250


#define	IR_SEND_H() 	    GPIO_SetBits(IR_PORT,IRSEND_PIN)
                      
#define IR_SEND_L()	    GPIO_ResetBits(IR_PORT,IRSEND_PIN)
                     
#define	IR_SEND_SW()	 if(GPIO_ReadOutputDataBit(IR_PORT,IRSEND_PIN))   	   \
                               GPIO_ResetBits(IR_PORT,IRSEND_PIN);		  	\
						 else  GPIO_SetBits(IR_PORT,IRSEND_PIN)	    					  

extern uint8_t IRrxflag;
#define IRRXSTART		0		//红外接收开始标志
#define IRRXBEGIN		1		//红外起始电平
#define IRRXHBEGIN		2		//红外起始高电平
#define IRRXLBEGIN		3		//红外起始低电平
#define IRRXHTIME		4		//红外高电平标志
#define IRRXLTIME		5		//红外低电平标志
#define IRRX1_0			6		//红外1~0跳变标志
#define IRRXERROR		7		//红外接收错误标志

extern uint8_t Irtxflag;   ///红外接收标志
#define IRRXFINSH       0               //红外接收结束
#define IRTXSTART       1               //红外发射开始
#define IRTXHBEGIN		2		//红外起始高电平
#define IRTXSW   		3		
#define IRTXDATA		4		//红外数据
#define IRTX1_0			5		//红外1~0跳变标志
#define IRTXFRIST       6
#define IRTXBIT         7

#define sbi_(port,bit) (port) |=0x01<<(bit)

#define cbi_(port,bit) (port) &=~(0x01<<(bit))

#define bit_is_set(port,bit)  port & (0x01 << (bit))

#define bit_is_clear(port,bit) !(port& (0x01 << (bit)))

struct infraredTime
{      
	uint8_t  datalen;               //第二段码的数据长度
	uint16_t begintime[2];				//起始位0
	uint16_t starttime[2];			//第一种电平时间
	uint16_t hightime[2];			//第二种电平时间
	uint16_t lowtime[2];			//第三种电平时间
	uint8_t irdata[IRDATALEN];		//数据存储
        uint8_t count;                          //不足4位存储
};

typedef struct infraredTime  irtime_t;
extern uint16_t Irtimebuf[TEMPTIMEBUF]; //红外0和1的时间缓存

void IRstudydata(void);

void IrInit(void);

void IRDataProcess(uint16_t *data);
void IrTransferdata(void);
void  IRstudyfinsh(void);
#endif
