#ifdef BLE_v1
    #define LED_RESETTING           17
    #define LED_FALL_DETECTED       18
    #define LED_NO_FALL             19  
#elif DEBUG_BOARD
    #define LED_RESETTING           22
    #define LED_NO_FALL             21
    #define LED_FALL_DETECTED       28
#else
    #define LED_POS1              	21 
		#define LED_POS2           			22 
    #define LED_POS3             		28 
    #define LED_POS4       					29 

		#define LED_RESETTING           LED_POS2 //yellow
    #define LED_NO_FALL             LED_POS3 //orange
    #define LED_FALL_DETECTED       LED_POS4 //red
    #define LED_OTHER               LED_POS1 
#endif

void errorLEDs(void);
void welcomeLEDs(void);
