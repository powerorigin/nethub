#ifndef RF_H
#define RF_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "cc1100.h"  
/*********************************************************************
 * MACROS
 */
 
#define RF_BUF_LEN 64  
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

extern uint8_t  Rf_Buf[RF_BUF_LEN];  
/*********************************************************************
 * FUNCTIONS
 */

extern void Rf_Init(void);
extern void Rf_SetChannel(uint8_t channr);
extern void Rf_SetWORmode(void);
extern void Rf_EnterWORmode(void);
extern void Rf_EnterIdleMode(void);
extern void Rf_FlushTxBuf(void);
extern void Rf_FlushRxBuf(void);
extern void Rf_SendPacket(uint8_t *txBuffer, uint8_t size);
extern uint8_t Rf_ReceivePacket(uint8_t *rxBuffer, uint8_t *length);
extern void Rf_SendStart(uint8_t *txBuffer, uint8_t size);
extern void Rf_SendEnd(void);
extern void Rf_ReceiveStart(void);
extern uint8_t Rf_ReceiveEnd(uint8_t *rxBuffer, uint8_t *length);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* RF_H */
