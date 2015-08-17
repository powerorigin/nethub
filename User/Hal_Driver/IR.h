
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
#define IRRXSTART		0		//������տ�ʼ��־
#define IRRXBEGIN		1		//������ʼ��ƽ
#define IRRXHBEGIN		2		//������ʼ�ߵ�ƽ
#define IRRXLBEGIN		3		//������ʼ�͵�ƽ
#define IRRXHTIME		4		//����ߵ�ƽ��־
#define IRRXLTIME		5		//����͵�ƽ��־
#define IRRX1_0			6		//����1~0�����־
#define IRRXERROR		7		//������մ����־

extern uint8_t Irtxflag;   ///������ձ�־
#define IRRXFINSH       0               //������ս���
#define IRTXSTART       1               //���ⷢ�俪ʼ
#define IRTXHBEGIN		2		//������ʼ�ߵ�ƽ
#define IRTXSW   		3		
#define IRTXDATA		4		//��������
#define IRTX1_0			5		//����1~0�����־
#define IRTXFRIST       6
#define IRTXBIT         7

#define sbi_(port,bit) (port) |=0x01<<(bit)

#define cbi_(port,bit) (port) &=~(0x01<<(bit))

#define bit_is_set(port,bit)  port & (0x01 << (bit))

#define bit_is_clear(port,bit) !(port& (0x01 << (bit)))

struct infraredTime
{      
	uint8_t  datalen;               //�ڶ���������ݳ���
	uint16_t begintime[2];				//��ʼλ0
	uint16_t starttime[2];			//��һ�ֵ�ƽʱ��
	uint16_t hightime[2];			//�ڶ��ֵ�ƽʱ��
	uint16_t lowtime[2];			//�����ֵ�ƽʱ��
	uint8_t irdata[IRDATALEN];		//���ݴ洢
        uint8_t count;                          //����4λ�洢
};

typedef struct infraredTime  irtime_t;
extern uint16_t Irtimebuf[TEMPTIMEBUF]; //����0��1��ʱ�仺��

void IRstudydata(void);

void IrInit(void);

void IRDataProcess(uint16_t *data);
void IrTransferdata(void);
void  IRstudyfinsh(void);
#endif
