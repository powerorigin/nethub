/**
  ******************************************************************************
  * @file    IR.c 
  * @author  msl
  * @version V1.1.0
  * @date    17-9-2014
  * @brief   ����ѧϰ����
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "IR.h"
#include "string.h"

#define IRRXVALTIMER	500		//�ߵ͵�ƽ��ʱ��ȷ������ֵ 500US
#define IRRXHLTIMER		200		//��ƽ����ֵ 200US
#define IRENDTIMER		5000	//������ʱ��
#define IRSTARTTIMER		9000	//������ʱ��
#define TIMECOUNT       50000   //��ʱ��һ��װʱ��50MS
#define IRRXHTIMER      3000    //�ǲ����ط���
#define DATABETYTIME    260     //�ж�������0��1
#define IRSTARTTIME     2000    //������ʼ�������ʱ��
#define Time13us    0x04

static irtime_t IrTest;
uint16_t Irtimebuf[TEMPTIMEBUF]; //����0��1��ʱ�仺��
static uint8_t irdatalen = 0;               //����λ��
/*volatile irparameter_t  IrTxparameter = {2,24};*/

uint8_t IRrxflag=0;
uint8_t Irtxflag = 0;   ///������ձ�־
uint8_t IRstudyflag = 0;

static uint16_t studyadder = 0x1000;

static uint8_t hwred_bit_status;


void IrInit(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure; 
	EXTI_InitTypeDef   EXTI_InitStructure;  
	NVIC_InitTypeDef   NVIC_InitStructure; 

	  GPIO_InitStructure.GPIO_Pin = IRSEND_PIN;     //����IOģʽ   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(IR_PORT, &GPIO_InitStructure);  

	  GPIO_InitStructure.GPIO_Pin = IRRED_PIN;     //����IOģʽ   
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //����Ҫ����
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	  GPIO_Init(IR_PORT, &GPIO_InitStructure);  

	   /* Connect EXTI0 Line to PA.00 pin */    
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4); //�趨�����ж��ߡ�
		
	
		EXTI_ClearITPendingBit(EXTI_Line4);       //�����·����λ  
	/* Configure EXTI0 line */    
		EXTI_InitStructure.EXTI_Line = EXTI_Line4;     //�����ж�����   
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;    //�����ش���   
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;   
		EXTI_Init(&EXTI_InitStructure); 
		
		
		/* Enable and set EXTI0 Interrupt to the lowest priority */   
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;      //�����ж����ȼ�    
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;   
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
		NVIC_Init(&NVIC_InitStructure);
}

