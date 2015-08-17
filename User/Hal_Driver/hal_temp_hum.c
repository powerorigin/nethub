#include "hal_temp_hum.h"
#include "ucos_ii.h"

uint8_t  fac_us;																		
		    								   
void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;														//LOADμ??μ	    	 
	ticks=nus*fac_us; 																				//Dèòaμ??ú??êy	  		 
	tcnt=0;
	told=SysTick->VAL;        																//????è?ê±μ???êy?÷?μ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;													//?aà?×￠òaò???SYSTICKê?ò???μY??μ???êy?÷?í?éò?á?.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;																	//ê±??3?1y/μèóúòa?ó3ùμ?ê±??,?òí?3?.
		}  
	}; 									    
}

//Reset DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 											//SET OUTPUT
    DHT11_DQ_OUT=0; 											//GPIOA.0=0
		//delay_us(20000);  
    OSTimeDlyHMSM(0,0,0,20); 	//1s延时，释放CPU控制权  								//Pull down Least 18ms
    DHT11_DQ_OUT=1; 											//GPIOA.0=1    
	delay_us(30);     										//Pull up 20~40us
}

u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();												//SET INPUT	 
  while (DHT11_DQ_IN&&retry<100)				//DHT11 Pull down 40~80us
	{
		retry++;
		delay_us(1);
	}	 
	
	if(retry>=100)
		return 1;
	else 
		retry=0;
	
  while (!DHT11_DQ_IN&&retry<100)				//DHT11 Pull up 40~80us
	{
		retry++;
		delay_us(1);
	}
	
	if(retry>=100)
		return 1;														//chack error	    
	
	return 0;
}

u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)					//wait become Low level
	{
		retry++;
		delay_us(1);
	}
	
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)				//wait become High level
	{
		retry++;
		delay_us(1);
	}
	
	delay_us(40);//wait 40us
	
	if(DHT11_DQ_IN)
		return 1;
	else 
		return 0;		   
}

u8 DHT11_Read_Byte(void)    
{        
  u8 i,dat;
  dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
  }						    
  
	return dat;
}

u8 DHT11_Read_Data(u8 *temperature,u8 *humidity)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humidity=buf[0];
			*temperature=buf[2];
		}
	}
	else 
		return 1;
	
	return 0;	    
}
	 
u8 DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);				 
 	GPIO_SetBits(GPIOA,GPIO_Pin_6);						 
			    
	DHT11_Rst();  
	return DHT11_Check();
} 
