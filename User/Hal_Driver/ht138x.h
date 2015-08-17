#ifndef HT138X_H
#define HT138X_H

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

typedef struct 
{
	uint8_t 	timersec;
	uint8_t 	timermin;
	uint8_t 	timerhour;
	uint8_t		timerday;
	uint8_t     timermonth;
	uint8_t     timerweek;
	uint8_t     timeryear;
}_timer_tab;
  
extern _timer_tab timer_tab;

#define AT24CXX_TIME_ADDR       0x10
//√¸¡Ó 
#define HT138X_CMD_SEC          0x80  //
#define HT138X_CMD_MIN          0x82  //
#define HT138X_CMD_HOUR         0x84  //
#define HT138X_CMD_DATE         0x86  //
#define HT138X_CMD_MONTH        0x88  //
#define HT138X_CMD_DAY          0x8A  //
#define HT138X_CMD_YEAR         0x8C  //
#define HT138X_CMD_WP           0x8E  //
  
//
#define HT138X_BIT_12H          0x80  //12h
#define HT138X_BIT_CH           0x80  //
#define HT138X_BIT_WP           0x80  //
#define HT138X_BIT_PM           0x20  //
/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

extern void Ht138x_Init(void);
extern void ht138x_singleWrite(uint8_t cmd, uint8_t value);
extern uint8_t ht138x_singleRead(uint8_t cmd);
void Read_HT1381_NowTimer(void);
void Int_HT1381(void);
void Set_Timer();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HT138X_H */