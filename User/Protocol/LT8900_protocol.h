
#ifndef LT8900_PROTOCL
#define LT8900_PROTOCL

#include <includes.h>

#define DATALEN     10

void LT_TxData();
void LT_RxData();
void LT_init();

extern uint8_t Lt8900_Rxdata[DATALEN];
extern uint8_t Lt8900_Txdata[DATALEN];
#endif
