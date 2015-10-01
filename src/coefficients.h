
// coefficients.h

#ifndef ALGO_CONSTANTS
#define ALGO_CONSTANTS

#include <stdint.h>

typedef struct 
{
	//char uname_len;
	char uname[20];
	uint32_t device_id;
	
	
	float xyz_filter;
	float resultant_filter;
	
	uint32_t fall_detection_window;
	float freefall_threshold;
	float x_angle_threshold;
	float y_angle_threshold;	
	float impact_threshold;
	
	
} PersoValues;

#define PERSO_SIZE (13) //13 WORDS = 52 bytes


//for accelerometer.c
// used for smoothing acquisition of xyz and the resultant
// - not good.
#ifdef FILTERING
	#define SMOOTHING_FACTOR_XYZ_DEFAULT        0.37
	#define SMOOTHING_FACTOR_SV_DEFAULT         0.8
#else
	#define SMOOTHING_FACTOR_XYZ_DEFAULT     1
	#define SMOOTHING_FACTOR_SV_DEFAULT      0.8
#endif


// in main.c, in process_step_count()
// 

#define FALL_DETECTION_WINDOW_DEFAULT 10  //6 periods = 6 * 1/20hz = 300ms

#define FREEFALL_THRESHOLD_DEFAULT 0.5

#define X_ANGLE_THRESHOLD_DEFAULT (1.5)
#define Y_ANGLE_THRESHOLD_DEFAULT (1.5)

#define IMPACT_HIGH_DEFAULT 3.5
#define IMPACT_LOW_DEFAULT 0.1

void revert_to_defaults(void);
extern const PersoValues defaultPerso;
#define flashPerso ((PersoValues*) FLAG_AREA_START)
extern PersoValues tempPerso;
PersoValues* getPerso(void);

void setUsername(char* name);

void writeTempToFlashPerso(void);
void copyLiveToTemp(void);
void copyCurrentToTemp(void);
float string_to_f(uint8_t* s);
void initCoefficients(void);

#endif //ALGO_CONSTANTS