/***********************************************************
�������ƣ�ReadIrData(void)
�������ܣ���ȡ�������ݺʹ���
***********************************************************/
void ReadIrData(void)
{
	if(bit_is_set(Irtxflag,IRRXFINSH))
	   IRDataProcess(&Irtimebuf[0]);
	if(studyadder < 0x1000)
	{
		//AT24CXX_Write(studyadder,&IrTest,sizeof(irtime_t));
		studyadder = 0x1000;
	}

}
/***********************************************************
�������ƣ�
�������ܣ������������
***********************************************************/
void IRDataProcess(uint16_t *data)
{
	uint16_t num,j,i = 0;
	
	IrTest.datalen = 0;
	for (i=0;i<irdatalen;i++)
	{
		if (data[i] > IRSTARTTIME)
		{
			IrTest.begintime[0] =  data[i];
			IrTest.begintime[1] = data[i+1];
			break;
		}
	}
       if(irdatalen%2 == 1)
	  num = irdatalen - 3;
       else  num = irdatalen - 1;
    IrTest.hightime[0] = data[num];
	IrTest.hightime[1] = data[num+1];
	while (num > i+2 )
	{
		if ( (( data[num] + data[num+1] ) > ( IrTest.hightime[0]+IrTest.hightime[1] ) + IRRXVALTIMER ) || ( ( data[num] + data[num+1] ) < ( IrTest.hightime[0]+IrTest.hightime[1] ) - IRRXVALTIMER )) 
		{
			
                        if((IrTest.lowtime[0]+IrTest.lowtime[1]) == 0)
                        {
                            IrTest.lowtime[0] = data[num];
                            IrTest.lowtime[1] = data[num+1];
                        }
                        else if( (( data[num] + data[num+1] ) > ( IrTest.lowtime[0]+IrTest.lowtime[1] ) + IRRXVALTIMER ) || ( ( data[num] + data[num+1] ) < ( IrTest.lowtime[0]+IrTest.lowtime[1] ) - IRRXVALTIMER ))
                        {
                            IrTest.starttime[0] = data[num];
                            IrTest.starttime[1] = data[num+1];
			    break;
                        }  
                       
		}
		num -= 2;
	}
	num = i + 2;
	i = 0;
	while (num < (irdatalen - 1))
	{
		IrTest.irdata[IrTest.datalen]=IrTest.irdata[IrTest.datalen]>>2;
		if ( ((  data[num] + data[num+1] )<(IrTest.hightime[0]+IrTest.hightime[1]+IRRXHLTIMER)) && ( data[num] + data[num+1]>(IrTest.hightime[0]+IrTest.hightime[1]-IRRXHLTIMER)) )
                {
                   sbi_(IrTest.irdata[IrTest.datalen],7);
                   sbi_(IrTest.irdata[IrTest.datalen],6);
		}
                else if ( (( data[num] + data[num+1] )<(IrTest.lowtime[0]+IrTest.lowtime[1]+IRRXHLTIMER)) && ( data[num] + data[num+1]>(IrTest.lowtime[0]+IrTest.lowtime[1]-IRRXHLTIMER)) )
		{
                   sbi_(IrTest.irdata[IrTest.datalen],7);
                   cbi_(IrTest.irdata[IrTest.datalen],6);
                }
                else if ( (( data[num] + data[num+1] )<(IrTest.starttime[0]+IrTest.starttime[1]+IRRXHLTIMER)) && ( data[num] + data[num+1]>(IrTest.starttime[0]+IrTest.starttime[1]-IRRXHLTIMER)) )
		{
                   cbi_(IrTest.irdata[IrTest.datalen],7);
                   sbi_(IrTest.irdata[IrTest.datalen],6);
                }
		i = i + 2;
                IrTest.count = i;
		if (i>7)
		{
			i=0;
			IrTest.datalen++;
		}
		num +=2;
	}
        cbi_(Irtxflag,IRRXFINSH);
		TIM_SetAutoreload(TIM2,50000);
}

void IRsendwait(void)
{
  uint8_t i; 
  for(i=0;i<84;i++)
      __nop();  
}
/***********************************************************
�������ƣ���ʱ��1��ѯ��ʱ
�������ܣ�38KHz����
***********************************************************/
void IRTimer(uint16_t time,uint8_t ttl)
{
       // TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
     //   TIM1_SetAutoreload(time);
//	 TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE );
	 TIM_ClearITPendingBit(TIM2, TIM_IT_Update );  		//���TIMx�����жϱ�־
	 TIM_SetCounter(TIM2,0);;
	while (TIM_GetCounter(TIM2) < time)
	{
               if (ttl==1) 
               {
                 IR_SEND_SW();
                 IRsendwait();

               }
               else
		  IR_SEND_L();

	}
	IR_SEND_L();
//   TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );    
}

