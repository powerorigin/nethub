/*STM32 uip ��ֲ
*ancheel
*www.anchey.com
*/
#include <stm32f10x.h>
#include <misc.h>
#include "clock-arch.h"

volatile clock_time_t sys_ticks;

//ʹ��SysTick��Ϊϵͳ�δ�
void clock_arch_init(void)
{
    sys_ticks = 0;

    SysTick_Config(72000000UL / CLOCK_CONF_SECOND);

}


/*
;*****************************************************************************************************
;*                            			End Of File
;*****************************************************************************************************
;*/
