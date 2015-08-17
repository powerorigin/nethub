
/*
ancheel changed it for STM32 2010-6-6
www.anchey.com
*/

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#include <stdint.h>

typedef uint32_t clock_time_t;
//typedef uint16_t clock_time_t;//8位机宜用16位数作为系统滴答

#define CLOCK_CONF_SECOND 100//系统滴答周期 10ms
//extern clock_time_t clock_time(void);
extern volatile clock_time_t sys_ticks;

#define clock_time() sys_ticks//我们使用宏定义来提升系统性能

extern void clock_arch_init(void);

#endif /* __CLOCK_ARCH_H__ */
