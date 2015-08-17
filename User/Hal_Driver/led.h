#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

#define LED_CONFIG_PIN	 		   GPIO_Pin_10
#define LED_SIGNAL_PIN   	 		 GPIO_Pin_11


#define LED_CONFIG_OFF   GPIO_SetBits(GPIOB,LED_CONFIG_PIN)
#define LED_CONFIG_ON    GPIO_ResetBits(GPIOB,LED_CONFIG_PIN)
	
#define LED_SIGNAL_ON    GPIO_SetBits(GPIOB,LED_SIGNAL_PIN)
#define LED_SIGNAL_OFF   GPIO_ResetBits(GPIOB,LED_SIGNAL_PIN)
/*********************************************************************
*********************************************************************/

void  Led_Init();
#ifdef __cplusplus
}
#endif

#endif /* LED_H */