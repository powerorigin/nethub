#ifndef _HAL_UART_H
#define _HAL_UART_H

#include <stdio.h>
#include <stm32f10x.h>

void UART_Configuration(void);
void NVIC_Configuration(void);
void UART1_Send_DATA(uint8_t data);
void UART1_SendBuf_DATA(uint8_t* buf,uint16_t length);

#endif /*_HAL_UART_H*/