/***********************************************************
�������ƣ����Ⲩ�η��ͳ���
�������ܣ��������ݣ�
***********************************************************/
void IrTransferdata(void)
{
	uint8_t i=0,j=0;

    //   TIM1_TimeBaseInit(0x0010, TIM1_COUNTERMODE_UP, TIMECOUNT, 0x00);
   //    TIM1_ITConfig(TIM1_IT_UPDATE, DISABLE);
//	     TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE );
  //      TIM1_Cmd(ENABLE);

              IRTimer(IrTest.begintime[0],1);
              IRTimer(IrTest.begintime[1],0);
              for (i=0;i<IrTest.datalen;i++)
              {
                      for (j=0;j<8;j=j+2)
                      {
                              
                              switch((IrTest.irdata[i]>>j) & 0x03)
                              {
                                 case 0x03 :  IRTimer(IrTest.hightime[0],1);
                                               IRTimer(IrTest.hightime[1],0);
                                               break;
                                 case 0x02 :  IRTimer(IrTest.lowtime[0],1);
                                               IRTimer(IrTest.lowtime[1],0);
                                               break;
                                 case 0x01 :  IRTimer(IrTest.starttime[0],1);
                                               IRTimer(IrTest.starttime[1],0);
                                               break;
                                    
                               default :break;
                              }
                      }
              }
              if(IrTest.count != 8)
              {
                  for(j=8-IrTest.count;j<7;j=j+2)
                  {
                       switch((IrTest.irdata[i]>>j) & 0x03)
                                  {
                                     case 0x03 :  IRTimer(IrTest.hightime[0],1);
                                                   IRTimer(IrTest.hightime[1],0);
                                                   break;
                                     case 0x02 :  IRTimer(IrTest.lowtime[0],1);
                                                   IRTimer(IrTest.lowtime[1],0);
                                                   break;
                                     case 0x01 :  IRTimer(IrTest.starttime[0],1);
                                                   IRTimer(IrTest.starttime[1],0);
                                                   break;
                                        
                                  default :break;
                                  }
                  }
              }
              IRTimer(IrTest.lowtime[0],1);
              IRTimer(IrTest.lowtime[1],0);

 //       TIM1_Cmd(DISABLE);
 //       TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
 //         TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
  //      TIM1_UpdateDisableConfig(ENABLE);
//	sei();
}

/***********************************************************
�������ƣ�void Irstudydata(void)
�������ܣ�ѧϰ���⣻
***********************************************************/
void Irstudydata(void)
{	   
      cbi_(IRrxflag,IRRXSTART);
      cbi_(Irtxflag,IRRXFINSH);
      IRstudyflag = 1;
      memset(&IrTest.datalen,'\0',sizeof(IrTest));
 /*     TIM1_TimeBaseInit(0x0010, TIM1_COUNTERMODE_UP, TIMECOUNT, 0x00);
      TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
      TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
      TIM1_Cmd(ENABLE);	  */
  //    hwred_bit_status = (BitStatus)0x01;
}

/***********************************************************
�������ƣ�void CloseStudy(void)
�������ܣ��رպ���ѧϰ��
***********************************************************/
// void CloseStudy(void)
// {	   
//      cbi_(EIMSK,INT1); 		//ʧ���ⲿ�ж�1
// }
/******************************************
������:��������жϷ������
�γ�����:��
���ز���:��
*******************************************/
void IRstudydata(void)
{
	uint16_t datatimerbuf;      //����������ݻ���
       
 //      if(hwred_bit_status != GPIO_ReadInputPin(IR_PORT, IRRED_PIN))
   //     {
   //        hwred_bit_status = (BitStatus)!hwred_bit_status;
           if( bit_is_clear(Irtxflag,IRRXFINSH) && IRstudyflag)
            {
              datatimerbuf = TIM_GetCounter(TIM2);//- TIMECOUNT;
              TIM_SetCounter(TIM2,0);    
              if(bit_is_clear(IRrxflag,IRRXSTART))
                  {
                          sbi_(IRrxflag,IRRXSTART);
                          memset((void *)Irtimebuf,'\0',300);
                          irdatalen = 0;
                  //	uart_print(&irdatalen,1);
                  }
                  else if(irdatalen < TEMPTIMEBUF)
                  {
                          Irtimebuf[irdatalen] = datatimerbuf;
                          irdatalen++;
                  }
  
          }
   //     }
}
/******************************************
������:������ս����������
�γ�����:��
���ز���:��
*******************************************/

void  IRstudyfinsh(void)
{
     if(bit_is_set(IRrxflag,IRRXSTART))     //��������ж�
    {
	   /* if( bit_is_clear(IRrxflag,IRRXERROR))*/
//	    Irtimebuf[irdatalen] = datatimerbuf[1] + IrTest.IRfrequency;  //��������һ������
        sbi_(Irtxflag,IRRXFINSH);
    	cbi_(IRrxflag,IRRXSTART);
  //      TIM1_Cmd(DISABLE);
  //      Thread_set_event(RF_THREAD_ID, IR_STUDY_EV);
        IRstudyflag = 0;
/*    	cbi_(IRrxflag,IRRXBEGIN);*/
     } 
}